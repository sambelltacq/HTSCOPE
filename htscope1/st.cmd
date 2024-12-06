dbLoadDatabase("../../dbd/htscope1.dbd")
htscope1_registerRecordDeviceDriver(pdbbase)

# Turn on asynTraceFlow and asynTraceError for global trace, i.e. no connected asynUser.
#asynSetTraceMask("", 0, 17)

multiChannelScopeConfigure("acq1102_123", 16, 100000, 2)

#dbLoadRecords("../../db/testAsynPortDriver.db","P=testAPD:,R=scope1:,PORT=testAPD,ADDR=0,TIMEOUT=1,NPOINTS=1000")
dbLoadRecords("../../db/asynRecord.db","P=acq1102_123:,R=,PORT=acq1102_123,ADDR=0,OMAX=80,IMAX=80")
dbLoadRecords("../../db/htscope1.db","UUT=acq1102_123,TIMEOUT=0")
dbLoadRecords("../../db/htscope1_ch.db","UUT=acq1102_123,CH=01,IX=0,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","UUT=acq1102_123,CH=02,IX=1,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","UUT=acq1102_123,CH=03,IX=2,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","UUT=acq1102_123,CH=04,IX=3,TIMEOUT=0,NPOINTS=100000")
asynSetTraceMask("acq1102_123",0,0xff)
#asynSetTraceIOMask("testAPD",0,0x2)
iocInit()

