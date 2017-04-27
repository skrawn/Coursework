
import tornado.httpserver
import tornado.websocket
import tornado.ioloop
import tornado.web

import paho.mqtt.client as mqtt

import asyncio
from concurrent.futures import ProcessPoolExecutor

import aiocoap.resource as resource
import aiocoap

import datetime
import logging

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

class BlockResource(resource.Resource):
	def __init__(self):
		super(BlockResource, self).__init__()
		self.content = ("This is the resource's default content. It is padded" \
			"with numbers to be large enough to trigger blockwise "\
			"transfer.\n" + "0123456789\n" * 100).encode("ascii")

	@asyncio.coroutine
	def render_get(self, request):
		return aiocoap.Message(payload=self.content)

	@asyncio.coroutine
	def render_put(self, request):
		print("PUT payload: %s" % request.payload)
		self.content = request.payload
		payload = ("I've accepted the new payload. You may inspect it here in "\
			"Python's repr format:\n\n%r" % self.content).encode("ascii")
		return aiocoap.Message(payload=payload)

class SeparateLargeResource(resource.Resource):
	def __init__(self):
		super(SeparateLargeResource, self).__init__()

	@asyncio.coroutine
	def render_get(self, request):
		yield from asyncio.sleep(3)
		payload = "Three rings for the elven kings under the sky, seven rings"\
			"for dwarven loards in their halls of stone, nine rings for"\
			"mortal men doomed to die, one ring for the dark lord on his"\
			"dark throne.".encode("ascii")
		return aiocoap.Message(payload=payload)
	

class TimeResource(resource.ObservableResource):
	def __init__(self):
		super(TimeResource, self).__init__()
		self.notify()

	def notify(self):
		self.updated_state()
		asyncio.get_event_loop().call_later(6, self.notify)

	def update_observation_count(self, count):
		if count:
			print("Keeping the lcock nearby to trigger observations")
		else:
			print("Stowing away the clock until someone asks again")

	@asyncio.coroutine
	def render_get(self, request):
		payload = datetime.datetime.now().strftime("%Y-%m-%d %H:%M").encode('ascii')
		return aiocoap.Message(payload=payload)


def on_connect(mqttc, obj, flags, rc):
	#print("rc: " + str(rc))
	mqttc.subscribe(mqttChannel)

def on_message(mqttc, obj, msg):
	#print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))
	# Paho MQTT appends a b and single quotes to the payload; need to strip that off
	message = str(msg.payload).strip('b')	
	message = message.strip("'")	
	if (message == "mqttTest"):
		mqttc.publish(mqttChannel, message + "Resp")		

def on_publish(mqttc, obj, mid):
	print("mid: " + str(mid))

def on_subscribe(mqttc, obj, mid, granted_qos):
	print("Subscribed: " + str(mid) + " " + str(granted_qos))


mqttc = mqtt.Client();
mqttc.on_message = on_message
mqttc.on_connect = on_connect
#mqttc.on_publish = on_publish
#mqttc.on_subscribe = on_subscribe
mqttChannel = "dingus"

application = tornado.web.Application([	
	(r'/', PageHandler),
	(r'/ws', WSHandler),])
database = TinyDB('project3.json')


if __name__ == "__main__":
	executor = ProcessPoolExecutor(2)
	http_server = tornado.httpserver.HTTPServer(application)
	http_server.listen(8888)
	mqttc.connect_async("iot.eclipse.org", 1883, 69)
	mqttc.loop_start()

	coapSite = resource.Site()
	coapSite.add_resource(('.well-known', 'core'), resource.WKCResource(coapSite.get_resources_as_linkheader))
	coapSite.add_resource(('time',), TimeResource())
	coapSite.add_resource(('other', 'block'), BlockResource())
	coapSite.add_resource(('other', 'separate'), SeparateLargeResource())

	loop = asyncio.get_event_loop()
	#asyncio.ensure_future(loop.run_in_executor(executor, aiocoap.Context.create_server_context(coapSite)))
	#asyncio.ensure_future(loop.run_in_executor(executor, tornado.ioloop.IOLoop.instance().start))
	#loop.run_forever()
	tasks = [
		asyncio.async(aiocoap.Context.create_server_context(coapSite)),
		asyncio.async(tornado.ioloop.IOLoop.instance().start())]
	loop.run_until_complete(asyncio.wait(tasks))
	loop.close()
	
	#asyncio.Task(aiocoap.Context.create_server_context(coapSite))
	#print("test1")
	#asyncio.get_event_loop().run_forever()
	#asyncio.get_event_loop().
	#print("test2")
	
