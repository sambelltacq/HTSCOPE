# preamble
# command
#./scripts/make_htscope_st.cmd.py --nchan=16 acq1102_001 acq1102_002
dbLoadDatabase("../../dbd/htscope1.dbd")
htscope1_registerRecordDeviceDriver(pdbbase)

# Turn on asynTraceFlow and asynTraceError for global trace, i.e. no connected asynUser.
#asynSetTraceMask("", 0, 17)

# uut acq1102_001

multiChannelScopeConfigure("acq1102_001", 16, 100000, 2)
    
dbLoadRecords("../../db/htscope1.db","PFX=hoy6:pgm:,UUT=acq1102_001,TIMEOUT=0")

dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_001,CH=01,IX=0,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_001,CH=02,IX=1,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_001,CH=03,IX=2,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_001,CH=04,IX=3,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_001,CH=05,IX=4,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_001,CH=06,IX=5,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_001,CH=07,IX=6,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_001,CH=08,IX=7,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_001,CH=09,IX=8,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_001,CH=10,IX=9,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_001,CH=11,IX=10,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_001,CH=12,IX=11,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_001,CH=13,IX=12,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_001,CH=14,IX=13,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_001,CH=15,IX=14,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_001,CH=16,IX=15,TIMEOUT=0,NPOINTS=100000")
asynSetTraceMask("acq1102_001",0,0xff))
    
# uut acq1102_002

multiChannelScopeConfigure("acq1102_002", 16, 100000, 2)
    
dbLoadRecords("../../db/htscope1.db","PFX=hoy6:pgm:,UUT=acq1102_002,TIMEOUT=0")

dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_002,CH=01,IX=0,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_002,CH=02,IX=1,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_002,CH=03,IX=2,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_002,CH=04,IX=3,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_002,CH=05,IX=4,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_002,CH=06,IX=5,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_002,CH=07,IX=6,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_002,CH=08,IX=7,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_002,CH=09,IX=8,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_002,CH=10,IX=9,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_002,CH=11,IX=10,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_002,CH=12,IX=11,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_002,CH=13,IX=12,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_002,CH=14,IX=13,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_002,CH=15,IX=14,TIMEOUT=0,NPOINTS=100000")
dbLoadRecords("../../db/htscope1_ch.db","PFX=hoy6:pgm:,UUT=acq1102_002,CH=16,IX=15,TIMEOUT=0,NPOINTS=100000")
asynSetTraceMask("acq1102_002",0,0xff))
    
# postamble

dbloadRecords("../../db/htscope1_main.db","PFX=hoy6:pgm:,UUTS=acq1102_001,acq1102_002")
iocInit()
# end
