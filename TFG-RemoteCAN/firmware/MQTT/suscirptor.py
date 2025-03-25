import paho.mqtt.client as mqtt

# Callback cuando el cliente recibe un mensaje
def on_message(client, userdata, message):
    print(f"Mensaje recibido en {message.topic}: {message.payload.decode()}")

# Configurar el cliente MQTT
client = mqtt.Client()
client.on_message = on_message  # Asignar la función de callback

# Conectar al broker (localhost si lo tienes en tu máquina)
client.connect("37.15.75.44", 1883, 60)

# Suscribirse a un tópico (por ejemplo, "test/topic")
client.subscribe("test/topic")

# Mantener el cliente en ejecución
print("Esperando mensajes...")
client.loop_forever()
