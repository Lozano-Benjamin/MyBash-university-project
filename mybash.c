#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h> 

#include "command.h"
#include "execute.h"
#include "parser.h"
#include "parsing.h"
#include "builtin.h"

static void show_prompt(void) {
    char s[100];
    printf ("mybash %s> ", getcwd(s, 100));
    fflush (stdout);
}

int main(int argc, char *argv[]) {
    pipeline pipe;
    Parser input;
    quit = false;


    while (!quit) {
        input = parser_new(stdin);      //poner esta linea adentro del bucle
        show_prompt();                  //resuelve el problema de que no pida
        pipe = parse_pipeline(input);

        /* Hay que salir luego de ejecutar? */
        quit = parser_at_eof(input);    

        // if (pipe == NULL) {
        //     printf("eeee que?!?!? \n");
        // }
        if (pipe != NULL) {
            execute_pipeline(pipe);
            pipe = pipeline_destroy(pipe);
        }
        /*
         * COMPLETAR
         *
         */
    }
    input = parser_destroy(input); input = NULL;
    return EXIT_SUCCESS;
}

