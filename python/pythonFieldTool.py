from PySide import QtCore, QtGui

import maya.mel as mel

import os
import os.path

import subprocess
import sys


import pythonUI as customUI
import importUI as importUI

reload(customUI)
reload(importUI)

from shiboken import wrapInstance

import maya.OpenMayaUI as omui
 
def maya_main_window():
    main_window_ptr = omui.MQtUtil.mainWindow()
    return wrapInstance(long(main_window_ptr), QtGui.QWidget)

class importWindow(QtGui.QDialog):
    def __init__(self, parent=None, default = '', terrainPath = '', heightmapPath = '', texturePath = '', wallsPath = '', treesPath = '', terrainSize = 50): 
        super(importWindow, self).__init__(parent)
        self.setWindowFlags(QtCore.Qt.Tool)
        self.ui =  importUI.Ui_Dialog()
        self.ui.setupUi(self)  
        
        self.ui.terrainButton.clicked.connect(lambda: self.search(0))
        self.ui.heightmapButton.clicked.connect(lambda: self.search(1))
        self.ui.textureButton.clicked.connect(lambda: self.search(2))
        self.ui.wallsButton.clicked.connect(lambda: self.search(3))
        
        self.ui.terrainBox.toggled.connect(self.switchGroupBox) 
        self.ui.heightmapBox.toggled.connect(self.switchGroupBox)       
        
        self.ui.importButton.clicked.connect(self.importToMaya)
        self.ui.cancelButton.clicked.connect(self.close)
        
        if len(terrainPath) > 0:
            self.ui.terrainLine.setText(terrainPath + "/Terrain.obj")
            
        if len(heightmapPath) > 0:
            self.ui.heightmapLine.setText(heightmapPath + "/heightmap.png")
            
        if len(texturePath) > 0:
            self.ui.textureLine.setText(texturePath + "/terrainTexture.png")
            
        if len(wallsPath) > 0:
            self.ui.wallsLine.setText(wallsPath)       
            
            filetype = wallsPath + "/region*"  
                        
            for line in subprocess.check_output(["ls", filetype]).split('\n'):
                filename = line.rsplit('/', 1)[-1]
                
                if len(filename) > 1:
                    self.ui.wallsEdit.appendPlainText(filename)
            
        if len(treesPath) > 0:
            self.ui.treesLine.setText(treesPath)
            
            
            filetype = treesPath + "/*obj"  
                        
            for line in subprocess.check_output(["ls", filetype]).split('\n'):
                filename = line.rsplit('/', 1)[-1]
                
                if len(filename) > 1:
                    self.ui.treesEdit.appendPlainText(filename)
                    
        else:
            self.ui.treesBox.setVisible(False)
            
            windowRect = self.geometry()
            windowRect.setHeight(self.minimumHeight())
            self.setGeometry(windowRect) 
            
            settingsRect = self.ui.importSettings.geometry()               
            settingsRect.setHeight(self.ui.importSettings.minimumHeight())
            self.ui.importSettings.setGeometry(settingsRect)
            
            screenRes = QtGui.QDesktopWidget().screenGeometry()
            self.move((screenRes.width() / 2.0) - (self.width() / 2.0), (screenRes.height() / 2.0) - (self.height() / 2.0))
            
        self.defaultLocation = default
        self.terrainChecked = True
        self.terrainSize = terrainSize
        
        if not cmds.contextInfo('heightmapTool', exists = True):
            cmds.artPuttyCtx('heightmapTool')

        
    def switchGroupBox(self):
        self.terrainChecked = not self.terrainChecked
        self.ui.terrainBox.setChecked(self.terrainChecked)
        self.ui.heightmapBox.setChecked(not self.terrainChecked)
        
    def importToMaya(self):        
        if self.terrainChecked:
            if len(str(self.ui.terrainLine.text())) < 1:
                flags = QtGui.QMessageBox.StandardButton.Ok
                
                result = QtGui.QMessageBox.warning(self, "No Terrain Path", "File path for the terrain not given", flags)
                    
                if result == QtGui.QMessageBox.Ok:
                    return
            else:                
                filename = cmds.file(self.ui.terrainLine.text(), i=True, ns="Terrain", mnc=True)
                
        else:
            if len(str(self.ui.heightmapLine.text())) < 1:
                flags = QtGui.QMessageBox.StandardButton.Ok
                
                result = QtGui.QMessageBox.warning(self, "No Heightmap Path", "File path for the heightmap not given", flags)
                    
                if result == QtGui.QMessageBox.Ok:
                    return
            else:
                if cmds.objExists("HeightmapTerrain"):
                    cmds.select("HeightmapTerrain")
                    cmds.delete()
                
                cmds.polyPlane(n="HeightmapTerrain", w=self.terrainSize, h=self.terrainSize, sx = 100, sy = 100)
                cmds.select("HeightmapTerrain")    
                
                cmds.setToolTo('heightmapTool')
                cmds.artPuttyCtx('heightmapTool', md = 20, e = True)
                cmds.artPuttyCtx('heightmapTool', ifl = self.ui.heightmapLine.text(), e = True)

                cmds.setToolTo('selectSuperContext')
                cmds.setAttr("HeightmapTerrain.scaleY", -1)
                cmds.polyNormal(nm = 0)
                
                cmds.select("HeightmapTerrain.f[9900:9999]")
                cmds.delete()
                cmds.select("HeightmapTerrain")
                cmds.polySetToFaceNormal()
                cmds.polySoftEdge(a = 180)
                
                cmds.select(cl = True)
                
        if self.ui.textureBox.isChecked():
            if not cmds.objExists("TerrainShader"):
                lambertNode = cmds.shadingNode("lambert", asShader = True, n = "TerrainShader")
                fileNode = cmds.shadingNode("file", asTexture = True, n = "TerrainTexture", icm = True)
                shadingGrp = cmds.sets(renderable = True, noSurfaceShader = True, empty = True, n = "TerrainShadingGroup")
                            
                cmds.setAttr("TerrainTexture.fileTextureName", self.ui.textureLine.text(), type = "string")
                cmds.connectAttr("%s.outColor" %lambertNode, "%s.surfaceShader" %shadingGrp)
                cmds.connectAttr("%s.outColor" %fileNode, "%s.color" %lambertNode)
            
            else:
                cmds.setAttr("TerrainTexture.fileTextureName", self.ui.textureLine.text(), type = "string")
            
            if self.terrainChecked:
                cmds.select("Terrain:*")
            else:
                cmds.select("HeightmapTerrain")
                        
                        
            cmds.sets(fe = "TerrainShadingGroup", e = True) 
            
        if self.ui.wallsBox.isChecked():
            
            filepath = ''
            
            if len(self.ui.wallsEdit.toPlainText()) > 1:
                filepath = self.ui.wallsLine.text() + "/"
                for c in self.ui.wallsEdit.toPlainText():                
                    if c == '\n':
                        cmds.file(filepath, i=True, ns="Walls", mnc=True)
                        filepath = self.ui.wallsLine.text() + "/"
                    else:
                        filepath += c
            
                                              
        self.close()
        
    def search(self, lineEdit):
        filepath = ''
        filetype = ''
        if lineEdit is 0:
            if len(self.defaultLocation) < 1 and len(self.ui.terrainLine.text()) < 1:
                filepath = QtGui.QFileDialog.getOpenFileName(self, "Choose a file", ".", "Geometry files (*.obj)")
            elif len(self.ui.terrainLine.text()) > 0:
                filepath = QtGui.QFileDialog.getOpenFileName(self, "Choose a file", self.ui.terrainLine.text(), "Geometry files (*.obj)")
            else:
                filepath = QtGui.QFileDialog.getOpenFileName(self, "Choose a file", str(self.defaultLocation), "Geometry files (*.obj)")
                
        elif lineEdit is 1:
            if len(self.defaultLocation) < 1 and len(self.ui.heightmapLine.text()) < 1:
                filepath = QtGui.QFileDialog.getOpenFileName(self, "Choose a file", ".", "Image files (*.png *.jpg)")
            elif len(self.ui.heightmapLine.text()) > 0:
                filepath = QtGui.QFileDialog.getOpenFileName(self, "Choose a file", self.ui.heightmapLine.text(), "Image files (*.png *.jpg)")
            else:
                filepath = QtGui.QFileDialog.getOpenFileName(self, "Choose a file", str(self.defaultLocation), "Image files (*.png *.jpg)")
                
        elif lineEdit is 2:
            if len(self.defaultLocation) < 1 and len(self.ui.textureLine.text()) < 1:
                filepath = QtGui.QFileDialog.getOpenFileName(self, "Choose a file", ".", "Image files (*.png *.jpg)")
            elif len(self.ui.textureLine.text()) > 0:
                filepath = QtGui.QFileDialog.getOpenFileName(self, "Choose a file", self.ui.textureLine.text(), "Image files (*.png *.jpg)")
            else:
                filepath = QtGui.QFileDialog.getOpenFileName(self, "Choose a file", str(self.defaultLocation), "Image files (*.png *.jpg)")
                
        elif lineEdit is 3:
            if len(self.defaultLocation) < 1 and len(self.ui.wallsLine.text()) < 1:
                filepath = QtGui.QFileDialog.getExistingDirectory(self, "Choose a folder", ".", QtGui.QFileDialog.ShowDirsOnly)
            elif len(self.ui.wallsLine.text()) > 0:
                filepath = QtGui.QFileDialog.getExistingDirectory(self, "Choose a folder", self.ui.wallsLine.text(), QtGui.QFileDialog.ShowDirsOnly)
            else:
                filepath = QtGui.QFileDialog.getExistingDirectory(self, "Choose a folder", str(self.defaultLocation), QtGui.QFileDialog.ShowDirsOnly)
            
            if len(filepath) < 1:
                return
                                        
            filetype = filepath + "/region*"
            files = []
                       
            try:
                files = subprocess.check_output(["ls", filetype]).split('\n')
            except subprocess.CalledProcessError as e:
                flags = QtGui.QMessageBox.StandardButton.Retry
                flags |= QtGui.QMessageBox.StandardButton.Cancel
                
                result = QtGui.QMessageBox.critical(self, "No files found", "Could not find any obj files at " + filepath, flags)
                
                if result == QtGui.QMessageBox.Retry:
                    self.search(lineEdit)
                    return
                elif result == QtGui.QMessageBox.Cancel:
                    return   
            
            
        if len(filepath) < 1:
            return                                    
                    
        if lineEdit is 0:
            self.ui.terrainLine.setText(filepath[0])       
                                
        elif lineEdit is 1:
            self.ui.heightmapLine.setText(filepath[0])      
                
        elif lineEdit is 2:
            self.ui.textureLine.setText(filepath[0])
            
        elif lineEdit is 3:  
            self.ui.wallsEdit.clear()
            self.ui.wallsLine.setText(filepath)            
                        
            for line in subprocess.check_output(["ls", filetype]).split('\n'):
                filename = line.rsplit('/', 1)[-1]
                
                if len(filename) > 1:
                    self.ui.wallsEdit.appendPlainText(filename)
            
    defaultLocation = ''
    terrainChecked = True
    terrainSize = 50
            

class ControlMainWindow(QtGui.QDialog): 
    def __init__(self, parent=None): 
        super(ControlMainWindow, self).__init__(parent)
        self.setWindowFlags(QtCore.Qt.Tool)
        self.ui =  customUI.Ui_Dialog()
        self.ui.setupUi(self)
        
        self.ui.slider3D.setVisible(False)
        self.ui.slider3D.valueChanged.connect(self.change3D)
        
        self.ui.loadSettingsButton.clicked.connect(self.loadSettings)
        self.ui.resetSettingsButton.clicked.connect(self.resetSettings)
        self.ui.saveSettingsButton.clicked.connect(lambda: self.saveSettings(False))
        
        self.ui.farmMeshButton.clicked.connect(lambda: self.search(0))
        self.ui.treeMeshButton.clicked.connect(lambda: self.search(1))
        self.ui.terrainButton.clicked.connect(lambda: self.search(2))
        self.ui.textureButton.clicked.connect(lambda: self.search(3))
        self.ui.heightmapButton.clicked.connect(lambda: self.search(4))
        self.ui.wallsButton.clicked.connect(lambda: self.search(5))
        self.ui.locationButton.clicked.connect(lambda: self.search(6))
        self.ui.defaultButton.clicked.connect(lambda: self.search(7))
        self.ui.pushButton.clicked.connect(lambda: self.search(8))
        self.ui.pushButton_2.clicked.connect(lambda: self.search(9))  
        
        self.ui.postGenTerrain.clicked.connect(lambda: self.moveFile(0))
        self.ui.postGenTexture.clicked.connect(lambda: self.moveFile(1))
        self.ui.postGenHeightmap.clicked.connect(lambda: self.moveFile(2))
        self.ui.postGenWalls.clicked.connect(lambda: self.moveFile(3))

        self.ui.importButton.clicked.connect(self.createImportWindow)      
        
        self.ui.loadWallsButton.clicked.connect(self.loadOrtho)
        self.ui.loadTextureButton.clicked.connect(self.loadTexture)
        self.ui.loadHeightmapButton.clicked.connect(self.loadHeightmap)
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
        self.workingDir = customUI.__file__.rsplit('\\',1)[0]
        self.execDir = 'E:/mattt/Documents/Uni/FMP/Terrain/debug'
        self.notFirstClose = False
    
    def closeEvent(self, event):
        if self.notFirstClose:
            subprocess.call(["rm", "-f", self.execDir + "/Output/*"])
        else:
            self.notFirstClose = True

        event.accept()
        
    def moveFile(self, file):
        if file is 0:
            if len(self.ui.terrainLine.text()) < 1:
                flags = QtGui.QMessageBox.StandardButton.Ok
            
                result = QtGui.QMessageBox.warning(self, "No Export Path", "File path to export terrain not found", flags)
                    
                if result == QtGui.QMessageBox.Ok:
                    return 
            
            if os.path.exists(self.ui.terrainLine.text() + "/Terrain.obj"):
                flags = QtGui.QMessageBox.StandardButton.Ok
                flags |= QtGui.QMessageBox.StandardButton.Cancel
                
                result = QtGui.QMessageBox.warning(self, "File Exists", "A terrain obj already exists at " + self.ui.terrainLine.text() + "\nThis will replace it", flags)
                    
                if result == QtGui.QMessageBox.Cancel:
                    return 
                elif result == QtGui.QMessageBox.Ok:
                    subprocess.call(["rm", "-f", self.ui.terrainLine.text() + "/Terrain.obj"])
            
            os.rename(self.execDir + "/Output/Terrain.obj", self.ui.terrainLine.text() + "/Terrain.obj")
        
        elif file is 1:
            if len(self.ui.textureLine.text()) < 1:
                flags = QtGui.QMessageBox.StandardButton.Ok
            
                result = QtGui.QMessageBox.warning(self, "No Export Path", "File path to export texture not found", flags)
                    
                if result == QtGui.QMessageBox.Ok:
                    return 
            
            if os.path.exists(self.ui.textureLine.text() + "/terrainTexture.png"):
                flags = QtGui.QMessageBox.StandardButton.Ok
                flags |= QtGui.QMessageBox.StandardButton.Cancel
                
                result = QtGui.QMessageBox.warning(self, "File Exists", "A texture already exists at " + self.ui.textureLine.text() + "\nThis will replace it", flags)
                    
                if result == QtGui.QMessageBox.Cancel:
                    return 
                elif result == QtGui.QMessageBox.Ok:
                    subprocess.call(["rm", "-f", self.ui.textureLine.text() + "/terrainTexture.png"])
                    
            os.rename(self.execDir + "/Output/terrainTexture.png", self.ui.textureLine.text() + "/terrainTexture.png")
            
            self.ui.loadTextureButton.setEnabled(True)
       
        elif file is 2:
            if len(self.ui.heightmapLine.text()) < 1:
                flags = QtGui.QMessageBox.StandardButton.Ok
            
                result = QtGui.QMessageBox.warning(self, "No Export Path", "File path to export heightmap not found", flags)
                    
                if result == QtGui.QMessageBox.Ok:
                    return 
            
            if os.path.exists(self.ui.heightmapLine.text() + "/heightmap.png"):
                flags = QtGui.QMessageBox.StandardButton.Ok
                flags |= QtGui.QMessageBox.StandardButton.Cancel
                
                result = QtGui.QMessageBox.warning(self, "File Exists", "A heightmap already exists at " + self.ui.heightmapLine.text() + "\nThis will replace it", flags)
                    
                if result == QtGui.QMessageBox.Cancel:
                    return 
                elif result == QtGui.QMessageBox.Ok:
                    subprocess.call(["rm", "-f", self.ui.heightmapLine.text() + "/heightmap.png"])
              
            os.rename(self.execDir + "/Output/heightmap.png", self.ui.heightmapLine.text() + "/heightmap.png")
         
            self.ui.loadHeightmapButton.setEnabled(True)
         
        elif file is 3:
            if len(self.ui.wallsLine.text()) < 1:
                flags = QtGui.QMessageBox.StandardButton.Ok
            
                result = QtGui.QMessageBox.warning(self, "No Export Path", "File path to export heightmap not found", flags)
                    
                if result == QtGui.QMessageBox.Ok:
                    return 
            
            if os.path.exists(self.ui.wallsLine.text() + "/orthoImage.png"):
                flags = QtGui.QMessageBox.StandardButton.Ok
                flags |= QtGui.QMessageBox.StandardButton.Cancel
                
                result = QtGui.QMessageBox.warning(self, "File Exists", "Some walls files already exist at " + self.ui.wallsLine.text() + "\nThis will replace them", flags)
                    
                if result == QtGui.QMessageBox.Cancel:
                    return 
                elif result == QtGui.QMessageBox.Ok:
                    subprocess.call(["rm", "-f", self.ui.wallsLine.text() + "/orthoImage.png"])
                    subprocess.call(["rm", "-f", self.ui.wallsLine.text() + "/region*"])
                    
            os.rename(self.execDir + "/Output/orthoImage.png", self.ui.wallsLine.text() + "/orthoImage.png")
            
            for line in subprocess.check_output(["ls", self.execDir + "/Output/region*"]).split('\n'):
                filename = line.rsplit('/', 1)[-1]
                
                if len(filename) > 1:          
                    os.rename(line, self.ui.wallsLine.text() + "/" + filename)
            
            self.ui.loadWallsButton.setEnabled(True)
            
        
    def createImportWindow(self):
        importer = importWindow(self, self.defaultLocation, self.ui.terrainLine.text(), self.ui.heightmapLine.text(), self.ui.textureLine.text(), self.ui.wallsLine.text(), self.ui.treeMeshLine.text(), self.ui.terrainSizeBox.value())
        importer.show()

    def loadSettings(self):        
        if len(self.ui.lineEdit.text()) < 1:
            flags = QtGui.QMessageBox.StandardButton.Ok
            
            result = QtGui.QMessageBox.warning(self, "No Load Path", "File path to load settings not found", flags)
                
            if result == QtGui.QMessageBox.Ok:
                return 
          
        filepath = self.ui.lineEdit.text()
                
        if not os.path.exists(filepath + "/fieldSettings.txt"):
            flags = QtGui.QMessageBox.StandardButton.Ok
            
            result = QtGui.QMessageBox.warning(self, "No Settings Found", "No settings file found at " + filepath, flags)
                
            if result == QtGui.QMessageBox.Ok:
                return    
                               
        filepath = str(filepath)
        
        self.resetSettings()
        self.ui.lineEdit.setText(filepath)
        
        f = open(filepath + "/fieldSettings.txt", 'r')     
           
        value = int(f.readline())
        
        if value > -1:            
            self.ui.seedValueCheck.setChecked(True)
            self.ui.seedValueBox.setValue(value)
        else:
            self.ui.seedValueCheck.setChecked(False)
            
        self.ui.terrainSizeBox.setValue(int(f.readline()))
        self.ui.startPointSizeBox.setValue(int(f.readline()))
        self.ui.roughnessBox.setValue(float(f.readline()))
        self.ui.heightmapIterationsBox.setValue(int(f.readline()))
                    
        f.readline()
        
        if f.readline() == "True\n":
            self.ui.farmPosition.setChecked(True)
        else:
            self.ui.farmPosition.setChecked(False)
        
        if self.ui.farmPosition.isChecked():
            self.ui.translateXBox.setValue(float(f.readline()))
            self.ui.translateYBox.setValue(float(f.readline()))
            self.ui.translateZBox.setValue(float(f.readline()))
            
            f.readline()
            
            line = f.readline()
            self.ui.farmMeshLine.setText(line.rsplit('/',1)[0])
            
            while line.find('#') is  -1:  
                modelName = line.rsplit('/', 1)[-1].rstrip()
                self.ui.farmMeshBrowser.appendPlainText(modelName)
                line = f.readline()
            
        f.readline()
        
        if f.readline() == "True\n":
            self.ui.trees.setChecked(True)
        else:
            self.ui.trees.setChecked(False)
                    
        if self.ui.trees.isChecked():
            if f.readline() == "True\n":
                self.ui.normalTreesCheck.setChecked(True)
            else:
                self.ui.normalTreesCheck.setChecked(False)
                                        
            if f.readline() == "True\n":
                self.ui.clusterTreesCheck.setChecked(True)
            else:
                self.ui.clusterTreesCheck.setChecked(False)                
            
            f.readline()
            
            line = f.readline()
            self.ui.treeMeshLine.setText(line.rsplit('/',1)[0])
            
            while line.find('#') is  -1:  
                modelName = line.rsplit('/', 1)[-1].rstrip()
                self.ui.treeMeshBrowser.appendPlainText(modelName)
                line = f.readline()
         
        f.readline()
          
        line  = f.readline().rstrip()
        
        if line.find('#') is -1:            
            self.ui.defaultLine.setText(line)
                   
        f.readline() 
         
        if f.readline() == "True\n":
            self.ui.groupBox.setChecked(True)
            self.ui.terrainLine.setText(f.readline().rstrip())            
            
            if f.readline() == "True\n":
                self.ui.triangulateCheck.setChecked(True)
            else:
                self.ui.triangulateCheck.setChecked(False)
            
        else:
            self.ui.groupBox.setChecked(False)      
                    
        f.readline()        
        
        if f.readline() == "True\n":
            self.ui.groupBox_2.setChecked(True)
            self.ui.textureLine.setText(f.readline().rstrip())
        else:
            self.ui.groupBox_2.setChecked(False)      
                    
        f.readline()
        
        if f.readline() == "True\n":
            self.ui.groupBox_3.setChecked(True)
            self.ui.heightmapLine.setText(f.readline().rstrip())
        else:
            self.ui.groupBox_3.setChecked(False)      
                    
        f.readline()
        
        if f.readline() == "True\n":
            self.ui.groupBox_4.setChecked(True)
            self.ui.wallsLine.setText(f.readline().rstrip())
        else:
            self.ui.groupBox_4.setChecked(False)      
                    
        f.readline()
        
        if f.readline() == "True\n":
            self.ui.groupBox_5.setChecked(True)
            self.ui.locationLine.setText(f.readline().rstrip())
        else:
            self.ui.groupBox_5.setChecked(False)      
                    
        f.readline()
        f.close()
         
        
    def resetSettings(self):
        #Reset cache path
        self.ui.lineEdit.setText('')
        
        #Reset terrain values
        self.ui.seedValueCheck.setChecked(False)
        self.ui.seedValueBox.setValue(0)
        self.ui.terrainSizeBox.setValue(1)
        self.ui.startPointSizeBox.setValue(1)
        self.ui.roughnessBox.setValue(0.0000)
        self.ui.heightmapIterationsBox.setValue(1)
        
        #Farm position
        self.ui.farmPosition.setChecked(False)
        self.ui.translateXBox.setValue(0.0000)
        self.ui.translateYBox.setValue(0.0000)
        self.ui.translateZBox.setValue(0.0000)
        self.ui.farmMeshLine.setText('')
        self.ui.farmMeshBrowser.clear()
        
        #Tree stuff
        self.ui.trees.setChecked(False)
        self.ui.normalTreesCheck.setChecked(True)
        self.ui.clusterTreesCheck.setChecked(True)
        self.ui.treeMeshLine.setText('')
        self.ui.treeMeshBrowser.clear()
        
        #Export
        self.defaultLocation = ''
        self.ui.defaultLine.setText('')
        
        self.ui.groupBox.setChecked(False)
        self.ui.terrainLine.setText('')
        self.ui.triangulateCheck.setChecked(False)
        
        self.ui.groupBox_2.setChecked(False)
        self.ui.textureLine.setText('')
        self.ui.loadTextureButton.setEnabled(False)
        
        self.ui.groupBox_3.setChecked(False)
        self.ui.heightmapLine.setText('')
        self.ui.loadHeightmapButton.setEnabled(False)
        
        self.ui.groupBox_4.setChecked(False)
        self.ui.wallsLine.setText('')
        self.ui.loadWallsButton.setEnabled(False)
                
        self.ui.groupBox_5.setChecked(False)
        self.ui.locationLine.setText('')
        
        self.ui.generateProgress.setEnabled(False)
        self.ui.importButton.setEnabled(False)
        self.ui.importProgress.setEnabled(False)
        
    def saveSettings(self, generateCall): 
        filepath = ''
         
        if not generateCall:            
            if len(self.ui.lineEdit_2.text()) < 1:
                flags = QtGui.QMessageBox.StandardButton.Ok
                
                result = QtGui.QMessageBox.warning(self, "No Save Path", "File path to save settings not found", flags)
                    
                if result == QtGui.QMessageBox.Ok:
                    return  
             
            filepath = self.ui.lineEdit_2.text()
                    
            if os.path.exists(filepath + "/fieldSettings.txt"):
                flags = QtGui.QMessageBox.StandardButton.Ok
                flags |= QtGui.QMessageBox.StandardButton.Cancel
                
                result = QtGui.QMessageBox.warning(self, "File Exists", "A settings file already exists at " + filepath + "\nThis will replace it", flags)
                    
                if result == QtGui.QMessageBox.Ok:
                    open(filepath + "/fieldSettings.txt", 'w').close()
                if result == QtGui.QMessageBox.Cancel:
                    return  
        else:
            filepath = self.execDir + "/Output"
            
        
        if self.hasSettings:
            os.rename(self.settingsDir + "/fieldSettings.txt", filepath + "/fieldSettings.txt")
            self.settingsDir = filepath
        else:                                
            f = open(filepath + "/fieldSettings.txt", 'w')
            
            if self.ui.seedValueCheck.isChecked():
                f.write(str(self.ui.seedValueBox.value()) + '\n')
            else:
                f.write(str(-1) + '\n')
            f.write(str(self.ui.terrainSizeBox.value()) + '\n')
            f.write(str(self.ui.startPointSizeBox.value()) + '\n')
            f.write(str(self.ui.roughnessBox.value()) + '\n')
            f.write(str(self.ui.heightmapIterationsBox.value()) + '\n')
                    
            f.write("------------------\n")
            if self.ui.farmPosition.isChecked():
                f.write(str(True) + '\n')
                f.write(str(self.ui.translateXBox.value()) + '\n')
                f.write(str(self.ui.translateYBox.value()) + '\n')
                f.write(str(self.ui.translateZBox.value()) + '\n')
                
                f.write("#\n")
                
                if len(self.ui.farmMeshBrowser.toPlainText()) > 1:
                    f.write(self.ui.farmMeshLine.text() + "/")
                    for c in self.ui.farmMeshBrowser.toPlainText():                
                        if c == '\n':
                            f.write('\n' + self.ui.farmMeshLine.text() + "/")
                        else:
                            f.write(c) 
                             
                    f.write('\n')                
                    
                f.write("#\n")
            else:
                f.write(str(False) + '\n') 
                       
            f.write("------------------\n")
                      
            if self.ui.trees.isChecked():
                f.write(str(True) + '\n')
                f.write(str(self.ui.normalTreesCheck.isChecked()) + '\n')
                f.write(str(self.ui.clusterTreesCheck.isChecked()) + '\n')
                
                f.write("#\n")
                if len(self.ui.treeMeshBrowser.toPlainText()) > 1:
                    f.write(self.ui.treeMeshLine.text() + "/")
                    for c in self.ui.treeMeshBrowser.toPlainText():                
                        if c == '\n':
                            f.write('\n' + self.ui.treeMeshLine.text() + "/")
                        else:
                            f.write(c) 
                             
                    f.write('\n') 
                    
                f.write("#\n")
            else:
                f.write(str(False) + '\n')
             
            f.write("------------------\n")
              
            if len(self.ui.defaultLine.text()) > 0:
                f.write(self.ui.defaultLine.text() + '\n')
            else:
                f.write("#\n")
                       
            f.write("------------------\n")
                        
            if self.ui.groupBox.isChecked():
                f.write(str(True) + '\n')
                
                if len(self.ui.terrainLine.text()) > 0:
                    f.write(self.ui.terrainLine.text() + '\n')
                else:
                    f.write("#\n")
                    
                f.write(str(self.ui.triangulateCheck.isChecked()) + '\n')
            else:
                f.write(str(False) + '\n')
                
            f.write("------------------\n")
            
            if self.ui.groupBox_2.isChecked():
                f.write(str(True) + '\n')
                
                if len(self.ui.textureLine.text()) > 0:
                    f.write(self.ui.textureLine.text() + '\n')
                else:
                    f.write("#\n")
            else:
                f.write(str(False) + '\n')
                                
            f.write("------------------\n")
            
            if self.ui.groupBox_3.isChecked():
                f.write(str(True) + '\n')
                if len(self.ui.heightmapLine.text()) > 0:
                    f.write(self.ui.heightmapLine.text() + '\n')
                else:
                    f.write("#\n")        
            else:
                f.write(str(False) + '\n') 
                       
            f.write("------------------\n")
            
            if self.ui.groupBox_4.isChecked():
                f.write(str(True) + '\n')
                if len(self.ui.wallsLine.text()) > 0:
                    f.write(self.ui.wallsLine.text() + '\n')
                else:
                    f.write("#\n")
            else:
                f.write(str(False) + '\n') 
                       
            f.write("------------------\n")
            
            if self.ui.groupBox_5.isChecked():
                f.write(str(True) + '\n')
                if len(self.ui.locationLine.text()) > 0:
                    f.write(self.ui.locationLine.text() + '\n')
                else:
                    f.write("#\n")        
            else:
                f.write(str(False) + '\n')
                       
            f.write("------------------\n")        
                
            f.close()
            
            self.hasSettings = True
            self.settingsDir = filepath
        
        print "Settings saved at " + filepath
                    
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
                flags |= QtGui.QMessageBox.StandardButton.Cancel
                
                result = QtGui.QMessageBox.critical(self, "No files found", "Could not find any obj files at " + filepath, flags)
                
                if result == QtGui.QMessageBox.Retry:
                    self.search(lineEdit)
                    return
                elif result == QtGui.QMessageBox.Cancel:
                    self.isNotValid = True
                    return    
                    
                    
        if lineEdit == 0:
            self.ui.farmMeshBrowser.clear()
            self.ui.farmMeshLine.setText(filepath)            
                        
            for line in subprocess.check_output(["ls", filetype]).split('\n'):
                filename = line.rsplit('/', 1)[-1]
                
                if len(filename) > 1:
                    self.ui.farmMeshBrowser.appendPlainText(filename)
                                
        elif lineEdit == 1:
            self.ui.treeMeshBrowser.clear()
            self.ui.treeMeshLine.setText(filepath)            
                        
            for line in subprocess.check_output(["ls", filetype]).split('\n'):
                filename = line.rsplit('/', 1)[-1]
                
                if len(filename) > 1:
                    self.ui.treeMeshBrowser.appendPlainText(filename)
                
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
            
        elif lineEdit == 8:
            self.ui.lineEdit.setText(filepath)
            
        elif lineEdit == 9:
            self.ui.lineEdit_2.setText(filepath)
            
            
                    
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
                   
        if len(self.ui.wallsLine.text()) > 0:
            img = QtGui.QPixmap(self.ui.wallsLine.text() + '/orthoImage.png')        
            self.loadImage(img)
        
    def loadTexture(self):
        if not self.is2D:
            self.ui.switch3DButton.setText("Switch to 3D")
            self.is2D = True
            
        if len(self.ui.textureLine.text()) > 0:
            img = QtGui.QPixmap(self.ui.textureLine.text() + '/terrainTexture.png')        
            self.loadImage(img)
               
               
    def loadHeightmap(self):
        if not self.is2D:
            self.ui.switch3DButton.setText("Switch to 3D")
            self.is2D = True
          
        if len(self.ui.heightmapLine.text()) > 0:
            img = QtGui.QPixmap(self.ui.heightmapLine.text() + '/heightmap.png')        
            self.loadImage(img)
    
    def load3D(self):
        if self.is2D:
            self.ui.imageButton.setDown(True)
            self.change3D(10)
            
            self.is2D = False
            
            self.ui.slider3D.setVisible(True)
                        
            self.ui.imageButton.setCursor(QtCore.Qt.CursorShape.ArrowCursor)
            
        else:
            if (len(self.ui.textureLine.text()) is 0) and (len(self.ui.heightmapLine.text()) is 0) and (len(self.ui.wallsLine.text()) is 0):
                flags = QtGui.QMessageBox.StandardButton.Ok

                result = QtGui.QMessageBox.warning(self, "No 2D Path", "No 2D images were exported (Texture, Heightmap, Walls)", flags)

                if result == QtGui.QMessageBox.Ok:
                    return 
            
            self.ui.imageButton.setDown(False)
            
            if len(self.ui.wallsLine.text()) > 0:                     
                img = QtGui.QPixmap(self.ui.wallsLine.text() + '/orthoImage.png')
            
                self.loadImage(img)
                self.ui.imageButton.setCursor(QtCore.Qt.CursorShape.CrossCursor)
                
            elif len(self.ui.textureLine.text()) > 0:                     
                img = QtGui.QPixmap(self.ui.textureLine.text() + '/terrainTexture.png')
            
                self.loadImage(img)
                self.ui.imageButton.setCursor(QtCore.Qt.CursorShape.CrossCursor)
                
            elif len(self.ui.heightmapLine.text()) > 0:                     
                img = QtGui.QPixmap(self.ui.heightmapLine.text() + '/heightmap.png')
            
                self.loadImage(img)
                
                
            self.ui.switch3DButton.setText("Switch to 3D")
            self.is2D = True  
            
            self.ui.slider3D.setVisible(False)
            
            
    def change3D(self, value):
        img = QtGui.QPixmap(self.execDir + '/Output/perspImage' + str(value) + '.png')
      
        self.loadImage(img)
        self.ui.switch3DButton.setText("Switch to 2D")
        self.is2D = False
        
    def setPos(self):  
        if not self.ui.imageButton.isEnabled():
            return   
        elif self.ui.imageButton.cursor().shape() == QtCore.Qt.CursorShape.ArrowCursor:
            self.ui.imageButton.setDown(True)
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
        if self.isGenerated:
            flags = QtGui.QMessageBox.StandardButton.Ok
            flags |= QtGui.QMessageBox.StandardButton.Cancel
            
            result = QtGui.QMessageBox.warning(self, "Regeneration Warning", "Doing this will regenerate the fields, do you want to proceed?", flags)
                
            if result == QtGui.QMessageBox.Cancel:
                return  
        
        print("\nStarting terrain process\n");
        
        if not self.hasSettings:
            self.saveSettings(True)
                        
        if len(self.ui.wallsLine.text()) < 1:
            subprocess.call(["rm", self.execDir + "/Output/region*"])
        else:
            subprocess.call(["rm", self.ui.wallsLine.text() + "/region*"])
            
    
        p = subprocess.Popen([self.execDir + "/Terrain.exe", self.settingsDir + "/fieldSettings.txt"], stdout = subprocess.PIPE, bufsize = 1)
        
        for line in iter(p.stdout.readline, b''):
            print line,
        
        p.stdout.close()
        p.wait()
        
        output = ''
        
        if len(self.ui.terrainLine.text()) > 1:
            output = self.ui.terrainLine.text()
            cmds.file(self.ui.terrainLine.text() + "/Terrain.obj", i=True, ns="Terrain", mnc=True)
            
            cmds.select("Terrain:*")
            cmds.polySetToFaceNormal()
            cmds.polyNormal(nm = 0, ch = 0)
            cmds.polySoftEdge(a = 180, ch = 0)
            cmds.select("Terrain:*")
        
            cmds.file(self.ui.terrainLine.text() + "/Terrain.obj", f = True, typ = "OBJexport", es = True, op="groups=0; ptgroups=0; materials=0; smoothing=0; normals=1")
        
            cmds.select("Terrain:*")
            cmds.delete()
        else:
            cmds.file(self.execDir+ "/Output/Terrain.obj", i=True, ns="Terrain", mnc=True)
            
            cmds.select("Terrain:*")
            cmds.polySetToFaceNormal()
            cmds.polyNormal(nm = 0, ch = 0)
            cmds.polySoftEdge(a = 180, ch = 0)
            cmds.select("Terrain:*")
            
            cmds.file(self.execDir + "/Output/Terrain.obj", f = True, typ = "OBJexport", es = True, op="groups=0; ptgroups=0; materials=0; smoothing=0; normals=1")
            
            cmds.select("Terrain:*")
            cmds.delete()
            
            output = self.execDir + '/Output'
        
        filetype = output + "/*.mtl"          
        subprocess.call(["rm", "-f", filetype])
        
        if len(self.ui.wallsLine.text()) > 1:
            filetype = self.ui.wallsLine.text() + "/*.mtl"
            subprocess.call(["rm", "-f", filetype])

        print("Done")
        
        if self.is2D:
            self.load3D() 
        else:
            self.change3D(10)
        
        self.ui.saveSettingsButton.setEnabled(True)
        
        self.ui.postGenTerrain.setEnabled(True)
        
        if len(self.ui.textureLine.text()) > 0:
            self.ui.loadTextureButton.setEnabled(True)
            
        self.ui.postGenTexture.setEnabled(True)
        
        if len(self.ui.heightmapLine.text()) > 0:
            self.ui.loadHeightmapButton.setEnabled(True)
        
        self.ui.postGenHeightmap.setEnabled(True)
        
        if len(self.ui.wallsLine.text()) > 0:
            self.ui.loadWallsButton.setEnabled(True)
        
        self.ui.postGenWalls.setEnabled(True)
        
        self.ui.switch3DButton.setEnabled(True)
        self.ui.importButton.setEnabled(True)
        
        self.isGenerated = True 
        
    def centerWindow(self):                
        screenRes = QtGui.QDesktopWidget().screenGeometry()
        self.move((screenRes.width() / 2.0) - (self.width() / 2.0), (screenRes.height() / 2.0) - (self.height() / 2.0))
        
    is2D = True
    isNotValid = False
    defaultLocation = ''
    isGenerated = False
    hasSettings = False
    settingsDir = ''
    workingDir = ''
    execDir = ''
    notFirstClose = False
   
global UI

try:
    UI.close()
except: 
    pass

UI = ControlMainWindow(parent=maya_main_window())

UI.show()