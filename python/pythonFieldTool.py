from PySide import QtCore, QtGui
import subprocess

import pythonUI as customUI

reload(customUI)

from shiboken import wrapInstance

import maya.OpenMayaUI as omui
 
def maya_main_window():
    main_window_ptr = omui.MQtUtil.mainWindow()
    return wrapInstance(long(main_window_ptr), QtGui.QWidget)
 
class ControlMainWindow(QtGui.QDialog): 
    def __init__(self, parent=None): 
        super(ControlMainWindow, self).__init__(parent)
        self.setWindowFlags(QtCore.Qt.Tool)
        self.ui =  customUI.Ui_Dialog()
        self.ui.setupUi(self)
        self.ui.pushButton.clicked.connect(self.loadImage)
        self.ui.pushButton_2.clicked.connect(self.quitWindow)
        self.centerWindow()
        
        img = QtGui.QPixmap("E:/mattt/Documents/Uni/FMP/Terrain/ui/tmpImage.png")
    
        self.ui.label.resize(img.size())
        self.resize(img.width() + 60, img.height() + 100)       
        
        self.ui.vLayoutWidget.setFixedWidth(img.width() + 40)
        
        self.ui.vLayoutWidget.setFixedHeight(img.height() + 40)
        
        
        self.centerWindow()
        
        scaled_img = img.scaled(self.ui.label.size(), QtCore.Qt.KeepAspectRatio)
        
        self.ui.label.setPixmap(img)
        self.update()
  
    def loadImage(self):          
        # filename = QtGui.QFileDialog.getOpenFileName(self, 'Open Image', '/', 'Image Files (*.png)')
        
        # print(filename[0])
        # img = QtGui.QPixmap(filename[0])
        
        print("\nStarting terrain process\n");

        print(subprocess.check_output("E:\mattt\Documents\Uni\FMP\Terrain\debug\Terrain.exe"))

        print("Finished")
        
        img = QtGui.QPixmap('E:/mattt/Documents/Uni/FMP/Terrain/debug/orthoImage.png')
        
        self.ui.label.setEnabled(True)
        self.ui.label.resize(img.size())
        self.resize(img.width() + 60, img.height() + 100)
        
        
        self.ui.vLayoutWidget.setFixedWidth(img.width() + 40)
        
        self.ui.vLayoutWidget.setFixedHeight(img.height() + 40)
        
        
        self.centerWindow()
        
        scaled_img = img.scaled(self.ui.label.size(), QtCore.Qt.KeepAspectRatio)
        self.ui.label.setPixmap(scaled_img)
        self.update()
        
        
    def quitWindow(self):
        print 'Quit Window'
        self.close()
        
    def centerWindow(self):                
        screenRes = QtGui.QDesktopWidget().screenGeometry()
        self.move((screenRes.width() / 2.0) - (self.width() / 2.0), (screenRes.height() / 2.0) - (self.height() / 2.0))
        
        
myWin = ControlMainWindow(parent=maya_main_window())
myWin.show()