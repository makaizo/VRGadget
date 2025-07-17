#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <string>
#include <functional>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

class MQTTClient {
private:
    WiFiClient wifi_client;
    PubSubClient mqtt_client;
    std::string broker_address;
    int port;
    std::string subscribe_topic;
    std::string mqtt_token;
    std::function<void(const std::string&)> notify_callback;
    
    static void on_message_callback(char* topic, byte* payload, unsigned int length);
    static MQTTClient* instance; // For static callback
    
    bool reconnect();

public:
    MQTTClient(const std::string& mqtt_token, const std::string& subscribe_topic);
    ~MQTTClient();
    
    void subscribe(std::function<void(const std::string&)> callback);
    bool start();
    void stop();
    void publish(const std::string& topic, const std::string& data);
    void loop();
    bool isConnected();
};

#endif // MQTT_CLIENT_H
