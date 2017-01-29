
#ifndef ENSISHELL_JOB_H
#define ENSISHELL_JOB_H

struct Job {
    int pid;
    char **cmd;
};

struct Job* newJob(int _pid, char **_cmd) {
    struct Job* job = malloc(sizeof(struct Job));
    job->pid = _pid;
    job->cmd = _cmd;
    return job;
}

#endif //ENSISHELL_JOB_H
