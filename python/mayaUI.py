import maya.cmds as cmds

# Function that queries the textField and prints it to the
# History pane in the script editor
def printTxtField ( fieldID ):
    print cmds.textField( fieldID, query=True, text=True)

# Define an id string for the window first
winID = 'newWindow'

# # Test to make sure that the UI isn't already active
# if cmds.window(winID, exists=True):
#     cmds.deleteUI(winID)

# # Now create a fresh UI window
# cmds.window(winID, w=500, h=1000, rtf=True)

# # Add a Layout - columnLayout stacks controls vertically
# cmds.columnLayout(cal='center')

# # Add controls into this Layout
# textField = cmds.textField(w=100)

# cmds.button(label='click me', command='printTxtField(textField)', w=100, h=30)
# cmds.button(label='quit',command='cmds.deleteUI(winID)', w=100, h=30)


# # Display the window
# cmds.showWindow()

cmds.window(title=winID)
# Test to make sure that the UI isn't already active
if cmds.window(winID, exists=True):
    cmds.deleteUI(winID)

cmds.columnLayout()
cmds.textFieldGrp('obj1', label='Name', text ="Please enter your name", w=100)
cmds.textFieldGrp('obj2', label='Address', text = "Please enter your address",w=100)

cmds.rowLayout(nc=2)
cmds.button(label="Edit", width=100, al='center')
cmds.button(label="Reset", width=100, al='center')
cmds.showWindow()