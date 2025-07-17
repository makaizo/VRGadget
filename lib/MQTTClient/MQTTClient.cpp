#include "MQTTClient.h"
#include <iostream>

// Static instance for callback
MQTTClient* MQTTClient::instance = nullptr;

MQTTClient::MQTTClient(const std::string& mqtt_token, const std::string& subscribe_topic)
    : mqtt_client(wifi_client), broker_address("mqtt.beebotte.com"), port(1883), 
      subscribe_topic(subscribe_topic), mqtt_token(mqtt_token) {
    
    // Set static instance for callback
    instance = this;
    
    // Configure MQTT client
    mqtt_client.setServer(broker_address.c_str(), port);
    mqtt_client.setCallback(on_message_callback);
}

MQTTClient::~MQTTClient() {
    stop();
    instance = nullptr;
}

void MQTTClient::on_message_callback(char* topic, byte* payload, unsigned int length) {
    if (instance == nullptr) return;
    
    // Convert payload to string
    std::string message;
    message.reserve(length);
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    Serial.print("Received message: ");
    Serial.println(message.c_str());
    
    // Parse JSON and extract command
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, message);
    
    if (error) {
        Serial.print("JSON parsing failed: ");
        Serial.println(error.c_str());
        return;
    }
    
    if (doc["data"].is<std::string>()) {
        std::string command = doc["data"].as<std::string>();
        if (!command.empty()) {
            if (instance->notify_callback) {
                instance->notify_callback(command);
            }
        }
    }
}

void MQTTClient::subscribe(std::function<void(const std::string&)> callback) {
    notify_callback = callback;
}

bool MQTTClient::reconnect() {
    unsigned int retry_count = 0;
    while (!mqtt_client.connected()) {
        Serial.print("Attempting MQTT connection...");
        
        // Create a random client ID
        String clientId = "ESP32Client-";
        clientId += String(random(0xffff), HEX);
        
        // Attempt to connect with username (token) and no password
        if (mqtt_client.connect(clientId.c_str(), mqtt_token.c_str(), "")) {
            Serial.println("connected");
            
            // Subscribe to topic
            mqtt_client.subscribe(subscribe_topic.c_str());
            Serial.print("Subscribed to: ");
            Serial.println(subscribe_topic.c_str());
            
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
        if (retry_count >= 5) {
            Serial.println("[Error] Maximum MQTT connection retries reached and stops reconnet.");
            return false;
        }
        retry_count++;
    }
    return true;
}

bool MQTTClient::start() {
    if (!WiFi.isConnected()) {
        Serial.println("WiFi not connected. Cannot start MQTT client.");
        return false;
    }
    
    Serial.print("Connecting to MQTT broker at ");
    Serial.print(broker_address.c_str());
    Serial.print(":");
    Serial.println(port);
    
    if (reconnect()) {
        Serial.println("MQTT client connected successfully");
        return true;
    } else {
        Serial.println("MQTT client failed to connect");
        return false;
    }

}

void MQTTClient::stop() {
    if (mqtt_client.connected()) {
        Serial.println("Stopping MQTT client...");
        mqtt_client.disconnect();
    }
}

void MQTTClient::publish(const std::string& topic, const std::string& data) {
    if (mqtt_client.connected()) {
        mqtt_client.publish(topic.c_str(), data.c_str());
    } else {
        Serial.println("MQTT client not connected. Cannot publish message.");
    }
}

void MQTTClient::loop() {
    if (!mqtt_client.connected()) {
        if (reconnect()) {
            Serial.println("MQTT client reconnected successfully");
        } else {
            Serial.println("MQTT client failed to reconnect");
        }
    }
    mqtt_client.loop();
}

bool MQTTClient::isConnected() {
    return mqtt_client.connected();
}
