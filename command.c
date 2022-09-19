#include <endian.h>
#include <stdio.h>

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h> //Libreria para strings

#include "command.h"
#include "strextra.h"
#include <glib-2.0/glib.h>
#include <string.h>


struct scommand_s {
    GSList *comm_args;
    char *out;
    char *in;
};

struct pipeline_s {
    GSList* command_list;
    bool  wait;
};


scommand scommand_new(void) {

    scommand self = malloc(sizeof(struct scommand_s)); /* Reservamos memoria e inicializamos todo en NULL */
    self->comm_args = NULL;
    self->out = NULL;
    self->in = NULL;

    assert(self != NULL && scommand_is_empty (self) &&
        scommand_get_redir_in (self) == NULL &&
        scommand_get_redir_out (self) == NULL);

    return self;
}


scommand scommand_destroy(scommand self){
    assert(self != NULL);

    g_slist_free_full(self->comm_args, free); /* Aca utilizamos una funcion de GLIB que se encarga de liberar todos los elementos
                                                de la lista utilizando free(), en este caso funciona porque son punteros. */
    self->comm_args = NULL; /* Setteamos en NULL */

    free(self->in); /* De aca al return, simplemente liberamos los punteros y setteamos en NULL */
    self->in = NULL;

    free(self->out);
    self->out = NULL;

    free(self);
    self = NULL;

    assert(self == NULL);

    return self;
}

/* Modificadores */

void scommand_push_back(scommand self, char * argument){   
    assert(self != NULL && argument != NULL);
    self -> comm_args = g_slist_append(self->comm_args, argument);  /* Esta funcion agrega elementos por detras de la GSList*/
    assert(!scommand_is_empty(self));
} 


/* Version rota del Pop NO ELIMINAR hasta poder arreglar los problemas que nos trae. */
void scommand_pop_front(scommand self){
    assert(self != NULL && !scommand_is_empty(self));
    GSList *head = self -> comm_args;
    self -> comm_args = g_slist_remove_link(self -> comm_args, head);

    free(head -> data);
    head -> data = NULL;
    
    g_slist_free_1(head);
}



char* scommand_head_and_pop(scommand self){
    assert(self != NULL && !scommand_is_empty(self));

    char* res = g_slist_nth_data(self -> comm_args,0);

    self -> comm_args = g_slist_remove(self->comm_args, res);

    assert(res != NULL);
    return res;
}


void scommand_set_redir_in(scommand self, char * filename) {
    assert(self != NULL);
    if(self -> in == NULL){ /* En caso de que no haya nada asignado al *in, se asigna directo */
        self->in = filename;
    }else{                  /* En caso de que haya algun elemento, se libera y luego se reasigna. */
        free(self-> in);
        self->in = filename;
    }
}

void scommand_set_redir_out(scommand self, char * filename) {
    assert(self != NULL);
    if(self -> out == NULL){/* En caso que no haya nada asignado al *out, se asigna directo */
        self->out = filename;
    }else{                  /* En caso de que haya algun elemento, se libera y luego se reasigna. */
       free(self -> out);
       self->out = filename; 
    }
}

/* Proyectores */



bool scommand_is_empty(const scommand self){ 
    assert (self !=NULL);
    return (g_slist_length(self->comm_args) == 0); /* esta funcion nos devuelve el length de la lista de argumentos, 
                                                    chequeamos que sea 0. */
}



unsigned int scommand_length(const scommand self){ 
    assert (self !=NULL);
    unsigned int length= g_slist_length(self->comm_args); /* esta funcion calcula el largo de la lista. */
    return length;
}



char * scommand_front(const scommand self){ 
    assert(self != NULL && !scommand_is_empty(self));
    char * result = g_slist_nth_data(self->comm_args, 0);  /* Esta funcion devuelve el elemento que se encuentre en el indice 0
                                                                de la lista */
    assert(result != NULL);                                
    return result;
} 


char * scommand_get_redir_in(const scommand self){ 
    assert(self != NULL);
    return self->in; /* Devolvemos el string al que apunta *in */
} 
char * scommand_get_redir_out(const scommand self){ 
    assert(self != NULL);
    return self->out; /* Devolvemos el string al que apunta *out */
}

char * scommand_to_string(const scommand self){
    assert(self!=NULL);

    char *res = strdup(""); /* Inicializamos res como string en "", strdup ya realiza
                                  la reserva de memoria. */

    for(unsigned int i = 0u; i < scommand_length(self); ++i){
        char *tmp = g_slist_nth_data(self -> comm_args, i); /* Tomamos elemento en posicion i de la GSList */
        res = strmergefree(res, tmp);                /* concatenamos res con el i-esimo argumento */
        res = strmergefree(res, " ");               /* En cada iteracion agregamos un espacio */
    }

    if(self -> out != NULL){
        res = strmergefree(res, " > ");     /* En caso de haber un archivo output, concatenamos el simbolo. */
        res = strmergefree(res, self -> out); /* Concatenamos el contenido de *out */
    }

    if(self -> in != NULL){
        res = strmergefree(res, " < "); /* En caso de haber un archivo input, concatenamos el simbolo */
        res = strmergefree(res, self -> in); /* Concatenamos el contenido de *in */
    }
    assert(scommand_is_empty(self) || scommand_get_redir_in(self)==NULL || scommand_get_redir_out(self)==NULL || strlen(res) > 0);
    return res;
}

pipeline pipeline_new(void){ //Benja
    pipeline result = malloc(sizeof(struct pipeline_s)); /* Reservamos memoria e inicializamos la lista de commands en NULL */
    result->command_list = NULL;
    result->wait = true;    /* Inicializamos la espera en true */
    assert(result != NULL  && pipeline_is_empty(result) && pipeline_get_wait( result));
    return result;
}

pipeline pipeline_destroy(pipeline self){
    assert(self != NULL);
    while(self->command_list != NULL){
        pipeline_pop_front(self);
    }
    free(self);
    self = NULL;
    return (self);
}

/* Modificadores */

void pipeline_push_back(pipeline self, scommand sc){   
    assert(self != NULL && sc != NULL);
    self -> command_list = g_slist_append(self->command_list, sc);  /* Agregamos a la lista de scommands el comando sc */
    assert(!pipeline_is_empty(self));
}

/* Esto esta aun en arreglos, revisar. */

scommand pipeline_head_and_pop(pipeline self){
    assert(self != NULL && !pipeline_is_empty(self));

    scommand res = g_slist_nth_data(self -> command_list,0);

    self -> command_list= g_slist_remove(self->command_list, res);

    assert(res != NULL);
    return res;
}


void pipeline_pop_front(pipeline self){
    assert(self != NULL && !pipeline_is_empty(self));

    GSList *head = self -> command_list;

    self -> command_list = g_slist_remove_link(self->command_list, head);

    head -> data = scommand_destroy(head -> data);

    g_slist_free_1(head);
    
}

void pipeline_set_wait(pipeline self, const bool w) {
    assert(self != NULL);
    self->wait = w;     /* Cambiamos el valor booleano de wait por w. */
}

/* Proyectores */

bool pipeline_is_empty(const pipeline self) {
    assert(self != NULL);
    return (self->command_list == NULL);   /* Si la lista de scommands esta vacia, entonces la pipe tambien. */
}


unsigned int pipeline_length(const pipeline self){ 
    assert(self !=NULL);
    unsigned int length= g_slist_length(self->command_list);    /* Funcion que calcula el tam de la GSList de scommands */
    assert((length==0) == pipeline_is_empty(self));
    return length;
}

scommand pipeline_front(const pipeline self){ 
    assert (self!=NULL && !pipeline_is_empty(self));
    scommand result = g_slist_nth_data(self->command_list,0);   /* Obtenemos el scommand ubicado en indice 0 (El frente) */
    assert(result!=NULL); 
    return result;
}


bool pipeline_get_wait(const pipeline self){ 
    assert( self != NULL);
    return self->wait;  /* Devolvemos el valor de el campo wait */
}


char * pipeline_to_string(const pipeline self){
    assert(self != NULL);

    // Inicializamos un string con memoria:
    char *res_pipeline = strdup("");

    // Concatenamos los scommands con "| " ó "&" (según corresponda):
    for (unsigned int i = 0u; i < g_slist_length(self->command_list); ++i){
        scommand current_scommand = g_slist_nth_data(self->command_list, i);
        char *simple_command = scommand_to_string(current_scommand);
        res_pipeline = strmergefree(res_pipeline, simple_command);
        if(i != g_slist_length(self->command_list) - 1u){
            res_pipeline = strmergefree(res_pipeline, "| ");
        }
        free(simple_command);
    }
    // Concatenamos "&" si corresponde:
    if(!self->wait){
        res_pipeline = strmergefree(res_pipeline, "&");
    }
    
    return (res_pipeline);
}