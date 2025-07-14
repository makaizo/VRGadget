#include "MQTTClient.h"
#include "CommandsHandler.h"
#include "CredentialHandler.h"
#include <WiFi.h>
#include <M5Atom.h>
#include <memory>

std::unique_ptr<CommandsHandler> commands_handler;
std::unique_ptr<MQTTClient> mqtt_client;
Credentials credentials;

void call_command(const std::string& command) {
    if (!commands_handler) {
        Serial.println("Commands handler not initialized");
        return;
    }
    Serial.print("Handling command: ");
    Serial.println(command.c_str());
    
    if (command == "start_heating") {
        Serial.println("start_heating command");
        commands_handler->start_heating();
    }
    else if (command == "finish_heating") {
        Serial.println("finish_heating command");
        commands_handler->finish_heating();
    }
    else if (command == "start_cooling") {
        Serial.println("start_cooling command");
        commands_handler->start_cooling();
    }
    else if (command == "finish_cooling") {
        Serial.println("finish_cooling command");
        commands_handler->finish_cooling();
    }
    else if (command == "start_splash") {
        Serial.println("start_splash command");
        commands_handler->start_splash();
    }
    else if (command == "finish_splash") {
        Serial.println("finish_splash command");
        commands_handler->finish_splash();
    }
    else {
        Serial.print("Unknown command: ");
        Serial.println(command.c_str());
    }
}

void notify(const std::string& command) {
    call_command(command);
}

void initialize_wifi() {
    if (credentials.wifi_ssid.empty() || credentials.wifi_password.empty()) {
        Serial.println("WiFi credentials not loaded!");
        return;
    }
    
    Serial.print("Connecting to WiFi: ");
    Serial.println(credentials.wifi_ssid.c_str());
    WiFi.begin(credentials.wifi_ssid.c_str(), credentials.wifi_password.c_str());
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println();
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void initialize_mqtt(const std::string& mqtt_token) {
    mqtt_client.reset(new MQTTClient(mqtt_token, "LC500/command"));
    mqtt_client->subscribe(notify);
    mqtt_client->start();
}

void setup() {
    // Initialize M5Atom
    M5.begin(true, false, true);
    
    // Initialize Serial
    Serial.begin(115200);
    Serial.println("Start main");
    
    // Load credentials from JSON file
    Serial.println("Loading credentials...");
    credentials = CredentialHandler::read_credentials("/credentials.json");
    
    if (credentials.mqtt_token.empty()) {
        Serial.println("Warning: MQTT token not loaded");
    } else {
        Serial.println("MQTT token loaded successfully");
    }
    
    if (credentials.wifi_ssid.empty()) {
        Serial.println("Warning: WiFi SSID not loaded");
    } else {
        Serial.print("WiFi SSID loaded: ");
        Serial.println(credentials.wifi_ssid.c_str());
    }
    
    // Initialize WiFi
    initialize_wifi();
    
    // Initialize MQTT
    initialize_mqtt(credentials.mqtt_token);
    
    // Initialize Commands handler
    commands_handler.reset(new CommandsHandler());
    
    Serial.println("MQTT Command Handler started.");
}

void loop() {
    // Update M5Atom
    M5.update();
    
    // Handle MQTT
    if (mqtt_client) {
        mqtt_client->loop();
    }
    
    delay(10);
}
