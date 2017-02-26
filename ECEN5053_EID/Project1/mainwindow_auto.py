# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'mainwindow.ui'
#
# Created by: PyQt5 UI code generator 5.5.1
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.setWindowModality(QtCore.Qt.NonModal)
        MainWindow.resize(800, 460)
        self.centralWidget = QtWidgets.QWidget(MainWindow)
        self.centralWidget.setObjectName("centralWidget")
        self.btnRefresh = QtWidgets.QPushButton(self.centralWidget)
        self.btnRefresh.setGeometry(QtCore.QRect(130, 290, 151, 81))
        font = QtGui.QFont()
        font.setPointSize(20)
        self.btnRefresh.setFont(font)
        self.btnRefresh.setObjectName("btnRefresh")
        self.btnQuit = QtWidgets.QPushButton(self.centralWidget)
        self.btnQuit.setGeometry(QtCore.QRect(510, 290, 151, 81))
        font = QtGui.QFont()
        font.setPointSize(20)
        self.btnQuit.setFont(font)
        self.btnQuit.setObjectName("btnQuit")
        self.label = QtWidgets.QLabel(self.centralWidget)
        self.label.setGeometry(QtCore.QRect(110, 20, 571, 81))
        font = QtGui.QFont()
        font.setPointSize(23)
        self.label.setFont(font)
        self.label.setAlignment(QtCore.Qt.AlignCenter)
        self.label.setObjectName("label")
        self.label_2 = QtWidgets.QLabel(self.centralWidget)
        self.label_2.setGeometry(QtCore.QRect(130, 110, 181, 51))
        font = QtGui.QFont()
        font.setPointSize(23)
        self.label_2.setFont(font)
        self.label_2.setObjectName("label_2")
        self.label_3 = QtWidgets.QLabel(self.centralWidget)
        self.label_3.setGeometry(QtCore.QRect(130, 190, 181, 51))
        font = QtGui.QFont()
        font.setPointSize(23)
        self.label_3.setFont(font)
        self.label_3.setObjectName("label_3")
        self.txtTemp = QtWidgets.QLabel(self.centralWidget)
        self.txtTemp.setGeometry(QtCore.QRect(440, 120, 81, 41))
        font = QtGui.QFont()
        font.setPointSize(23)
        self.txtTemp.setFont(font)
        self.txtTemp.setObjectName("txtTemp")
        self.txtHumidity = QtWidgets.QLabel(self.centralWidget)
        self.txtHumidity.setGeometry(QtCore.QRect(450, 190, 81, 41))
        font = QtGui.QFont()
        font.setPointSize(23)
        self.txtHumidity.setFont(font)
        self.txtHumidity.setObjectName("txtHumidity")
        MainWindow.setCentralWidget(self.centralWidget)
        self.menuBar = QtWidgets.QMenuBar(MainWindow)
        self.menuBar.setGeometry(QtCore.QRect(0, 0, 800, 23))
        self.menuBar.setObjectName("menuBar")
        MainWindow.setMenuBar(self.menuBar)
        self.mainToolBar = QtWidgets.QToolBar(MainWindow)
        self.mainToolBar.setObjectName("mainToolBar")
        MainWindow.addToolBar(QtCore.Qt.TopToolBarArea, self.mainToolBar)
        self.statusBar = QtWidgets.QStatusBar(MainWindow)
        self.statusBar.setObjectName("statusBar")
        MainWindow.setStatusBar(self.statusBar)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))
        self.btnRefresh.setText(_translate("MainWindow", "Refresh"))
        self.btnQuit.setText(_translate("MainWindow", "Quit"))
        self.label.setText(_translate("MainWindow", "DHT22 Temperature and Humity Sensor"))
        self.label_2.setText(_translate("MainWindow", "Temperature"))
        self.label_3.setText(_translate("MainWindow", "Humidity"))
        self.txtTemp.setText(_translate("MainWindow", "100 F"))
        self.txtHumidity.setText(_translate("MainWindow", "80 %"))

