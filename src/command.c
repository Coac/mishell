#include "command.h"

struct Command* newCommand() {
    struct Command* commands = (struct Command*) malloc(sizeof(struct Command));

    commands->prev = NULL;
    commands->next = NULL;

    return commands;
}

struct Command* getNextCommand(struct Command* command) {
    command->next = newCommand();
    command->next->prev = command;

    return command->next;
}

void freeCommands(struct Command* command) {
    struct Command* current = command;

    while (current != NULL) {
        current = command->prev;
        free(command);
        command = current;
    }
}

struct Command* getFirstCommand(struct Command* command) {
    while (command->prev != NULL) {
        command = command->prev;
    }

    return command;
}
