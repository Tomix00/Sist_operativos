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
    scommand result;
    result = malloc(sizeof(struct scommand_s));
    result->input = NULL;
    result->output = NULL;
    result->command = NULL;

    assert(result != NULL && scommand_is_empty(result));
    assert(scommand_get_redir_in (result) == NULL);
    assert(scommand_get_redir_out (result) == NULL);

    return result;
}

scommand scommand_destroy(scommand self){
    assert(self!=NULL);

    //for (guint i = 0; i < g_slist_length(self->command); i++)
    //{
    //    free((char*)g_slist_nth(self->command, i));
    //}
    //g_slist_free(self->command);
    //free(self->input);
    //free(self->output);
    //free(self);
    //self = NULL;

    while(self->command != NULL){
        scommand_pop_front(self);
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

    assert(!scommand_is_empty(self));
}

void scommand_pop_front(scommand self){
    assert(self!=NULL);
    assert(!scommand_is_empty(self));

    self->command = g_slist_delete_link(self->command, self->command);
}

void scommand_set_redir_in(scommand self, char * filename){
    assert(self!=NULL);

    self->input = filename;
}

void scommand_set_redir_out(scommand self, char * filename){
    assert(self!=NULL);

    self->output = filename;
}

bool scommand_is_empty(const scommand self){
    assert(self!=NULL);

    bool result = false;
    if (g_slist_length(self->command)==0 && self->input == NULL && self->output == NULL){
        result = true;
    }

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
    result = (scommand)g_slist_nth_data(self->commands,1);

    assert(result != NULL);
    return result;
}

bool pipeline_get_wait(const pipeline self){
    assert(self != NULL);

    return self->wait;
}

//char * pipeline_to_string(const pipeline self){
//    assert(self != NULL);
//
//    char *result = (char*)g_strdup("");
//    char *str = NULL;
//    GSList* current_command = NULL;
//
//    current_command = self->commands;
//    while (current_command != NULL){
//        result = strmerge(result, str=scommand_to_string(current_command->data));
//        free(str);
//        current_command = g_slist_next(current_command);
//        if(current_command != NULL){
//            result = strmerge(result,CHR_SPACE);
//            result = strmerge(result,CHR_PIPE);
//            result = strmerge(result,CHR_SPACE);
//        }
//    }
//
//    assert(pipeline_is_empty(self) || strlen(result)>0);
//    /* 
//    * 
//    * 
//    * situacion para cuando pipeline tiene wait=true
//    * 
//    * 
//    */
//    
//    return result;
//}

