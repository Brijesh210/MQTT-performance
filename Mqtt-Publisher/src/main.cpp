#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Uni
// #define wifi_ssid "BrijeshWiFi"
// #define wifi_password "IoTlab32768"
// #define mqtt_server "192.168.50.54"
// #define mqtt_user "vrel"
// #define mqtt_password "vrel2021"

#define wifi_ssid "Ondraszek"
#define wifi_password "340@brijesh"
#define mqtt_server "157.158.56.54"
#define mqtt_user "vrel"
#define mqtt_password "vrel2018"

// MQTT topic name (change it to" BrijeshPUB1")
#define MQTTClientName "BrijeshPUB"
#define topicName "/vrel/brijesh/temp/"


#define lastWillTopic "/lastwill/temp"
#define lastWillMessage "off"
#define mqttWelcomeMessage "on"
#define messege "Hello World"

WiFiClient espClient;
PubSubClient client(espClient);

// Interrupt
const byte interruptPin = D2;
byte state = LOW;
boolean flag = true;

long startTimeMicros = 0;
long endTimeMicros = 0;
long deltaTime = 0;
long deltaTime2 = 0;

char buffer[30];
char buffer2[30];
int count = 0;

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
        for(int i = 0 ; i < 100 ; i++){
            sprintf(buffer, "%s%d" ,topicName, i);
            client.publish(buffer, String(millis()).c_str(), false);
        }
        // digitalWrite(interruptPin, state);
        // Serial.println("msg sent");
        // delay(1000);
        // if (state == HIGH ){
        //     state = LOW;
        // }else{
        //     state = HIGH;
        // }
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
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    
    Serial.print("wifi connneccted!");
    delay(1000);

    //Interrupt pin Mode
    pinMode(interruptPin, OUTPUT);
    //digitalWrite(interruptPin, LOW);

    //MQTT
    client.setServer(mqtt_server, 1883);
}
void loop()
{
    if (!client.connected())
        reconnect();

    client.loop();
    if(flag){
        for(int i = 0 ; i < 10; ++i){
            digitalWrite(interruptPin, state);
            mqttPublish();
            if (state == LOW)
                state = HIGH;
            else
                state = LOW;
            delay(500);
            flag = false;
        }
    }
}