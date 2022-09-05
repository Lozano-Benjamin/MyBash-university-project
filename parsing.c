#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "parsing.h"
#include "parser.h"
#include "command.h"

static scommand parse_scommand(Parser p) {
    scommand new_command = scommand_new();
    arg_kind_t type = ARG_NORMAL;
    char *aux = parser_next_argument(p, &type);
/*     bool flag = false; */
    while (type == ARG_NORMAL && !parser_at_eof(p) && aux != NULL) {
        scommand_push_back(new_command, aux);
        aux = parser_next_argument(p, &type);
/*         if (aux == ""){
            flag = true;
            break;
        } */
    }
    if (type == ARG_INPUT) {
        scommand_set_redir_in(new_command, aux);
        aux = parser_next_argument(p, &type);
    }
    if (type == ARG_OUTPUT) {
        scommand_set_redir_out(new_command, aux);
        aux = parser_next_argument(p, &type);
    }
/*     if(flag){
        new_command = scommand_destroy(new_command);
    } */
    return new_command;
}

pipeline parse_pipeline(Parser p) {
    pipeline result = pipeline_new();
    scommand cmd = NULL;
    bool error = false, another_pipe=true;
    cmd = parse_scommand(p); //
    error = (cmd==NULL); /* Comando inválido al empezar */
    while (another_pipe && !error) {    
        pipeline_push_back(result, cmd);
        parser_op_pipe(p, &another_pipe);
        if(another_pipe){
            cmd = parse_scommand(p);
        }
    }
    /* Opcionalmente un OP_BACKGROUND al final */
    
    /*
     *
     * COMPLETAR
     *
     */

    /* Tolerancia a espacios posteriores */
    /* Consumir todo lo que hay inclusive el \n */
    /* Si hubo error, hacemos cleanup */

    return NULL; // MODIFICAR
}

