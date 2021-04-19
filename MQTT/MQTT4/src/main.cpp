#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define wifi_ssid "Ondraszek"
#define wifi_password "340@brijesh"

#define mqtt_server "157.158.56.54"
#define mqtt_user "vrel"
#define mqtt_password "vrel2018"

// MQTT messages
#define MQTTClientName "BrijeshFromVREL9"
#define servoTopic "/vrel/brijesh/temp"

//MQTT last will
#define lastWillTopic "/sut/aeii/iot-open.eu/vrel/node9/state"
#define lastWillMessage "off"
#define mqttWelcomeMessage "on"

// WiFi & MQTT
WiFiClient espClient;
PubSubClient client(espClient);

//Time Variable
const byte interruptPin = 5;
volatile byte state = LOW;
unsigned long startTimeMicros = 0;
unsigned long endTimeMicros = 0;
unsigned long deltaTime = 0;
int n;

void setup_wifi() {
  delay(10);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);
  n=0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if (client.connect(MQTTClientName, mqtt_user, mqtt_password, lastWillTopic, 0, true, lastWillMessage)) {
      client.subscribe(servoTopic);
      Serial.print("subcribed..");
    } else {
      delay(5000);
    }
  } 
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
  Serial.println();
}

void setup()
{
  delay(10);
  Serial.begin(9600);
  setup_wifi();
  Serial.println("WIFI connected");

  client.setServer(mqtt_server, 1883);
  client.setCallback(mqttCallback);
}

void loop()
{ 
  if (!client.connected())
    reconnect();
  client.loop();
}