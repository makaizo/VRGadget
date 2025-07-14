#include "CommandsHandler.h"
#include <iostream>

CommandsHandler::CommandsHandler() {
    // Constructor - no initialization needed
}

void CommandsHandler::start_heating() {
    std::cout << "[start_heating] command executed" << std::endl;
}

void CommandsHandler::finish_heating() {
    std::cout << "[finish_heating] command executed" << std::endl;
}

void CommandsHandler::start_cooling() {
    std::cout << "[start_cooling] command executed" << std::endl;
}

void CommandsHandler::finish_cooling() {
    std::cout << "[finish_cooling] command executed" << std::endl;
}

void CommandsHandler::start_splash() {
    std::cout << "[start_splash] command executed" << std::endl;
}

void CommandsHandler::finish_splash() {
    std::cout << "[finish_splash] command executed" << std::endl;
}
