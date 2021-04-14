#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

//send multiple msgs, with unique msg and topic , sub to topic and check msg arrival time 
//todo : while publishig topic name yet not clear, (need to make constant since willdcard may be affected ,while)

#include <Arduino.h>
#include <PubSubClient.h>

#define wifi_ssid "internal.IOT"
#define wifi_password "IoTlab32768"
#define mqtt_server "192.168.90.5"
#define mqtt_user "vrel"
#define mqtt_password "vrel2018"
#define MQTTClientName "Brijesh"
#define topicName "/vrel/brijesh/temp/"
#define topicNameSub "/vrel/brijesh/temp/+"


#define message ""
#define lastWillTopic "iatlastwilltopic"
#define lastWillMessage "off"
#define mqttWelcomeMessage "on"

unsigned long start;
unsigned long end;
unsigned long execTime;

unsigned long stampEnd = 0;
unsigned long stampStart =0;
unsigned long stampTime =0;

int temp; 
int flag = 1;


WiFiClient espClient;
PubSubClient client(espClient);

char buffer[30];
char buffer2[30];
char buffer3[30];
int numberOfSensor = 100;

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
        if (client.connect(MQTTClientName, mqtt_user, mqtt_password, lastWillTopic, 0, true, lastWillMessage))
            for (int i=0; i < numberOfSensor; i++){
                sprintf(buffer3, "%s%d" ,topicName,i); 
                client.subscribe(buffer3);  
            }
        else
            delay(5000);
      
    }
    Serial.print("Connected!");
}
void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  
while(count  <= numberOfSensor){  
//    total time till (publishing , to subcribing 
//    if (count == numberOfSensor -1){
//        end = millis();
//        count = 0;
//    }
    
//    if (strcmp(String(topic),"pir1Status")==0){
//      
//    }
    stampEnd = millis();
    memcpy(&stampStart, payload, sizeof(unsigned long));
    stampTime = stampEnd-stampStart;
    Serial.println(stampTime);
    count++;
}
}

void mqttPublish()
{
    if (client.connected())
    {
        if (flag == 1 ){
            for (int i = 1; i< numberOfSensor ; i++)
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
