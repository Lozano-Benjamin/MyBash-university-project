#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "command.h"
#include <glib.h>


struct scommand_s {
    GSList* comm_args;
    char* out;
    char* in;
};

scommand scommand_new(void) {
/*
 * Nuevo `scommand', sin comandos o argumentos y los redirectores vacíos
 *   Returns: nuevo comando simple sin ninguna cadena y redirectores vacíos.
 * Ensures: result != NULL && scommand_is_empty (result) &&
 *  scommand_get_redir_in (result) == NULL &&
 *  scommand_get_redir_out (result) == NULL
 */

    scommand self = malloc(sizeof(struct scommand_s));
    self->comm_args = NULL;
    self->out = NULL;
    self->in = NULL;

    assert(self != NULL && scommand_is_empty (self) &&
        scommand_get_redir_in (self) == NULL &&
        scommand_get_redir_out (self) == NULL);

    return self;
}


