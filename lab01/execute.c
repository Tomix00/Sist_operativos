#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "execute.h"
#include "builtin.h"

typedef unsigned int nat;
typedef char* string;

static void single_ex_cmd(scommand cmd){
    assert(cmd != NULL);

    const nat cmd_len = scommand_length(cmd);
    string re_in  = scommand_get_redir_in(cmd);
    string re_out = scommand_get_redir_out(cmd);

    string myargs[257];

    for(nat i = 0 ; i<cmd_len ; i++){                   //load all commands and arguments
        myargs[i] = strdup(scommand_front(cmd));
        scommand_pop_front(cmd);
    }
    myargs[cmd_len] = NULL;                             //last one must be NULL

    int rc = fork();
    if (rc < 0){
        perror("Fork");
        exit(1);

    }else if (rc == 0){

        if (re_in != NULL){                             //redirect input
            close(STDIN_FILENO);
            int input_fd = open(re_in, O_RDONLY);

            if(input_fd < 0){
                perror("Open");
                exit(1);
            }else{
                dup2(input_fd, STDIN_FILENO);
                close(input_fd);
            }
            assert(input_fd == STDIN_FILENO);
        }

        if (re_out != NULL){                            //redirect output
            close(STDOUT_FILENO);
            int output_fd = open(re_out, O_CREAT |O_WRONLY| O_TRUNC| S_IRWXU);

            if(output_fd < 0){
                perror("Open");
                exit(1);
            }else{
                dup2(output_fd, STDOUT_FILENO);
                close(output_fd);
            }
        }

        execvp(myargs[0],myargs);
        perror("execvp");
        exit(1);
    }else{
        wait(NULL);
    }
}

void execute_pipeline(pipeline apipe){
    nat pipelen = pipeline_length(apipe);

    if (pipeline_is_empty(apipe)){                      //if there is no commands
        exit(EXIT_SUCCESS);

    } else if (builtin_alone(apipe)){                   //if there is only an internal command
        builtin_run(pipeline_front(apipe));
        exit(EXIT_SUCCESS);

    } else {                                            //other case
        bool pipe_wait = pipeline_get_wait(apipe);
        if(pipe_wait){

        }
    }
}
