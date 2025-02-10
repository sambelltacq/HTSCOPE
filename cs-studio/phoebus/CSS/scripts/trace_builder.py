import time
from java.util.logging import Logger
from org.csstudio.display.builder.runtime.script import PVUtil

"""

Each trace input has 3 pvs:

loc://TRACE_EN_1
loc://TRACE_CHAN_1
loc://TRACE_UUT_1

When changed the full pv is updated 

loc://trace_pv_1
"""

#Startup
logger = Logger.getLogger('trace_build')
widget = locals()['widget']
pvs = locals()['pvs']
display = widget.topDisplayModel
timeout = 2

#main

trace_num = int(widget.effectiveMacros.getValue('trace_num'))

env = {
    "host": widget.effectiveMacros.getValue('HOST'),
    "user": widget.effectiveMacros.getValue('USER'),
    "uut": pvs[1].read().getValue(),
    "chan": int(pvs[2].read().getValue()),
}

enabled = int(pvs[0].read().getValue())
y_pv = "{host}:{user}:{uut}:CH:{chan:02d}".format(**env)
x_pv = "{host}:{user}:{uut}:TB".format(**env)

trace_pv = "loc://trace_pvs_{}".format(trace_num)

logger.info("{}[x]: '{}' ".format(trace_pv, x_pv))
logger.info("{}[y]: '{}' ".format(trace_pv, y_pv))

PVUtil.writePV(trace_pv, "{},{},{}".format(enabled, x_pv, y_pv), timeout)
