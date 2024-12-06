#!../../bin/linux-x86_64/htscope1

#- You may have to change htscope1 to something else
#- everywhere it appears in this file

< envPaths

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/htscope1.dbd"
htscope1_registerRecordDeviceDriver pdbbase

## Load record instances
dbLoadTemplate "db/user.substitutions"
dbLoadRecords "db/htscope1Version.db", "user=pgm"
dbLoadRecords "db/dbSubExample.db", "user=pgm"

#- Set this to see messages from mySub
#-var mySubDebug 1

#- Run this to trace the stages of iocInit
#-traceIocInit

cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs
#seq sncExample, "user=pgm"
