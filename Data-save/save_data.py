import paho.mqtt.client as mqttClient
import time


def on_connect(client, userdata, flags, rc):
    if rc == 0:

        print("Connected to broker")

        global Connected  # Use global variable
        Connected = True  # Signal connection

    else:

        print("Connection failed")


def on_message(client, userdata, message):
    print("message received  ", str(message.payload.decode("utf-8")), \
          "topic", message.topic)

    with open('data\\data1.txt', 'a+') as f:
        f.write("Message received: " + str(message.payload.decode("utf-8")) + "\n")
    f.close()


Connected = False  # global variable for the state of the connection

# broker_address = "192.168.50.54"
broker_address = "157.158.56.54"
port = 1883
user = "vrel"
# password = "vrel2021"
password = "vrel2018"

client = mqttClient.Client("BrijeshSUB1")  # create new instance
client.username_pw_set(user, password=password)  # set username and password
client.on_connect = on_connect  # attach function to callback
client.on_message = on_message  # attach function to callback
client.connect(broker_address, port, 60)  # connect
client.subscribe("vrel/brijesh/save")  # subscribe
# client.subscribe("$SYS/broker/store/messages/count")
# client.subscribe("$SYS/broker/store/messages/bytes")  # subscribe

print("subscribed")

client.loop_forever()# then keep listening forever
time.sleep(10)