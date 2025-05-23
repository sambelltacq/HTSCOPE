import time
from java.util.logging import Logger
from org.csstudio.display.builder.runtime.script import PVUtil
from org.phoebus.framework.macros import Macros

# Startup
t0 = time.time()
logger = Logger.getLogger('init_controls')
widget = locals()['widget']
pvs = locals()['pvs']
display = widget.topDisplayModel


# Functions
def get_macros(widget):
	macros = widget.getPropertyValue("macros")
	extracted = {}
	for key in macros.getNames():
		extracted[key] = macros.getValue(key)
	return extracted

def set_macros(widget, new_macros):
    macros = Macros()
    for key in new_macros:
        macros.add(key, new_macros[key])
    widget.setPropertyValue("macros", macros)
    return macros

def create_string_pv(name, value):
    timeout = 2
    dtype="VString"
    pvname = 'loc://{}<{}>("")'.format(name, dtype)
    pv = PVUtil.createPV(pvname, timeout)
    pv.write(value)
    logger.info("Create {} {}".format(pvname, value))
    return pv

def create_long_pv(name, value):
    timeout = 2
    dtype="VLong"
    pvname = 'loc://{}<{}>(0)'.format(name, dtype)
    pv = PVUtil.createPV(pvname, timeout)
    pv.write(value)
    logger.info("Create {} {}".format(pvname, value))
    return pv

def create_enum_pv(name, value):
    dtype = "VEnum"
    timeout = 2
    value = ','.join('"{}"'.format(v) for v in value)
    pvname = 'loc://{}<{}>(0, {})'.format(name, dtype, value)
    pv = PVUtil.createPV(pvname, timeout)
    logger.info("Create {}".format(pvname))
    return pv

# Main
logger.info("Start")

uuts = [PVUtil.getString(pv) for pv in pvs if len(PVUtil.getString(pv)) > 0]

macros = get_macros(display)
macros['UUTS'] = ','.join(uuts)
set_macros(display, macros)

max_uuts = 4
max_traces = 8

for idx in range(max_uuts):
    pvname = "UUT_{}".format(idx + 1)
    pvvalue = uuts[idx] if len(uuts) > idx else ''
    macros[pvname] = pvvalue
    create_string_pv(pvname, pvvalue)

for idx in range(max_traces):
    pvname = "trace_pvs_{}".format(idx + 1)
    create_string_pv(pvname, "")

    pvname= "TRACE_UUT_{}".format(idx + 1)
    create_enum_pv(pvname, uuts)

widget.setPropertyValue("macros", macros)
widget.setPropertyValue("file", "opi/ht_scope_controls.bob")
logger.info("Complete {:0.2f}s".format(time.time() - t0))

