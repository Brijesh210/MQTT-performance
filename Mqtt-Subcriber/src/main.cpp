#include <ArduinoJson.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// University
#define wifi_ssid "BrijeshWiFi"
#define wifi_password "IoTlab32768"
#define mqtt_server "192.168.50.54"
#define mqtt_user "vrel"
#define mqtt_password "vrel2021"

// Home
// #define wifi_ssid "Kujawska"
// #define wifi_password "17@brijesh"
// #define mqtt_server "157.158.56.54"
// #define mqtt_user "vrel"
// #define mqtt_password "vrel2018"

// MQTT messages (change it to "BrijeshSUB1")
#define MQTTClientName "BrijeshSUB7"
#define topicToSubscribe "brijesh/mqtt/hello/+"
#define topicForSave "vrel/brijesh/save"

//Data point
#define totalSampleToCollect 20
#define payload_size 1
#define topic_size 8192

WiFiClient espClient;
PubSubClient client(espClient);
const byte interruptPin = D2;

unsigned long startTimeMicros = 0;
unsigned long endTimeMicros = 0;
unsigned long deltaTime = 0;
int sampleCounter = 0;
int count = 0;

char bufferSave1[100];
char bufferSave2[6000];

StaticJsonDocument<6000> doc;
char JSONBuffer[6000];
JsonArray ResultValues = doc.createNestedArray("Results");

void setup_wifi()
{
    delay(10);
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }
}

void reconnect()
{
    while (!client.connected())
    {
        if (client.connect(MQTTClientName, mqtt_user, mqtt_password))
        {
            client.subscribe(topicToSubscribe);
            Serial.print("subcribed..");
        }
        else
        {
            delay(5000);
        }
    }
}

void mqttPublish()
{
    if (client.connected())
    {
        delay(10);
        sprintf(bufferSave1, "%s", topicForSave);
        sprintf(bufferSave2, "%s", JSONBuffer);
        client.publish(bufferSave1, bufferSave2, false);
    }
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    if (startTimeMicros > 0 && endTimeMicros == 0)
    {
        count++;
        if (count == topic_size)
        {
            endTimeMicros = millis();
            deltaTime = endTimeMicros - startTimeMicros;

            Serial.print("time: ");
            Serial.println(deltaTime);


            ResultValues.add(deltaTime);

            startTimeMicros = 0;
            endTimeMicros = 0;
            deltaTime = 0;

            Serial.println();
            sampleCounter++;
            count = 0;
        }
        if (sampleCounter == totalSampleToCollect)
        {
            serializeJson(doc, JSONBuffer);
            Serial.println();
            serializeJson(doc, Serial);
            mqttPublish();
            Serial.println("Published");
            ResultValues.clear();
            doc.garbageCollect();
            sampleCounter = 0;
        }
    }
}

void IRAM_ATTR interruptHanddler()
{
  startTimeMicros = millis();
}

void setup()
{
    delay(10);
    Serial.begin(9600);
    setup_wifi();
    Serial.println("WIFI connected");
    client.setServer(mqtt_server, 1883);
    client.setBufferSize((payload_size * 2) + 256);

    delay(100);
    client.setCallback(mqttCallback);
    
    delay(1000);
    pinMode(interruptPin, INPUT_PULLUP);
    attachInterrupt(interruptPin, interruptHanddler, CHANGE);
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();
}
