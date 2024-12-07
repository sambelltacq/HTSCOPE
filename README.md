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

2. PAN
 - Moves start of data SPAN/2 to the right.


Test data:
ramp  800000000 1 4 1 > Downloads/ramp800M-1-4-1
ramp 1600000000 1 2 1 > Downloads/ramp1600M-1-2-1
ramp  100000000 1 2 16 > Downloads/ramp100M-1-2-16

for best results:
ln -s Downloads/ramp800M-1-4-1 acq1102_123


Generate st.cmd
./scripts/make_htscope_st.cmd.py --nchan=16 --data32=0 --ndata=100000 acq1102_123

cd bin/linux-x86_64
./htscope1 ../../st.cmd


Now run the opi set from OPI

Press Refresh for new data
Stride and Start are effective.
For best large span results, disable fast [ODD] channels


