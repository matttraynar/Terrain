import os, sys
    
cmd_subfolder = os.environ['MAYA_SCRIPT_PATH'].split(';')[2] + "/FieldTool"

if cmd_subfolder not in sys.path:
     sys.path.insert(0, cmd_subfolder)
     
import pythonFieldTool as fieldTool
reload(fieldTool)
                         
global UI

try:
    UI.close()
except: 
    pass

UI = fieldTool.ControlMainWindow(parent = fieldTool.maya_main_window()) 

UI.show()