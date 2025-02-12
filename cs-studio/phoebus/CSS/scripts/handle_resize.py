import time
from java.util.logging import Logger
from org.csstudio.display.builder.runtime.script import PVUtil
from org.csstudio.display.builder.representation import ToolkitRepresentation

"""Resize plot to window size"""

#Startup
logger = Logger.getLogger('handle_resize')
widget = locals()['widget']
pvs = locals()['pvs']
display = widget.topDisplayModel

# Main

if PVUtil.getInt(pvs[0]) > 0:
    pane = ToolkitRepresentation.getToolkit(display).getModelRoot()
    height = int(pane.getHeight()) - 50
    width = int(pane.getWidth()) - 50
    widget.setPropertyValue("width", width)
    widget.setPropertyValue("height", height)
    logger.info("Plot resized to {}x{}".format(width, height))
