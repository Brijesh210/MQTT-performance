// https://www.hivemq.com/blog/mqtt-essentials-part-5-mqtt-topics-best-practices/
// https://github.com/mqtt/mqtt.org/wiki/SYS-Topics
// single +
// mutli # 
// beginning $
// $SYS/#    ??? not sure, need to consider all data trafic need to consider (static topics)


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define wifi_ssid "internal.IOT"
#define wifi_password "IoTlab32768"
#define mqtt_server "192.168.90.5"
#define mqtt_user "vrel"
#define mqtt_password "vrel2018"
#define MQTTClientName "Brijesh"
#define message "welcome"
#define lastWillTopic "iatlastwilltopic"
#define lastWillMessage "off"
#define mqttWelcomeMessage "on"

//-----+----"1/+/2"

#define topicNameOne "/vrel/brijesh/"
#define topicNameTwo "/temp"
#define topicSubWildCard "/vrel/brijesh/+/temp"

//------#-----"1/#/2"

// #define topicNameOne "/vrel/brijesh/"
// #define topicNameTwo "/temp"
// #define topicSubWildCard "/vrel/#/temp"




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
            client.subscribe(topicSubWildCard);
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

//publishing unique msg over ,unique topic, every 100ms 
//  + , #
void mqttPublish()
{
    if (client.connected())
    {
        if (flag == 1 ){
            for (int i = 1; i< 100 ; i++)
            {
                sprintf(buffer, "%s%d%s" ,topicNameOne,i, topicNameTwo);
                sprintf(buffer2, "%s%d",message,i);
                client.publish(buffer , String(buffer2).c_str(), false);
                delay(100);
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