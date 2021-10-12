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
#define topicName "/vrel/brijesh/temp/"

#define lastWillTopic "/lastwill/temp"
#define lastWillMessage "off"
#define mqttWelcomeMessage "on"
const char *messege = "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";

WiFiClient espClient;
PubSubClient client(espClient);

// Interrupt
const byte interruptPin = D2;
byte state = LOW;
boolean flag = true;

long startTimeMicros = 0;
long endTimeMicros = 0;
long deltaTime = 0;
long deltaTime2 = 0;

const int totalSampleToPublish = 5;
const int totalTopicToPublishEachTime = 10;

char buffer[150];
char buffer2[512];
int count = 0;

void reconnect()
{
    while (!client.connected())
    {
        // if (client.connect(MQTTClientName, mqtt_user, mqtt_password, lastWillTopic, 0, true, lastWillMessage))
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
        sprintf(buffer2, "%s", messege);
        for (int i = 1; i <= totalTopicToPublishEachTime; i++)
        {
            sprintf(buffer, "%s%d", topicName, i);
            client.publish(buffer, buffer2, false);
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
        reconnect();

    client.loop();
    if (count < totalSampleToPublish)
    {
        digitalWrite(interruptPin, state);
        mqttPublish();
        state = HIGH;
        digitalWrite(interruptPin, state);
        state = LOW;
        Serial.print("Messages published on multiple topic!");
        delay(5000);
        count++;
    }
}