#!/usr/bin/env python3

import argparse
import sys

def print_preamble(args):
    if args.output == '-':
        args.fp = sys.stdout
    else:
        args.fp = open(args.output, "w")
    args.fp.write("# preamble\n")
    args.fp.write("""
dbLoadDatabase("../../dbd/htscope1.dbd")
htscope1_registerRecordDeviceDriver(pdbbase)

# Turn on asynTraceFlow and asynTraceError for global trace, i.e. no connected asynUser.
#asynSetTraceMask("", 0, 17)
""")



def print_uut(uut, args):
    args.fp.write(f"\n# uut {uut}\n")
    args.fp.write(f"""
multiChannelScopeConfigure("{uut}", {args.nchan}, {args.ndata}, {4 if args.data32 == 1 else 2})
    """)
    tm = "TIMEOUT=0"
    uutdb="../../db/htscope1.db"
    args.fp.write(f"""
dbLoadRecords("{uutdb}","UUT={uut},{tm}")""")
    chdb = "../../db/htscope1_ch.db"
    for ix in range(args.nchan):
        ch = f"{ix+1:02}"
        args.fp.write(f"""
dbLoadRecords("{chdb}","UUT={uut},CH={ch},IX={ix},{tm},NPOINTS={args.ndata}")""")
    args.fp.write(f"""
asynSetTraceMask("{uut}",0,0xff))
    """)

def print_postamble(args):
    args.fp.write("\n# postamble\n")
    args.fp.write("iocInit()\n")
    args.fp.write("# end\n")
    args.fp.close()

def init(args):
    if args.nchan is None:
        print("@@todo gather nchan: we need HAPI for this")
    if args.data32 is None:
        print("@@todo gather data32: we need HAPI for this")

def run_main(args):
    init(args)
    print_preamble(args)
    for uut in args.uuts:
        print_uut(uut, args)
    print_postamble(args)

def get_parser():
    parser = argparse.ArgumentParser(description="create htscope epics record definition")
    parser.add_argument('--output', '-O', default="st.cmd", help="record definition file name [st.cmd]")
    parser.add_argument('--nchan', default=None, type=int, help="specify number of channels (or use hapi to automate)")
    parser.add_argument('--data32', default=None, type=int, help="set to 1 for d32 data (or use hapi to automate)")
    parser.add_argument('--ndata',  default=100000, type=int, help="number of data elements in WF")
    parser.add_argument('uuts', nargs='+', help="uut1[, uut2...]")
    return parser

if __name__ == '__main__':
    run_main(get_parser().parse_args())
