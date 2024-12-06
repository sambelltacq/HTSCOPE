/*
 * MultichannelScope.cpp
 *
 *  Created on: 6 Dec 2024
 *      Author: pgm
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <iocsh.h>
#include <epicsExport.h>

#include "asynPortDriver.h"

#include <string>

#define PS_NCHAN 		"NCHAN"				/* asynInt32, 		r/o */
#define PS_NSAM			"NSAM"				/* asynInt32,       r/o */
#define PS_CHANNEL		"CHANNEL"			/* ,       r/o */
#define PS_REFRESH		"REFRESH"			/* asynInt32, 		r/w */

typedef epicsFloat64 CTYPE;

#include <sys/stat.h>
#include <iostream>
#include <string>

long GetFileSize(const std::string& filename) {
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}


class MultiChannelScope : public asynPortDriver {
public:
    MultiChannelScope(const char *portName, int numChannels, int maxPoints, unsigned data_size);

    // Add methods for scope functionality
    ~MultiChannelScope();
    /* These are the methods that we override from asynPortDriver */
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);

    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
    virtual asynStatus readFloat64Array(asynUser *pasynUser, epicsFloat64 *value,
                                        size_t nElements, size_t *nIn);
private:
    const unsigned nchan;
    const unsigned nsam;
    const int ssb;

	int P_NCHAN;
	int P_NSAM;
	int P_CHANNEL;
	int P_REFRESH;

    // Add private members for scope data
    long data_len;
    epicsInt16* RAW;		    // array [SAMPLE][CH]

    CTYPE** CHANNELS;			// array [CH][SAMPLE]
    FILE* fp;
    unsigned stride;
    unsigned startoff;


    void get_data() {
    	printf("%s\n", __FUNCTION__);

    	for (unsigned isam = 0; isam < nsam; ++isam){
    		for (unsigned ic = 0; ic < nchan; ++ic){
    			CHANNELS[ic][isam] = RAW[(isam+startoff)*ic+ic];
    		}
    	}

    	for (unsigned ic = 0; ic < nchan; ic++){
    		printf("%s ic:%d nsam:%d P_CHANNEL:%d\n", __FUNCTION__, ic, nsam, P_CHANNEL);
    		doCallbacksFloat64Array(CHANNELS[ic], nsam, P_CHANNEL, ic);
    	}
    }
};


MultiChannelScope::MultiChannelScope(const char *portName, int numChannels, int maxPoints, unsigned data_size) :
		 asynPortDriver(portName,
							 numChannels,
							 asynInt32Mask | asynFloat64Mask | asynFloat64ArrayMask | asynDrvUserMask,
							 asynInt32Mask | asynFloat64Mask | asynFloat64ArrayMask,
							 ASYN_CANBLOCK | ASYN_MULTIDEVICE,
							 1,
							 0,
							 0),
							 nchan(numChannels), nsam(maxPoints),
							 ssb(numChannels*data_size),
							 stride(1), startoff(0)
{
	createParam(PS_NCHAN,               asynParamInt32,         	&P_NCHAN);
	createParam(PS_NSAM,                asynParamInt32,         	&P_NSAM);
	createParam(PS_CHANNEL,             asynParamInt16Array,        &P_CHANNEL);
	createParam(PS_REFRESH,				asynParamInt32,             &P_REFRESH);

	setIntegerParam(P_NCHAN, 			nchan);
	setIntegerParam(P_NSAM, 			nsam);
	/*
	// Create parameters for each channel
	for (int ii = 0; ii < numChannels; ii++) {
		char paramName[20];
		sprintf(paramName, "CH_%02d", ii);
		createParam(paramName, asynParamFloat64, &channelParams[i]);
	}
	*/

	printf("P_NCHAN:%d P_NSAM:%d P_CHANNEL:%d P_REFRESH:%d\n", P_NCHAN, P_NSAM, P_CHANNEL, P_REFRESH);

	char datafile[128];
	sprintf(datafile, "%s/%s", getenv("HOME"), portName);

	fp = fopen(datafile, "r");
	if (fp == 0){
		perror(datafile);
		exit(errno);
	}
	data_len = GetFileSize(datafile);
	printf("file: %s data_len %lu\n", datafile, data_len);


	data_len -= data_len%(ssb);

	RAW = (epicsInt16*)mmap(0, data_len, PROT_READ, MAP_SHARED, fileno(fp), 0);

	printf("RAW:%p\n", RAW);

	CHANNELS = new CTYPE* [nchan];
	for (unsigned ic = 0; ic < nchan; ++ic){
		CHANNELS[ic] = new CTYPE [maxPoints];
		for (unsigned isam = 0; isam < nsam; ++isam){
			CHANNELS[ic][isam] = (ic+1)*0.1;
		}
	}
}

MultiChannelScope::~MultiChannelScope() {
	munmap(RAW, data_len);
	fclose(fp);
}
/* Configuration routine.  Called directly, or from the iocsh function below */


asynStatus MultiChannelScope::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
	asynStatus status = asynSuccess;
    int function = pasynUser->reason;
    const char *paramName;
    const char* functionName = "writeInt32";

    /* Set the parameter in the parameter library. */
    status = (asynStatus) setIntegerParam(function, value);

    /* Fetch the parameter string name for possible use in debugging */
    getParamName(function, &paramName);

    if (function == P_REFRESH){
    	get_data();
    }

    /* Do callbacks so higher layers see any changes */
    status = (asynStatus) callParamCallbacks();

    if (status)
        epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                  "%s:%s: status=%d, function=%d, name=%s, value=%d",
				  __FUNCTION__, functionName, status, function, paramName, value);
    else
        asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
              "%s:%s: function=%d, name=%s, value=%d\n",
			  __FUNCTION__, functionName, function, paramName, value);
    return status;
}

asynStatus MultiChannelScope::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
	asynStatus status = asynSuccess;
	return status;
}
asynStatus MultiChannelScope::readFloat64Array(asynUser *pasynUser, epicsFloat64 *value,
                                    size_t nElements, size_t *nIn)
{
	asynStatus status = asynSuccess;
	return status;
}
extern "C" {

	/** EPICS iocsh callable function to call constructor for the testAsynPortDriver class.
	  * \param[in] portName The name of the asyn port driver to be created.
	  * \param[in] maxPoints The maximum  number of points in the volt and time arrays */
	int multiChannelScopeConfigure(const char *portName, int nchan, int maxPoints, unsigned data_size)
	{
		//return MultiChannelScope::factory(portName, nchan, maxPoints, data_size);
		printf("%s, %s, %d, %d %d\n", __FUNCTION__, portName, nchan, maxPoints, data_size);
		new MultiChannelScope(portName, nchan, maxPoints, data_size);

		return 0;
	}

	/* EPICS iocsh shell commands */

	static const iocshArg initArg0 = { "uut", iocshArgString };
	static const iocshArg initArg1 = { "max chan", iocshArgInt };
	static const iocshArg initArg2 = { "max points", iocshArgInt };
	static const iocshArg initArg3 = { "data size", iocshArgInt };
	static const iocshArg * const initArgs[] = { &initArg0, &initArg1, &initArg2, &initArg3 };
	static const iocshFuncDef initFuncDef = { "multiChannelScopeConfigure", 4, initArgs };
	static void initCallFunc(const iocshArgBuf *args)
	{
		multiChannelScopeConfigure(args[0].sval, args[1].ival, args[2].ival, args[3].ival);
	}

	void multiChannelScopeRegister(void)
	{
	    iocshRegister(&initFuncDef,initCallFunc);
	}

	epicsExportRegistrar(multiChannelScopeRegister);
}
