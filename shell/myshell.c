#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#define BACKGROUND '&'
#define PIPE '|'



/* ERROR MESSAGES */

/* This function prints an error message for fork() function */
void fork_failed_msg(){
    perror("Error: fork() failed");
}

/* This function prints an error message for pipe() function */
void pipe_failed_msg(){
    perror("Error: pipe() failed");
}

/* This function prints an error message for execvp() function */
void execution_failed_msg(){
    perror("Error: execvp() failed");
}

/* This function prints an error message for dup2() function */
void dup_failed_msg(){
    perror("Error: dup2() failed");
}

/* This function prints an error message for sigaction() function */
void sigaction_failed_msg(){
    perror("Error: sigaction() failed");
}

/* EXECUTIONS */

/*
 * This function executes a process in the background.
 */
int exec_background_process(int count, char **arglist){
    arglist[count-1] = NULL;
    int pid = fork();
    if (pid < 0) { /* handles error */
        fork_failed_msg();
        return 0;
    }
    if (pid == 0) { /* CHILD executes the process in the background */

        char *filename = arglist[0];
        char **arguments = arglist;

        if (execvp(filename, arguments) == -1) { /* handle error */
            execution_failed_msg();
            exit(1);
        }
    }
    else { /* PARENT */
    }
    return 1;
}

/*
 * This function receives a command that executes the first program
 * and send it's stdout to the stdin of the second program through a pipe.
 */
int exec_pipe_process(int pipe_index, char **arglist){
    arglist[pipe_index] = NULL;
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1) { /* handle error */
        pipe_failed_msg();
        return 0;
    }

    int pid1 = fork(); /* handle error */
    if (pid1 < 0) {
        fork_failed_msg();
        return 0;
    }
    if (pid1 == 0) {  /* CHILD that writes */
        struct sigaction new_action;
        memset(&new_action, 0, sizeof(struct sigaction));
        new_action.sa_handler = SIG_DFL; /* terminate */

        if(sigaction(SIGINT, &new_action, NULL) != 0){
            sigaction_failed_msg();
            /* close pipe */
            close(pipe_fd[1]);
            close(pipe_fd[0]);
            exit(1);  /* end child process */
        }

        close(STDOUT_FILENO);

        if(dup2(pipe_fd[1], STDOUT_FILENO) == -1){ /* handle error */
            dup_failed_msg();
            /* close pipe */
            close(pipe_fd[1]);
            close(pipe_fd[0]);
            exit(1);  /* end child process */
        }
        /* write to pipe instead of to stdout */
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        char *filename1 = arglist[0];
        char **arguments1 = arglist;

        if (execvp(filename1, arguments1) == -1) { /* handle error */
            execution_failed_msg();
            exit(1);
        }
    }
    else {
        /* PARENT
         * creates another child that will read from pipe */
        int pid2 = fork();
        if (pid2 < 0) { /* handle error */
            fork_failed_msg();
            return 0;
        }
        if (pid2 == 0) { /* CHILD that reads */

            struct sigaction new_action;
            memset(&new_action, 0, sizeof(struct sigaction));
            new_action.sa_handler = SIG_DFL; /* terminate */

            if(sigaction(SIGINT, &new_action, NULL) != 0){
                sigaction_failed_msg();
                exit(1);
            }

            close(STDIN_FILENO);

            if(dup2(pipe_fd[0], STDIN_FILENO) == -1){ /* handle error */
                dup_failed_msg();
                /* close pipe */
                close(pipe_fd[1]);
                close(pipe_fd[0]);
                /* end child process */
                exit(1);
            }
            close(pipe_fd[1]);
            close(pipe_fd[0]);

            char *filename2 = arglist[pipe_index+1];
            char **arguments2 = arglist + (pipe_index+1);

            if (execvp(filename2, arguments2) == -1) { /* handle error */
                execution_failed_msg();
                exit(1);
            }
        }
        else {
            /* PARENT waits for both processes to end in order to finish */
            close(pipe_fd[0]);
            close(pipe_fd[1]);

            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        }
    }
    return 1;
}

/*
 * This function executes a process that contains no pipe nor background.
 */
int exec_normal_process(char **arglist){
    int pid = fork();
    if (pid < 0) { /* handle error */
        fork_failed_msg();
        return 0;
    }
    if (pid == 0) { /* CHILD executes the process in the background */
        struct sigaction new_action;
        memset(&new_action, 0, sizeof(struct sigaction));
        new_action.sa_handler = SIG_DFL;

        if(sigaction(SIGINT, &new_action, NULL) != 0){
            sigaction_failed_msg();
            exit(1);
        }

        char *filename = arglist[0];
        char **arguments = arglist;

        if (execvp(filename, arguments) == -1) { /* handle error */
            execution_failed_msg();
            exit(1);
        }
    }
    else {
        /* PARENT */
        waitpid(pid, NULL, 0);
    }
    return 1;
}

/* DEFINE THE KIND OF EXECUTION */

/*
 * This function checks if the order is an execution of a command in the background
 */
int is_background_process(int count, char **arglist){
    return (arglist[count-1][0] == BACKGROUND) && (arglist[count-1][1] == '\0');
}

/*
 * This function checks if the order contains a pipe.
 * If it does, returns it's index, otherwise, returns -1.
 */
int is_pipe_process(int count, char **arglist){
    for(int i = 0; i < count; i++) {
        if((arglist[i][0] == PIPE) && (arglist[i][1] == '\0')){
            return i;
        }
    }
    return -1;
}



/* ASSIGNMENT MAIN FUNCTIONS */

/*The skeleton calls this function before the first invocation of process_arglist().
 * This function returns 0 on success; any other return value indicates an error.
 * You can use this function for any initialization and setup that you think are necessary for your process_arglist().
 * If you don’t need any initialization, just have this function return immediately;
 * but you must provide it for the skeleton to compile. */
int prepare(){

    struct sigaction new_action_sigint, new_action_sigchld;
    memset(&new_action_sigint, 0, sizeof(struct sigaction));
    memset(&new_action_sigchld, 0, sizeof(struct sigaction));

    new_action_sigint.sa_handler = SIG_IGN;
    new_action_sigint.sa_flags = SA_RESTART;

    new_action_sigchld.sa_handler = SIG_IGN;

    if(sigaction(SIGCHLD, &new_action_sigchld, NULL) != 0){
        sigaction_failed_msg();
        return -1;
    }

    if(sigaction(SIGINT, &new_action_sigint, NULL) != 0){
        sigaction_failed_msg();
        return -1;
    }

    return 0;
}

/*
 * Input This function receives an array arglist with count non-NULL words.
 * This array contains the parsed command line.
 * The last entry in the array, arglist[count], is NULL. (So overall the array size is count+1.)
 */
int process_arglist(int count, char **arglist) {
    int is_pipe;
    if (is_background_process(count, arglist)) {
        return exec_background_process(count, arglist);
    }
    /* we may assume that a line won't contain both "|" and "&" */
     if((is_pipe = is_pipe_process(count, arglist)) != -1){
         return exec_pipe_process(is_pipe, arglist);
     }
     /* if we are here there is no '&' nor '|' */
     return exec_normal_process(arglist);
}

/*
 * The skeleton calls this function before exiting.
 * This function returns 0 on success; any other return value indicates an error.
 * You can use this function for any cleanups related to process_arglist() that you think are necessary.
 * If you don’t need any cleanups, just have this function return immediately;
 * but you must provide it for the skeleton to compile.
 * Note that cleaning up the arglist array is not your responsibility.
 * It is taken care of by the skeleton code.
 */
int finalize(){
    return 0;
}