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



