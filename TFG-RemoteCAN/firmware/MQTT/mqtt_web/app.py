from flask import Flask, render_template, jsonify, request
from flask_mqtt import Mqtt

app = Flask(__name__)

app.config['MQTT_BROKER_URL'] = '37.15.75.249'
app.config['MQTT_BROKER_PORT'] = 1883
app.config['MQTT_KEEPALIVE'] = 60  # Set KeepAlive time in seconds
#app.config['MQTT_TLS_ENABLED'] = False  
topic_speed = 'car/speed'
topic_rpm = 'car/rpm'
speed = 0
rpm = 0

mqtt_client = Mqtt(app)


@mqtt_client.on_connect()
def handle_connect(client, userdata, flags, rc):
    if rc == 0:
        print('Connected successfully')
        mqtt_client.subscribe(topic_rpm) 
        mqtt_client.subscribe(topic_speed) 
    else:
        print('Bad connection. Code:', rc)


@mqtt_client.on_message()
def handle_mqtt_message(client, userdata, message):
    global speed, rpm
    if message.topic == "car/speed":
        speed = int(message.payload.decode())
        print(speed)
    elif message.topic == "car/rpm":
        rpm = int(message.payload.decode())
        print(rpm)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/data', methods=['GET'])
def get_data():
    print(speed,rpm)
    return jsonify({
        'speed': speed,
        'rpm': rpm
    })

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
