import sys

import PyQt5
from PyQt5.QtWidgets import *
from PyQt5 import QtCore

import time
import threading
from threading import Thread

import mainwindow_auto

import Adafruit_DHT


class MainWindow(QMainWindow, mainwindow_auto.Ui_MainWindow):
    # Periodic update handler
    def updateSensor_5s(self):
        next_call = time.time()
        global quit_app
        while not(quit_app):
            self.updateSensor()
            next_call = next_call + 4
            sleep_time = next_call - time.time()
            if sleep_time < 0:
                sleep_time = 5
            time.sleep(sleep_time)

    def celsiusToFahrenheit(self, tempC):
        return (tempC * 9 / 5 + 32)

    def updateSensor(self):
        global useTempF
        _translate = QtCore.QCoreApplication.translate
        self.txtStatus.setText(_translate("MainWindow", "Updating"))
        humidity, temperature = Adafruit_DHT.read_retry(Adafruit_DHT.DHT22, 4)
        if (useTempF):
            self.txtTemp.setText(_translate("MainWindow", '{0:0.1f} F'.format(
                self.celsiusToFahrenheit(temperature))))
        else:
            self.txtTemp.setText(_translate("MainWindow", '{0:0.1f} C'.format(
                temperature)))

        self.txtHumidity.setText(_translate("MainWindow", '{0:0.1f} %'.format(humidity)))
        self.txtStatus.setText(_translate("MainWindow", "Connected"))

    # Button click handlers
    def pressedQuit(self):
        quit_app = True
        exit(1)

    def tempSliderChanged(self):
        global useTempF
        value = self.sldrTemp.value()
        if (value == 0):
            useTempF = False
        else:
            useTempF = True

    def __init__(self):
        super(self.__class__, self).__init__()
        self.setupUi(self)

        # Button hooks
        self.sldrTemp.valueChanged.connect(lambda: self.tempSliderChanged())
        self.btnQuit.clicked.connect(lambda: self.pressedQuit())

        # Periodic thread for automatic updates
        timerThread = threading.Thread(target=self.updateSensor_5s)
        timerThread.daemon = True
        timerThread.start()


quit_app = False
updating = False
useTempF = False


def main():
    app = QApplication(sys.argv)
    form = MainWindow()
    form.show()

    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
