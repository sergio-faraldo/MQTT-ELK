# MQTT-ELK
En este trabajo se ha implementado un sistema que permite el envío de datos MQTT a un servidor centralizado de recogida de logs en la nube (Stack ELK), prestando especial atención a la seguridad de los datos, desde que salen del sensor hasta que llegan a la nube.

La securización se ha realizado aprovechando las opciones de cifrado TLS que soportan los protocolos usados, además de creando una red VPN entre el servidor en la nube y el servidor intermedio de recogida de datos MQTT.
