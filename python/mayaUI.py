import maya.cmds as cmds

# Start with the Window
cmds.window(title="Simple UI in Maya" )

# Add a single column layout to add controls into
cmds.columnLayout()

# Add controls to the Layout
cmds.button( label="Click me")

# Display the window
cmds.showWindow()