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

const char *messege = "12345678"; // 8 bytes
char *mes1;  // 1024 bytes = 1 kb

WiFiClient espClient;
PubSubClient client(espClient);

// Interrupt
const byte interruptPin = D2;
byte state = LOW;
boolean flag = true;
boolean flagw = true;
boolean flagwe = true;


const int totalSampleToPublish = 50;
const int LEVEL_1 = 1;
const int LEVEL_2 = 1;
const int LEVEL_3 = 1;

char topicBuffer[150];
char messageBuffer[2048];
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
        if(flagwe){
            Serial.println("");
            Serial.print("Hello");
            Serial.print(mes1);
            flagwe = false;
        }
        
        sprintf(messageBuffer, "%s", mes1);
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
    Serial.print("Override:");
    Serial.print(client.setBufferSize(1024));
    Serial.println();

    mes1 = (char*)malloc(1024*sizeof(char));
    char add = 'a';
    for (int i = 0; i < 1000; i++)
    {
        mes1[i] = add;
    }
  
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
        if (flagw){
            Serial.print("msg");
            Serial.print(mes1);
            flagw = false;
        }
        digitalWrite(interruptPin, state);
        mqttPublish();
        state = HIGH;
        digitalWrite(interruptPin, state);
        state = LOW;
        delay(3000);
        count++;
    }
}