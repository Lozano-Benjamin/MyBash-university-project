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
    printf ("mybash %s>", getcwd(s, 100));
    fflush (stdout);
}

int main(int argc, char *argv[]) {
    pipeline pipe;
    Parser input;
    bool quit = false;

    input = parser_new(stdin);
    while (!quit) {
        show_prompt();
        pipe = parse_pipeline(input);
        execute_pipeline(pipe);

        /* Hay que salir luego de ejecutar? */
        quit = parser_at_eof(input);
        /*
         * COMPLETAR
         *
         */
    }
    input = parser_destroy(input); input = NULL;
    return EXIT_SUCCESS;
}

