#include "pipe.h"
#include <stdlib.h>

struct Pipe* newPipe() {
    struct Pipe* pipes = (struct Pipe*) malloc(sizeof(struct Pipe));

    pipes->prev = NULL;
    pipes->next = NULL;

    return pipes;
}

struct Pipe* getNextPipe(struct Pipe* pipes) {
    pipes->next = newPipe();
    pipes->next->prev = pipes;

    return pipes->next;
}

void freePipes(struct Pipe* pipes) {
    struct Pipe* prev = pipes;
    struct Pipe* head = prev;

    while (prev != NULL) {
        struct Pipe* oldPrev = prev;
        prev = prev->prev;
        free(oldPrev);
    }

    free(head);
}
