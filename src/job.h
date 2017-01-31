
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

#endif //ENSISHELL_JOB_H
