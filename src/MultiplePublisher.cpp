//send multiple msgs, with unique msg and topic , sub to topic and check msg arrival time 
//todo : while publishig topic name yet not clear, (need to make constant since willdcard may be affected ,while)

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define wifi_ssid "internal.IOT"
#define wifi_password "IoTlab32768"
#define mqtt_server "192.168.90.5"
#define mqtt_user "vrel"
#define mqtt_password "vrel2018"
#define MQTTClientName "Brijesh"
#define topicName "/vrel/brijesh/temp/"
#define topicNameSub "/vrel/brijesh/temp/+"


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

char buffer[30];
char buffer2[30];
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
    delay(1000);
}
void reconnect()
{
    while (!client.connected())
    {
        if (client.connect(
                MQTTClientName, mqtt_user, mqtt_password, lastWillTopic, 0, true, lastWillMessage))
            client.subscribe(topicNameSub);
        else
            delay(5000);
    }
    Serial.print("Connected!");
}
void mqttCallback(char* topic, byte* payload, unsigned int length)
{
while(count  <= 100){   
    if (count == 99 && payload){
        end = millis();
        count = 0;
    }
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();
    count++;
}
}

void mqttPublish()
{
    if (client.connected())
    {
        if (flag == 1 ){
            for (int i = 1; i< 100 ; i++)
            {
                sprintf(buffer, "%s%d" ,topicName,i);
                sprintf(buffer2, "%s%d",message,i);
                client.publish(buffer , String(buffer2).c_str(), false);
            }
            flag =0;
            start = millis();
        }
    }
}

void loop()
{
    if (!client.connected())
        reconnect();
    client.loop();
    mqttPublish();
    execTime = start - end;
    Serial.print(execTime);

}