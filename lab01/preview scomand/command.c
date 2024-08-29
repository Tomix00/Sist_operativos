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

char * pipeline_to_string(const pipeline self){
    assert(self != NULL);

    char *result = (char*)g_strdup("");
    char *str = NULL;
    GSList* current_command = NULL;

    current_command = self->commands;
    while (current_command != NULL){
        result = strmerge(result, str=scommand_to_string(current_command->data));
        free(str);
        current_command = g_slist_next(current_command);
        if(current_command != NULL){
            result = strmerge(result,CHR_SPACE);
            result = strmerge(result,CHR_PIPE);
            result = strmerge(result,CHR_SPACE);
        }
    }

    assert(pipeline_is_empty(self) || strlen(result)>0);
    /* 
    * 
    * 
    * situacion para cuando pipeline tiene wait=true
    * 
    * 
    */
    
    return result;
}

