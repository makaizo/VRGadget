#ifndef COMMANDS_HANDLER_H
#define COMMANDS_HANDLER_H

#include <string>
#include <M5Atom.h>
#include "AtomMotion.h"

class CommandsHandler {
public:
    // Command types for better organization
    enum class CommandType {
        HEATING,
        COOLING,
        SPLASH
    };

    // Motor and LED configuration constants
    static constexpr uint8_t PELTIER_CHANNEL = 1;
    static constexpr uint8_t SPLASH_CHANNEL = 2;
    static constexpr int8_t HEATING_VALUE = -127;
    static constexpr int8_t COOLING_VALUE = 127;
    static constexpr int8_t SPLASH_VALUE = 127;
    static constexpr int8_t STOP_VALUE = 0;
    
    // LED colors
    static constexpr uint32_t LED_RED = 0xff0000;      // Heating only
    static constexpr uint32_t LED_BLUE = 0x0000ff;     // Cooling only
    static constexpr uint32_t LED_GREEN = 0x00ff00;    // Splash only
    static constexpr uint32_t LED_YELLOW = 0xffff00;    // Heating + Splash (red + green)
    static constexpr uint32_t LED_CYAN = 0x00ffff;  // Cooling + Splash (blue + green)
    static constexpr uint32_t LED_OFF = 0x000000;      // Off

    CommandsHandler();
    
    // Public interface methods
    void start_heating();
    void finish_heating();
    void start_cooling();
    void finish_cooling();
    void start_splash();
    void finish_splash();
    
    // Status query methods
    bool is_heating_active() const { return is_heating; }
    bool is_cooling_active() const { return is_cooling; }
    bool is_splash_active() const { return is_splashing; }

private:
    // State variables
    bool is_heating;
    bool is_cooling;
    bool is_splashing;
    
    // Hardware interface
    AtomMotion atom_motion;
    
    // Helper method to update LED based on current state
    void update_led_color();
};

#endif // COMMANDS_HANDLER_H
