#include "CredentialHandler.h"
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>
#include <Arduino.h>

Credentials CredentialHandler::read_credentials(const std::string& file_path) {
    Credentials creds;
    
    // Initialize SPIFFS if not already initialized
    if (!SPIFFS.begin(true)) {
        Serial.println("[Error] Failed to mount SPIFFS");
        return creds;
    }
    
    // Open file for reading
    File file = SPIFFS.open(file_path.c_str(), "r");
    if (!file) {
        Serial.println("[Error] Failed to open credentials file");
        return creds;
    }
    
    // Read file content
    String fileContent = file.readString();
    file.close();
    
    // Parse JSON
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, fileContent);
    
    if (error) {
        Serial.print("[Error] Failed to parse JSON: ");
        Serial.println(error.c_str());
        return creds;
    }
    
    // Extract credentials
    if (doc["beebotteToken"].is<String>()) {
        creds.mqtt_token = doc["beebotteToken"].as<String>().c_str();
    }
    if (doc["WifiSSID"].is<String>()) {
        creds.wifi_ssid = doc["WifiSSID"].as<String>().c_str();
    }
    if (doc["WifiPassword"].is<String>()) {
        creds.wifi_password = doc["WifiPassword"].as<String>().c_str();
    }
    
    return creds;
}
