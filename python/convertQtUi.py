import sys, pprint

import maya.standalone
maya.standalone.initialize()

from pysideuic import compileUi

#Print what we're running
print "Running " + sys.argv[0]

filepath = ''

#Check for a passed in filepath or ask for one
if(len(sys.argv) < 2):    
    filepath = raw_input("Need a filepath argument:\n")  
else:
    filepath = sys.argv[1]

#Print
print "Filepath: " + filepath

#Remove the file extension
filepath = filepath[:(len(filepath) - 3)]

#Open a new py file to put stuff in
pyfile = open(filepath + ".py", 'w')

#Use pyside to compile the ui
compileUi(filepath + ".ui", pyfile, False, 4,False)

#Close the file
pyfile.close()

#Now open it for reading
f = open(filepath + ".py", 'r')

#Get the file lines
lines = f.readlines()

#Close the file
f.close()

#Open again, this time for writing
f = open(filepath + ".py", 'w')


#Compile adds 'import <name_of_resource>_rc' to the end of the pyfile
#so iterate through the lines and only store the ones without the 
#resource in them
for line in lines:
    if "_rc\n" not in line:
        f.write(line)

#Finally close
f.close()
