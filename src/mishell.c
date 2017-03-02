/*****************************************************
 * Copyright Grégory Mounié 2008-2015                *
 *           Simon Nieuviarts 2002-2009              *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "variante.h"
#include "readcmd.h"
#include "job.h"
#include "command.h"

#ifndef VARIANTE
#error "Variante non défini !!"
#endif

#ifndef MISHELL_DEBUG
#define MISHELL_DEBUG 0
#endif

/* Guile (1.8 and 2.0) is auto-detected by cmake */
/* To disable Scheme interpreter (Guile support), comment the
 * following lines.  You may also have to comment related pkg-config
 * lines in CMakeLists.txt.
 */
#if USE_GUILE == 1

#include <libguile.h>
#include <fcntl.h>
#include <stdbool.h>
#include <bits/siginfo.h>

#endif

struct job_node *jobList;

void terminate(char *line) {
#if USE_GNU_READLINE == 1
    /* rl_clear_history() does not exist yet in centOS 6 */
    clear_history();
#endif
    if (line)
        free(line);
    printf("exit\n");
    exit(0);
}

/**
 * Copies an array of strings to another one.
 *
 * @param to_copy array of strings to be copied
 * @return the new array of strings
 */
char **string_array_copy(char **to_copy) {
    char **new_str_arr = malloc(sizeof(to_copy));
    for (int l = 0; to_copy[l] != 0; l++) {
        new_str_arr[l] = malloc(strlen(to_copy[l]) + 1);
        strcpy(new_str_arr[l], to_copy[l]);

        printf("%s ", new_str_arr[l]);
    }

    return new_str_arr;
}

/**
 * Initialize the input and output streams to read
 * and/or write to files.
 *
 * @param in nullable input file
 * @param out nullable output file
 */
void compute_file_redirection(char *in, char *out) {
    if (out) {
        int outFile = open(out, O_RDWR | O_CREAT, 0666);
        dup2(outFile, STDOUT_FILENO);
        close(outFile);
    }

    if (in) {
        int inFile = open(in, O_RDONLY);
        dup2(inFile, STDIN_FILENO);
        close(inFile);
    }
}

/**
 * Initialize the input and output streams for commands
 * to allow chained pipes.
 *
 * @param command command to be connected to the pipes
 * @param isLast indicates whether the command is the last one or not
 */
void compute_pipes(struct command *command, bool isLast) {
    if (!isLast) {
        dup2(command->fd[1], STDOUT_FILENO); // we listen for the next command
    }
    close(command->fd[0]);
    close(command->fd[1]);

    if (command->prev != NULL) {
        dup2(command->prev->fd[0], STDIN_FILENO); // we write on the previous command pipe
        close(command->prev->fd[0]);
        close(command->prev->fd[1]);
    }
}

/**
 * Prints the user entered command, can be useful for debugging.
 *
 * @param current current sequence id
 * @param cmd command and arguments to be displayed
 * @return the total number of command and arguments
 */
int print_cmd(int current, char **cmd) {
    int j;

#if MISHELL_DEBUG == 1
    printf("seq[%d]: ", current);
#endif
    for (j = 0; cmd[j] != 0; j++) {
#if MISHELL_DEBUG == 1
        printf("'%s' ", cmd[j]);
#endif
    }
#if MISHELL_DEBUG == 1
    printf("\n");
#endif

    return j;
}

/**
 * Reads the entered command and tries to run it.
 *
 * @param l the entered textual command
 */
void compute_cmd(struct cmdline *l) {
    int i, j;
    if (!l) {
        terminate(0);
    }

    if (l->err) {
        /* Syntax error, read another command */
        printf("error: %s\n", l->err);
        return;
    }

#if MISHELL_DEBUG == 1
    if (l->in) printf("in: %s\n", l->in);
    if (l->out) printf("out: %s\n", l->out);
    if (l->bg) printf("background (&)\n");
#endif

    struct command *command = new_command();
    command->pid = 0;
    /* Display each command of the pipe */
    for (i = 0; l->seq[i] != 0; i++) {
        command->ind_cmd = i;
        char **cmd = l->seq[i];
        j = print_cmd(i, cmd);

        if (strcmp(cmd[0], "jobs") == 0) {
            remove_jobs(jobList);
            return;
        }

        pipe(command->fd);
        pid_t child_pid = fork();
        command->pid = child_pid;
        if (child_pid < 0) {
            perror("fork:");
        }

        if (child_pid == 0) {
            compute_file_redirection(l->in, l->out);
            compute_pipes(command, l->seq[i + 1] == 0);
            execvp(cmd[0], cmd);

            exit(EXIT_FAILURE);
        }

        if (command->prev != NULL) { // if not first, the i-1 pipe can be closed as it is not used anymore
            close(command->prev->fd[0]);
            close(command->prev->fd[1]);
        }

        command = get_next_command(command);
    }

    command = get_first_command(command);

    while (command->next != NULL) {
        if (command->pid > 0) {
            if (l->bg) {
                struct job *job = new_job(command->pid, string_array_copy(l->seq[command->ind_cmd]), j);
                add_job(jobList, job);

                printf("\n");
            } else {
                int status;
                waitpid(command->pid, &status, 0);
            }
        }

        command = command->next;
    }

    free_commands(command);
}


#if USE_GUILE == 1

int question6_executer(char *line) {
    /* Question 6: Insert your code to execute the command line
     * identically to the standard execution scheme:
     * parsecmd, then fork+execvp, for a single command.
     * pipe and i/o redirection are not required.
     */
    struct cmdline *l = parsecmd(&line);
    compute_cmd(l);
    return 0;
}

SCM executer_wrapper(SCM x) {
    return scm_from_int(question6_executer(scm_to_locale_stringn(x, 0)));
}

#endif

/**
 * Callback for asynchronous commands.
 */
void sigchld_handler(int sig, siginfo_t *siginfo, void *context) {
    if (sig != SIGCHLD) return;

    pid_t pid = siginfo->si_pid;

    struct job_node *current = jobList;
    while (current->next) {
        current = current->next;

        if (current->job->pid == pid) {
            remove_job(jobList, current);
            printf("Terminated child : %d\n", siginfo->si_pid);
            return;
        }
    }
}

int main() {
    printf("Variante %d: %s\n", VARIANTE, VARIANTE_STRING);

#if USE_GUILE == 1
    scm_init_guile();
    /* register "executer" function in scheme */
    scm_c_define_gsubr("executer", 1, 0, 0, executer_wrapper);
#endif

    jobList = new_job_node(NULL);

    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_sigaction = &sigchld_handler;
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGCHLD, &act, NULL) < 0) {
        perror("sigaction");
        return 1;
    }

    while (1) {
        struct cmdline *l;
        char *line = 0;
        char *prompt = "mishell ⚡ ";

        /* Readline use some internal memory structure that
           can not be cleaned at the end of the program. Thus
           one memory leak per command seems unavoidable yet */
        line = readline(prompt);
        if (line == 0 || !strncmp(line, "exit", 4)) {
            terminate(line);
        }

#if USE_GNU_READLINE == 1
        add_history(line);
#endif


#if USE_GUILE == 1
        /* The line is a scheme command */
        if (line[0] == '(') {
            char catchligne[strlen(line) + 256];
            sprintf(catchligne,
                    "(catch #t (lambda () %s) (lambda (key . parameters) (display \"mauvaise expression/bug en scheme\n\")))",
                    line);
            scm_eval_string(scm_from_locale_string(catchligne));
            free(line);
            continue;
        }
#endif

        /* parsecmd free line and set it up to 0 */
        l = parsecmd(&line);
        compute_cmd(l);
    }
}
