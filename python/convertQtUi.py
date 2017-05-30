import sys, pprint

import maya.standalone
maya.standalone.initialize()

from pysideuic import compileUi

print "Running " + sys.argv[0]

filepath = ''

if(len(sys.argv) < 2):    
    filepath = raw_input("Need a filepath argument:\n")  
else:
    filepath = sys.argv[1]
    
filepath = filepath[:(len(filepath) - 3)]

pyfile = open(filepath + ".py", 'w')

compileUi(filepath + ".ui", pyfile, False, 4,False)

pyfile.close()

