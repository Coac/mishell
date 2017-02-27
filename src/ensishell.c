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

#ifndef VARIANTE
#error "Variante non défini !!"
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

struct JobNode* jobList;

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

char** stringArrayCopy(char** toCopy) {
    char **newStrArr = malloc(sizeof(toCopy));
    for (int l = 0; toCopy[l] != 0; l++) {
        newStrArr[l] = malloc( strlen( toCopy[l] ) + 1 );
        strcpy( newStrArr[l], toCopy[l] );

        printf("%s ", newStrArr[l]);
    }

    return newStrArr;
}

void computeFileRedirection(char* in, char* out) {
    if(out) {
        int outFile = open(out, O_RDWR | O_CREAT, 0666);
        dup2(outFile, STDOUT_FILENO);
        close(outFile);
    }

    if(in) {
        int inFile = open(in, O_RDONLY);
        dup2(inFile, STDIN_FILENO);
        close(inFile);
    }
}

void computePipes(int fds[][2], int current, bool isLast) {
    if (!isLast) { // if not last
        dup2(fds[current][1], 1); // we listen for the next
    }
    close(fds[current][0]);
    close(fds[current][1]);

    if (current != 0) { // if not first
        dup2(fds[current-1][0], 0); // we write on the pipe
        close(fds[current-1][0]);
        close(fds[current-1][1]); // we don't need to listen anymore
    }
}

int printCmd(int current, char **cmd) {
    int j;

    printf("seq[%d]: ", current);
    for (j = 0; cmd[j] != 0; j++) {
        printf("'%s' ", cmd[j]);
    }
    printf("\n");

    return j;
}

void computeCmd(struct cmdline *l) {
    int i, j;
    if (!l) {
        terminate(0);
    }

    if (l->err) {
        /* Syntax error, read another command */
        printf("error: %s\n", l->err);
        return;
    }

    if (l->in) printf("in: %s\n", l->in);
    if (l->out) printf("out: %s\n", l->out);
    if (l->bg) printf("background (&)\n");

    int fds[100][2];
    pid_t pids[100];
    /* Display each command of the pipe */
    for (i = 0; l->seq[i] != 0; i++) {
        char **cmd = l->seq[i];
        j = printCmd(i, cmd);

        if(strcmp(cmd[0], "jobs") == 0) {
            printJobs(jobList);
            continue;
        }

        pipe(fds[i]);
        pid_t childPid = fork();
        pids[i] = childPid;

        if (childPid < 0) {
            perror("fork:");
        }

        if (childPid == 0) {
            computeFileRedirection(l->in, l->out);
            computePipes(fds, i, l->seq[i+1] == 0);

            execvp(cmd[0], cmd);

            return;
        }

        if (i > 0) { // if not first, the i-1 pipe can be closed as it is not used anymore
            close(fds[i-1][0]);
            close(fds[i-1][1]);
        }
    }

    for (int k = 0; l->seq[k] != 0; k++) {
		char **cmd = l->seq[k];
    	pid_t childPid = pids[k];
		if(l->bg) {
			printf("%d", j);
			if(childPid > 0) {
			   struct Job* job = newJob(childPid, stringArrayCopy(cmd), j);
			   addJob(jobList, job);
			}
		} else {
			int status;
			waitpid(childPid, &status, 0);
		}
    }
}


#if USE_GUILE == 1

int question6_executer(char *line) {
    /* Question 6: Insert your code to execute the command line
     * identically to the standard execution scheme:
     * parsecmd, then fork+execvp, for a single command.
     * pipe and i/o redirection are not required.
     */
    struct cmdline *l = parsecmd(&line);
    computeCmd(l);
    return 0;
}

SCM executer_wrapper(SCM x) {
    return scm_from_int(question6_executer(scm_to_locale_stringn(x, 0)));
}

#endif

void SigchldHandler(int sig, siginfo_t *siginfo, void *context) {
    if(sig != SIGCHLD) return;

    pid_t pid = siginfo->si_pid;

    struct JobNode *current = jobList;
    while (current->next) {
        current = current->next;

        if(current->job->pid == pid) {
            removeJob(jobList, current);
            printf("Terminated child : %d\n", siginfo->si_pid);
            return;
        }
    }
}

int main()
{
    printf("Variante %d: %s\n", VARIANTE, VARIANTE_STRING);

#if USE_GUILE == 1
    scm_init_guile();
    /* register "executer" function in scheme */
    scm_c_define_gsubr("executer", 1, 0, 0, executer_wrapper);
#endif

    jobList = newJobNode(NULL);

    struct sigaction act;
    memset (&act, '\0', sizeof(act));
    act.sa_sigaction = &SigchldHandler;
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGCHLD, &act, NULL) < 0) {
        perror ("sigaction");
        return 1;
    }

    while (1) {
        struct cmdline *l;
        char *line = 0;
        char *prompt = "ensishell>";

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
        computeCmd(l);
    }
}
