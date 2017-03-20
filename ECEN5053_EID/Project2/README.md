## Project 2

This project demonstrates the connection between an RPi with a DHT22 and a second RPi with a web interface. The sensor RPi stores the latest, maximum, minimum, and average samples from the DHT22 in a database (tinyDB). Using a tornado webserver, the sensor RPi creates a host interface for the client RPi to open up a connection that displays the values available from the sensor RPi database.
