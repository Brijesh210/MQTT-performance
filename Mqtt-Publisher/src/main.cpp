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

// MQTT topic name, for each new connection change from to" BrijeshPUB1"
#define MQTTClientName "BrijeshPUB"
#define topic_name "brijesh/mqtt/hello"
#define total_sample 20
#define payload_size 10

const char *messege = "12345678"; // 8 bytes
char *mes1;

WiFiClient espClient;
PubSubClient client(espClient);

// Interrupt
const byte interruptPin = D2;
byte state = LOW;
boolean flag = true;

char messageBuffer[payload_size + 1];
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
        client.publish(topic_name, messageBuffer, false);
        Serial.println("sent");
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
    client.setBufferSize((payload_size * 2) + 256);

    // mes1 = (char *)malloc(payload_size * sizeof(char));
    char add = 'a';
    for (int i = 0; i < payload_size; i++)
    {
        messageBuffer[i] = add;
    }
    // sprintf(messageBuffer, "%s", mes1);
    // free(mes1);
    // mes1 = NULL;
}
void loop()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();
    if (count < total_sample)
    {
        digitalWrite(interruptPin, state);
        mqttPublish();
        if (state == HIGH)
        {
            state = LOW;
        }
        else
        {
            state = HIGH;
        }
        delay(3000);
        count++;
    }
}