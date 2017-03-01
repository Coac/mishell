#ifndef ENSISHELL_PIPE_H
#define ENSISHELL_PIPE_H

#include <stdlib.h>

struct command {
    int fd[2];
    struct command* prev;
    struct command* next;
    pid_t pid;
    char **cmd;
};

extern struct command* new_command();
extern struct command* get_next_command(struct command *command);
extern void free_commands(struct command *command);
extern struct command* get_first_command(struct command *command);

#endif //ENSISHELL_PIPE_H
