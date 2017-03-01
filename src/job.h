#ifndef ENSISHELL_JOB_H
#define ENSISHELL_JOB_H

/**
 * A job is a command running in background, described
 * by a process ID, a textual command and a command id.
 */
struct job {
    int pid;
    char **cmd;
    int cmd_count;
};

/**
 * A job node is a node from a chained list of jobs.
 */
struct job_node {
    struct job *job;
    struct job_node *next;
};

/**
 * Creates a new job data structure.
 *
 * @param pid process ID of the job
 * @param cmd textual command to be executed
 * @param cmd_count ID of the command
 * @return an initialized job
 */
struct job *new_job(int pid, char **cmd, int cmd_count);

/**
 * Destroys a job and free the memory.
 *
 * @param job job to be destroyed
 */
void free_job(struct job *job);

/**
 * Creates a jobs list from a job.
 *
 * @param job job to be appended to the new list
 * @return the job node
 */
struct job_node *new_job_node(struct job *job);

/**
 * Destroys a job node.
 *
 * @param node node to be destroyed
 */
void free_job_node(struct job_node *node);

/**
 * Appends a job to a jobs list.
 *
 * @param head head of the jobs list
 * @param job job to be appended to the list
 */
void add_job(struct job_node *head, struct job *job);

/**
 * Removes a job from a jobs list.
 *
 * @param head head of the jobs list
 * @param job job to be removed from the list
 */
void remove_job(struct job_node *head, struct job_node *job_node);

/**
 * Waits and removes all the jobs from a list.
 *
 * @param head head of the jobs list
 */
void remove_jobs(struct job_node *head);

#endif //ENSISHELL_JOB_H
