import paho.mqtt.client as mqtt
import random

# Configuración del broker
BROKER = "37.15.75.249"  # Cambia esto si usas un broker externo
PUERTO = 1883
TOPICO = "car/speed"
MENSAJE = random.randint(20, 120)

# Crear cliente MQTT
cliente = mqtt.Client()

# Conectar al broker
cliente.connect(BROKER, PUERTO, 60)

# Publicar mensaje
cliente.publish(TOPICO, MENSAJE)

# Cerrar conexión
cliente.disconnect()

print(f"Mensaje enviado al tópico '{TOPICO}': {MENSAJE}")
