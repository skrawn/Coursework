
import json
import time
import boto3

from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient

class AWS_IOT():
    host = "a25ycfrxo9zs0g.iot.us-west-2.amazonaws.com"
    rootCAPath = "root-CA.crt"
    privateKeyPath = "DHT22_Server.private.key"
    certificatePath = "DHT22_Server.cert.pem" 
    mqttClient = None

    def AWSMQTT_Callback(self, client, userdata, message):
	    print("Received a new message: ")
	    print(message.payload)
	    print("from topic: ")
	    print(message.topic)
	    print("--------------\n\n")

    def SendData(self, temperature, humidity, timestamp):
        message = json.dumps({'temperature':temperature, 
            'humidity':humidity, 'timestamp': timestamp}, sort_keys=True,
            indent=4, separators=(',', ':'))
        print("Sending message: " + message)
        self.mqttClient.publish("sdk/test/dht22", message, 1)

    def Start(self):
        self.mqttClient = AWSIoTMQTTClient("dht22")
        self.mqttClient.configureEndpoint(self.host, 8883)
        self.mqttClient.configureCredentials(self.rootCAPath, self.privateKeyPath, self.certificatePath)

        # AWSIoTMQTTClient connection configuration - values from basicPubSub.py example
        self.mqttClient.configureAutoReconnectBackoffTime(1, 32, 20)
        self.mqttClient.configureOfflinePublishQueueing(-1)  # Infinite offline Publish queueing
        self.mqttClient.configureDrainingFrequency(2)  # Draining: 2 Hz
        self.mqttClient.configureConnectDisconnectTimeout(10)  # 10 sec
        self.mqttClient.configureMQTTOperationTimeout(5)  # 5 sec

        self.mqttClient.connect()
        self.mqttClient.subscribe("dht22_request", 1, self.AWSMQTT_Callback)

        # Purge the SQS queue
        sqs = boto3.client('sqs')
        sqs.purge_queue(QueueUrl="https://sqs.us-west-2.amazonaws.com/824840598937/DHT22_Queue")
