#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "builtin.h"
#include "strextra.h"
#include "tests/syscall_mock.h"

 #include <unistd.h>

bool builtin_is_internal(scommand cmd){
    bool result;

    if (strcmp(scommand_front(cmd),"cd" )==0 ||strcmp(scommand_front(cmd),"exit" )==0  || strcmp(scommand_front(cmd),"help" ) == 0 ){
        result = true;
    } 
    else{
        result = false;
    }

    return result;
}

bool builtin_alone(pipeline p){
    bool result;
    if (pipeline_length(p)==1 && builtin_is_internal(pipeline_front(p))){
        result = true;
    }
    else{
        result = false;
    }

    return result;
}

void builtin_run(scommand cmd){
    assert(builtin_is_internal(cmd));
    
    if (strcmp(scommand_front(cmd), "cd")==0){
        scommand_pop_front(cmd);
        
        char* cd_parameter = NULL;
        if(scommand_length(cmd)==0){
            cd_parameter = getenv("HOME");
        }
        else{
            cd_parameter = strdup("");
            cd_parameter = strmerge(cd_parameter, scommand_front(cmd));
        }

        int status = chdir(cd_parameter);
        if(status!=0){
            printf("Error CD\n");
        }
    }

    if (strcmp(scommand_front(cmd), "help")==0){
        printf("Nombre del shell: \n Nombre de sus autores:\n Comandos y su Descripcion:\n"); //INGLES/ESPAÑOL DEPENDE DE COMO HIZO EL MANEJO DE ERRORES MARCO
    } 

    if (strcmp(scommand_front(cmd), "exit")==0){
        printf("Cerrando Mybash...\n");
        exit(EXIT_SUCCESS);
    }
}