#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "command.h"
#include "builtin.h"

//librerias para que ande las syscalls
#include <unistd.h> 
#include <sys/syscall.h>
#include <sys/types.h>


extern bool quit;

static void run_cd(scommand cmd) {

/*
Para el cd son tres casos (pues lo complicado lo maneja la syscall)
ver si los argumentos estan de mas (error)
ver si el path es correcto (ver si retorna error y printear)
andar normalito (aca todo lo complicado lo maneja la syscall)
ademas, cd (a secas) te manda a home

*/
    unsigned int n = scommand_length(cmd);
    if (n > 2) {    //cd algo/algo basura
        printf("Muchos argumentos\n");
    }
    else if (n == 2) { //cd path
        scommand_pop_front(cmd);
        char* path = scommand_front(cmd);
        int err = syscall(SYS_chdir, path);
        if (err != 0) {
            printf("pucha, no se encontró el directorio :( \n");
        }
    }
    else if (n == 1) { //cd (a secas, te salta a home)
        int err = syscall(SYS_chdir, "/home");
        if (err != 0) {
            printf("No se como esto dio un error \n");
        }
    }
}


static void run_help(scommand cmd) {
    printf ("\n");
    printf ("Mybash :) \n");
    printf ("\n");
    printf ("Somos 4 autores: \nBenjamin Lozano\nFabrizio Longhi\nGaston Bonfils\nFacundo Ayala\n");
    printf ("\n");
    printf ("Hay tres comandos internos:\nhelp: es este mensaje :) \nexit: sale de la consola (porfavor no lo hagas:/ )\ncd: te mueve de directorio, usando path relativo y absoluto (ej: /home/USUARIO/Documentos/)\n ");
    printf ("\n");
}

static void run_exit(scommand cmd)  {
    printf ("Chau chau!!! nos vemoooooos!!! c:\n");
    quit= true;
}

bool builtin_is_internal(scommand cmd) {
    assert(cmd != NULL);
    bool is_cd = strcmp(scommand_front(cmd),"cd") == 0;
    bool is_help = strcmp(scommand_front(cmd),"help") == 0;
    bool is_exit = strcmp(scommand_front(cmd),"exit") == 0;
    return is_cd || is_help || is_exit;
}


bool builtin_alone(pipeline p){
    assert(p!=NULL);
    bool is_alone = pipeline_length(p) == 1 && builtin_is_internal(pipeline_front(p));
    return is_alone; 
}


void builtin_run(scommand cmd){
    assert(builtin_is_internal(cmd));
    if (strcmp(scommand_front(cmd),"cd") == 0){
        run_cd(cmd);
    }
    else if (strcmp(scommand_front(cmd),"help") == 0){
        run_help(cmd);
    }
    else if (strcmp(scommand_front(cmd),"exit") == 0){
        run_exit(cmd);
    }

}


