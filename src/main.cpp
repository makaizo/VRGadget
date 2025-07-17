#include "MQTTClient.h"
#include "CommandsHandler.h"
#include "CredentialHandler.h"
#include <WiFi.h>
#include <M5Atom.h>
#include <memory>

// Global objects
std::unique_ptr<CommandsHandler> commands_handler;
std::unique_ptr<MQTTClient> mqtt_client;
Credentials credentials;

// Manual mode state management
enum class ManualMode : int {
    STOP = 0,
    COOLING = 1,
    HEATING = 2,
    SPLASH = 3,
    MAX_MODE = 3
};

ManualMode current_manual_mode = ManualMode::STOP;

// Constants
namespace Config {
    constexpr unsigned long SERIAL_BAUD_RATE = 115200;
    constexpr unsigned long WIFI_CONNECT_DELAY = 500;
    constexpr unsigned long MAIN_LOOP_DELAY = 100;
    constexpr unsigned long ERROR_HALT_DELAY = 1000;
    constexpr unsigned int MAX_WIFI_RETRIES = 5;
    constexpr const char* CREDENTIALS_FILE_PATH = "/credentials.json";
    constexpr const char* MQTT_TOPIC = "VRGadget/command";
}

// Command handling function - simplified version
void call_command(const std::string& command) {
    if (!commands_handler) {
        Serial.println("[Error] Commands handler not initialized");
        return;
    }
    
    Serial.print("[Info] Handling command: ");
    Serial.println(command.c_str());
    
    if (command == "start_heating") {
        Serial.println("[Info] Executing start_heating command");
        commands_handler->start_heating();
    }
    else if (command == "finish_heating") {
        Serial.println("[Info] Executing finish_heating command");
        commands_handler->finish_heating();
    }
    else if (command == "start_cooling") {
        Serial.println("[Info] Executing start_cooling command");
        commands_handler->start_cooling();
    }
    else if (command == "finish_cooling") {
        Serial.println("[Info] Executing finish_cooling command");
        commands_handler->finish_cooling();
    }
    else if (command == "start_splash") {
        Serial.println("[Info] Executing start_splash command");
        commands_handler->start_splash();
    }
    else if (command == "finish_splash") {
        Serial.println("[Info] Executing finish_splash command");
        commands_handler->finish_splash();
    }
    else {
        Serial.print("[Error] Unknown command: ");
        Serial.println(command.c_str());
    }
}

// MQTT callback function
void mqtt_command_callback(const std::string& command) {
    call_command(command);
}

// Initialization functions
bool initialize_serial() {
    Serial.begin(Config::SERIAL_BAUD_RATE);
    Serial.println("[Info] Serial communication initialized");
    Serial.println("[Info] VRGadget starting up...");
    return true;
}

bool load_credentials() {
    Serial.println("[Info] Loading credentials from file system");
    credentials = CredentialHandler::read_credentials(Config::CREDENTIALS_FILE_PATH);
    
    // Validate MQTT token
    if (credentials.mqtt_token.empty()) {
        Serial.println("[Error] MQTT token not loaded from credentials file");
        Serial.println("[Fatal] Cannot continue without MQTT token. System halted.");
        return false;
    }
    
    // Validate WiFi credentials
    if (credentials.wifi_ssid.empty() || credentials.wifi_password.empty()) {
        Serial.println("[Error] WiFi credentials not loaded from credentials file");
        Serial.println("[Fatal] Cannot continue without WiFi credentials. System halted.");
        return false;
    }
    
    Serial.println("[Info] Credentials loaded successfully");
    return true;
}

bool initialize_wifi() {
    Serial.print("[Info] Connecting to WiFi network: ");
    Serial.println(credentials.wifi_ssid.c_str());
    
    WiFi.begin(credentials.wifi_ssid.c_str(), credentials.wifi_password.c_str());
    
    // Wait for connection with visual feedback
    unsigned int retry_count = 0;
    while (WiFi.status() != WL_CONNECTED && retry_count < Config::MAX_WIFI_RETRIES) {
        delay(Config::WIFI_CONNECT_DELAY);
        Serial.print(".");
        retry_count++;
    }
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println();
        Serial.println("[Error] Failed to connect to WiFi network");
        return false;
    } else {
        Serial.println("[Info] Connected to WiFi network successfully");
        Serial.print("[Info] Device IP address: ");
        Serial.println(WiFi.localIP());
        return true;
    }
    
}

bool initialize_mqtt() {
    Serial.println("[Info] Initializing MQTT client");
    
    mqtt_client.reset(new MQTTClient(credentials.mqtt_token, Config::MQTT_TOPIC));
    mqtt_client->subscribe(mqtt_command_callback);
    if (mqtt_client->start()) {
        Serial.println("[Info] MQTT client initialized and started successfully");
        return true;
    } else {
        Serial.println("[Error] Failed to start MQTT client");
        return false;
    }
    
}

bool initialize_commands_handler() {
    Serial.println("[Info] Initializing commands handler");
    
    commands_handler.reset(new CommandsHandler());
    Serial.println("[Info] Commands handler initialized successfully");
    return true;
}

void handle_fatal_error(const char* error_message) {
    Serial.println(error_message);
    // Serial.println("[Fatal] System cannot continue. Entering infinite loop.");
    
    // while (true) {
    //     delay(Config::ERROR_HALT_DELAY);
    //     // System halted - could add LED indication here
    // }
}

// Manual mode management
void cycle_manual_mode() {
    int next_mode = static_cast<int>(current_manual_mode) + 1;
    if (next_mode > static_cast<int>(ManualMode::MAX_MODE)) {
        next_mode = static_cast<int>(ManualMode::STOP);
    }
    current_manual_mode = static_cast<ManualMode>(next_mode);
    
    Serial.print("[Info] Manual mode changed to: ");
    Serial.println(static_cast<int>(current_manual_mode));
}

void apply_manual_mode() {
    if (!commands_handler) {
        Serial.println("[Error] Cannot apply manual mode - commands handler not initialized");
        return;
    }
    
    switch (current_manual_mode) {
        case ManualMode::STOP:
            Serial.println("[Info] Manual mode: Stopping all operations");
            commands_handler->finish_heating();
            commands_handler->finish_cooling();
            commands_handler->finish_splash();
            break;
            
        case ManualMode::COOLING:
            Serial.println("[Info] Manual mode: Starting cooling");
            commands_handler->start_cooling();
            commands_handler->finish_heating();
            commands_handler->finish_splash();
            break;
            
        case ManualMode::HEATING:
            Serial.println("[Info] Manual mode: Starting heating");
            commands_handler->start_heating();
            commands_handler->finish_cooling();
            commands_handler->finish_splash();
            break;
            
        case ManualMode::SPLASH:
            Serial.println("[Info] Manual mode: Starting splash");
            commands_handler->start_splash();
            commands_handler->finish_heating();
            commands_handler->finish_cooling();
            break;
    }
}

void handle_button_press() {
    cycle_manual_mode();
    apply_manual_mode();
}

// Main Arduino functions
void setup() {
    // Initialize M5Atom hardware
    M5.begin(true, false, true);
    
    // Initialize serial communication
    if (!initialize_serial()) {
        handle_fatal_error("[Fatal] Failed to initialize serial communication");
    }
    
    // Load configuration credentials
    if (!load_credentials()) {
        handle_fatal_error("[Fatal] Failed to load credentials");
    }
    
    // Initialize WiFi connection
    if (!initialize_wifi()) {
        handle_fatal_error("[Fatal] Failed to initialize WiFi connection");
    }
    
    // Initialize MQTT client
    if (!initialize_mqtt()) {
        handle_fatal_error("[Fatal] Failed to initialize MQTT client");
    }
    
    // Initialize commands handler
    if (!initialize_commands_handler()) {
        handle_fatal_error("[Fatal] Failed to initialize commands handler");
    }
    
    Serial.println("[Info] ========================================");
    Serial.println("[Info] VRGadget setup completed successfully");
    Serial.println("[Info] System ready to receive commands");
    Serial.println("[Info] ========================================");
}

void loop() {
    // Update M5Atom hardware state
    M5.update();
    
    // Handle MQTT communication
    if (mqtt_client && mqtt_client->isConnected()) {
        mqtt_client->loop();
    } 
    // else if (mqtt_client) {
    //     Serial.println("[Error] MQTT client not connected");
    // }
    
    // Handle manual button control
    if (M5.Btn.wasPressed()) {
        handle_button_press();
    }
    
    // Small delay to prevent excessive CPU usage
    delay(Config::MAIN_LOOP_DELAY);
}
