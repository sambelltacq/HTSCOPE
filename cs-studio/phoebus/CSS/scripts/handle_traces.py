import time
from java.util.logging import Logger
from org.csstudio.display.builder.runtime.script import PVUtil

"""
When a trace pv changes update the plot widget trace property
"""

#Startup
logger = Logger.getLogger('handle_traces')
widget = locals()['widget']
pvs = locals()['pvs']
display = widget.topDisplayModel
t0 = time.time()


#functions

def is_inited(widget):
    return widget.getUserData('init') != None

def set_inited(widget):
    widget.setUserData('init', True)

def is_invalid(value):
    return not len(value) == 3

def is_stale(pv):
    timestamp = float(PVUtil.getTimeInMilliseconds(pv)) / 1000
    return t0 - timestamp > 1


t0 = time.time()
for trace_num, pv in enumerate(pvs):

    value = PVUtil.getString(pv).split(',')
    
    if is_inited(widget) and is_stale(pv): continue

    if is_invalid(value): continue

    (en_pv, x_pv, y_pv) = value

    logger.info("Trace[{}][name] set '{}' ".format(trace_num, y_pv))
    logger.info("Trace[{}][y] set '{}' ".format(trace_num, y_pv))
    logger.info("Trace[{}][x] set '{}' ".format(trace_num, x_pv))
    logger.info("Trace[{}][visible] set '{}' ".format(trace_num, en_pv))

    widget.setPropertyValue("traces[{}].name".format(trace_num), y_pv)
    widget.setPropertyValue("traces[{}].y_pv".format(trace_num), y_pv)
    widget.setPropertyValue("traces[{}].x_pv".format(trace_num), x_pv)
    widget.setPropertyValue("traces[{}].visible".format(trace_num), int(en_pv))

set_inited(widget)
