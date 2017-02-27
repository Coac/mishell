#ifndef ENSISHELL_PIPE_H
#define ENSISHELL_PIPE_H

struct Pipe {
    int fd[2];
    struct Pipe* prev;
    struct Pipe* next;
};

extern struct Pipe* newPipe();
extern struct Pipe* getNextPipe(struct Pipe* pipe);
extern void freePipes(struct Pipe* pipe);

#endif //ENSISHELL_PIPE_H
