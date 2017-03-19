import sys
import os

import PyQt5
from PyQt5.QtWidgets import *
from PyQt5 import QtCore

import datetime
import json
import mainwindow_auto
import Adafruit_DHT
import time

import threading
from threading import Thread

from pprint import pprint

from tinydb import TinyDB, Query


class MainWindow(QMainWindow, mainwindow_auto.Ui_MainWindow):
    # Periodic update handler
    def updateSensor_5s(self):
        next_call = time.time()
        global quit_app
        while not(quit_app):
            self.updateSensor()
            next_call = next_call + 5
            sleep_time = next_call - time.time()
            if sleep_time < 0:
                sleep_time = 5
            time.sleep(sleep_time)

    def celsiusToFahrenheit(self, tempC):
        return (tempC * 9 / 5 + 32)

    def updateSensor(self):
        global useTempF, avg_temp_sum, avg_hum_sum
        _translate = QtCore.QCoreApplication.translate
        self.txtStatus.setText(_translate("MainWindow", "Updating"))
        humidity, temperature = Adafruit_DHT.read_retry(Adafruit_DHT.DHT22, 4)
        now = datetime.datetime.now()
        currTime = now.strftime("%m/%d/%y %I:%M:%S %p")
        strTemp = '{0:0.1f}'.format(temperature)
        strHum = '{0:0.1f}'.format(humidity)
        avg_temp_sum = avg_temp_sum + float(strTemp)
        avg_hum_sum = avg_hum_sum + float(strHum)
        self.addToDB(float(strTemp), float(strHum),
            currTime)
        self.txtDate_Current.setText(_translate("MainWindow", currTime))
        if (useTempF):
            self.txtTemp.setText(_translate("MainWindow", '{0:0.1f} F'.format(
                self.celsiusToFahrenheit(temperature))))
        else:
            self.txtTemp.setText(_translate("MainWindow", '{0:0.1f} C'.format(
                temperature)))

        self.txtHumidity.setText(_translate("MainWindow", '{0:0.1f} %'.format(
            humidity)))
        self.txtStatus.setText(_translate("MainWindow", "Connected"))
        self.updateStats()

    def addToDB(self, temp, humidity, date):
        global database
        # For consistency, temperature should be stored as degrees C
        database.insert({'tempC': temp, 'humidity': humidity, 'date': date})

    def updateStats(self):
        global max_temp, max_hum, min_temp, min_hum, avg_temp, avg_hum
        _translate = QtCore.QCoreApplication.translate
        dbQ = Query()

        # Max temperature
        db_data = database.get(dbQ['tempC'] > max_temp)
        if (db_data is not None):
            max_temp = db_data['tempC']
            self.txtDate_Max_Temp.setText(_translate("MainWindow", db_data['date']))
            if (useTempF):
                self.txtTemp_Max.setText(_translate("MainWindow", '{0} F'.format(
                    self.celsiusToFahrenheit(max_temp))))
            else:
                self.txtTemp_Max.setText(_translate("MainWindow", '{0} C'.format(
                    max_temp)))

        # Max humidity
        db_data = database.get(dbQ['humidity'] > max_hum)
        if (db_data is not None):
            max_hum = db_data['humidity']
            self.txtDate_Max_Hum.setText(_translate("MainWindow", db_data['date']))
            self.txtHumidity_Max.setText(_translate("MainWindow", '{0} %'.format(
                max_hum)))

        # Min temperature
        db_data = database.get(dbQ['tempC'] < min_temp)
        if (db_data is not None):
            min_temp = db_data['tempC']
            self.txtDate_Min_Temp.setText(_translate("MainWindow", db_data['date']))
            if (useTempF):
                self.txtTemp_Min.setText(_translate("MainWindow", '{0} F'.format(
                    self.celsiusToFahrenheit(min_temp))))
            else:
                self.txtTemp_Min.setText(_translate("MainWindow", '{0} C'.format(
                    min_temp)))

        # Min humidity
        db_data = database.get(dbQ['humidity'] < min_hum)
        if (db_data is not None):
            print(db_data)
            min_hum = db_data['humidity']
            self.txtDate_Min_Hum.setText(_translate("MainWindow", db_data['date']))
            self.txtHumidity_Min.setText(_translate("MainWindow", '{0} %'.format(
                min_hum)))

        self.updateAverage()

    def updateAverage(self):
        global avg_temp, avg_hum, avg_temp_sum, avg_hum_sum
        _translate = QtCore.QCoreApplication.translate
        last_avg_temp = avg_temp
        last_avg_hum = avg_hum
        avg_temp = avg_temp_sum / len(database)
        avg_hum = avg_hum_sum / len(database)
        now = datetime.datetime.now()
        currTime = now.strftime("%m/%d/%y %I:%M:%S %p")
        if (last_avg_temp != avg_temp):
            if (useTempF):
                self.txtTemp_Avg.setText(_translate("MainWindow", '{0:0.1f} F'.format(
                    self.celsiusToFahrenheit(avg_temp))))
            else:
                self.txtTemp_Avg.setText(_translate("MainWindow", '{0:0.1f} C'.format(
                    avg_temp)))
            self.txtDate_Avg_Temp.setText(_translate("MainWindow", currTime))

        if (last_avg_hum != avg_hum):
            self.txtHumidity_Avg.setText(_translate("MainWindow", '{0:0.1f} %'.format(
                avg_hum)))
            self.txtDate_Avg_Hum.setText(_translate("MainWindow", currTime))

    # Button click handlers
    def pressedQuit(self):
        quit_app = True

        # Get rid of the old database
        os.remove("project2.json")
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
database = TinyDB('project2.json')
max_temp = 0.0
max_hum = 0.0
min_temp = 999.9
min_hum = 100.0
avg_temp = 0.0
avg_hum = 0.0
avg_temp_sum = 0.0
avg_hum_sum = 0.0


def main():
    app = QApplication(sys.argv)
    form = MainWindow()
    form.show()

    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
