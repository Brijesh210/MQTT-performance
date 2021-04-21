#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


#define wifi_ssid "Ondraszek"
#define wifi_password "340@brijesh"
#define mqtt_server "157.158.56.54"
#define mqtt_user "vrel"
#define mqtt_password "vrel2018"
#define MQTTClientName "brijesh"

#define topicName "/vrel/brijesh/temp"
#define lastWillTopic "/vrel/brijesh/lastwill"
#define lastWillMessage "off"
#define mqttWelcomeMessage "on"
#define messege "Hello World"

WiFiClient espClient;
PubSubClient client(espClient);

// Interrupt
const byte interruptPin = D2;
int state = 0;


char buffer[30];
char buffer2[30];
int count = 0;

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
    pinMode(interruptPin, OUTPUT);
    Serial.print("wifi connneccted!");
    client.setServer(mqtt_server, 1883);
    Serial.print("connecting To MQTT");
    delay(1000);
    digitalWrite(interruptPin, LOW);
}

void reconnect()
{
    while (!client.connected())
    {
        if (client.connect(
                MQTTClientName, mqtt_user, mqtt_password, lastWillTopic, 0, true, lastWillMessage))
        {
            client.publish(lastWillTopic, mqttWelcomeMessage, true);
        }
        else
        {
            delay(5000);
            Serial.print("Conneting...");
        }
    }
    Serial.print("Mqtt Connected!");
}

void mqttPublish()
{
    if (client.connected())
    {
        {
            sprintf(buffer, "%s" ,topicName);
            client.publish(buffer, String(messege).c_str(), false);
            digitalWrite(interruptPin, HIGH);
            delay(5000);
            digitalWrite(interruptPin, LOW);
        }
    }
}
void loop()
{
    if (!client.connected())
        reconnect();

    client.loop();
    mqttPublish();    
}