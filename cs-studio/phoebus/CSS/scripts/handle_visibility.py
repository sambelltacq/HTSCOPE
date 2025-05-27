import time
from java.util.logging import Logger
from org.csstudio.display.builder.runtime.script import PVUtil

""" if uut macro not in loc://uuts hide widget """

#Startup
logger = Logger.getLogger('handle_visibility')
widget = locals()['widget']
pvs = locals()['pvs']
display = widget.topDisplayModel


def get_macros(widget):
	macros = widget.getEffectiveMacros()
	extracted = {}
	for key in macros.getNames():
		extracted[key] = macros.getValue(key)
	return extracted

uut = get_macros(widget)['UUT']
uuts = [PVUtil.getString(pv) for pv in pvs if len(PVUtil.getString(pv)) > 0]

if uut in uuts:
	visible = True
else:
	visible = False

logger.info("{} '{}' set visibility {}".format(widget.name, uut, visible))
widget.setPropertyValue('visible', visible)