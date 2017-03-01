#include "command.h"

struct command *new_command() {
    struct command *commands = (struct command *) malloc(sizeof(struct command));

    commands->prev = NULL;
    commands->next = NULL;

    return commands;
}

struct command *get_next_command(struct command *command) {
    command->next = new_command();
    command->next->prev = command;

    return command->next;
}

void free_commands(struct command *command) {
    struct command *current = command;

    while (current != NULL) {
        current = command->prev;
        free(command);
        command = current;
    }

    command = NULL;
}

struct command *get_first_command(struct command *command) {
    while (command->prev != NULL) {
        command = command->prev;
    }

    return command;
}
