#include "builtin.h"
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>


bool builtin_is_internal(scommand cmd){
    if (strcmp(scommand_front(cmd),"cd" )==0 ||strcmp(scommand_front(cmd),"exit" )==0  || strcmp(scommand_front(cmd),"help" ) == 0 ){
    return true;
    } else {
    return false;
    }
}
bool builtin_alone(pipeline p){
    if (pipeline_length(p)==1){return true;}else{return false;}
}

void builtin_run(scommand cmd){
    assert(builtin_is_internal(cmd));
    if (strcmp(scommand_front(cmd), "cd"))
    {
        scommand_pop_front(cmd);
        char* cd_parameter = strup("");
        if (scommand_length(cmd)>0)
        {
            strmerge(cd_parameter, scommand_front(cmd));
        }
        if (chdir(cd_parameter) != 0 ){
            printf("error");
            }else { 
            printf("todo bien master");
        }
    }
    
    if (strcmp(scommand_front(cmd), "help")) {
        printf("nombre del shell:\n nombre de sus autores:\n comandos y su descripcion:\n"); //INGLES/ESPAÑOL DEPENDE DE COMO HIZO EL MANEJO DE ERRORES MARCO
    } 

    if (strcmp(scommand_front(cmd), "exit")) {
        if (builtin_alone(cmd))
        {
            /* code */
        }
        
        exit();
        }