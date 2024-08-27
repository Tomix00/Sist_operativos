#include "command.h"
#include <assert.h>
#include <stdlib.h>
#include <glib.h>


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

    g_slist_free_full(self->commands,NULL);
    free(self);

    self = NULL;

    assert(self == NULL);

    return self;
}

void pipeline_push_back(pipeline self, scommand sc);

void pipeline_pop_front(pipeline self);

void pipeline_set_wait(pipeline self, const bool w);

bool pipeline_is_empty(const pipeline self){
    assert(self != NULL);

    return g_slist_length(self->commands) == 0;
}

unsigned int pipeline_length(const pipeline self){
    assert(self != NULL);

    return g_slist_length(self->commands);
}

scommand pipeline_front(const pipeline self){
    assert(self != NULL && !pipeline_is_empty(self));

    scommand result;
    result = self->commands->data;

    assert(result != NULL);

    return result;
}

bool pipeline_get_wait(const pipeline self){
    assert(self != NULL);

    return self->wait;
}

char * pipeline_to_string(const pipeline self);

