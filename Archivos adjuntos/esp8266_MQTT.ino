#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHTesp.h"

/************************* WiFi *********************************/

#define WLAN_SSID       "MOVISTAR_A316"
#define WLAN_PASS       "*************"

/************************* MQTT *********************************/

#define IP_BROKER       "192.168.1.46"
#define PUERTO_BROKER   1883
#define USUARIO         "user"
#define PASSWORD        "passwd"

// Feeds de humedad y temperatura
#define FEED_HUM        "/feeds/0/humid"
#define FEED_TEM        "/feeds/0/temp"

// SHA1 fingerprint
static const char *fingerprint PROGMEM = "34 09 42 B2 91 97 58 DA EC EB 13 D0 FC 41 5A 0C 27 91 77 26";

// Cambio mínimo necesario para que se envíe un mensaje MQTT
#define HUM_DELTA 1
#define TEM_DELTA 0.1


// Tiempo entre envíos (milisegundos)
#define SLEEP_TIME 60000
/************************** x **********************************/

// WiFiFlientSecure (TLS)
WiFiClientSecure client;

// Cliente MQTT
Adafruit_MQTT_Client mqtt(&client, IP_BROKER, PUERTO_BROKER, USUARIO, PASSWORD);

// Sensor de Humedad y temperatura
DHTesp dht;

// Feeds
Adafruit_MQTT_Publish humid = Adafruit_MQTT_Publish(&mqtt, USUARIO FEED_HUM);
Adafruit_MQTT_Publish temp = Adafruit_MQTT_Publish(&mqtt, USUARIO FEED_TEM);

// Valores auxiliares
float prevhum;
float prevtemp;

/************************** x **********************************/

void setup() {
  Serial.begin(115200);
  delay(10);
  
  // Conexión WiFi
  Serial.println(); Serial.println();
  Serial.print("Conectando a ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  delay(2000);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("Conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Comparar fingerprint
  client.setFingerprint(fingerprint);
  
  // Conexión del sensor DHT (GPIO 12)
  dht.setup(12, DHTesp::DHT11);

  // set variables auxiliares al mínimo
  prevhum = 0;
  prevtemp = -273;
}

void loop() {
  // Lectura de sensores
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  // Si los valores han cambiado suficiente
  if(prevhum-humidity>HUM_DELTA || humidity-prevhum>HUM_DELTA || prevtemp-temperature>TEM_DELTA || temperature-prevtemp>TEM_DELTA){
    
    // Asegurarse de que aún hay conexión MQTT
    MQTT_connect();
  
    Serial.print(F("\nEnviando valores: Temp="));
    Serial.print(temperature);
    Serial.print(", humid=");
    Serial.println(humidity);
    if (humid.publish(humidity) && temp.publish(temperature)) {
      Serial.println(F("OK"));
        // asigno los valores actuales a prevhum y prevtemp
        prevhum = humidity;
        prevtemp = temperature;
    } else {
      Serial.println(F("ERROR"));
    }
  }
  
  // espero SLEEP_TIME minuto antes de volver a ejecutar el loop
  delay(SLEEP_TIME);

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Estableciendo conexión MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }

  Serial.println("Conectado al broker MQTT");
}
