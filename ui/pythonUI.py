# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'pythonUI.ui'
#
# Created: Fri Jun 02 11:47:30 2017
#      by: pyside-uic 0.2.14 running on PySide 1.2.0
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        Dialog.setObjectName("Dialog")
        Dialog.resize(1100, 800)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(Dialog.sizePolicy().hasHeightForWidth())
        Dialog.setSizePolicy(sizePolicy)
        Dialog.setMinimumSize(QtCore.QSize(1100, 800))
        self.verticalLayout_8 = QtGui.QVBoxLayout(Dialog)
        self.verticalLayout_8.setObjectName("verticalLayout_8")
        self.vLayoutWidget = QtGui.QWidget(Dialog)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.MinimumExpanding, QtGui.QSizePolicy.MinimumExpanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.vLayoutWidget.sizePolicy().hasHeightForWidth())
        self.vLayoutWidget.setSizePolicy(sizePolicy)
        self.vLayoutWidget.setMinimumSize(QtCore.QSize(1050, 750))
        self.vLayoutWidget.setObjectName("vLayoutWidget")
        self.horizontalLayout_6 = QtGui.QHBoxLayout(self.vLayoutWidget)
        self.horizontalLayout_6.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout_6.setObjectName("horizontalLayout_6")
        self.verticalLayout = QtGui.QVBoxLayout()
        self.verticalLayout.setObjectName("verticalLayout")
        self.topLayoutWidget = QtGui.QWidget(self.vLayoutWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.MinimumExpanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.topLayoutWidget.sizePolicy().hasHeightForWidth())
        self.topLayoutWidget.setSizePolicy(sizePolicy)
        self.topLayoutWidget.setMinimumSize(QtCore.QSize(720, 0))
        self.topLayoutWidget.setObjectName("topLayoutWidget")
        self.horizontalLayout_2 = QtGui.QHBoxLayout(self.topLayoutWidget)
        self.horizontalLayout_2.setSizeConstraint(QtGui.QLayout.SetDefaultConstraint)
        self.horizontalLayout_2.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        spacerItem = QtGui.QSpacerItem(78, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_2.addItem(spacerItem)
        self.genTerrainButton = QtGui.QPushButton(self.topLayoutWidget)
        self.genTerrainButton.setObjectName("genTerrainButton")
        self.horizontalLayout_2.addWidget(self.genTerrainButton)
        self.genFieldsButton = QtGui.QPushButton(self.topLayoutWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.genFieldsButton.sizePolicy().hasHeightForWidth())
        self.genFieldsButton.setSizePolicy(sizePolicy)
        self.genFieldsButton.setAutoFillBackground(False)
        self.genFieldsButton.setAutoDefault(True)
        self.genFieldsButton.setDefault(False)
        self.genFieldsButton.setFlat(False)
        self.genFieldsButton.setObjectName("genFieldsButton")
        self.horizontalLayout_2.addWidget(self.genFieldsButton)
        self.show3DButton = QtGui.QPushButton(self.topLayoutWidget)
        self.show3DButton.setObjectName("show3DButton")
        self.horizontalLayout_2.addWidget(self.show3DButton)
        spacerItem1 = QtGui.QSpacerItem(78, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_2.addItem(spacerItem1)
        self.verticalLayout.addWidget(self.topLayoutWidget)
        self.label = QtGui.QLabel(self.vLayoutWidget)
        self.label.setEnabled(False)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.MinimumExpanding, QtGui.QSizePolicy.MinimumExpanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label.sizePolicy().hasHeightForWidth())
        self.label.setSizePolicy(sizePolicy)
        self.label.setMinimumSize(QtCore.QSize(720, 720))
        self.label.setText("")
        self.label.setAlignment(QtCore.Qt.AlignCenter)
        self.label.setObjectName("label")
        self.verticalLayout.addWidget(self.label)
        self.bottomLayoutWidget = QtGui.QWidget(self.vLayoutWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.MinimumExpanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.bottomLayoutWidget.sizePolicy().hasHeightForWidth())
        self.bottomLayoutWidget.setSizePolicy(sizePolicy)
        self.bottomLayoutWidget.setMinimumSize(QtCore.QSize(720, 0))
        self.bottomLayoutWidget.setObjectName("bottomLayoutWidget")
        self.horizontalLayout = QtGui.QHBoxLayout(self.bottomLayoutWidget)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout.setObjectName("horizontalLayout")
        spacerItem2 = QtGui.QSpacerItem(78, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem2)
        self.closeWindowButton = QtGui.QPushButton(self.bottomLayoutWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.closeWindowButton.sizePolicy().hasHeightForWidth())
        self.closeWindowButton.setSizePolicy(sizePolicy)
        self.closeWindowButton.setObjectName("closeWindowButton")
        self.horizontalLayout.addWidget(self.closeWindowButton)
        spacerItem3 = QtGui.QSpacerItem(78, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem3)
        self.verticalLayout.addWidget(self.bottomLayoutWidget)
        self.horizontalLayout_6.addLayout(self.verticalLayout)
        self.tabWidget = QtGui.QTabWidget(self.vLayoutWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.tabWidget.sizePolicy().hasHeightForWidth())
        self.tabWidget.setSizePolicy(sizePolicy)
        self.tabWidget.setMinimumSize(QtCore.QSize(325, 0))
        self.tabWidget.setMaximumSize(QtCore.QSize(16777215, 900))
        self.tabWidget.setObjectName("tabWidget")
        self.tab = QtGui.QWidget()
        self.tab.setObjectName("tab")
        self.top = QtGui.QWidget(self.tab)
        self.top.setGeometry(QtCore.QRect(10, 10, 301, 720))
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.MinimumExpanding, QtGui.QSizePolicy.MinimumExpanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.top.sizePolicy().hasHeightForWidth())
        self.top.setSizePolicy(sizePolicy)
        self.top.setMinimumSize(QtCore.QSize(300, 720))
        self.top.setMaximumSize(QtCore.QSize(16777215, 900))
        self.top.setLayoutDirection(QtCore.Qt.LeftToRight)
        self.top.setObjectName("top")
        self.widget = QtGui.QWidget(self.top)
        self.widget.setGeometry(QtCore.QRect(9, 10, 283, 714))
        self.widget.setObjectName("widget")
        self.verticalLayout_3 = QtGui.QVBoxLayout(self.widget)
        self.verticalLayout_3.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout_3.setObjectName("verticalLayout_3")
        self.Settings = QtGui.QFrame(self.widget)
        self.Settings.setMinimumSize(QtCore.QSize(200, 0))
        self.Settings.setFrameShape(QtGui.QFrame.StyledPanel)
        self.Settings.setFrameShadow(QtGui.QFrame.Plain)
        self.Settings.setObjectName("Settings")
        self.verticalLayout_2 = QtGui.QVBoxLayout(self.Settings)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.horizontalLayout_3 = QtGui.QHBoxLayout()
        self.horizontalLayout_3.setObjectName("horizontalLayout_3")
        self.label_2 = QtGui.QLabel(self.Settings)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_2.sizePolicy().hasHeightForWidth())
        self.label_2.setSizePolicy(sizePolicy)
        self.label_2.setMinimumSize(QtCore.QSize(100, 0))
        self.label_2.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_2.setObjectName("label_2")
        self.horizontalLayout_3.addWidget(self.label_2)
        self.seedValueBox = QtGui.QSpinBox(self.Settings)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.seedValueBox.sizePolicy().hasHeightForWidth())
        self.seedValueBox.setSizePolicy(sizePolicy)
        self.seedValueBox.setMinimumSize(QtCore.QSize(100, 0))
        self.seedValueBox.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.seedValueBox.setButtonSymbols(QtGui.QAbstractSpinBox.NoButtons)
        self.seedValueBox.setMaximum(100000000)
        self.seedValueBox.setObjectName("seedValueBox")
        self.horizontalLayout_3.addWidget(self.seedValueBox)
        self.verticalLayout_2.addLayout(self.horizontalLayout_3)
        self.horizontalLayout_5 = QtGui.QHBoxLayout()
        self.horizontalLayout_5.setObjectName("horizontalLayout_5")
        self.label_4 = QtGui.QLabel(self.Settings)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_4.sizePolicy().hasHeightForWidth())
        self.label_4.setSizePolicy(sizePolicy)
        self.label_4.setMinimumSize(QtCore.QSize(100, 0))
        self.label_4.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_4.setObjectName("label_4")
        self.horizontalLayout_5.addWidget(self.label_4)
        self.terrainSizeBox = QtGui.QSpinBox(self.Settings)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.terrainSizeBox.sizePolicy().hasHeightForWidth())
        self.terrainSizeBox.setSizePolicy(sizePolicy)
        self.terrainSizeBox.setMinimumSize(QtCore.QSize(100, 0))
        self.terrainSizeBox.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.terrainSizeBox.setButtonSymbols(QtGui.QAbstractSpinBox.NoButtons)
        self.terrainSizeBox.setObjectName("terrainSizeBox")
        self.horizontalLayout_5.addWidget(self.terrainSizeBox)
        self.verticalLayout_2.addLayout(self.horizontalLayout_5)
        self.horizontalLayout_4 = QtGui.QHBoxLayout()
        self.horizontalLayout_4.setObjectName("horizontalLayout_4")
        self.label_3 = QtGui.QLabel(self.Settings)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_3.sizePolicy().hasHeightForWidth())
        self.label_3.setSizePolicy(sizePolicy)
        self.label_3.setMinimumSize(QtCore.QSize(100, 0))
        self.label_3.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_3.setObjectName("label_3")
        self.horizontalLayout_4.addWidget(self.label_3)
        self.startPointSizeBox = QtGui.QSpinBox(self.Settings)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.startPointSizeBox.sizePolicy().hasHeightForWidth())
        self.startPointSizeBox.setSizePolicy(sizePolicy)
        self.startPointSizeBox.setMinimumSize(QtCore.QSize(100, 0))
        self.startPointSizeBox.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.startPointSizeBox.setButtonSymbols(QtGui.QAbstractSpinBox.NoButtons)
        self.startPointSizeBox.setMaximum(100)
        self.startPointSizeBox.setObjectName("startPointSizeBox")
        self.horizontalLayout_4.addWidget(self.startPointSizeBox)
        self.verticalLayout_2.addLayout(self.horizontalLayout_4)
        self.horizontalLayout_8 = QtGui.QHBoxLayout()
        self.horizontalLayout_8.setObjectName("horizontalLayout_8")
        self.label_5 = QtGui.QLabel(self.Settings)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_5.sizePolicy().hasHeightForWidth())
        self.label_5.setSizePolicy(sizePolicy)
        self.label_5.setMinimumSize(QtCore.QSize(100, 0))
        self.label_5.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_5.setObjectName("label_5")
        self.horizontalLayout_8.addWidget(self.label_5)
        self.roughnessBox = QtGui.QDoubleSpinBox(self.Settings)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.roughnessBox.sizePolicy().hasHeightForWidth())
        self.roughnessBox.setSizePolicy(sizePolicy)
        self.roughnessBox.setMinimumSize(QtCore.QSize(100, 0))
        self.roughnessBox.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.roughnessBox.setButtonSymbols(QtGui.QAbstractSpinBox.NoButtons)
        self.roughnessBox.setDecimals(3)
        self.roughnessBox.setObjectName("roughnessBox")
        self.horizontalLayout_8.addWidget(self.roughnessBox)
        self.verticalLayout_2.addLayout(self.horizontalLayout_8)
        self.horizontalLayout_9 = QtGui.QHBoxLayout()
        self.horizontalLayout_9.setObjectName("horizontalLayout_9")
        self.label_6 = QtGui.QLabel(self.Settings)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_6.sizePolicy().hasHeightForWidth())
        self.label_6.setSizePolicy(sizePolicy)
        self.label_6.setMinimumSize(QtCore.QSize(100, 0))
        self.label_6.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_6.setObjectName("label_6")
        self.horizontalLayout_9.addWidget(self.label_6)
        self.heightmapIterationsBox = QtGui.QSpinBox(self.Settings)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.heightmapIterationsBox.sizePolicy().hasHeightForWidth())
        self.heightmapIterationsBox.setSizePolicy(sizePolicy)
        self.heightmapIterationsBox.setMinimumSize(QtCore.QSize(100, 0))
        self.heightmapIterationsBox.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.heightmapIterationsBox.setButtonSymbols(QtGui.QAbstractSpinBox.NoButtons)
        self.heightmapIterationsBox.setObjectName("heightmapIterationsBox")
        self.horizontalLayout_9.addWidget(self.heightmapIterationsBox)
        self.verticalLayout_2.addLayout(self.horizontalLayout_9)
        self.verticalLayout_3.addWidget(self.Settings)
        self.farmPosition = QtGui.QGroupBox(self.widget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.farmPosition.sizePolicy().hasHeightForWidth())
        self.farmPosition.setSizePolicy(sizePolicy)
        self.farmPosition.setMinimumSize(QtCore.QSize(200, 0))
        self.farmPosition.setMaximumSize(QtCore.QSize(16777215, 300))
        self.farmPosition.setCheckable(True)
        self.farmPosition.setChecked(False)
        self.farmPosition.setObjectName("farmPosition")
        self.verticalLayout_6 = QtGui.QVBoxLayout(self.farmPosition)
        self.verticalLayout_6.setObjectName("verticalLayout_6")
        self.verticalLayout_4 = QtGui.QVBoxLayout()
        self.verticalLayout_4.setObjectName("verticalLayout_4")
        self.farmTranslateLayout = QtGui.QWidget(self.farmPosition)
        self.farmTranslateLayout.setMinimumSize(QtCore.QSize(0, 30))
        self.farmTranslateLayout.setMaximumSize(QtCore.QSize(16777215, 30))
        self.farmTranslateLayout.setObjectName("farmTranslateLayout")
        self.horizontalLayout_10 = QtGui.QHBoxLayout(self.farmTranslateLayout)
        self.horizontalLayout_10.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout_10.setObjectName("horizontalLayout_10")
        self.label_7 = QtGui.QLabel(self.farmTranslateLayout)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_7.sizePolicy().hasHeightForWidth())
        self.label_7.setSizePolicy(sizePolicy)
        self.label_7.setMinimumSize(QtCore.QSize(50, 0))
        self.label_7.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_7.setObjectName("label_7")
        self.horizontalLayout_10.addWidget(self.label_7)
        self.translateXBox = QtGui.QDoubleSpinBox(self.farmTranslateLayout)
        self.translateXBox.setMinimumSize(QtCore.QSize(48, 0))
        self.translateXBox.setFrame(True)
        self.translateXBox.setButtonSymbols(QtGui.QAbstractSpinBox.NoButtons)
        self.translateXBox.setDecimals(3)
        self.translateXBox.setMinimum(-99.99)
        self.translateXBox.setObjectName("translateXBox")
        self.horizontalLayout_10.addWidget(self.translateXBox)
        self.translateYBo = QtGui.QDoubleSpinBox(self.farmTranslateLayout)
        self.translateYBo.setMinimumSize(QtCore.QSize(48, 0))
        self.translateYBo.setFrame(True)
        self.translateYBo.setButtonSymbols(QtGui.QAbstractSpinBox.NoButtons)
        self.translateYBo.setDecimals(3)
        self.translateYBo.setMinimum(-99.9)
        self.translateYBo.setObjectName("translateYBo")
        self.horizontalLayout_10.addWidget(self.translateYBo)
        self.translateZBox = QtGui.QDoubleSpinBox(self.farmTranslateLayout)
        self.translateZBox.setMinimumSize(QtCore.QSize(48, 0))
        self.translateZBox.setFrame(True)
        self.translateZBox.setButtonSymbols(QtGui.QAbstractSpinBox.NoButtons)
        self.translateZBox.setDecimals(3)
        self.translateZBox.setMinimum(-99.99)
        self.translateZBox.setObjectName("translateZBox")
        self.horizontalLayout_10.addWidget(self.translateZBox)
        self.verticalLayout_4.addWidget(self.farmTranslateLayout)
        self.farmMeshLayout = QtGui.QWidget(self.farmPosition)
        self.farmMeshLayout.setMinimumSize(QtCore.QSize(0, 30))
        self.farmMeshLayout.setMaximumSize(QtCore.QSize(16777215, 30))
        self.farmMeshLayout.setObjectName("farmMeshLayout")
        self.horizontalLayout_11 = QtGui.QHBoxLayout(self.farmMeshLayout)
        self.horizontalLayout_11.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout_11.setObjectName("horizontalLayout_11")
        self.label_8 = QtGui.QLabel(self.farmMeshLayout)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_8.sizePolicy().hasHeightForWidth())
        self.label_8.setSizePolicy(sizePolicy)
        self.label_8.setMinimumSize(QtCore.QSize(10, 0))
        self.label_8.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_8.setObjectName("label_8")
        self.horizontalLayout_11.addWidget(self.label_8)
        self.farmMeshLine = QtGui.QLineEdit(self.farmMeshLayout)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.MinimumExpanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.farmMeshLine.sizePolicy().hasHeightForWidth())
        self.farmMeshLine.setSizePolicy(sizePolicy)
        self.farmMeshLine.setMinimumSize(QtCore.QSize(100, 0))
        self.farmMeshLine.setObjectName("farmMeshLine")
        self.horizontalLayout_11.addWidget(self.farmMeshLine)
        self.farmMeshButton = QtGui.QPushButton(self.farmMeshLayout)
        self.farmMeshButton.setMinimumSize(QtCore.QSize(10, 0))
        self.farmMeshButton.setText("")
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap("../../Uni/FMP/Terrain/ui/browseFolder.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.farmMeshButton.setIcon(icon)
        self.farmMeshButton.setIconSize(QtCore.QSize(25, 25))
        self.farmMeshButton.setCheckable(False)
        self.farmMeshButton.setAutoDefault(False)
        self.farmMeshButton.setDefault(False)
        self.farmMeshButton.setFlat(True)
        self.farmMeshButton.setObjectName("farmMeshButton")
        self.horizontalLayout_11.addWidget(self.farmMeshButton)
        self.verticalLayout_4.addWidget(self.farmMeshLayout)
        self.verticalLayout_6.addLayout(self.verticalLayout_4)
        self.farmMeshBrowser = QtGui.QTextBrowser(self.farmPosition)
        self.farmMeshBrowser.setMaximumSize(QtCore.QSize(16777215, 175))
        self.farmMeshBrowser.setObjectName("farmMeshBrowser")
        self.verticalLayout_6.addWidget(self.farmMeshBrowser)
        self.verticalLayout_3.addWidget(self.farmPosition)
        self.trees = QtGui.QGroupBox(self.widget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.MinimumExpanding, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.trees.sizePolicy().hasHeightForWidth())
        self.trees.setSizePolicy(sizePolicy)
        self.trees.setMinimumSize(QtCore.QSize(0, 200))
        self.trees.setMaximumSize(QtCore.QSize(16777215, 300))
        self.trees.setCheckable(True)
        self.trees.setChecked(False)
        self.trees.setObjectName("trees")
        self.verticalLayout_7 = QtGui.QVBoxLayout(self.trees)
        self.verticalLayout_7.setObjectName("verticalLayout_7")
        self.clusterTreesLayout = QtGui.QWidget(self.trees)
        self.clusterTreesLayout.setMaximumSize(QtCore.QSize(16777215, 30))
        self.clusterTreesLayout.setObjectName("clusterTreesLayout")
        self.horizontalLayout_13 = QtGui.QHBoxLayout(self.clusterTreesLayout)
        self.horizontalLayout_13.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout_13.setObjectName("horizontalLayout_13")
        self.normalTreesCheck = QtGui.QCheckBox(self.clusterTreesLayout)
        self.normalTreesCheck.setChecked(True)
        self.normalTreesCheck.setObjectName("normalTreesCheck")
        self.horizontalLayout_13.addWidget(self.normalTreesCheck)
        self.clusterTreesCheck = QtGui.QCheckBox(self.clusterTreesLayout)
        self.clusterTreesCheck.setMinimumSize(QtCore.QSize(0, 20))
        self.clusterTreesCheck.setChecked(True)
        self.clusterTreesCheck.setObjectName("clusterTreesCheck")
        self.horizontalLayout_13.addWidget(self.clusterTreesCheck)
        self.verticalLayout_7.addWidget(self.clusterTreesLayout)
        self.TreeMeshLayout = QtGui.QWidget(self.trees)
        self.TreeMeshLayout.setMaximumSize(QtCore.QSize(16777215, 30))
        self.TreeMeshLayout.setObjectName("TreeMeshLayout")
        self.horizontalLayout_12 = QtGui.QHBoxLayout(self.TreeMeshLayout)
        self.horizontalLayout_12.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout_12.setObjectName("horizontalLayout_12")
        self.label_9 = QtGui.QLabel(self.TreeMeshLayout)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_9.sizePolicy().hasHeightForWidth())
        self.label_9.setSizePolicy(sizePolicy)
        self.label_9.setMinimumSize(QtCore.QSize(10, 0))
        self.label_9.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_9.setObjectName("label_9")
        self.horizontalLayout_12.addWidget(self.label_9)
        self.treeMeshLine = QtGui.QLineEdit(self.TreeMeshLayout)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.MinimumExpanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.treeMeshLine.sizePolicy().hasHeightForWidth())
        self.treeMeshLine.setSizePolicy(sizePolicy)
        self.treeMeshLine.setMinimumSize(QtCore.QSize(100, 0))
        self.treeMeshLine.setObjectName("treeMeshLine")
        self.horizontalLayout_12.addWidget(self.treeMeshLine)
        self.treeMeshButton = QtGui.QPushButton(self.TreeMeshLayout)
        self.treeMeshButton.setMinimumSize(QtCore.QSize(10, 0))
        self.treeMeshButton.setText("")
        self.treeMeshButton.setIcon(icon)
        self.treeMeshButton.setIconSize(QtCore.QSize(25, 25))
        self.treeMeshButton.setCheckable(False)
        self.treeMeshButton.setAutoDefault(False)
        self.treeMeshButton.setDefault(False)
        self.treeMeshButton.setFlat(True)
        self.treeMeshButton.setObjectName("treeMeshButton")
        self.horizontalLayout_12.addWidget(self.treeMeshButton)
        self.verticalLayout_7.addWidget(self.TreeMeshLayout)
        self.treeMeshBrowser = QtGui.QTextBrowser(self.trees)
        self.treeMeshBrowser.setMaximumSize(QtCore.QSize(16777215, 175))
        self.treeMeshBrowser.setObjectName("treeMeshBrowser")
        self.verticalLayout_7.addWidget(self.treeMeshBrowser)
        self.verticalLayout_3.addWidget(self.trees)
        self.tabWidget.addTab(self.tab, "")
        self.tab_2 = QtGui.QWidget()
        self.tab_2.setObjectName("tab_2")
        self.tabWidget.addTab(self.tab_2, "")
        self.horizontalLayout_6.addWidget(self.tabWidget)
        self.verticalLayout_8.addWidget(self.vLayoutWidget)

        self.retranslateUi(Dialog)
        self.tabWidget.setCurrentIndex(0)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(QtGui.QApplication.translate("Dialog", "Dialog", None, QtGui.QApplication.UnicodeUTF8))
        self.genTerrainButton.setText(QtGui.QApplication.translate("Dialog", "Generate Terrain", None, QtGui.QApplication.UnicodeUTF8))
        self.genFieldsButton.setText(QtGui.QApplication.translate("Dialog", "Generate Fields", None, QtGui.QApplication.UnicodeUTF8))
        self.show3DButton.setText(QtGui.QApplication.translate("Dialog", "Show 3D", None, QtGui.QApplication.UnicodeUTF8))
        self.closeWindowButton.setText(QtGui.QApplication.translate("Dialog", "Close Window", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("Dialog", "Seed Value", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("Dialog", "Terrain Size", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("Dialog", "Start Point Size", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("Dialog", "Roughness Value", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setText(QtGui.QApplication.translate("Dialog", "Heightmap Iterations", None, QtGui.QApplication.UnicodeUTF8))
        self.farmPosition.setTitle(QtGui.QApplication.translate("Dialog", "Farm Position", None, QtGui.QApplication.UnicodeUTF8))
        self.label_7.setText(QtGui.QApplication.translate("Dialog", "Translate", None, QtGui.QApplication.UnicodeUTF8))
        self.label_8.setText(QtGui.QApplication.translate("Dialog", "Farm Meshes", None, QtGui.QApplication.UnicodeUTF8))
        self.trees.setTitle(QtGui.QApplication.translate("Dialog", "Trees", None, QtGui.QApplication.UnicodeUTF8))
        self.normalTreesCheck.setText(QtGui.QApplication.translate("Dialog", "Normal Trees", None, QtGui.QApplication.UnicodeUTF8))
        self.clusterTreesCheck.setText(QtGui.QApplication.translate("Dialog", "Cluster Trees", None, QtGui.QApplication.UnicodeUTF8))
        self.label_9.setText(QtGui.QApplication.translate("Dialog", "Tree Meshes", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab), QtGui.QApplication.translate("Dialog", "Settings", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_2), QtGui.QApplication.translate("Dialog", "Export", None, QtGui.QApplication.UnicodeUTF8))

