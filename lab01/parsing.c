#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "parsing.h"
#include "parser.h"
#include "command.h"

static bool background = false;

static scommand parse_scommand(Parser p) {
    scommand command = scommand_new();
    arg_kind_t type;
    char *arg = NULL;
    bool quit = false, pipe = false, match = true, command_ready = false;

    
    while(!quit && !parser_at_eof(p)){
        match = false;
        parser_skip_blanks(p);
        arg = parser_next_argument(p,&type);        
        parser_op_pipe(p,&pipe);
        parser_op_background(p,&background);
        
        //Vemos si matchea con los tipos requeridos para scommand
        if(type==ARG_NORMAL && arg!=NULL){
            scommand_push_back(command,arg);
            match = true;
            command_ready = true;
        }
        if(type==ARG_INPUT && arg!=NULL){
            scommand_set_redir_in(command,arg);
            match = true;
        }
        if(type==ARG_OUTPUT && arg!=NULL){
            scommand_set_redir_out(command,arg);
            match = true;
        }
        
        //Vemos caso de error en el que no hubo match ni pipe o background
        if((!pipe || !background) && !match){
            command = scommand_destroy(command);
            quit = true;
        }
        //Caso contrario definimos quit para ver si damos por terminado el parseo del scommand
        else{
            quit = pipe | background;
        }
        
        parser_skip_blanks(p);
    }
    //En caso de no haber completado la estructura minima de scommand se trata de un error
    if(!command_ready){
        command = scommand_destroy(command);
    }

    return command;
}

pipeline parse_pipeline(Parser p) {
    pipeline result = pipeline_new();
    scommand cmd = NULL;
    bool error=false, another_pipe=true;
    
    if(!parser_at_eof(p)){
        //Consumimos del parser hasta el \n mas proximo, parte que queremos parsear
        parser_garbage(p,&another_pipe);
        
        if(another_pipe){
            static FILE *input = NULL;
            input = fmemopen(parser_last_garbage(p), strlen(parser_last_garbage(p)), "r");
            //Guardamos en aux_parser la parte que queremos parsear
            Parser aux_parser = parser_new(input);
        
            while(!parser_at_eof(aux_parser) && !error &){   
                cmd = parse_scommand(aux_parser);
                error = (cmd==NULL);
                
                if(!error){
                    pipeline_push_back(result,cmd);
                }
            }
        
            if(result!=NULL && !error){
                pipeline_set_wait(result,!background);
            }
        }
        //Si no hubiera caracteres para parsear, devolvemos NULL
        else{
            result = pipeline_destroy(result);
        }
    }
    
    return result;
}

