#ifndef ENSISHELL_JOB_H
#define ENSISHELL_JOB_H

struct Job {
    int pid;
    char **cmd;
    int cmdCount;
};

// Job Linked list
struct JobNode {
    struct Job* job;
    struct JobNode* next;
};

struct Job* newJob(int _pid, char **_cmd, int _cmdCount);
void freeJob(struct Job* job);
struct JobNode* newJobNode(struct Job* job);
void freeJobNode(struct JobNode *node);
void addJob(struct JobNode *head, struct Job *job);
void removeJob(struct JobNode *head, struct JobNode *jobNode);
void printJobs(struct JobNode *head);

#endif //ENSISHELL_JOB_H
