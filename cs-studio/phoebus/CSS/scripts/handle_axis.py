from org.csstudio.display.builder.runtime.script import PVUtil
from java.util.logging import Logger


"""Sets the y axis title"""

# Startup
logger = Logger.getLogger('handle_axis')
widget = locals()['widget']
pvs = locals()['pvs']
display = widget.topDisplayModel

# Main
value = PVUtil.getInt(pvs[0])

if value == 0:
    title = "Level (Codes)"
elif value == 1:
    title = "Volts (V)"

logger.info("y axis title = {}".format(title))
widget.setPropertyValue('y_axes[0].title', title)