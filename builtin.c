#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "command.h"
#include "builtin.h"
#include "strextra.h"

//librerias para que ande las syscalls
#include <unistd.h> 
#include <sys/syscall.h>
#include <sys/types.h>
/* Inicializacion global de colores del prompt */
#define ANSI_COLOR_RESET   "\x1b[0m"
char COLOR_TEXT[32] = "\x1b[0m";    /* Colores del texto */
char COLOR_HOST[32] = "\033[38;2;153;255;51m";  /* Colores pertenecientes al host */
char COLOR_PATH[32] = "\033[38;2;153;204;255m"; /* Colores del path */


/* Funcion que muestra el prompt */
void show_prompt(void) {
    char cwd[1024]; /* Creamos un string para el path */
    getcwd(cwd, 1024);  /* Esta funcion nos devuelve el path y la guarda en el array cwd */

    char host[256];     /* Creamos un string para el host */
    gethostname(host, 256);   /* Esta funcion nos devuelve el nombre del host y 
                                            lo guarda en el string host */

    char user[256];    /* Creamos un string para el user */
    getlogin_r(user, 256); /* Esta funcion nos devuelve el usuario loggeado y 
                                            lo guarda en el string user */


    printf ("%smybash %s%s@%s:%s %s> %s",COLOR_TEXT ,COLOR_HOST,user, host, COLOR_PATH ,cwd, COLOR_TEXT); /* Printf que se muestra en cada ejecucion de pipe */
    fflush (stdout);
}

/* Funcion para cambiar el color del prompt */
static void color_change(scommand cmd) {
    //uso : color 0/1/2/3 (((paletas)))
    uint n = scommand_length(cmd);  /* Vemos el largo del comando */
    if (n == 2) {   /* En caso de ser igual a 2 (argumento color + paleta) */
        scommand_pop_front(cmd);    /* Eliminamos el argumento color */
        char* palet = scommand_front(cmd);  /* Tomamos la paleta introducida */
        if (strcmp(palet, "0") == 0) {  /* Si es 0, se usa la paleta por defecto */

            strcpy(COLOR_TEXT,"\x1b[0m");
            strcpy(COLOR_HOST,"\033[38;2;153;255;51m");
            strcpy(COLOR_PATH,"\033[38;2;153;204;255m");
        }
        else if (strcmp(palet, "1") == 0) { /* Si es 1, se usa la paleta hacker */
            strcpy(COLOR_TEXT,"\033[38;2;00;255;00m");
            strcpy(COLOR_HOST,"\033[38;2;00;255;00m");
            strcpy(COLOR_PATH,"\033[38;2;00;255;00m");
        }
        else if (strcmp(palet, "2") == 0) { /* Si es 2, se usa la paleta invierno gris. */
            strcpy(COLOR_TEXT,"\033[38;2;156;186;188m");
            strcpy(COLOR_HOST,"\033[38;2;92;142;148m");
            strcpy(COLOR_PATH,"\033[38;2;220;209;226m");
        }
        else if (strcmp(palet, "3") == 0) { /* Si es 3, se usa la paleta invierno gris. */
            strcpy(COLOR_TEXT,"\033[38;2;248;177;147m");
            strcpy(COLOR_HOST,"\033[38;2;244;114;128m");
            strcpy(COLOR_PATH,"\033[38;2;195;118;138m");
        }
        else {  /* Si el argumento de paleta no es 0,1,2 se da un aviso. */
            printf("Paleta invalida.\n Paletas disponibles del 0 al 2\n");
        }

    }
    else {  /* Si no se da el formato correcto se larga un aviso. */
        printf("Se puede cambiar la paleta de colores de mybash usando color con un número del 0 al 3\n"
        "Paletas disponibles: \n"
        "0 : defecto\n"
        "1 : 'hacker'\n"
        "2 : invierno gris\n"
        "3 : rosita?\n");
    }

}

static void run_cd(scommand cmd) {

/*
Para el cd son tres casos (pues lo complicado lo maneja la syscall)
ver si los argumentos estan de mas (error)
ver si el path es correcto (ver si retorna error y printear)
andar normalito (aca todo lo complicado lo maneja la syscall)
ademas, cd (a secas) te manda a home

*/
    unsigned int n = scommand_length(cmd);
    assert(cmd != NULL);
    if (n > 2) {    //cd algo/algo basura
        printf("Muchos argumentos\n");  /* Aviso de exceso de args */
    }
    else if (n == 2) { /* Caso donde querramos saltar a otro directorio en base al que estamos. */
        scommand_pop_front(cmd);
        char* path = scommand_front(cmd);
        int err = syscall(SYS_chdir, path);
        if (err != 0) {
            printf("pucha, no se encontró el directorio :( \n");
        }
    }
    else if (n == 1) { /* Caso en donde querramos volver al home */
        char* home_path = getenv("HOME"); //escriben en home_path "home/usuario"
        int err = syscall(SYS_chdir, home_path);
        if (err != 0) {
            printf("No se como esto dio un error \n");
        }
    }
}


static void run_help(scommand cmd) {
    printf ("\nBienvenido a Mybash :)\nHecho por: \n"
    "Benjamin Lozano\nFabrizio Longhi\nGaston Bonfils\nFacundo Ayala\n"
    "Comandos internos:\n"
    " -help: es este mensaje :) \n -exit: sale de la consola (porfavor no lo hagas :c )\n "
    "-cd: cambia de directorio, usando path relativo o absoluto (ej: /home/USUARIO/Documentos/)\n "
    "-color: cambia la paleta de colores. Usar 'color' para ver la lista. \n");
}

static void run_exit(scommand cmd)  {
    printf ("Chau chau!!! nos vemoooooos!!! c:\n"); /* Despedida */
    quit= true;
}

bool builtin_is_internal(scommand cmd) {
    assert(cmd != NULL);
    bool is_cd = strcmp(scommand_front(cmd),"cd") == 0,
    is_help = strcmp(scommand_front(cmd),"help") == 0,
    is_exit = strcmp(scommand_front(cmd),"exit") == 0,
    is_color = strcmp(scommand_front(cmd),"color") == 0;
    return is_cd || is_help || is_exit || is_color;
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
    else if (strcmp(scommand_front(cmd),"color") == 0){
        color_change(cmd);
    }

}


