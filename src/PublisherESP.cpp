#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


#define wifi_ssid "BrijeshWiFi"
#define wifi_password "IoTlab32768"
#define mqtt_server "192.168.50.54"
#define mqtt_user "vrel"
#define mqtt_password "vrel2021"
#define MQTTClientName "brijesh"

#define topicName "/vrel/brijesh/temp"
#define lastWillTopic "vrel/brijesh/lastwill"
#define lastWillMessage "off"
#define mqttWelcomeMessage "on"
#define messege "Hello World"

WiFiClient espClient;
PubSubClient client(espClient);

// Interrupt
const byte interruptPin = 5;
volatile byte state = LOW;


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
    Serial.print("wifi connneccted!");
    client.setServer(mqtt_server, 1883);
    Serial.print("connecting To MQTT");
    delay(1000);

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
        Serial.print("client connected");
        if (count == 0)
        {
            sprintf(buffer, "%s" ,topicName);
            client.publish(buffer, String(messege).c_str(), false);
            state = !state;
            Serial.print("Message sent");
            digitalWrite(interruptPin, state);
            delay(5000);
            count = 1;
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