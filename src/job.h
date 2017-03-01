#ifndef ENSISHELL_JOB_H
#define ENSISHELL_JOB_H

struct job {
    int pid;
    char **cmd;
    int cmd_count;
};

// Job Linked list
struct job_node {
    struct job* job;
    struct job_node* next;
};

struct job* new_job(int pid, char **cmd, int cmd_count);
void free_job(struct job *job);
struct job_node* new_job_node(struct job *job);
void free_job_node(struct job_node *node);
void add_job(struct job_node *head, struct job *job);
void remove_job(struct job_node *head, struct job_node *job_node);
void remove_jobs(struct job_node *head);

#endif //ENSISHELL_JOB_H
