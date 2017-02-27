# always seem to need this
import sys
 
# This gets the Qt stuff
import PyQt5
from PyQt5.QtWidgets import *
from PyQt5 import QtCore

# For periodic timer tasks
import time, threading
from threading import Thread, Lock
 
# This is our window from QtCreator
import mainwindow_auto

import Adafruit_DHT

mutex = Lock()
 
# create class for our Raspberry Pi GUI
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
		self.txtTemp.setText(_translate("MainWindow", '{0:0.1f} C'.format(temperature)))
		self.txtHumidity.setText(_translate("MainWindow", '{0:0.1f} %'.format(humidity)))
		mutex.release()
		
	# Button click handlers
	def pressedRefresh(self): 
		self.updateSensor()
		
	def pressedQuit(self):
		quit_app = True
		exit(1)
	
	# access variables inside of the UI's file
	def __init__(self):
		super(self.__class__, self).__init__()
		self.setupUi(self) # gets defined in the UI file

		# Button hooks
		self.btnRefresh.clicked.connect(lambda: self.pressedRefresh())
		self.btnQuit.clicked.connect(lambda: self.pressedQuit())
	
		# Period thread for automatic updates
		timerThread = threading.Thread(target=self.updateSensor_5s)
		timerThread.daemon = True
		timerThread.start()

quit_app = False
updating = False

# I feel better having one of these
def main():
	# a new app instance
	app = QApplication(sys.argv)
	form = MainWindow()
	form.show()
	#form.updateSensor_5s()
	# without this, the script exits immediately.
	sys.exit(app.exec_())
 
# python bit to figure how who started This
if __name__ == "__main__":
 	main()
