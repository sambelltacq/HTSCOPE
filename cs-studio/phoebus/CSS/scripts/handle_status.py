from java.util.logging import Logger
from org.csstudio.display.builder.runtime.script import PVUtil

"""Expand status string"""

#Startup
logger = Logger.getLogger('handle_status')
widget = locals()['widget']
pvs = locals()['pvs']
display = widget.topDisplayModel

#Functions
def get_macros(widget):
    macros = widget.getPropertyValue("macros")
    extracted = {}
    for key in macros.getNames():
        extracted[key] = macros.getValue(key)
    return extracted

def expand_status(status, uuts):
    runtime, status = status[1:].split(':')
    status = status.split(',')
    out = 'Runtime {}\n'.format(runtime)

    for idx, state in enumerate(status):
        if idx >= len(uuts): continue
        cstate = state[:1]
        total = state[1:]
        uut = uuts[idx]

        out += "{} {}MB\n".format(uut, total)
    return out

#Main
status = PVUtil.getString(pvs[0])

if status[0] == '+':
    uuts = get_macros(widget)['UUTS'].split(',')
    status = expand_status(status, uuts)


widget.setPropertyValue("text", status)