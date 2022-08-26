#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "command.h"
#include <glib.h>

scommand scommand_new(void) {
    return NULL;
}
/*
 * Nuevo `scommand', sin comandos o argumentos y los redirectores vacíos
 *   Returns: nuevo comando simple sin ninguna cadena y redirectores vacíos.
 * Ensures: result != NULL && scommand_is_empty (result) &&
 *  scommand_get_redir_in (result) == NULL &&
 *  scommand_get_redir_out (result) == NULL
 */