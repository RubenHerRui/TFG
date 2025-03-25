import paho.mqtt.client as mqtt

# Configuración del broker
BROKER = "37.15.75.44"  # Cambia esto si usas un broker externo
PUERTO = 1883
TOPICO = "test/topic"
MENSAJE = "¡Hola desde Python!"

# Crear cliente MQTT
cliente = mqtt.Client()

# Conectar al broker
cliente.connect(BROKER, PUERTO, 60)

# Publicar mensaje
cliente.publish(TOPICO, MENSAJE)

# Cerrar conexión
cliente.disconnect()

print(f"Mensaje enviado al tópico '{TOPICO}': {MENSAJE}")
