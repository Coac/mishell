#ifndef ENSISHELL_PIPE_H
#define ENSISHELL_PIPE_H

#include <stdlib.h>

struct Command {
    int fd[2];
    struct Command* prev;
    struct Command* next;
    pid_t pid;
    char **cmd;
};

extern struct Command* newCommand();
extern struct Command* getNextCommand(struct Command *command);
extern void freeCommands(struct Command *command);
extern struct Command* getFirstCommand(struct Command* command);

#endif //ENSISHELL_PIPE_H
