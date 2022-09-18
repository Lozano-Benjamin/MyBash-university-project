#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "parsing.h"
#include "parser.h"
#include "command.h"

static scommand parse_scommand(Parser p) {
    scommand new_command = scommand_new();  /* Inicializacion del scommand */
    arg_kind_t type =ARG_NORMAL ;   /* Inicializacion del type */
    char *aux = parser_next_argument(p, &type); /* Aca vamos a parsear el primer argumento */
    bool flag = false;  /* La flag de incompletitud se va a mantener en false al principio. */
    while (type == ARG_NORMAL && !parser_at_eof(p) && p != NULL && aux != NULL) {
        if (aux == NULL) {  /* En caso de que el argumento sea NULL la flag se va a levantar */
            flag = true;    
            break;  /* El bucle se rompe si la flag esta levantada */
        }
        scommand_push_back(new_command, aux);   /* En caso de que aux no sea NULL, se va a ir introduciendo en 
                                                                  la variable new_command */
        aux = parser_next_argument(p, &type); /* Parseamos un nuevo comando */
    }

    for (int i = 0; i<2; i++) {         /* Aca checkeamos todas las permutaciones posibles de los argumentos input y output. */
        if (type == ARG_INPUT) {
            if (aux == NULL) {  /* Si el comando esta incompleto, se rompe. Es decir si existe "ls <" 
                                    el input seria NULL. */
            flag = true;
            break;              /* Si esta incompleto, rompemos el bucle. */
        }
        scommand_set_redir_in(new_command, aux);    /* Setteamos el input en el scommand */
        aux = parser_next_argument(p, &type);     /* Parseamos el siguiente argumento */
        }
        else if (type == ARG_OUTPUT) {  /* Caso en el que el argumento sea de output */
            if (aux == NULL) {  /* Si el output no existe, pero si la redireccion, se rompe
                                    ejemplo: "ls >" */
            flag = true;
            break;  /* Si esta incompleto, rompemos el bucle */
        }
        scommand_set_redir_out(new_command, aux);   /* Setteamos el output en el scommand. */
        aux = parser_next_argument(p, &type);     /* Parseamos el siguiente argumento */
        }   
    }

    if (flag || scommand_is_empty(new_command)) {   /* Dos posibilidades de que devolvamos NULL en el parseo:
                                                            que se levante la flag o que el scommand este vacio. */
        new_command = scommand_destroy(new_command);    /* Destruimos el scommand */
        new_command = NULL;                                    /* Lo setteamos en NULL */
    }   

    return new_command; 
}

pipeline parse_pipeline(Parser p) {
    pipeline result = pipeline_new();       /* Inicializacion de la pipe */
    scommand cmd = NULL;        /* variable auxiliar donde vamos a ir parseando cada command. */
    bool error = false, another_pipe=true, background_status = false, garbage = false;  /* Inicializacion de los booleanos */
    cmd = parse_scommand(p); /* Parseamos el primer comando de la pipe */
    error = (cmd==NULL); /* checkeamos si existe primer comando. */
    while (another_pipe && !error && p != NULL) {    /* Vamos a iterar mientras existan pipes, comandos y no hayamos llegado al fin del parser */
        pipeline_push_back(result, cmd);    /* metemos un comando a la pipe */
        parser_op_pipe(p, &another_pipe);   /* checkeamos si existe otra pipe */
        if(another_pipe){       
            cmd = parse_scommand(p);    /* En caso de existir otra pipe, vamos a parsear otro comando mas */
        }
        error = (cmd == NULL);  /* Revisamos si existe otro comando, o en caso de existir, si es valido */
         
    }
    if (p != NULL && result != NULL) {  /* Si no llegamos al final del parser y la pipe no es NULL*/
        parser_op_background(p,&background_status); /* vemos si la pipe esta en background */
        parser_garbage(p, &garbage);    /* Como terminamos de parsear la pipe completa, no queremos basura */
        if (background_status) {
            pipeline_set_wait(result, false);   /* En caso de que la pipe se quiera ejecutar en background, setteamos la
                                                            espera en false, ya que asi no esperaremos la ejecucion de los comandos. */
        }
    }

    if (error) {    /* En caso de que la pipe contenga comandos incompletos */
        result = pipeline_destroy(result);  /* Destruimos la pipe */
        result = NULL;                            /* Setteamos la pipe en NULL */
    }
    return result; 
}
