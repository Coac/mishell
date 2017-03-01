#ifndef ENSISHELL_PIPE_H
#define ENSISHELL_PIPE_H

#include <stdlib.h>

/**
 * A command is a node in a double chained list containing a
 * textual command, a file descriptor used to read and write
 * data, a process ID and pointers to the previous and the
 * next command in the list.
 */
struct command {
    int fd[2];
    struct command *prev;
    struct command *next;
    pid_t pid;
    char **cmd;
};

/**
 * Creates a new allocated command structure.
 *
 * @return the new command
 */
extern struct command *new_command();

/**
 * Creates and returns a new command defined as the
 * next command.
 *
 * @param command the current command
 * @return the next command ; prev is pointing to the command passed as an argument
 */
extern struct command *get_next_command(struct command *command);

/**
 * Destroy and frees all the commands, from the passed one to the
 * first one.
 *
 * @param command the last command of the list
 */
extern void free_commands(struct command *command);

/**
 * Gets the first command of the list.
 *
 * @param command the current command node
 * @return the head of the commands list
 */
extern struct command *get_first_command(struct command *command);

#endif //ENSISHELL_PIPE_H
