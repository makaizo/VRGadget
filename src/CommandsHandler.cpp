#include "CommandsHandler.h"
#include <iostream>

CommandsHandler::CommandsHandler() 
    : is_heating(false), is_cooling(false), is_splashing(false) {
    atom_motion.Init();
    update_led_color();
    std::cout << "[Info] [CommandsHandler] initialized" << std::endl;
}

void CommandsHandler::start_heating() {
    atom_motion.SetMotorSpeed(PELTIER_CHANNEL, HEATING_VALUE);
    is_heating = true;
    is_cooling = false;
    update_led_color();
    std::cout << "[Info] [start_heating] command executed" << std::endl;
}

void CommandsHandler::finish_heating() {
    if (is_heating) {
        atom_motion.SetMotorSpeed(PELTIER_CHANNEL, STOP_VALUE);
        is_heating = false;
        update_led_color();
    }
    std::cout << "[Info] [finish_heating] command executed" << std::endl;
}

void CommandsHandler::start_cooling() {
    atom_motion.SetMotorSpeed(PELTIER_CHANNEL, COOLING_VALUE);
    is_cooling = true;
    is_heating = false;
    update_led_color();
    std::cout << "[Info] [start_cooling] command executed" << std::endl;
}

void CommandsHandler::finish_cooling() {
    if (is_cooling) {
        atom_motion.SetMotorSpeed(PELTIER_CHANNEL, STOP_VALUE);
        is_cooling = false;
        update_led_color();
    }
    std::cout << "[Info] [finish_cooling] command executed" << std::endl;
}

void CommandsHandler::start_splash() {
    atom_motion.SetMotorSpeed(SPLASH_CHANNEL, SPLASH_VALUE);
    is_splashing = true;
    update_led_color();
    std::cout << "[Info] [start_splash] command executed" << std::endl;
}

void CommandsHandler::finish_splash() {
    if (is_splashing) {
        atom_motion.SetMotorSpeed(SPLASH_CHANNEL, STOP_VALUE);
        is_splashing = false;
        update_led_color();
    }
    std::cout << "[Info] [finish_splash] command executed" << std::endl;
}

void CommandsHandler::update_led_color() {
    uint32_t color = LED_OFF;
    
    // Determine color based on current state combinations
    if (is_heating && is_splashing) {
        color = LED_YELLOW;  // Red + Green = Brown
    } else if (is_cooling && is_splashing) {
        color = LED_CYAN;  // Blue + Green = Emerald
    } else if (is_heating) {
        color = LED_RED;
    } else if (is_cooling) {
        color = LED_BLUE;
    } else if (is_splashing) {
        color = LED_GREEN;
    }
    
    M5.dis.drawpix(0, color);
}
