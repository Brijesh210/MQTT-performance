#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//Not working at uni, (I haven't change client name at home , but try it with different name)
// #define wifi_ssid "BrijeshWiFi"
// #define wifi_password "IoTlab32768"
// #define mqtt_server "192.168.50.54"
// #define mqtt_user "vrel"
// #define mqtt_password "vrel2021"

// working 
#define wifi_ssid "Ondraszek"
#define wifi_password "340@brijesh"
#define mqtt_server "157.158.56.54"
#define mqtt_user "vrel"
#define mqtt_password "vrel2018"

// MQTT messages (change it to "BrijeshSUB1")
#define MQTTClientName "BrijeshSUB"
#define servoTopic "/vrel/brijesh/temp/+"

//MQTT last will
#define lastWillTopic "/sut/aeii/iot-open.eu/vrel/node9/state"
#define lastWillMessage "off"
#define mqttWelcomeMessage "on"

// WiFi & MQTT
WiFiClient espClient;
PubSubClient client(espClient);

//Time Variable
const byte interruptPin = D2;
int state = 0;
bool flag = true;
bool flagInterrupt = false;


unsigned long startTimeMicros = 0;
unsigned long endTimeMicros = 0;
unsigned long deltaTime = 0;
int n;

void setup_wifi() {
  delay(10);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);
  n=0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
}

void reconnect()
{
  while (!client.connected()) {
    if (client.connect(MQTTClientName, mqtt_user, mqtt_password, lastWillTopic, 0, true, lastWillMessage)) {
      client.subscribe(servoTopic);
      Serial.print("subcribed..");
    } else {
      delay(5000);
    }
  } 
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  if (startTimeMicros > 0 && endTimeMicros == 0 && flagInterrupt == true){
    if (strcmp(topic, servoTopic)== 0){
      endTimeMicros = millis();
      Serial.print("msg...");
    }
    endTimeMicros = millis();
    deltaTime = endTimeMicros - startTimeMicros;
    Serial.print("time: ");
    Serial.println(deltaTime);
    startTimeMicros = 0;
    endTimeMicros = 0;
    flagInterrupt = false;
    Serial.println("---");
  }
}

void IRAM_ATTR interruptHanddler(){
  startTimeMicros = millis();
  Serial.print("start...");
  flagInterrupt = true;
  endTimeMicros = 0;
}

void setup()
{
  delay(10);
  Serial.begin(9600);
  setup_wifi();
  Serial.println("WIFI connected");
  client.setServer(mqtt_server, 1883);
  client.setCallback(mqttCallback);

  // Interrupt
  delay(1000);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(interruptPin,interruptHanddler, CHANGE);
}

void loop()
{ 
  if (!client.connected()){
    reconnect();
  }
  client.loop();
}
