# HTSCOPE : Interactive Scope display for HTS data store in HOST ramdisk

The plan is to have an local IOC that maps the data to WF records, and to display this data in 
cs-studio (and ultimately, Phoebus).

Data set size:
16ch x 2b x 2M x 10s = 6.4GB,  maybe 4 devices so 24GB.

We know from experience that cs-studio can display 100k points.
Let's assume that each WF record will span 100k points.

To browse the data rapidly, we have two controls:

1. Zoom  aka stride. 10s data is 2M points.
 - Z0: STRIDE=200              # SPAN=10s 2M / 200 = 100k
 - Z1: STRIDE=100              # SPAN= 5s
 - Z2: STRIDE= 50              # SPAN= 2.5s
 - Z3: STRIDE= 20              # SPAN= 1s
 - Z4: STRIDE= 10              # SPAN= 0.5s
 - Z5: STRIDE=  5              # SPAN= 0.25s
 - Z6: STRIDE=  2              # SPAN= 0.1s
 - Z7: STRIDE=  1              # SPAN= 0.05s

2. PAN  : increment/decrement DELAY
 - Moves start of data SPAN/2 to the right.


## Build

1. We assume there is already a build of EPICS BASE, ASYN, STREAMDEVICE, SNC
Peter used EPICS7 from ACQ400_ESW_TOP
https://github.com/D-TACQ/ACQ400_ESW_TOP

Not because we want to run this on a ZYNQ (well, it's an idea!), but because it saved time with a pre-cooked build environment.
This ends up with the full BASE under /use/local/epics/base, with an $EPICS_BASE pre-defined.

2. Build this project
```
mkdir PROJECTS; cd PROJECTS
git clone https://github.com/D-TACQ/HTSCOPE
cd HTSCOPE; make
```

3. Make Test data, at $HOME:
```
ramp  800000000 1 4 1 > Downloads/ramp800M-1-4-1
ramp 1600000000 1 2 1 > Downloads/ramp1600M-1-2-1
ramp  100000000 1 2 16 > Downloads/ramp100M-1-2-16
```
for best results:
```
ln -s Downloads/ramp800M-1-4-1 acq1102_123
```
4. Make a cs-studio workspace 
.. and add PROJECTS/HTSCOPE/OPI as a project

5. Generate st.cmd
```
./scripts/make_htscope_st.cmd.py --nchan=16 --data32=0 --ndata=100000 acq1102_123
```

6. Run the IOC
```
cd bin/linux-x86_64
./htscope1 ../../st.cmd
```

Now run the opi set from OPI

Press Refresh for new data
Stride and Start are effective.
For best large span results, disable fast [ODD] channels
for pictures see https://github.com/D-TACQ/HTSCOPE/releases/download/v0.0.1/HTSCOPE-concept.pdf

## Running a composite system:

### Define db
```usage: make_htscope_st.cmd.py [-h] [--output OUTPUT] [--nchan NCHAN] [--data32 DATA32] [--ndata NDATA] [--prefix PREFIX] uuts [uuts ...]

create htscope epics record definition

positional arguments:
  uuts                 uut1[, uut2...]

options:
  -h, --help           show this help message and exit
  --output, -O OUTPUT  record definition file name [st.cmd]
  --nchan NCHAN        specify number of channels (or use hapi to automate)
  --data32 DATA32      set to 1 for d32 data (or use hapi to automate)
  --ndata NDATA        number of data elements in WF
  --prefix PREFIX      prefix for PV's, default="$(hostname):$USER"
```

1. PREFIX : db macro PFX: this can be blank, but we suggest that HOST:USER: is a better choice.

```
eg
magnetics1:adam     adam's personal view (with custom PAN/ZOOM)
magnetics1:bruce    bruce's personable view (with custom PAN/ZOOM)
```

Having an IOC per user isn't really so wasteful, it lets each user control their own view.
Of course, only one user should be allowed to press "RUN/STOP" !



### Toplevel control
htscope1_main.db defines records that are common to the whole system:
```
./scripts/make_htscope_st.cmd.py --nchan=16 acq1102_001 acq1102_002

tail st.cmd
dbloadRecords("../../db/htscope1_main.db","PFX=hoy6:pgm:,UUTS=acq1102_001,acq1102_002")
```

1. $(PFX):SHOT_TIME   :: shot run length in s
2. $(PFX):UUTS        :: list of UUTs in set
3. $(PFX):RUNSTOP

Glue: we suggest a pyepics wrapper that blocks on RUNSTOP and starts/stops htstream.py as a spawnd task



