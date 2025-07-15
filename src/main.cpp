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
        Serial.println("[Error] Commands handler not initialized");
        return;
    }
    Serial.print("[Info] Handling command: ");
    Serial.println(command.c_str());
    
    if (command == "start_heating") {
        Serial.println("[Info] start_heating command");
        commands_handler->start_heating();
    }
    else if (command == "finish_heating") {
        Serial.println("[Info] finish_heating command");
        commands_handler->finish_heating();
    }
    else if (command == "start_cooling") {
        Serial.println("[Info] start_cooling command");
        commands_handler->start_cooling();
    }
    else if (command == "finish_cooling") {
        Serial.println("[Info] finish_cooling command");
        commands_handler->finish_cooling();
    }
    else if (command == "start_splash") {
        Serial.println("[Info] start_splash command");
        commands_handler->start_splash();
    }
    else if (command == "finish_splash") {
        Serial.println("[Info] finish_splash command");
        commands_handler->finish_splash();
    }
    else {
        Serial.print("[Error] Unknown command: ");
        Serial.println(command.c_str());
    }
}

void notify(const std::string& command) {
    call_command(command);
}

void initialize_wifi() {
    if (credentials.wifi_ssid.empty() || credentials.wifi_password.empty()) {
        Serial.println("[Error] WiFi credentials not loaded!");
        return;
    }
    
    Serial.print("[Info] Connecting to WiFi: ");
    Serial.println(credentials.wifi_ssid.c_str());
    WiFi.begin(credentials.wifi_ssid.c_str(), credentials.wifi_password.c_str());
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println();
    Serial.println("[Info] WiFi connected!");
    // Serial.print("[Info] IP address: ");
    // Serial.println(WiFi.localIP());
}

void initialize_mqtt(const std::string& mqtt_token) {
    mqtt_client.reset(new MQTTClient(mqtt_token, "VRGadget/command"));
    mqtt_client->subscribe(notify);
    mqtt_client->start();
}

void setup() {
    // Initialize M5Atom
    M5.begin(true, false, true);
    
    // Initialize Serial
    Serial.begin(115200);
    Serial.println("[Info] Start main");
    
    Serial.println("[Info] Load credentials");
    credentials = CredentialHandler::read_credentials("/credentials.json");
    if (credentials.mqtt_token.empty()) {
        Serial.println("[Error] MQTT token not loaded");
        Serial.println("[Fatal] Cannot continue without MQTT token. Halting.");
        while(true) {
            delay(1000); // Infinite loop to halt execution
        }
    }
    if (credentials.wifi_ssid.empty()) {
        Serial.println("[Error] WiFi SSID not loaded");
        Serial.println("[Fatal] Cannot continue without WiFi credentials. Halting.");
        while(true) {
            delay(1000); // Infinite loop to halt execution
        }
    }

    initialize_wifi();
    initialize_mqtt(credentials.mqtt_token);
    commands_handler.reset(new CommandsHandler());
    Serial.println("[Info] Setup complete, ready to receive commands");
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
