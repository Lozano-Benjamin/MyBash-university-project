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
   bool flag = false; 
    while ( type==ARG_NORMAL && !parser_at_eof(p) && aux != NULL ) {
                if (aux == NULL) {
                flag = true;
                }
                scommand_push_back(new_command, aux);
                aux = parser_next_argument(p, &type);
    }

    for (int i = 0; i<2; i++) {
        if (type == ARG_INPUT) {
                if (aux == NULL) {
                    flag = true;
                    break;
                }
                scommand_set_redir_in(new_command, aux);
                aux = parser_next_argument(p, &type);
        }else if (type == ARG_OUTPUT) {
                if (aux == NULL) {
                    flag = true;
                    break;
                }
                scommand_set_redir_out(new_command, aux);
                aux = parser_next_argument(p, &type);
        }
    }

    if (flag) {
        new_command = scommand_destroy(new_command);
        new_command = NULL;
    }

    return new_command; 
}     



pipeline parse_pipeline(Parser p) {
    pipeline result = pipeline_new();
    scommand cmd = NULL;
    bool error = false, another_pipe=true, background_status = false;
    cmd = parse_scommand(p); //
    error = (cmd==NULL); /* Comando inválido al empezar */
    while (another_pipe && !error) {    
        pipeline_push_back(result, cmd);
        parser_op_pipe(p, &another_pipe);
        if(another_pipe){
            cmd = parse_scommand(p);
        }
        error = (cmd == NULL);
    }
    parser_op_background(p,&background_status);
    if (background_status) {
        pipeline_set_wait(result, false); 
    }
    if (error) {
        result = pipeline_destroy(result);
        result = NULL;
    }
    
    return result; 
}
