import sys

import PyQt5
from PyQt5.QtWidgets import *
from PyQt5 import QtCore

import time
import threading
from threading import Thread, Lock

import mainwindow_auto

import Adafruit_DHT

mutex = Lock()


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

    def updateSensor(self):
        mutex.acquire()
        _translate = QtCore.QCoreApplication.translate
        humidity, temperature = Adafruit_DHT.read_retry(Adafruit_DHT.DHT22, 4)

    # Button click handlers

    def __init__(self):
        super(self.__class__, self).__init__()
        self.setupUi(self)

        # Button hooks
        #self.sldrTemp.

        # Periodic thread for automatic updates
        timerThread = threading.Thread(target=self.updateSensor_5s)
        timerThread.daemon = True
        timerThread.start()


quit_app = False
updating = False


def main():
    app = QApplication(sys.argv)
    form = MainWindow()
    form.show()

    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
