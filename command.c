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

    scommand self = malloc(sizeof(struct scommand_s));
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

    g_slist_free_full(self->comm_args, free); //usar pop
    self->comm_args = NULL;

    free(self->in);
    self->in = NULL;

    free(self->out);
    self->out = NULL;

    free(self);
    self = NULL;

    assert(self == NULL);

    return self;
}

/* Modificadores */

void scommand_push_back(scommand self, char * argument){ //ayala
    assert(self != NULL && argument != NULL);
    self -> comm_args = g_slist_append(self->comm_args, argument);
    assert(!scommand_is_empty(self));
} 
/*
 * Agrega por detrás una cadena a la secuencia de cadenas.
 *   self: comando simple al cual agregarle la cadena.
 *   argument: cadena a agregar. El TAD se apropia de la referencia.
 * Requires: self!=NULL && argument!=NULL
 * Ensures: !scommand_is_empty()
 */


void scommand_pop_front(scommand self){     //ayala
    assert(self != NULL && !scommand_is_empty(self));
    self -> comm_args = g_slist_remove(self->comm_args,g_slist_nth_data(self->comm_args, 0));
}
/*
 * Quita la cadena de adelante de la secuencia de cadenas.
 *   self: comando simple al cual sacarle la cadena del frente.
 * Requires: self!=NULL && !scommand_is_empty(self)
 */

void scommand_set_redir_in(scommand self, char * filename) {
    assert(self != NULL);
    self->in = filename;
}
void scommand_set_redir_out(scommand self, char * filename) {
    assert(self != NULL);
    self->out = filename;
}

/* Proyectores */

/*
 * Indica si la secuencia de cadenas tiene longitud 0.
 *   self: comando simple a decidir si está vacío.
 *   Returns: ¿Está vacío de cadenas el comando simple?
 * Requires: self!=NULL
 */

bool scommand_is_empty(const scommand self){ //fabro 
    assert (self !=NULL);
    return (g_slist_length(self->comm_args) == 0);
}

/*
 * Da la longitud de la secuencia cadenas que contiene el comando simple.
 *   self: comando simple a medir.
 *   Returns: largo del comando simple.
 * Requires: self!=NULL
 * Ensures: (scommand_length(self)==0) == scommand_is_empty()
 *
 */

unsigned int scommand_length(const scommand self){ //sssj (Poner ensures)
    assert (self !=NULL);
    unsigned int length= g_slist_length(self->comm_args);
    return length;
}



char * scommand_front(const scommand self){ //benja 
    assert(self != NULL && !scommand_is_empty(self));
    char * result = g_slist_nth_data(self->comm_args, 0); //Encontre esta funcion, dice que devuelve el dato entonces creo que va bien. Si no,
    assert(result != NULL);                                // g_slist_nth puede servir, habria que checkear.
    return result;
} 
/*
 * Toma la cadena de adelante de la secuencia de cadenas.
 *   self: comando simple al cual tomarle la cadena del frente.
 *   Returns: cadena del frente. La cadena retornada sigue siendo propiedad
 *     del TAD, y debería considerarse inválida si luego se llaman a
 *     modificadores del TAD. Hacer una copia si se necesita una cadena propia.
 * Requires: self!=NULL && !scommand_is_empty(self)
 * Ensures: result!=NULL
 */

char * scommand_get_redir_in(const scommand self){ //benja de nueva
    assert(self != NULL);
    return self->in;
} 
char * scommand_get_redir_out(const scommand self){ //benja otra vez
    assert(self != NULL);
    return self->out;
}
/*
 * Obtiene los nombres de archivos a donde redirigir la entrada (salida).
 *   self: comando simple a decidir si está vacío.
 *   Returns: nombre del archivo a donde redirigir la entrada (salida)
 *  o NULL si no está redirigida.
 * Requires: self!=NULL
 */

char * scommand_to_string(const scommand self){
    assert(self!=NULL);
    GSList *tmp = NULL;
    tmp = self -> comm_args;
    char *res = strdup("");
    if(tmp != NULL){
        res = strmerge(res, tmp->data);
        tmp = tmp -> next;
        while(tmp != NULL){
            res = strmerge(res, " ");
            res = strmerge(res, tmp -> data); // usar puntero aux que apunte a res para que no qeuuede colgando
            tmp = tmp -> next; // usar libreria glib
        }
    }
    if(self -> out != NULL){
        res = strmerge(res, " > ");
        res = strmerge(res, self -> out);
    }

    if(self -> in != NULL){
        res = strmerge(res, " < ");
        res = strmerge(res, self -> in);
    }
    assert(scommand_is_empty(self) || scommand_get_redir_in(self)==NULL || scommand_get_redir_out(self)==NULL || strlen(res) > 0);
    return res;
}



/* Preety printer para hacer debugging/logging.
 * Genera una representación del comando simple en un string (aka "serializar")
 *   self: comando simple a convertir.
 *   Returns: un string con la representación del comando simple similar
 *     a lo que se escribe en un shell. El llamador es dueño del string
 *     resultante.
 * Requires: self!=NULL
 * Ensures: scommand_is_empty(self) ||
 *   scommand_get_redir_in(self)==NULL || scommand_get_redir_out(self)==NULL ||
 *   strlen(result)>0
 */


/*
 * pipeline: tubería de comandos.
 * Ejemplo: ls -l *.c > out < in  |  wc  |  grep -i glibc  &
 * Secuencia de comandos simples que se ejecutarán en un pipeline,
 *  más un booleano que indica si hay que esperar o continuar.
 *
 * Una vez que un comando entra en el pipeline, la memoria pasa a ser propiedad
 * del TAD. El llamador no debe intentar liberar la memoria de los comandos que
 * insertó, ni de los comandos devueltos por pipeline_front().
 * pipeline_to_string() pide memoria internamente y debe ser liberada
 * externamente.
 *
 * Externamente se presenta como una secuencia de comandos simples donde:
 *           ______________________________
 *  front -> | scmd1 | scmd2 | ... | scmdn | <-back
 *           ------------------------------
 */



pipeline pipeline_new(void){ //Benja
    pipeline result = malloc(sizeof(struct pipeline_s));
    result->command_list = NULL;
    result->wait = true;
    assert(result != NULL  && pipeline_is_empty(result) && pipeline_get_wait( result));
    return result;
}
/*
 * Nuevo `pipeline', sin comandos simples y establecido para que espere.
 *   Returns: nuevo pipeline sin comandos simples y que espera.
 * Ensures: result != NULL
 *  && pipeline_is_empty(result)
 *  && pipeline_get_wait(result)
 */



pipeline pipeline_destroy(pipeline self){  //Benja
    assert(self != NULL);
    g_slist_free_full(self->command_list, free); //Libero cada scommand
    self->command_list = NULL; //Setteo en NULL
    free(self);     //Libero self
    self = NULL;        //Setteo en NULL
    assert(self == NULL);
    return self;
}
/*
 * Destruye `self'.
 *   self: tubería a a destruir.
 * Requires: self != NULL
 * Ensures: result == NULL
 */

/* Modificadores */

void pipeline_push_back(pipeline self, scommand sc){    //Facu (Revisar)
    assert(self != NULL && sc != NULL);
    self -> command_list = g_slist_append(self->command_list, sc);
    assert(!pipeline_is_empty(self));
}
/*
 * Agrega por detrás un comando simple a la secuencia.
 *   self: pipeline al cual agregarle el comando simple.
 *   sc: comando simple a agregar. El TAD se apropia del comando.
 * Requires: self!=NULL && sc!=NULL
 * Ensures: !pipeline_is_empty()
 */

void pipeline_pop_front(pipeline self){ // Facu (Revisar)
    assert(self != NULL && !pipeline_is_empty(self));
    self -> command_list = g_slist_remove(self->command_list, g_slist_nth_data(self->command_list, 0));
}
/*
 * Quita el comando simple de adelante de la secuencia.
 *   self: pipeline al cual sacarle el comando simple del frente.
 *      Destruye el comando extraido.
 * Requires: self!=NULL && !pipeline_is_empty(self)
 */

void pipeline_set_wait(pipeline self, const bool w) {
    assert(self != NULL);
    self->wait = w;
}

/* Proyectores */

bool pipeline_is_empty(const pipeline self) {
    assert(self != NULL);
    return (self->command_list == NULL);
}
/*
 * Indica si la secuencia de comandos simples tiene longitud 0.
 *   self: pipeline a decidir si está vacío.
 *   Returns: ¿Está vacío de comandos simples el pipeline?
 * Requires: self!=NULL
 */

unsigned int pipeline_length(const pipeline self){ // Fabri (Revisar y poner Ensures)
    assert (self !=NULL);
    unsigned int length=0;
    for (unsigned int i=0; i < g_slist_length(self->command_list); ++i){
        ++length;
    }

    assert((length==0) == pipeline_is_empty(self));
    return length;
}
/*
 * Da la longitud de la secuencia de comandos simples.
 *   self: pipeline a medir.
 *   Returns: largo del pipeline.
 * Requires: self!=NULL
 * Ensures: (pipeline_length(self)==0) == pipeline_is_empty()
 *
 */

scommand pipeline_front(const pipeline self){ // Fabri (Revisar y poner Ensures)
    assert (self!=NULL && !pipeline_is_empty(self));
    scommand result = g_slist_nth_data(self->command_list,0);
    assert(result!=NULL); 
    return result;
}
/*
 * Devuelve el comando simple de adelante de la secuencia.
 *   self: pipeline al cual consultar cual es el comando simple del frente.
 *   Returns: comando simple del frente. El comando devuelto sigue siendo
 *      propiedad del TAD.
 *      El resultado no es un "const scommand" ya que el llamador puede
 *      hacer modificaciones en el comando, siempre y cuando no lo destruya.
 * Requires: self!=NULL && !pipeline_is_empty(self)
 * Ensures: result!=NULL
 */

bool pipeline_get_wait(const pipeline self){ // Benja
    assert( self != NULL);
    return self->wait;
}
/*
 * Consulta si el pipeline tiene que esperar o no.
 *   self: pipeline a decidir si hay que esperar.
 *   Returns: ¿Hay que esperar en el pipeline self?
 * Requires: self!=NULL
 */

char * pipeline_to_string(const pipeline self){ //Benja.
    assert(self != NULL);
    GSList* command_list = self->command_list ;
    char *result = strdup(""); //Esta funcion lo que hace es inicializa y duplica un string.

    if (command_list != NULL) {

        char *aux = scommand_to_string(g_slist_nth_data(command_list,0u));
        result = strmerge(result,aux);

        for (unsigned int i = 1u; i < pipeline_length(self); i++) {
            result = strmerge(result, " | ");
            aux = scommand_to_string(g_slist_nth_data(command_list,i));
            result = strmerge(result, aux);
        }

        if (!pipeline_get_wait(self)) {
            result = strmerge(result, "&");
        }
    }


    assert(pipeline_is_empty(self) || pipeline_get_wait(self) || strlen(result) > 0);

    return result;
} 
/* Pretty printer para hacer debugging/logging.
 * Genera una representación del pipeline en una cadena (aka "serializar").
 *   self: pipeline a convertir.
 *   Returns: una cadena con la representación del pipeline similar
 *     a lo que se escribe en un shell. Debe destruirla el llamador.
 * Requires: self!=NULL
 * Ensures: pipeline_is_empty(self) || pipeline_get_wait(self) || strlen(result)>0
 */

