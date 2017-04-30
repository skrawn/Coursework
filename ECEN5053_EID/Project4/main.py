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

import aws_iot
import server

import threading
from threading import Thread

import asyncio
from concurrent.futures import ProcessPoolExecutor

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
        global useTempF, avg_temp_sum, avg_hum_sum, num_samples, curr_temp
        _translate = QtCore.QCoreApplication.translate
        self.txtStatus.setText(_translate("MainWindow", "Updating"))
        humidity, temperature = Adafruit_DHT.read_retry(Adafruit_DHT.DHT22, 4)
        now = datetime.datetime.now()

        currTime = now.strftime("%m/%d/%y %I:%M:%S %p")
        strTemp = '{0:0.1f}'.format(temperature)
        strHum = '{0:0.1f}'.format(humidity)

        AWS_Client.SendData(strTemp, strHum, currTime)

        avg_temp_sum = avg_temp_sum + float(strTemp)
        avg_hum_sum = avg_hum_sum + float(strHum)
        num_samples = num_samples + 1
        self.txtPackets.setText(_translate("MainWindow", '{0}'.format(num_samples)))

        curr_temp = float(strTemp)
        self.addToDB(float(strTemp), float(strHum), currTime)
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
        # Check for an existing curr entry
        dbQ = Query()
        db_data = database.get(dbQ['entry'] == 'curr')
        if (db_data is not None):
            database.update({'tempC': temp, 'humidity': humidity}, dbQ.entry == 'curr')
        else:
            # For consistency, temperature should be stored as degrees C
            database.insert({'entry': 'curr', 'tempC': temp, 'humidity': humidity, 'date': date})

    def updateStats(self):
        global max_temp, max_hum, min_temp, min_hum, avg_temp, avg_hum, database
        _translate = QtCore.QCoreApplication.translate
        dbQ = Query()

        # Check the for the max entry
        db_data = database.get(dbQ['entry'] == 'max')
        if (db_data is None):
            db_data = database.get(dbQ['entry'] == 'curr')
            max_temp = db_data['tempC']
            max_hum = db_data['humidity']
            database.insert({'entry': 'max', 'tempC': max_temp, 'humidity': max_hum, 'date': db_data['date']})
            self.txtDate_Max_Temp.setText(_translate("MainWindow", db_data['date']))
            if (useTempF):
                self.txtTemp_Max.setText(_translate("MainWindow", '{0} F'.format(
                    self.celsiusToFahrenheit(max_temp))))
            else:
                self.txtTemp_Max.setText(_translate("MainWindow", '{0} C'.format(
                    max_temp)))
            self.txtDate_Max_Hum.setText(_translate("MainWindow", db_data['date']))
            self.txtHumidity_Max.setText(_translate("MainWindow", '{0} %'.format(
                max_hum)))
        else:
            # Update the max entry
            db_data = database.get(dbQ['entry'] == 'curr')
            if (max_temp < db_data['tempC']):
                new_date = db_data['date']
                max_temp = db_data['tempC']
                database.update({'tempC': max_temp}, dbQ.entry == 'max')
                self.txtDate_Max_Temp.setText(_translate("MainWindow", new_date))
                if (useTempF):
                    self.txtTemp_Max.setText(_translate("MainWindow", '{0} F'.format(
                        self.celsiusToFahrenheit(max_temp))))
                else:
                    self.txtTemp_Max.setText(_translate("MainWindow", '{0} C'.format(
                        max_temp)))

            if (max_hum < db_data['humidity']):
                new_date = db_data['date']
                max_hum = db_data['humidity']
                database.update({'humidity': max_hum}, dbQ.entry == 'max')
                self.txtDate_Max_Hum.setText(_translate("MainWindow", new_date))
                self.txtHumidity_Max.setText(_translate("MainWindow", '{0} %'.format(max_hum)))

        # Check for the min entry
        db_data = database.get(dbQ['entry'] == 'min')
        if (db_data is None):
            db_data = database.get(dbQ['entry'] == 'curr')
            min_temp = db_data['tempC']
            min_hum = db_data['humidity']
            database.insert({'entry': 'min', 'tempC': min_temp, 'humidity':min_hum, 'date': db_data['date']})
            self.txtDate_Min_Temp.setText(_translate("MainWindow", db_data['date']))
            if (useTempF):
                self.txtTemp_Min.setText(_translate("MainWindow", '{0} F'.format(
                    self.celsiusToFahrenheit(min_temp))))
            else:
                self.txtTemp_Min.setText(_translate("MainWindow", '{0} C'.format(
                    min_temp)))
            self.txtDate_Min_Hum.setText(_translate("MainWindow", db_data['date']))
            self.txtHumidity_Min.setText(_translate("MainWindow", '{0} %'.format(
                min_hum)))
        else:
            # Update the min entry
            db_data = database.get(dbQ['entry'] == 'curr')
            if (min_temp > db_data['tempC']):
                new_date = db_data['date']
                min_temp = db_data['tempC']
                database.update({'tempC': min_temp}, dbQ.entry == 'min')
                self.txtDate_Min_Temp.setText(_translate("MainWindow", new_date))
                if (useTempF):
                    self.txtTemp_Min.setText(_translate("MainWindow", '{0} F'.format(
                        self.celsiusToFahrenheit(min_temp))))
                else:
                    self.txtTemp_Min.setText(_translate("MainWindow", '{0} C'.format(
                        min_temp)))

            if (min_hum > db_data['humidity']):
                new_date = db_data['date']
                min_hum = db_data['humidity']
                database.update({'humidity': min_hum}, dbQ.entry == 'min')
                self.txtDate_Min_Hum.setText(_translate("MainWindow", new_date))
                self.txtHumidity_Min.setText(_translate("MainWindow", '{0} %'.format(min_hum)))

        self.updateAverage()

    def updateAverage(self):
        global avg_temp, avg_hum, avg_temp_sum, avg_hum_sum, database, num_samples
        # For consistency, temperature should be stored as degrees C
        _translate = QtCore.QCoreApplication.translate
        dbQ = Query()
        last_avg_temp = avg_temp
        last_avg_hum = avg_hum
        avg_temp = float('{0:0.1f}'.format(avg_temp_sum / num_samples))
        avg_hum = float('{0:0.1f}'.format(avg_hum_sum / num_samples))
        now = datetime.datetime.now()
        currTime = now.strftime("%m/%d/%y %I:%M:%S %p")

        # Get the avg database entry
        db_data = database.get(dbQ['entry'] == 'avg')
        if (db_data is None):
            database.insert({'entry': 'avg', 'tempC': avg_temp, 'humidity': avg_hum, 'date': currTime})
            if (useTempF):
                self.txtTemp_Avg.setText(_translate("MainWindow", '{0:0.1f} F'.format(
                    self.celsiusToFahrenheit(avg_temp))))
            else:
                self.txtTemp_Avg.setText(_translate("MainWindow", '{0:0.1f} C'.format(
                    avg_temp)))
            self.txtDate_Avg_Temp.setText(_translate("MainWindow", currTime))
            self.txtHumidity_Avg.setText(_translate("MainWindow", '{0:0.1f} %'.format(
                avg_hum)))
            self.txtDate_Avg_Hum.setText(_translate("MainWindow", currTime))
        else:
            if (last_avg_temp != avg_temp):
                database.update({'tempC': avg_temp}, dbQ.entry == 'avg')
                if (useTempF):
                    self.txtTemp_Avg.setText(_translate("MainWindow", '{0:0.1f} F'.format(
                        self.celsiusToFahrenheit(avg_temp))))
                else:
                    self.txtTemp_Avg.setText(_translate("MainWindow", '{0:0.1f} C'.format(
                        avg_temp)))
                self.txtDate_Avg_Temp.setText(_translate("MainWindow", currTime))

            if (last_avg_hum != avg_hum):
                database.update({'humidity': avg_hum}, dbQ.entry == 'avg')
                self.txtHumidity_Avg.setText(_translate("MainWindow", '{0:0.1f} %'.format(
                    avg_hum)))
                self.txtDate_Avg_Hum.setText(_translate("MainWindow", currTime))

    # Button click handlers
    def pressedQuit(self):
        quit_app = True
        # Get rid of the old database
        os.remove("project3.json")
        exit(1)

    def tempSliderChanged(self):
        global max_temp, max_hum, min_temp, min_hum, avg_temp, avg_hum, useTempF, curr_temp
        value = self.sldrTemp.value()
        _translate = QtCore.QCoreApplication.translate
        if (value == 0):
            useTempF = False     
            self.txtTemp.setText(_translate("MainWindow", '{0:0.1f} C'.format(curr_temp)))
            self.txtTemp_Avg.setText(_translate("MainWindow", '{0:0.1f} C'.format(avg_temp)))
            self.txtTemp_Min.setText(_translate("MainWindow", '{0:0.1f} C'.format(min_temp)))
            self.txtTemp_Max.setText(_translate("MainWindow", '{0:0.1f} C'.format(max_temp)))
        else:
            useTempF = True
            self.txtTemp.setText(_translate("MainWindow", '{0:0.1f} F'.format(
                self.celsiusToFahrenheit(curr_temp))))
            self.txtTemp_Avg.setText(_translate("MainWindow", '{0:0.1f} F'.format(
                self.celsiusToFahrenheit(avg_temp))))
            self.txtTemp_Min.setText(_translate("MainWindow", '{0:0.1f} F'.format(
                self.celsiusToFahrenheit(min_temp))))
            self.txtTemp_Max.setText(_translate("MainWindow", '{0:0.1f} F'.format(
                self.celsiusToFahrenheit(max_temp))))

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
database = TinyDB('project3.json')
curr_temp = 0.0
curr_hum = 0.0
max_temp = 0.0
max_hum = 0.0
min_temp = 999.9
min_hum = 100.0
avg_temp = 0.0
avg_hum = 0.0
avg_temp_sum = 0.0
avg_hum_sum = 0.0
num_samples = 0
AWS_Client = aws_iot.AWS_IOT()


def main():
    executor = ProcessPoolExecutor(2)
    loop = asyncio.get_event_loop()    

    app = QApplication(sys.argv)
    form = MainWindow()
    form.show()
    AWS_Client.Start()

    serverThread = threading.Thread(target=server.Start_Server)
    serverThread.daemon = True
    serverThread.start()

    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
