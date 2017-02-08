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

    /* Display each command of the pipe */
    for (i = 0; l->seq[i] != 0; i++) {
        char **cmd = l->seq[i];
        printf("seq[%d]: ", i);
        for (j = 0; cmd[j] != 0; j++) {
            printf("'%s' ", cmd[j]);
        }

        if(strcmp(cmd[0], "jobs") == 0) {
            printJobs(jobList);
        }

        int childPid = fork();

        if (childPid < 0) {
            perror("fork:");
        }

        if (childPid == 0) {
            computeFileRedirection(l->in, l->out);
            execvp(cmd[0], cmd);
        }

        if(l->bg) {
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


int main()
{
    printf("Variante %d: %s\n", VARIANTE, VARIANTE_STRING);

#if USE_GUILE == 1
    scm_init_guile();
    /* register "executer" function in scheme */
    scm_c_define_gsubr("executer", 1, 0, 0, executer_wrapper);
#endif

    jobList = newJobNode(NULL);

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
