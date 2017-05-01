import sys
import json

import mainclient_auto

import PyQt5
from PyQt5.QtWidgets import *
from PyQt5 import QtCore
from PyQt5.QtCore import QThread

import threading
from threading import Thread

import boto3


class MainWindow(QMainWindow, mainclient_auto.Ui_MainClient):        

    num_samples = 0
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
    _translate = QtCore.QCoreApplication.translate

    def handleSQS(self):                        
        for message in queue.receive_messages(MessageAttributeNames=['Author']):
            if (message.body is not None):
                self.num_samples += 1
                self.txtPackets.setText(self._translate("MainClient", "{0}".format(self.num_samples)))
                json_message = json.loads(message.body)                
                hum = json_message['humidity']
                temp = json_message['temperature']
                date = json_message['timestamp']
                self.txtCurrTemp.setText(self._translate("MainClient", temp + " C"))
                self.txtCurrHum.setText(self._translate("MainClient", hum + " %"))
                self.txtCurrDate.setText(self._translate("MainClient", date))
                self.updateStats(temp, hum, date)

                if (self.num_samples <= 30):
                    self.progressBar.setProperty("value", (self.num_samples * 100) / 30)
                    if (self.num_samples == 30):
                        # Make the rest of the values visible and hide the progress bar
                        self.txtStatus.setText(self._translate("MainClient", "Connected"))
                        self.progressBar.setVisible(False)                        
                        self.txtMaxTemp.setVisible(True)
                        self.txtMinTemp.setVisible(True)
                        self.txtMaxHum.setVisible(True)
                        self.txtMinHum.setVisible(True)
                        self.txtAvgTemp.setVisible(True)
                        self.txtAvgHum.setVisible(True)
                        self.txtCurrTemp.setVisible(True)
                        self.txtCurrHum.setVisible(True)
                        self.txtDateAvgHum.setVisible(True)
                        self.txtDateAvgTemp.setVisible(True)
                        self.txtDateMaxHum.setVisible(True)
                        self.txtDateMinHum.setVisible(True)
                        self.txtDateMaxTemp.setVisible(True)
                        self.txtDateMinTemp.setVisible(True)
                        self.txtCurrDate.setVisible(True)                

                
    def updateStats(self, temp, hum, date):
        fTemp = float(temp)
        fHum = float(hum)
        if (fTemp > self.max_temp):
            self.max_temp = fTemp
            self.txtMaxTemp.setText(self._translate("MainClient", temp + " C"))
            self.txtDateMaxTemp.setText(self._translate("MainClient", date))
        
        if (fTemp < self.min_temp):
            self.min_temp = fTemp
            self.txtMinTemp.setText(self._translate("MainClient", temp + " C"))
            self.txtDateMinTemp.setText(self._translate("MainClient", date))

        if (fHum > self.max_hum):
            self.max_hum = fHum
            self.txtMaxHum.setText(self._translate("MainClient", hum + " %"))
            self.txtDateMaxHum.setText(self._translate("MainClient", date))

        if (fHum < self.min_hum):
            self.min_hum = fHum
            self.txtMinHum.setText(self._translate("MainClient", hum + " %"))
            self.txtDateMinHum.setText(self._translate("MainClient", date))
        
        self.avg_temp_sum += fTemp
        self.avg_hum_sum += fHum
        if ((self.avg_temp_sum / self.num_samples) != self.avg_temp):
            self.avg_temp = (self.avg_temp_sum / self.num_samples)
            self.txtAvgTemp.setText(self._translate("MainClient", "{0:0.1f} C".format(self.avg_temp)))
        if ((self.avg_hum_sum / self.num_samples) != self.avg_hum):
            self.avg_hum = (self.avg_hum_sum / self.num_samples)
            self.txtAvgHum.setText(self._translate("MainClient", "{0:0.1f} %".format(self.avg_hum)))

    def pressedQuit(self):        
        exit(1)

    def __init__(self):
        super(self.__class__, self).__init__()
        self.setupUi(self)

        # Button hooks
        self.btnQuit.clicked.connect(lambda: self.pressedQuit())        

        # Hide the rest of the controls until at least 30 samples arrive        
        self.txtMaxTemp.setVisible(False)
        self.txtMinTemp.setVisible(False)
        self.txtMaxHum.setVisible(False)
        self.txtMinHum.setVisible(False)
        self.txtAvgTemp.setVisible(False)
        self.txtAvgHum.setVisible(False)
        self.txtCurrTemp.setVisible(False)
        self.txtCurrHum.setVisible(False)
        self.txtDateAvgHum.setVisible(False)
        self.txtDateAvgTemp.setVisible(False)
        self.txtDateMaxHum.setVisible(False)
        self.txtDateMinHum.setVisible(False)
        self.txtDateMaxTemp.setVisible(False)
        self.txtDateMinTemp.setVisible(False)
        self.txtCurrDate.setVisible(False)

        self.txtStatus.setText(self._translate("MainClient", "Collecting samples..."))

        self.timer = QtCore.QTimer(self)
        self.timer.setInterval(2000)
        self.timer.timeout.connect(self.handleSQS)
        self.timer.start()


sqs = boto3.resource('sqs')
queue = sqs.get_queue_by_name(QueueName="DHT22_Queue")


def main():
    global form
    app = QApplication(sys.argv)
    form = MainWindow()
    form.show()    

    sys.exit(app.exec_())

if __name__ == "__main__":
    main()