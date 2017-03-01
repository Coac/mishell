#include "job.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

struct job *new_job(int pid, char **cmd, int cmd_count) {
    struct job *job = malloc(sizeof(struct job));
    job->pid = pid;
    job->cmd = cmd;
    job->cmd_count = cmd_count;

    return job;
}

void free_job(struct job *job) {
    char **jobCmd = job->cmd;
    for (int l = 0; l < job->cmd_count; l++) {
        free(jobCmd[l]);
    }
    free(job->cmd);
    job->cmd = NULL;
    free(job);
    job = NULL;
}

struct job_node *new_job_node(struct job *job) {
    struct job_node *jobList = (struct job_node *) malloc(sizeof(struct job_node));
    jobList->next = NULL;
    jobList->job = job;

    return jobList;
}

void free_job_node(struct job_node *node) {
    free_job(node->job);
    free(node);
    node = NULL;
}

void add_job(struct job_node *head, struct job *job) {
    struct job_node *current = head;
    struct job_node *newNode = new_job_node(job);

    while (current->next) {
        current = current->next;
    }

    current->next = newNode;
}

void remove_job(struct job_node *head, struct job_node *job_node) {
    struct job_node *current = head;
    struct job_node *prev = NULL;
    while (current->next) {
        prev = current;
        current = current->next;

        if (current == job_node) {
            if (current->next) {
                prev->next = current->next;
            } else {
                prev->next = NULL;
            }
            free_job_node(current);

            return;
        }

    }
}

void remove_jobs(struct job_node *head) {
    printf("Job list : \n");

    struct job_node *current = head;
    while (current->next) {
        current = current->next;
        struct job *job = current->job;
        printf("%d ", job->pid);

        char **jobCmd = job->cmd;
        for (int l = 0; l < job->cmd_count; l++) {
            printf("%s ", jobCmd[l]);
        }

        int status;
        pid_t result = waitpid(job->pid, &status, WNOHANG);
        if (result == 0) {
            printf("alive");
        } else if (result == -1) {
            if (errno == ECHILD) {
                perror("Error ECHILD!!");
            } else if (errno == EINTR) {
                perror("Error EINTR!!");
            } else {
                perror(" Error EINVAL!!");
            }
        } else {
            printf("exited");
            remove_job(head, current);
        }
        printf("\n");
    }
}
