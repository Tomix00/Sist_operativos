#include <assert.h>
#include <stdlib.h>
#include <glib.h>

#include "command.h"
#include "strextra.h"

#define CHR_SPACE " "
#define CHR_REDIR_IN "<"
#define CHR_REDIR_OUT ">"
#define CHR_PIPE "|"
#define CHR_BACKGROUND "&"

/*--------------------scommand implementation--------------------*/

struct scommand_s {
    GSList* command;
    char* input;
    char* output;
};

scommand scommand_new(void) {
    scommand result = NULL;
    result = malloc(sizeof(struct scommand_s));
    assert(result != NULL);

    result->command = NULL;
    result->input = NULL;
    result->output = NULL;

    assert(result != NULL);
    assert(scommand_is_empty(result));
    assert(scommand_get_redir_in (result) == NULL);
    assert(scommand_get_redir_out (result) == NULL);

    return result;
}

scommand scommand_destroy(scommand self){
    assert(self!=NULL);

    while(self->command != NULL){
        scommand_pop_front(self);
    }
    if(self->input){
        free(self->input);
    }
    if(self->output){
        free(self->output);
    }

    free(self);
    self = NULL;

    assert(self==NULL);
    return self;
}

void scommand_push_back(scommand self, char * argument){
    assert(self!=NULL);
    assert(argument!=NULL);

    self->command = g_slist_append(self->command, (gpointer)argument);

    assert(self != NULL);
    assert(!scommand_is_empty(self));
}

void scommand_pop_front(scommand self){
    assert(self!=NULL);
    assert(!scommand_is_empty(self));

    free((char *)g_slist_nth_data(self->command,0));
    self->command = g_slist_delete_link(self->command, self->command);
}

void scommand_set_redir_in(scommand self, char * filename){
    assert(self!=NULL);
    if(self->input){
        free(self->input);
    }
    self->input = filename;
}

void scommand_set_redir_out(scommand self, char * filename){
    assert(self!=NULL);
    if(self->output){
        free(self->output);
    }
    self->output = filename;
}

bool scommand_is_empty(const scommand self){
    assert(self!=NULL);

    bool result = g_slist_length(self->command)==0;

    return result;
}

unsigned int scommand_length(const scommand self){
    assert(self!=NULL);

    unsigned int length = g_slist_length(self->command);

    assert((length == 0) == (scommand_is_empty(self)));
    return length;
}

char * scommand_front(const scommand self){
    assert(self!=NULL);
    assert(!scommand_is_empty(self));

    char* result;
    result = (char*)g_slist_nth_data(self->command,0);

    assert(result != NULL);
    return result;
}

char * scommand_get_redir_in(const scommand self){    
    assert(self!=NULL);

    char *result = self->input;

    return result;
}

char * scommand_get_redir_out(const scommand self){
    assert(self!=NULL);

    char *result = self->output;

    return result;
}

char * scommand_to_string(const scommand self){
    assert(self != NULL);

    char * result = strdup("");
    if(!scommand_is_empty(self)){
        char * aux = result;


        GSList *current_arg = NULL;
        current_arg = self->command;

        while(current_arg != NULL){
            char *arg;
            arg = (char *)current_arg->data;

            result = strmerge(aux,arg);
            free(aux);
            aux = result;
            current_arg = g_slist_next(current_arg);

            if(current_arg != NULL){
                result = strmerge(aux,CHR_SPACE);
                free(aux);
                aux = result;
            }
        }
        assert(scommand_is_empty(self) || strlen(result)>0);

        if(scommand_get_redir_in(self) != NULL){
            result = strmerge(aux, CHR_SPACE);
            free(aux);
            aux = result;
            result = strmerge(aux, CHR_REDIR_IN);
            free(aux);
            aux = result;
            result = strmerge(aux, CHR_SPACE);
            free(aux);
            aux = result;
            result = strmerge(aux, scommand_get_redir_in(self));
            free(aux);
            aux = result;

        }
        assert (scommand_get_redir_in(self)==NULL || strlen(result)>0);

        if(scommand_get_redir_out(self) != NULL){
            result = strmerge(aux, CHR_SPACE);
            free(aux);
            aux = result;
            result = strmerge(aux, CHR_REDIR_OUT);
            free(aux);
            aux = result;
            result = strmerge(aux, CHR_SPACE);
            free(aux);
            aux = result;
            result = strmerge(aux, scommand_get_redir_out(self));
            free(aux);
        }
    }
    assert (scommand_get_redir_out(self)==NULL || strlen(result)>0);
    return result;
}

/*--------------------pipeline implementation--------------------*/

struct pipeline_s{
    GSList *commands;
    bool wait;
};


pipeline pipeline_new(void){
    pipeline result;
    result = malloc(sizeof(struct pipeline_s));
    result->commands = NULL;
    result->wait = true;

    assert(result != NULL);
    assert(pipeline_is_empty(result));
    assert(pipeline_get_wait(result));

    return result;
}

pipeline pipeline_destroy(pipeline self){
    assert(self != NULL);

    while(self->commands != NULL){
        pipeline_pop_front(self);
    }
    free(self);
    self =  NULL;

    assert(self == NULL);
    return self;
}

void pipeline_push_back(pipeline self, scommand sc){
    assert(self!=NULL);
    assert(sc!=NULL);

    self->commands = g_slist_append(self->commands,(gpointer)sc);

    assert(!pipeline_is_empty(self));
}

void pipeline_pop_front(pipeline self){
    assert(self != NULL);
    assert(!pipeline_is_empty(self));

    scommand_destroy(pipeline_front(self));
    self->commands = g_slist_delete_link(self->commands,self->commands);        
}

void pipeline_set_wait(pipeline self, const bool w){
    assert(self!=NULL);

    self->wait = w;
}

bool pipeline_is_empty(const pipeline self){
    assert(self != NULL);

    return g_slist_length(self->commands) == 0;
}

unsigned int pipeline_length(const pipeline self){
    assert(self != NULL);

    unsigned int result = g_slist_length(self->commands);
    
    assert((result==0)==pipeline_is_empty(self));
    return result;
}

scommand pipeline_front(const pipeline self){
    assert(self != NULL);
    assert(!pipeline_is_empty(self));

    scommand result = NULL;
    result = (scommand)g_slist_nth_data(self->commands,0);

    assert(result != NULL);
    return result;
}

bool pipeline_get_wait(const pipeline self){
    assert(self != NULL);

    return self->wait;
}

char * pipeline_to_string(const pipeline self){
   assert(self != NULL);

    char* result = strdup("");
   if(!pipeline_is_empty(self)){
    char *aux = result;
    char *str = NULL;

    GSList* current_command = NULL;

    current_command = self->commands;
    while (current_command != NULL){
        result = strmerge(aux, str=scommand_to_string(current_command->data));
        free(str);
        free(aux);
        aux = result;
         current_command = g_slist_next(current_command);
        if(current_command != NULL){
            //result = strmerge(result,CHR_SPACE);
            //result = strmerge(result,CHR_PIPE);
            //result = strmerge(result,CHR_SPACE);
            result = strmerge(aux," | ");
            free(aux);
            aux = result;
        }
    }
    if (self->wait == false)
    {
         result = strmerge(aux,CHR_SPACE);
         free(aux); 
         aux = result;

         result = strmerge(aux,CHR_BACKGROUND);
         free(aux); 
         aux = result;

         result = strmerge(aux,CHR_SPACE);
         free(aux);
    }
    
 }  
 assert(pipeline_is_empty(self) || strlen(result)>0); 
   return result;
}

