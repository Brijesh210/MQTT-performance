import paho.mqtt.client as mqttClient
import time


def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to broker")
        global Connected
        Connected = True
    else:
        print("Connection failed")


def on_message(client, userdata, message):
    print("message received  ", str(message.payload.decode("utf-8")), "topic", message.topic)
    with open('data\\data3.txt', 'a+') as f:
        f.write(str(message.payload.decode("utf-8")) + "\n")
    f.close()


Connected = False

broker_address = "192.168.50.54"
# broker_address = "157.158.56.54"
port = 1883
user = "vrel"
password = "vrel2021"
# password = "vrel2018"

client = mqttClient.Client("BrijeshSUB5")
client.username_pw_set(user, password=password)
client.on_connect = on_connect
client.on_message = on_message
client.connect(broker_address, port, 60)
client.subscribe("vrel/brijesh/save")

print("subscribed")
client.loop_forever()
time.sleep(10)
