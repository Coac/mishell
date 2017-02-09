
#ifndef ENSISHELL_JOB_H
#define ENSISHELL_JOB_H

struct Job {
    int pid;
    char **cmd;
    int cmdCount;
};

struct Job* newJob(int _pid, char **_cmd, int _cmdCount) {
    struct Job* job = malloc(sizeof(struct Job));
    job->pid = _pid;
    job->cmd = _cmd;
    job->cmdCount = _cmdCount;
    return job;
}

void freeJob(struct Job* job) {
    char **jobCmd = job->cmd;
    for (int l = 0; l < job->cmdCount; l++) {
        free(jobCmd[l]);
    }
    free(job->cmd);
    job->cmd = NULL;
    free(job);
    job = NULL;
}


// Job Linked list
struct JobNode {
    struct Job* job;
    struct JobNode* next;
};

struct JobNode* newJobNode(struct Job* job) {
    struct JobNode* jobList = (struct JobNode*) malloc(sizeof(struct JobNode));
    jobList->next = NULL;
    jobList->job = job;
    return jobList;
}

void freeJobNode(struct JobNode *node) {
    freeJob(node->job);
    free(node);
    node = NULL;
}

void addJob(struct JobNode *head, struct Job *job) {
    struct JobNode *current = head;
    struct JobNode *newNode = newJobNode(job);

    while (current->next) {
        current = current->next;
    }
    current->next = newNode;
}

void removeJob(struct JobNode *head, struct JobNode *jobNode) {
    struct JobNode *current = head;
    struct JobNode *prev = NULL;
    while (current->next) {
        prev = current;
        current = current->next;

        if(current == jobNode) {
            if(current->next) {
                prev->next = current->next;
            } else {
                prev->next = NULL;
            }
            freeJobNode(current);
            return;
        }

    }

}

void printJobs(struct JobNode *head)
{
    printf("Job list : \n");

    struct JobNode *current = head;
    while (current->next) {
        current = current->next;
        struct Job* job = current->job;
        printf("%d ", job->pid);

        char **jobCmd = job->cmd;
        for (int l = 0; l < job->cmdCount; l++) {
            printf("%s ", jobCmd[l]);
        }

        int status;
        pid_t result = waitpid(job->pid, &status, WNOHANG);
        if (result == 0) {
            printf("alive");
        } else if (result == -1) {
            printf("error");
        } else {
            printf("exited");
            removeJob(head, current);
        }
        printf(" %d %d ", status, result);

        printf("\n");
    }

}


#endif //ENSISHELL_JOB_H
