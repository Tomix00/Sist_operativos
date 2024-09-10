#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "execute.h"
#include "builtin.h"
#include "command.h"
#include "tests/syscall_mock.h"

#define READ_ONLY 0       
#define WRITE_ONLY 1
#define MAX_COMMANDS 15

void execute_scommand(scommand command){
    
    unsigned int length = scommand_length(command);                               //Largo del scommand del pipeline que queremos ejecutar
    char **argv = malloc(sizeof(char*)*(length+1));                               //Arreglo de strings con cada palabra del scommand y un NULL en la ultima posicion
    char *cmd = strdup(scommand_front(command));
    
    for (unsigned int i = 0; i<length; i++){
        argv[i] = strdup(scommand_front(command));        //Llenamos argv
        scommand_pop_front(command);
    }
    argv[length] = NULL;                                  //Ultima posicion NULL
    
    //Redirecciones
    if (scommand_get_redir_in(command) != NULL){
        int input_fd = open(scommand_get_redir_in(command), O_RDONLY, S_IRUSR);
            
        if (input_fd<0){
            perror("Open error -input");
            exit(1);
        }
        else{
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }
    }
    if (scommand_get_redir_out(command) != NULL){
        int output_fd = open(scommand_get_redir_out(command), O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR | S_IWUSR);
            
        if (output_fd<0){
            printf("Open error -output");
            exit(EXIT_FAILURE);
        }
        else{
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }
    }
    //Ejecucion del comando y liberar memoria
        execvp(cmd,argv);  

        free(command);
        for (unsigned int i=0; i<length; i++){
            free(argv[i]);
        }
        free(argv);

        exit(EXIT_FAILURE);
}

void execute_pipeline(pipeline apipe){
    assert(apipe!=NULL);
    
    if (!pipeline_is_empty(apipe)){
        unsigned int apipe_length = pipeline_length(apipe);

        
        if(builtin_is_internal(pipeline_front(apipe))){
                builtin_run(pipeline_front(apipe));
        }
        else{
            unsigned int command_index = 0;
            bool in_parent = true;
            pid_t pid;
            pid_t all_pids[MAX_COMMANDS];
            int fds[MAX_COMMANDS][2];

            while (!pipeline_is_empty(apipe) && in_parent){
                if(apipe_length!=1 && command_index!=apipe_length-1){                       
                    pipe(fds[command_index]);                              
                }
                pid = fork();
                if(pid<0){
                    exit(EXIT_FAILURE);
                }
                if(pid==0){
                    //Hijo
                    if(apipe_length != 1){
                        if(command_index!=0){                   
                            dup2(fds[command_index-1][READ_ONLY],STDIN_FILENO);
                            close(fds[command_index-1][READ_ONLY]);
                        }
                        if(command_index!=apipe_length-1){
                            dup2(fds[command_index][WRITE_ONLY],STDOUT_FILENO);
                        }
                        close(fds[command_index][WRITE_ONLY]);
                        close(fds[command_index][READ_ONLY]);
                    }
                    execute_scommand(pipeline_front(apipe));
                    in_parent = false;
                }
                else{
                    //Padre
                    if(apipe_length!=1){
                        if(command_index!=0){
                            close(fds[command_index-1][READ_ONLY]);
                        }
                        close(fds[command_index][WRITE_ONLY]);
                    }
                    
                    all_pids[command_index] = pid;
                    command_index++;
                    pipeline_pop_front(apipe);
                }
            }    
            
            //Falta ver que hacer si se dio un builtin error
            //Caso de background 
            if(pipeline_get_wait(apipe)){
                int wait_status;
                for(unsigned int j=0; j<=apipe_length-1; j++){
                    waitpid(all_pids[j],&wait_status,0);
                }
            }

            if(in_parent && apipe_length!=1){
                close(fds[command_index-1][READ_ONLY]);
            }
        }
    }   
}   



