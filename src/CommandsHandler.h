#ifndef COMMANDS_HANDLER_H
#define COMMANDS_HANDLER_H

#include <string>

class CommandsHandler {
public:
    CommandsHandler();
    
    void start_heating();
    void finish_heating();
    void start_cooling();
    void finish_cooling();
    void start_splash();
    void finish_splash();
};

#endif // COMMANDS_HANDLER_H
