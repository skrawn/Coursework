# always seem to need this
import sys
 
# This gets the Qt stuff
import PyQt5
from PyQt5.QtWidgets import *
 
# This is our window from QtCreator
import mainwindow_auto

import Adafruit_DHT
 
# create class for our Raspberry Pi GUI
class MainWindow(QMainWindow, mainwindow_auto.Ui_MainWindow):
	
	# Button click handlers
	def pressedRefresh(self):
		humidity, temperature = Adafruit_DHT.read_retry(Adafruit_DHT.DHT22, 4)
		txtTemp.setText("clicked")
	
	def pressedQuit(self):
		pass

	# access variables inside of the UI's file
	def __init__(self):
		super(self.__class__, self).__init__()
		self.setupUi(self) # gets defined in the UI file

		# Button hooks
		self.btnRefresh.clicked.connect(lambda: self.pressedRefresh())
		self.btnQuit.clicked.connect(lambda: self.pressedQuit())
 
# I feel better having one of these
def main():
 	# a new app instance
 	app = QApplication(sys.argv)
 	form = MainWindow()
 	form.show()
 	# without this, the script exits immediately.
 	sys.exit(app.exec_())
 
# python bit to figure how who started This
if __name__ == "__main__":
 	main()
