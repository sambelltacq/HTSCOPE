/*
 * MultichannelScope.cpp
 *
 *  Created on: 6 Dec 2024
 *      Author: pgm
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include <iocsh.h>
#include <epicsExport.h>

#include "MultiChannelScope.h"

#include <string>



#include <sys/stat.h>
#include <iostream>
#include <string>

long GetFileSize(const std::string& filename) {
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

#define PRDEB(n) (debug >= n) && printf

MultiChannelScope::MultiChannelScope(const char *portName, int numChannels, int maxPoints, unsigned _data_size) :
		 asynPortDriver(portName,
							 numChannels,
							 asynInt32Mask | asynFloat64Mask | asynFloat64ArrayMask | asynDrvUserMask,
							 asynInt32Mask | asynFloat64Mask | asynFloat64ArrayMask,
							 ASYN_CANBLOCK | ASYN_MULTIDEVICE,
							 1,
							 0,
							 0),
							 nchan(numChannels), nsam(maxPoints), data_size(_data_size),
							 ssb(numChannels*data_size),
							 RAW(0), fp(0), stride(1), startoff(0), EGU(0)
{
	createParam(PS_NCHAN,               asynParamInt32,         	&P_NCHAN);
	createParam(PS_NSAM,                asynParamInt32,         	&P_NSAM);
	createParam(PS_CHANNEL,             asynParamFloat64Array,      &P_CHANNEL);
	createParam(PS_TB,                  asynParamFloat64Array,      &P_TB);
	createParam(PS_REFRESH,				asynParamInt32,             &P_REFRESH);
	createParam(PS_REFRESHr,			asynParamInt32,             &P_REFRESHr);
	createParam(PS_MMAPUNMAP,			asynParamInt32,             &P_MMAPUNMAP);
	createParam(PS_MMAPUNMAPr,			asynParamInt32,             &P_MMAPUNMAPr);
	createParam(PS_FS,                  asynParamFloat64,           &P_FS);
	createParam(PS_STRIDE,              asynParamInt32,             &P_STRIDE);
	createParam(PS_DELAY,              	asynParamFloat64,           &P_DELAY);
	createParam(PS_ESLO,             	asynParamFloat64Array,      &P_ESLO);
	createParam(PS_EOFF,             	asynParamFloat64Array,      &P_EOFF);
	createParam(PS_EGU,  				asynParamInt32,             &P_EGU);
	createParam(PS_DEBUG,               asynParamInt32,             &P_DEBUG);

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





    /* Create the thread that computes the waveforms in the background */
	asynStatus status = (asynStatus)(epicsThreadCreate("MultiChannelScopeTask",
                          epicsThreadPriorityMedium,
                          epicsThreadGetStackSize(epicsThreadStackMedium),
                          (EPICSTHREADFUNC)::runTask,
                          this) == NULL);
    if (status) {
        printf("%s:%s: epicsThreadCreate failure\n", "mcScope", __FUNCTION__);
        return;
    }

    ESLO = new epicsFloat64[nchan];
    EOFF = new epicsFloat64[nchan];
    for (unsigned ic = 0; ic < nchan; ++ic){
        ESLO[ic] = 10.0/0x7fff;
        EOFF[ic] = 0;
    }
}



MultiChannelScope::~MultiChannelScope() {
	if (mmap_active){
		mmap_active = 0;
		unmap_uut_data();
	}
}

int MultiChannelScope::debug = 0;

/* Configuration routine.  Called directly, or from the iocsh function below */


void runTask(void *drvPvt)
{
	MultiChannelScope *pPvt = (MultiChannelScope *)drvPvt;
	pPvt->task();
}

void MultiChannelScope::task(void)
{
	asynStatus status = asynSuccess;

	lock();
	init_data();

	while(1){
		unlock();
		epicsThreadSleep(1);
		lock();
		if (mmap_active && refresh){
			get_tb();
			get_data();
			status = setIntegerParam(P_REFRESHr, refresh = 0);
			printf("%s %s rc %d\n", __FUNCTION__, "setIntegerParam", status);
#if 0
			if (status)
				epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
				              "%s:%s: status=%d, function=%d, name=%s, value=%d",
				               driverName, functionName, status, function, paramName, value);
			else
				asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
				              "%s:%s: function=%d, name=%s, value=%d\n",
				              driverName, functionName, function, paramName, value);
#endif
			status = callParamCallbacks();

			PRDEB(3)("%s %s rc %d\n", __FUNCTION__, "callParamCallbacks", status);
			PRDEB(2)("%s cleared refresh, callParamCallbacks() done\n", __FUNCTION__);
		}
	}
}

void MultiChannelScope::init_data() {
	CHANNELS = new CTYPE* [nchan];
	for (unsigned ic = 0; ic < nchan; ++ic){
		CHANNELS[ic] = new CTYPE [nsam];
		for (unsigned isam = 0; isam < nsam; ++isam){
			CHANNELS[ic][isam] = (ic+1)*0.1;
		}
    	doCallbacksFloat64Array(CHANNELS[ic], nsam, P_CHANNEL, ic);
	}
	TB = new TBTYPE[nsam];
	for (unsigned isam = 0; isam < nsam; ++isam){
		TB[isam] = isam;
	}
	doCallbacksFloat64Array(TB, nsam, P_TB, 0);
}

bool MultiChannelScope::mmap_uut_data() {
	char datafile[128];
	sprintf(datafile, "%s/%s", getenv("HOME"), portName);

	fp = fopen(datafile, "r");
	if (fp == 0){
		perror(datafile);
		return false;
	}
	long file_size = GetFileSize(datafile);

	data_len = file_size - file_size%ssb ;

	data_len_words   = data_len/data_size;
	data_len_samples = data_len/ssb;

	PRDEB(1)("data_len: %ld words: %ld samples %ld\n", data_len, data_len_words, data_len_samples);

	void* rc = mmap(0, data_len, PROT_READ, MAP_SHARED, fileno(fp), 0);
	if (rc == MAP_FAILED){
		RAW = 0;
		fprintf(stderr, "%s WARNING mmap fail data_len:%lu samples file_size:%ld\n", __FUNCTION__, data_len, file_size);
		return false;
	}else{
		RAW = (epicsInt16*)rc;
		PRDEB(1)("RAW:%p SUCCESS\n", RAW);
		return true;
	}
}

void MultiChannelScope::unmap_uut_data() {
	if (RAW){
		munmap(RAW, data_len);
		RAW = 0;
	}
	if (fp){
		fclose(fp);
		fp = 0;
	}
}
void MultiChannelScope::get_data() {
	double delay = 0;
	double fs = 2e6;

	asynStatus status = getDoubleParam(P_DELAY, &delay);
	if (status != asynSuccess){
		delay = 0;
		printf("ERROR: %s failed to retrieve P_DELAY, setting default %.3g\n", __FUNCTION__, delay);
	}
	status = getDoubleParam(P_FS, &fs);
	if (status != asynSuccess){
		fs = 2e6;
		printf("ERROR: %s failed to retrieve P_FS, setting default %.3g\n", __FUNCTION__, fs);
	}
	startoff = delay*fs;
	printf("%s startoff:%ld (samples) stride:%u\n", __FUNCTION__, startoff, stride);

	unsigned long start_cursor = startoff*nchan;
	int overlimit = 0;

	for (unsigned isam = 0; isam < nsam; ++isam){
		unsigned long cursor = start_cursor + isam*nchan*stride;
		if (cursor <= data_len_words+nchan*data_size){
			for (unsigned ic = 0; ic < nchan; ++ic){
				if (EGU){
					CHANNELS[ic][isam] = RAW[cursor+ic] * ESLO[ic] + EOFF[ic];
					PRDEB(2 - (ic == 0 && isam <20))("%s EGU ic:%d isam:%d cursor:%ld %04x %.4f\n", __FUNCTION__, ic, isam, cursor, RAW[cursor+ic], CHANNELS[ic][isam]);
				}else{
					CHANNELS[ic][isam] = RAW[cursor+ic];
					PRDEB(2 - (ic == 0 && isam <20))("%s RAW ic:%d isam:%d cursor:%ld %04x\n", __FUNCTION__, ic, isam, cursor, RAW[cursor+ic]);
				}
			}
		}else{
			PRDEB(1+overlimit)("cursor %lu reached limit of data at %d/%d fill with zeros (HACK) : set NORD would be better\n", cursor, isam, nsam);
			overlimit = 1;

			for (unsigned ic = 0; ic < nchan; ++ic){
				CHANNELS[ic][isam] = 0;
			}
		}
	}

	for (unsigned ic = 0; ic < nchan; ic++){
		PRDEB(4)("%s ic:%d nsam:%d P_CHANNEL:%d\n", __FUNCTION__, ic, nsam, P_CHANNEL);
		doCallbacksFloat64Array(CHANNELS[ic], nsam, P_CHANNEL, ic);
	}
	printf("%s 99 nchan:%d nsam:%d\n", __FUNCTION__, nchan, nsam);
}


void MultiChannelScope::get_tb() {
	double fs;                                   // pick a reasonable default
	int stride;
	double isi;
	double delay = 0;
	asynStatus status = (asynStatus) getDoubleParam(P_FS, &fs);
	if (status != asynSuccess){
		fs = 1e6;
		printf("ERROR: %s failed to retrieve fs, setting default %.3e\n", __FUNCTION__, fs);
	}
	status = (asynStatus) getIntegerParam(P_STRIDE, &stride);
	if (status != asynSuccess){
		stride = 1;
		printf("ERROR: %s failed to retrieve stride, setting default %d\n", __FUNCTION__, stride);
	}
	isi = (double)stride/fs;

	status = (asynStatus) getDoubleParam(P_DELAY, &delay);
	if (status != asynSuccess){
		delay = 0;
		printf("ERROR: %s failed to retrieve delay, setting default %.3e\n", __FUNCTION__, delay);
	}
	PRDEB(1)("%s create TB delay:%f isi:%.4g\n", __FUNCTION__, delay, isi);

	for (unsigned isam = 0; isam < nsam; ++isam, delay += isi){
		TB[isam] = delay;
		/*
		if (isam == 0 || (isam+1)%10000 == 0)
			printf("%s [%d] create TB delay:%f isi:%.4g\n", __FUNCTION__, isam, delay, isi);
		 */
	}
	PRDEB(1)("%s doCallbacksFloat64Array(%p, %d, %d, %d)\n", __FUNCTION__, TB, nsam, P_TB, 0);
	doCallbacksFloat64Array(TB, nsam, P_TB, 0);
}

asynStatus MultiChannelScope::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
	asynStatus status = asynSuccess;
    int function = pasynUser->reason;
    const char *paramName;
    int addr;
    const char* functionName = "writeInt32";

    status = parseAsynUser(pasynUser, &function, &addr, &paramName);
    if (status != asynSuccess) return status;

    PRDEB(1)("%s addr:%d f:%d %s v:%d\n", __FUNCTION__, function, addr, paramName, value);
    /* Set the parameter in the parameter library. */
    status = (asynStatus) setIntegerParam(addr, function, value);


    if (function == P_REFRESH){
    	refresh = 1;

    	FILE* fp = fopen("params.txt", "w");
    	reportParams(fp, 3);
    	fclose(fp);
    	status = (asynStatus) setIntegerParam(addr, P_REFRESHr, 0);
    	printf("%s %s rc %d\n", __FUNCTION__, "setIntegerParam", status);
    	status = callParamCallbacks();
    	printf("%s %s rc %d\n", __FUNCTION__, "callParamCallbacks", status);
    }else if (function == P_STRIDE){
    	stride = value;
    }else if (function == P_DEBUG){
    	debug = value;
    }else if (function == P_EGU){
    	EGU = value;
    }else if (function == P_MMAPUNMAP){
    	if (value == 1){
    		mmap_active = mmap_uut_data();
    	}else{
    		if (mmap_active){
    			mmap_active = 0;
    			unmap_uut_data();
    		}
    	}
    	status = (asynStatus) setIntegerParam(addr, P_MMAPUNMAPr, value);
    	if (status != 0) printf("ERROR %s setIntegerParam %d, %d, %d fail\n", __FUNCTION__,  addr, P_MMAPUNMAPr, value);
    }

    /* Do callbacks so higher layers see any changes */
    status = (asynStatus) callParamCallbacks(addr, addr);

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
	return asynPortDriver::writeFloat64(pasynUser, value);
}

asynStatus MultiChannelScope::writeFloat64Array(asynUser *pasynUser, epicsFloat64 *value,
                                        size_t nElements)
{
	asynStatus status = asynSuccess;
    const char *paramName;
    int addr;
    int function;
    epicsFloat64* pcal;
    status = parseAsynUser(pasynUser, &function, &addr, &paramName);
    if (status != asynSuccess) return status;

    PRDEB(1)("%s addr:%d f:%d %s\n", __FUNCTION__, addr, function, paramName);
    if (function == P_ESLO){
        pcal = ESLO;
    }else if (function == P_EOFF){
        pcal = EOFF;
    }else{
        return asynPortDriver::writeFloat64Array(pasynUser, value, nElements);
    }

    assert(nElements <= nchan);
    for (unsigned ic = 0; ic < nchan; ++ic){
    	PRDEB(2)("%s addr:%d %.4f\n", __FUNCTION__, function, value[ic]);
        pcal[ic] = value[ic];
    }
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
