from PySide import QtCore, QtGui

import subprocess
import sys

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
        
        self.ui.farmMeshButton.clicked.connect(lambda: self.search(0))
        self.ui.treeMeshButton.clicked.connect(lambda: self.search(1))
        self.ui.terrainButton.clicked.connect(lambda: self.search(2))
        self.ui.textureButton.clicked.connect(lambda: self.search(3))
        self.ui.heightmapButton.clicked.connect(lambda: self.search(4))
        self.ui.wallsButton.clicked.connect(lambda: self.search(5))
        self.ui.locationButton.clicked.connect(lambda: self.search(6))
        self.ui.defaultButton.clicked.connect(lambda: self.search(7))
        
        
        self.ui.loadWallsButton.clicked.connect(self.loadOrtho)
        self.ui.loadTextureButton.clicked.connect(self.loadTexture)
        self.ui.generateButton.clicked.connect(self.generate)
        self.ui.switch3DButton.clicked.connect(self.load3D)
        self.ui.imageButton.clicked.connect(self.setPos)
        
        self.centerWindow()      
          
        
        img = QtGui.QPixmap("E:/mattt/Documents/Uni/FMP/Terrain/ui/tmpImage.png")
                    
        scaled_img = img.scaled(self.ui.imageButton.size(), QtCore.Qt.KeepAspectRatio)
                
        newIcon = QtGui.QIcon(scaled_img)
        self.ui.imageButton.setIcon(newIcon)
        
        self.is2D = True
        self.isNotValid = False
        self.defaultLocation = ''
                
    def search(self, lineEdit):
        filepath = ''
        if len(self.defaultLocation) < 1:
            filepath = QtGui.QFileDialog.getExistingDirectory(self, "Choose a folder", ".", QtGui.QFileDialog.ShowDirsOnly)
        else:
            filepath = QtGui.QFileDialog.getExistingDirectory(self, "Choose a folder", str(self.defaultLocation), QtGui.QFileDialog.ShowDirsOnly)
        
        if len(filepath) < 1:
            return
                                    
        filetype = filepath + "/*.obj"
        files = []
        
        if lineEdit < 2:            
            try:
                files = subprocess.check_output(["ls", filetype]).split('\n')
            except subprocess.CalledProcessError as e:
                flags = QtGui.QMessageBox.StandardButton.Retry
                flags |= QtGui.QMessageBox.StandardButton.Ignore
                
                result = QtGui.QMessageBox.critical(self, "No files found", "Could not find any obj files at " + filepath, flags)
                
                if result == QtGui.QMessageBox.Retry:
                    self.search(lineEdit)
                    return
                elif result == QtGui.QMessageBox.Ignore:
                    self.isNotValid = True
                    return    
                    
                    
        if lineEdit == 0:
            self.ui.farmMeshLine.setText(filepath)            
                        
            for line in subprocess.check_output(["ls", filetype]).split('\n'):
                self.ui.farmMeshBrowser.append(line.rsplit('/', 1)[-1])
                
        elif lineEdit == 1:
            self.ui.treeMeshLine.setText(filepath)            
                        
            for line in subprocess.check_output(["ls", filetype]).split('\n'):
                self.ui.treeMeshBrowser.append(line.rsplit('/', 1)[-1])
                
        elif lineEdit == 2:
            self.ui.terrainLine.setText(filepath)
            
        elif lineEdit == 3:
            self.ui.textureLine.setText(filepath)
                    
        elif lineEdit == 4:
            self.ui.heightmapLine.setText(filepath)               
            
        elif lineEdit == 5:
            self.ui.wallsLine.setText(filepath)
            
        elif lineEdit == 6:
            self.ui.locationLine.setText(filepath)
            
        elif lineEdit == 7:
            self.ui.defaultLine.setText(filepath)
            self.defaultLocation = filepath
            
            
                    
    def loadImage(self, img): 
        if not self.ui.imageButton.isEnabled():    
            self.ui.imageButton.setEnabled(True)
                
        scaled_img = img.scaled(self.ui.imageButton.size(), QtCore.Qt.KeepAspectRatio)
                
        newIcon = QtGui.QIcon(scaled_img)
        self.ui.imageButton.setIcon(newIcon)
        
        self.update()
        
    def loadOrtho(self):  
        if not self.is2D:
            self.ui.switch3DButton.setText("Switch to 3D")
            self.is2D = True
                   
        img = QtGui.QPixmap('E:/mattt/Documents/Uni/FMP/Terrain/debug/orthoImage.png')        
        self.loadImage(img)
        
    def loadTexture(self):
        if not self.is2D:
            self.ui.switch3DButton.setText("Switch to 3D")
            self.is2D = True
            
        img = QtGui.QPixmap('E:/mattt/Documents/Uni/FMP/Terrain/debug/terrainTexture.png')        
        self.loadImage(img)
               
    
    def load3D(self):
        if self.is2D:
            img = QtGui.QPixmap('E:/mattt/Documents/Uni/FMP/Terrain/debug/perspImage.png')
        
            self.loadImage(img)
            self.ui.switch3DButton.setText("Switch to 2D")
            self.is2D = False
                        
            self.ui.imageButton.setCursor(QtCore.Qt.CursorShape.ArrowCursor)
            
        else:
            img = QtGui.QPixmap('E:/mattt/Documents/Uni/FMP/Terrain/debug/orthoImage.png')
        
            self.loadImage(img)
            self.ui.switch3DButton.setText("Switch to 3D")
            self.is2D = True  
            
            self.ui.imageButton.setCursor(QtCore.Qt.CursorShape.CrossCursor)
            
    def setPos(self):  
        if not self.ui.imageButton.isEnabled():
            return   
        elif self.ui.imageButton.cursor().shape() == QtCore.Qt.CursorShape.ArrowCursor:
            return
        
        if not self.ui.farmPosition.isChecked():
            self.ui.farmPosition.setChecked(True)
        
        
        rect = self.ui.imageButton.geometry()       
        
        xValue = (QtGui.QCursor.pos().x() - self.geometry().bottomLeft().x() - 20.0) / ((rect.bottomRight().x() + 1.0) - rect.bottomLeft().x())
        yValue = (QtGui.QCursor.pos().y() - self.geometry().topLeft().y() - 80.0) / ((rect.bottomLeft().y() + 1.0) - rect.topLeft().y())
            
        xValue *= self.ui.terrainSizeBox.value()
        yValue *= self.ui.terrainSizeBox.value()            
        
        self.ui.translateXBox.setValue(xValue)                
        self.ui.translateYBox.setValue(yValue)
        
    def generate(self):
        print("\nStarting terrain process\n");
        
        p = subprocess.Popen("E:\mattt\Documents\Uni\FMP\Terrain\debug\Terrain.exe", stdout = subprocess.PIPE, bufsize = 1)
        for line in iter(p.stdout.readline, b''):
            print line,
        
        p.stdout.close()
        p.wait()

        print("Finished")
        
        self.ui.loadTextureButton.setEnabled(True)
        self.ui.loadHeightmapButton.setEnabled(True)
        self.ui.loadWallsButton.setEnabled(True)
        self.ui.switch3DButton.setEnabled(True)
                
    def quitWindow(self):
        print 'Quit Window'
        self.close()
        
    def centerWindow(self):                
        screenRes = QtGui.QDesktopWidget().screenGeometry()
        self.move((screenRes.width() / 2.0) - (self.width() / 2.0), (screenRes.height() / 2.0) - (self.height() / 2.0))
        
    is2D = True
    isNotValid = False
    defaultLocation = ''
      
 
myWin = ControlMainWindow(parent=maya_main_window())

myWin.show()
