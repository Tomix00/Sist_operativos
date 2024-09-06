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
#define FD_READ 0
#define FD_WRITE 1

static void ex_cmd_single(scommand cmd){
    assert(cmd != NULL);

    const nat cmd_len = scommand_length(cmd);
    string re_in  = scommand_get_redir_in(cmd);
    string re_out = scommand_get_redir_out(cmd);

    string myargs[257];

    for(nat i = 0 ; i<cmd_len ; i++){                                               //loads command and arguments
        myargs[i] = strdup(scommand_front(cmd));
        scommand_pop_front(cmd);
    }
    myargs[cmd_len] = NULL;                                                         //last one must be NULL

    int rc = fork();                                                                //---if fork error
    if (rc < 0){
        perror("Fork");
        exit(1);

    }else if (rc == 0){                                                             //---if child proccess

        if (re_in != NULL){                                                             //redirect input
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

        if (re_out != NULL){                                                            //redirect output
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

        execvp(myargs[0],myargs);                                                       //run external command with/without arguments
        perror("execvp");
        exit(1);
    }else{                                                                          //---if father proccess
        wait(NULL);
    }
}

static void ex_cmd_multiple(pipeline apipe, nat pipelen){ 
    if (pipelen==1){                                        //---if only 1 command
        ex_cmd_single(pipeline_front(apipe));                   //runs that single command
        pipeline_pop_front(apipe);                              

    }else{                                                  //---if more than 1 command
        int fd[2];
        int rc;

        pipe(fd);

        scommand cmd1 = pipeline_front(apipe);                  //get 1st command
        pipeline_pop_front(apipe);

        rc = fork();

        scommand cmd2 = pipeline_front(apipe);                  //get 2nd command
        pipeline_pop_front(apipe);

        if (rc < 0){                                            //---if fork error
            perror("Fork");
            exit(EXIT_SUCCESS);

        }else if(rc == 0){                                      //---if child proccess
            close(fd[FD_READ]);                                     //close piperead
            dup2(fd[FD_WRITE], STDOUT_FILENO);                      //redirect pipewrite to standard output
            close(fd[FD_WRITE]);                                    //close pipewrite

            ex_cmd_single(cmd1);                                    //runs single command

        }else{                                                  //---if father proccess
            int rc2;
            rc2 = fork();

            if (rc2 < 0){                                           //---if second fork error
                perror("Fork");
                exit(EXIT_SUCCESS);

            }else if(rc2 == 0){                                     //---if second child proccess
                close(fd[FD_WRITE]);                                    //close pipewrite
                dup2(fd[FD_READ],STDIN_FILENO);                         //redirect piperead to standard input
                close(fd[FD_READ]);                                     //close piperead

                ex_cmd_single(cmd2);                                    //runs single command

            }else{                                                  //---if second father proccess
                wait(NULL);                                             //since this module is for only 2 commands, no more forks needed
                wait(NULL);

            }
        }
    }
}

void execute_pipeline(pipeline apipe){
    nat pipelen = pipeline_length(apipe);

    if (pipeline_is_empty(apipe)){                      //---if there is no commands
        exit(EXIT_SUCCESS);

    } else if (builtin_alone(apipe)){                   //---if there is only an internal command
        builtin_run(pipeline_front(apipe));             //run internal
        exit(EXIT_SUCCESS);

    } else {                                            //---if more than 1 command and no internals
        bool pipe_wait = pipeline_get_wait(apipe);

        if(pipe_wait){                                      //---if true, runs on foreground
            ex_cmd_multiple(apipe,pipelen);                     //runs list of commands
            pipeline_destroy(apipe);                            //after running, destroys pipeline

        }else{                                              //---if false, runs on background
            pid_t pid = fork();

            if(pid < 0){                                        //---if fork error
                perror("Fork");
                exit(EXIT_SUCCESS);

            }else if (pid == 0){                                //---if child proccess
                ex_cmd_multiple(apipe,pipelen);                     //runs list of commands
                pipeline_destroy(apipe);                            //after running, destroys pipeline

            }else{                                              //---if pid > 0 -> father proccess
                wait(NULL);

            }
        }
    }
}
