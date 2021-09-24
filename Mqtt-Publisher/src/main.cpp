#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Uni
#define wifi_ssid "BrijeshWiFi"
#define wifi_password "IoTlab32768"
#define mqtt_server "192.168.50.54"
#define mqtt_user "vrel"
#define mqtt_password "vrel2021"

// #define wifi_ssid "Kujawska"
// #define wifi_password "17@brijesh"
// #define mqtt_server "157.158.56.54"
// #define mqtt_user "vrel"
// #define mqtt_password "vrel2018"

// MQTT topic name (change it to" BrijeshPUB1")
#define MQTTClientName "BrijeshPUB"
#define topicName "/vrel/brijesh/temp/"


#define lastWillTopic "/lastwill/temp"
#define lastWillMessage "off"
#define mqttWelcomeMessage "on"
const char *messege = "hello world";

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

char buffer[150];
char buffer2[512];
int count = 0;

void reconnect()
{
    while (!client.connected())
    {
        if (client.connect(
                MQTTClientName, mqtt_user, mqtt_password, lastWillTopic, 0, true, lastWillMessage))
        {
            client.publish(lastWillTopic, mqttWelcomeMessage, false);
        }
        else
        {
            delay(5000);
            Serial.print("Conneting...");
        }
    }
    Serial.print("Mqtt Connected!");
}

void mqttPublish(int i)
{   
    if (client.connected())
    {
    //     for( int i = 1; i <= 100 ; i++){
    //         sprintf(buffer, "%s%d" ,topicName, i);
    //         sprintf(buffer2, "%d" , i);
    //         client.publish(buffer, buffer2, false);
    //    }

        sprintf(buffer, "%s%d" ,topicName, 1);
        // Initial file save
        //sprintf(buffer2, "%s" , messege);
        client.publish(buffer, buffer2, false);

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
      delay(100);
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
        for(int i = 1 ; i <= 50; ++i){
            digitalWrite(interruptPin, state);
            mqttPublish(i);
            delay(1000);
            if (state == LOW)
                state = HIGH;
            else
                state = LOW;
            flag = false;
        }
    }
}

/*
## Limitations

 -removed -It can only publish QoS 0 messages. It can subscribe at QoS 0 or QoS 1.
 - It can subscribe at QoS 0 or 1.
 - It can publish at QoS 0, 1 or 2. WARNING! No retransmission is supported to
   keep the library as much memory friendly as possible. (Without retransmission
   support, the publish QoS is only meaningful when the broker sends your
   message to a subscriber, supposing that the subscriber subscribes with a QoS
   greater then or equal to the publish QoS; consider that MQTT runs over TCP,
   so retransmission isn't really required in most cases, especially when
   publishing to the broker)
 - The maximum message size, including header, is **128 bytes** by default. This
   is configurable via `MQTT_MAX_PACKET_SIZE` in `PubSubClient.h`.
 - The keepalive interval is set to 15 seconds by default. This is configurable
*/