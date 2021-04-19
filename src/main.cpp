// simple pub , sub 

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define wifi_ssid "internal.IOT"
#define wifi_password "IoTlab32768"
#define mqtt_server "192.168.90.5"
#define mqtt_user "vrel"
#define mqtt_password "vrel2018"

#define MQTTClientName "Brijesh"
#define ldcTopicWithWildcard "/vrel/brijeshU4/LCD/+"

#define lastWillTopic "iatlastwilltopic"
#define lastWillMessage "off"
#define mqttWelcomeMessage "on"

WiFiClient espClient;
PubSubClient client(espClient);
LiquidCrystal_I2C lcd(0x3F, 20, 4);
char buffer[30];
int n = 0;
void mqttCallback(char* topic, byte* payload, unsigned int length);

void setup()
{
    delay(10);
    client.setServer(mqtt_server, 1883);
    client.setCallback(mqttCallback);
    lcd.init(D2, D1);
    lcd.backlight();
    lcd.home();
    delay(1000);
}
void reconnect()
{
    while (!client.connected())
    {
        if (client.connect(
                MQTTClientName, mqtt_user, mqtt_password, lastWillTopic, 0, true, lastWillMessage))
            client.subscribe(ldcTopicWithWildcard);
        else
            delay(5000);
    }
    lcd.setCursor(0, 1);
    lcd.print("Connected!");
}
void mqttCallback(char* topic, byte* payload, unsigned int length)
{
    char nLine = topic[strlen(topic) - 1];
    n = atoi(&nLine);
    if (n >= 0 && n <= 3)
    {
        lcd.setCursor(0, n);
        lcd.print(" ");
        for (int i = 0; i < length; i++)
        {
            lcd.setCursor(i, n);
            lcd.write((char)payload[i]);
        }
    }
}
void loop()
{
    if (!client.connected())
        reconnect();
    client.loop();
}