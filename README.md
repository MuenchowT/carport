# carport
esp8266 mqtt homie client

an esp8266 controller implementing the mqtt homie convention. Communication from/to mqtt server via w-lan.
the esp serves an 1-wire bus, and the items on the bus (for now, only temp-sensors) will be published to the mqtt server.
on the output side, a 4-relay bord is connected and can be switched via homie-mqtt-message.
