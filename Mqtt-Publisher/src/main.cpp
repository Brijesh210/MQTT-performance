#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// University
// #define wifi_ssid "BrijeshWiFi"
// #define wifi_password "IoTlab32768"
// #define mqtt_server "192.168.50.54"
// #define mqtt_user "vrel"
// #define mqtt_password "vrel2021"

// Home
#define wifi_ssid "Kujawska"
#define wifi_password "17@brijesh"
#define mqtt_server "157.158.56.54"
#define mqtt_user "vrel"
#define mqtt_password "vrel2018"

// MQTT topic name (change it to" BrijeshPUB1")
#define MQTTClientName "BrijeshPUB"
#define topicName "/vrel/"
#define topicSeprator "/"

const char *messege = "1vhvkjhkvvvkjhiiimv";

WiFiClient espClient;
PubSubClient client(espClient);

// Interrupt
const byte interruptPin = D2;
byte state = LOW;
boolean flag = true;

const int totalSampleToPublish = 50;
const int LEVEL_1 = 1;
const int LEVEL_2 = 1;
const int LEVEL_3 = 1;

char topicBuffer[150];
char messageBuffer[512];
int count = 0;

void reconnect()
{
    while (!client.connected())
    {
        if (client.connect(MQTTClientName, mqtt_user, mqtt_password))
        {
            Serial.print("Mqtt Connected!");
        }
        else
        {
            delay(5000);
            Serial.print("Conneting...");
        }
    }
}

void mqttPublish()
{
    if (client.connected())
    {
        sprintf(messageBuffer, "%s", messege);
        for (int i = 1; i <= LEVEL_1; ++i)
        {
            for (int j = 1; j <= LEVEL_2; ++j)
            {
                for (int k = 1; k <= LEVEL_3; ++k)
                {
                    sprintf(topicBuffer, "%s%d%s%d%s%d", topicName, i, topicSeprator, j, topicSeprator, k);
                    client.publish(topicBuffer, messageBuffer, false);
                }
            }
        }
    }
}
void setup()
{
    //Serial
    delay(10);
    Serial.begin(9600);

    //WIFI
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(100);
        Serial.print(".");
    }

    Serial.print("wifi connneccted!");
    delay(1000);

    //Interrupt pin Mode
    pinMode(interruptPin, OUTPUT);
    //digitalWrite(interruptPin, LOW);

    client.setServer(mqtt_server, 1883);
}
void loop()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();
    if (count < totalSampleToPublish)
    {
        digitalWrite(interruptPin, state);
        mqttPublish();
        state = HIGH;
        digitalWrite(interruptPin, state);
        state = LOW;
        delay(1000);
        count++;
    }
}