
import tornado.httpserver
import tornado.websocket
import tornado.ioloop
import tornado.web

import paho.mqtt.client as mqtt

from tinydb import TinyDB, Query

class PageHandler(tornado.web.RequestHandler):
	def get(self):
		self.render("client.html")

class WSHandler(tornado.websocket.WebSocketHandler):
	def check_origin(self, origin):
		return True

	def open(self):
		print("user is connected")

	def on_message(self, message):
		global database
		#print("receive messaged", message)
		dbQ = Query()
		if (message == "getCurrTemp"):
			db_data = database.get(dbQ['entry'] == 'curr')
			if (db_data is not None):
				self.write_message("Current Temperature: " + '{0} C'.format(db_data['tempC']))

		elif (message == "getCurrHum"):
			db_data = database.get(dbQ['entry'] == 'curr')
			if (db_data is not None):
				self.write_message("Current Humidity: " + '{0} %'.format(db_data['humidity']))

		elif (message == "getMaxTemp"):
			db_data = database.get(dbQ['entry'] == 'max')
			if (db_data is not None):
				self.write_message("Max Temperature: " + '{0} C'.format(db_data['tempC']))

		elif (message == "getMaxHum"):
			db_data = database.get(dbQ['entry'] == 'max')
			if (db_data is not None):
				self.write_message("Max Humidity: " + '{0} %'.format(db_data['humidity']))

		elif (message == "getMinTemp"):
			db_data = database.get(dbQ['entry'] == 'min')
			if (db_data is not None):
				self.write_message("Min Temperature: " + '{0} C'.format(db_data['tempC']))

		elif (message == "getMinHum"):
			db_data = database.get(dbQ['entry'] == 'min')
			if (db_data is not None):
				self.write_message("Min Humidity: " + '{0} %'.format(db_data['humidity']))

		elif (message == "getAvgTemp"):
			db_data = database.get(dbQ['entry'] == 'avg')
			if (db_data is not None):
				self.write_message("Average Temperature: " + '{0} C'.format(db_data['tempC']))

		elif (message == "getAvgHum"):
			db_data = database.get(dbQ['entry'] == 'avg')
			if (db_data is not None):
				self.write_message("Average Humidity: " + '{0} %'.format(db_data['humidity']))

		elif (message == "websocketTest"):
			self.write_message(message)

		elif (message == "mqttTest"):
			self.write_message(message)
			
		else:
			self.write_message("Unknown command" + message)

	def on_close(self):
		print("connection closed")
		

def on_connect(mqttc, obj, flags, rc):
	print("rc: " + str(rc))
	mqttc.subscribe("dingus")

def on_message(mqttc, obj, msg):
	print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))

def on_publish(mqttc, obj, mid):
	print("mid: " + str(mid))

def on_subscribe(mqttc, obj, mid, granted_qos):
	print("Subscribed: " + str(mid) + " " + str(granted_qos))


mqttc = mqtt.Client();
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_publish = on_publish
mqttc.on_subscribe = on_subscribe

application = tornado.web.Application([	
	(r'/', PageHandler),
	(r'/ws', WSHandler),])
database = TinyDB('project3.json')


if __name__ == "__main__":
	http_server = tornado.httpserver.HTTPServer(application)
	http_server.listen(8888)
	mqttc.connect_async("iot.eclipse.org", 1883, 69)
	mqttc.loop_start()
	tornado.ioloop.IOLoop.instance().start()
