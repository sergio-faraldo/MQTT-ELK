#!/bin/bash
/usr/bin/mosquitto_sub -h localhost -p 1883 -u user -P passwd --cafile /home/pi/ca.crt --insecure -t '#' -F '@FT@T@z,%t,%p' | /usr/bin/rotatelogs /home/pi/mqtt_messages/log 3600
