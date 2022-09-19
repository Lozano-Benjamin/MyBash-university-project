#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h> 

#include "command.h"
#include "execute.h"
#include "parser.h"
#include "parsing.h"
#include "builtin.h"

int main(int argc, char *argv[]) {
    pipeline pipe;
    Parser input;
    quit = false;

    while (!quit) {
        input = parser_new(stdin);   /* El parser toma la entrada por teclado */
        show_prompt();                      /* Mostramos prompt con ifno */
        pipe = parse_pipeline(input);   /* Se parsea lo ingresado por teclado */

        quit = parser_at_eof(input);    /* Revision si debemos salir */

        if (pipe != NULL) { /* Si la pipe no se rompe */
            execute_pipeline(pipe); /* Ejecutamos la pipe */
            pipe = pipeline_destroy(pipe);  /* Destruimos la pipe ejecutada */
        }

    }
    input = parser_destroy(input); input = NULL;    /* Destruimos el parser y seteamos en NULL */
    return EXIT_SUCCESS;

}