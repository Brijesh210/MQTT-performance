#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//198.168.50.254


#define wifi_ssid "BrijeshWiFi"
#define wifi_password "IoTlab32768"
#define mqtt_server "198.168.50.54"
#define mqtt_user "vrel"
#define mqtt_password "vrel2021"
#define MQTTClientName "brijesh"

#define topicName "/vrel/brijesh/temp"
#define lastWillTopic "iatlastwilltopic"
#define lastWillMessage "off"
#define mqttWelcomeMessage "on"

WiFiClient espClient;
PubSubClient client(espClient);

char buffer[30];
int n = 0;

// interrupt
const byte pin = 13;
const byte interruptPin = 5;
volatile byte state = LOW;
unsigned long startTimeMicros = 0;
unsigned long endTimeMicros = 0;
unsigned long deltaTime = 0;


void mqttCallback(char* topic, byte* payload, unsigned int length);

void setup()
{
    delay(10);
    Serial.begin(9600);
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.print("wifi connneccted!");
    client.setServer(mqtt_server, 1883);
    client.setCallback(mqttCallback);
    delay(1000);
    pinMode(interruptPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interruptPin),startTime, CHANGE);
}
void startTime(){
    startTimeMicros = micros();
}
void reconnect(){
    while (!client.connected())
    {
        if (client.connect(
                MQTTClientName, mqtt_user, mqtt_password, lastWillTopic, 0, true, lastWillMessage))
            client.subscribe(topicName);
        else
            delay(5000);
    }
    
}

void mqttCallback(char* topic, byte* payload, unsigned int length){
    endTimeMicros = micros();
    char nLine = topic[strlen(topic) - 1];
    n = atoi(&nLine);
    if (n >= 0 && n <= 3)
    { 
        for (int i = 0; i < length; i++)
        {
           Serial.print((char)payload[i]);
        }
    }
    deltaTime = startTimeMicros - endTimeMicros;
    Serial.print(deltaTime);

}
void loop(){
    if (!client.connected())
        reconnect();
    client.loop();
}