//check Latency only for 1 massge publish and subscribe by 1 sub



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
#define topicName "/vrel/brijesh/temp"

#define message "welcome"
#define lastWillTopic "iatlastwilltopic"
#define lastWillMessage "off"
#define mqttWelcomeMessage "on"

unsigned long start;
unsigned long end;
unsigned long execTime;
int flag = 1;

WiFiClient espClient;
PubSubClient client(espClient);
LiquidCrystal_I2C lcd(0x3F, 20, 4);
char buffer[30];
int n = 0;
int count = 1;
void mqttCallback(char* topic, byte* payload, unsigned int length);

void setup()
{
    delay(10);
    
    Serial.begin(9600);
    client.setServer(mqtt_server, 1883);
    client.setCallback(mqttCallback);
    Serial.print("connecting...");
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
            client.subscribe(topicName);
        else
            delay(5000);
    }
    lcd.setCursor(0, 0);
    lcd.print("Connected!");
}
void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  if (count == 1 && payload){
    end = millis();
    count = 0;
  }
  sprintf(buffer,"Received!");
  lcd.setCursor(7,1);
  lcd.print(buffer);
  for (int i = 0; i < length; i++)
  {
      lcd.setCursor(i, 2);
      lcd.write((char)payload[i]);
  }
}

void mqttPublish()
{
    if (client.connected())
    {
        if(flag == 1){
          start = millis();
          client.publish(topicName, String(message).c_str(), false);
          sprintf(buffer,"published!");
          lcd.setCursor(0,1);
          lcd.print(buffer);
        }
    }
}

void loop()
{
    if (!client.connected())
        reconnect();
    client.loop();
    mqttPublish();
    flag = 0;
    
    execTime = start - end;
    sprintf(buffer,"time = %2.2f", execTime);
    lcd.setCursor(0,3);
    lcd.print(buffer);
}