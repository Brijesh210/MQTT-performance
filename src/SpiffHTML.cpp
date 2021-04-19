#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "SSD1306.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "WiFiClient.h"
#include "SPIFFS.h"

// sendinf spiff file to websocket
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

//198.168.50.254



#define wifi_ssid "internal.IOT"
#define wifi_password "IoTlab32768"
#define mqtt_server "192.168.90.5"
#define mqtt_user "vrel"
#define mqtt_password "vrel2018"
#define MQTTClientName "Brijesh"
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
const byte interruptPin = 2;
volatile byte state = LOW;

// SPIFF html
IPAddress local_IP(192, 168, 2, 104); // Set your Static IP address
IPAddress gateway(192, 168, 1, 1);    // Set your Gateway IP address
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //option

AsyncWebServer server(80);


void mqttCallback(char* topic, byte* payload, unsigned int length);
void startTime();
void setup()
{
    Serial.begin(9600);
    delay(10);
    client.setServer(mqtt_server, 1883);
    client.setCallback(mqttCallback);
    delay(1000);
    pinMode(interruptPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interruptPin),startTime, CHANGE);

    // spiff 
    while (WiFi.status() != WL_CONNECTED){
        delay(2000);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println(WiFi.localIP());
    if (!SPIFFS.begin()) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

}
void startTime(){
    int startTime = millis();
}
void reconnect()
{
    while (!client.connected())
    {
        if (client.connect(
                MQTTClientName, mqtt_user, mqtt_password, lastWillTopic, 0, true, lastWillMessage))
            client.subscribe(topicName);
        else
            delay(5000);
    }
    
}

void mqttCallback(char* topic, byte* payload, unsigned int length)
{
    if (strcmp(String(topic),topicName)==0){
    }

    
    char nLine = topic[strlen(topic) - 1];
    n = atoi(&nLine);
    if (n >= 0 && n <= 3)
    { 
        for (int i = 0; i < length; i++)
        {
           Serial.print((char)payload[i]);
        }
    }
}
void loop()
{
    if (!client.connected())
        reconnect();
    client.loop();
}