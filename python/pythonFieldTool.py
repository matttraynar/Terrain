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
        self.ui.pushButton.clicked.connect(self.loadOrtho)
        self.ui.pushButton_2.clicked.connect(self.quitWindow)
        self.ui.pushButton_3.clicked.connect(self.loadTexture)
        self.ui.pushButton_4.clicked.connect(self.load3D)
        self.centerWindow()        
        
        img = QtGui.QPixmap("E:/mattt/Documents/Uni/FMP/Terrain/ui/tmpImage.png")
    
        self.ui.label.resize(img.size())
        self.resize(img.width() + 300, img.height() + 75)       
                
        self.setMinimumWidth(self.width())
        self.setMinimumHeight(self.height())
        
        self.setMaximumWidth(self.width())
        self.setMaximumHeight(self.height())
        
        self.centerWindow()        
                
        scaled_img = img.scaled(self.ui.label.size(), QtCore.Qt.KeepAspectRatio)
        self.ui.label.setPixmap(scaled_img)
        self.update()
                    
    def loadImage(self, img):        
        self.ui.label.setEnabled(True)
                
        scaled_img = img.scaled(self.ui.label.size(), QtCore.Qt.KeepAspectRatio)
        self.ui.label.setPixmap(scaled_img)
        self.update()
        
    def loadOrtho(self):          
        # filename = QtGui.QFileDialog.getOpenFileName(self, 'Open Image', '/', 'Image Files (*.png)')
        
        # print(filename[0])
        # img = QtGui.QPixmap(filename[0])
        
        # print("\nStarting terrain process\n");

        # print(subprocess.check_output("E:\mattt\Documents\Uni\FMP\Terrain\debug\Terrain.exe"))

        # print("Finished")
        
        img = QtGui.QPixmap('E:/mattt/Documents/Uni/FMP/Terrain/debug/orthoImage.png')
        
        self.loadImage(img)
        
        # self.ui.label.setEnabled(True)
                
        # scaled_img = img.scaled(self.ui.label.size(), QtCore.Qt.KeepAspectRatio)
        # self.ui.label.setPixmap(scaled_img)
        # self.update()
        
    def loadTexture(self):
        img = QtGui.QPixmap('E:/mattt/Documents/Uni/FMP/Terrain/debug/terrainTexture.png')
        
        self.loadImage(img)
        
        
    def load3D(self):
        img = QtGui.QPixmap('E:/mattt/Documents/Uni/FMP/Terrain/debug/perspImage.png')
        
        self.loadImage(img)
        
        
        
    def quitWindow(self):
        print 'Quit Window'
        self.close()
        
    def centerWindow(self):                
        screenRes = QtGui.QDesktopWidget().screenGeometry()
        self.move((screenRes.width() / 2.0) - (self.width() / 2.0), (screenRes.height() / 2.0) - (self.height() / 2.0))
        
        
myWin = ControlMainWindow(parent=maya_main_window())
myWin.show()