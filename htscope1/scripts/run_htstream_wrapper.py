#!/usr/bin/env python3

import epics
import socket
import time

import subprocess
import shlex

def run_process_with_live_output(command):
    process = subprocess.Popen(shlex.split(command), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)

    # universal_newlines=True, bufsize=1)

    while True:
        output = process.stdout.readline()
        if output == '' and process.poll() is not None:
            break
        if output:
            print(output.strip())

    return process.poll()


# get HOST:USER for local ioc

PFX =  f'{socket.gethostname().split(".")[0]}:'

run_request = 0

def onChange(pvname=None, value=None, char_value=None, **kwargs):
    global run_request
    run_request = value

print(f'PFX {PFX}')

RUNSTOP = epics.get_pv(f'{PFX}RUNSTOP', callback=onChange)
UUTS = epics.get_pv(f'{PFX}UUTS')
SHOT_TIME = epics.get_pv(f'{PFX}SHOT_TIME')
STATUS = epics.get_pv(f'{PFX}STATUS')


print(f'RUNSTOP: {RUNSTOP.get()} UUTS: {UUTS.get()} SHOT_TIME: {SHOT_TIME.get()}')

loopcount = 0
shot = 0

while True:
    if run_request == 1:
        shot += 1
        STATUS.put(f'run_request shot {shot}')
        print(f'run_request shot {shot}')
        uuts = ' '.join(UUTS.get().split(','))
        secs = int(SHOT_TIME.get())
        job = f'unbuffer ./scripts/ht_stream.py --concat=999999 --secs={secs} {uuts}'
        STATUS.put(f'run job {shot}')
        print(f'run job {shot} {job}')
        rc = run_process_with_live_output(job)
        STATUS.put(f'htstream complete rc {rc}')
        print(f'htstream complete rc {rc}')
        RUNSTOP.put(0)

    time.sleep(0.1)
    loopcount += 1
    if loopcount%10 == 1:
        print(f'waiting')

