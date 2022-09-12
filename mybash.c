#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h> 

#include "command.h"
#include "execute.h"
#include "parser.h"
#include "parsing.h"
#include "builtin.h"


#define ANSI_COLOR_RESET   "\x1b[0m"
#define COLOR_HOST "\033[38;2;153;255;51m"
#define COLOR_PATH "\033[38;2;153;204;255m"

static void show_prompt(void) {
    char cwd[100];
    getcwd(cwd, 100);
    char host[1024];
    gethostname(host, 1024);
    char user[1024];
    getlogin_r(user, 1024);

    printf ("mybash" COLOR_HOST " %s@%s:" COLOR_PATH" %s> " ANSI_COLOR_RESET, user, host ,cwd);
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

        // if ((pipe == NULL)) {
        //     printf("Sintaxis inválida\n");  //este print es para comandos como "ls >"
        // }
        
        if (pipe != NULL) {
            execute_pipeline(pipe);
            pipe = pipeline_destroy(pipe);
        }

    }
    input = parser_destroy(input); input = NULL;
    return EXIT_SUCCESS;

}