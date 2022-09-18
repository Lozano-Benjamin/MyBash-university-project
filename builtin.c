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

#define ANSI_COLOR_RESET   "\x1b[0m"
char COLOR_TEXT[32] = "\x1b[0m";
char COLOR_HOST[32] = "\033[38;2;153;255;51m";
char COLOR_PATH[32] = "\033[38;2;153;204;255m";


// void color_init(void) {
//     COLOR_HOST = "\033[38;2;153;255;51m";
//     COLOR_PATH = "\033[38;2;153;204;255m";
//     COLOR_TEXT = "\x1b[0m";
// }


void show_prompt(void) {
    char cwd[1024];
    getcwd(cwd, 1024);
    char host[1024];
    gethostname(host, 1024);
    char user[1024];
    getlogin_r(user, 1024);


    printf ("%smybash %s%s@%s:%s %s> %s",COLOR_TEXT ,COLOR_HOST,user, host, COLOR_PATH ,cwd, COLOR_TEXT);
    fflush (stdout);
}


static void color_change(scommand cmd) {
    //color 0/1/2/3 (((paletas)))
    //"\033[38;2;153;204;255m"
    uint n = scommand_length(cmd);
    if (n == 2) {
        scommand_pop_front(cmd);
        char* palet = scommand_front(cmd);
        if (strcmp(palet, "0") == 0) {

            strcpy(COLOR_TEXT,"\x1b[0m");
            strcpy(COLOR_HOST,"\033[38;2;153;255;51m");
            strcpy(COLOR_PATH,"\033[38;2;153;204;255m");
        }
        else if (strcmp(palet, "1") == 0) {
            strcpy(COLOR_TEXT,"\033[38;2;00;255;00m");
            strcpy(COLOR_HOST,"\033[38;2;00;255;00m");
            strcpy(COLOR_PATH,"\033[38;2;00;255;00m");
        }
        else if (strcmp(palet, "2") == 0) {
            strcpy(COLOR_TEXT,"\033[38;2;156;186;188m");
            strcpy(COLOR_HOST,"\033[38;2;92;142;148m");
            strcpy(COLOR_PATH,"\033[38;2;220;209;226m");
        }
        else {
            printf("Paleta invalida.\n Paletas disponibles del 0 al 2");
        }

    }
    else {
        printf("Se puede cambiar la paleta de colores de mybash usando color con un número del 0 al 2\nPaletas disponibles: \n0 : defecto\n1 : 'hacker'\n2 : invierno gris \n");
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
    printf ("Autores: \nBenjamin Lozano\nFabrizio Longhi\nGaston Bonfils\nFacundo Ayala\n");
    printf ("\n");
    printf ("Comandos internos:\n -help: es este mensaje :) \n -exit: sale de la consola (porfavor no lo hagas :c )\n -cd: cambia de directorio, usando path relativo o absoluto (ej: /home/USUARIO/Documentos/)\n -color: cambia la paleta de colores (usar color solo para ver su uso)");
    printf ("\n");
}

static void run_exit(scommand cmd)  {
    printf ("Chau chau!!! nos vemoooooos!!! c:\n");
    exit(2);
    //quit= true;
}

bool builtin_is_internal(scommand cmd) {
    assert(cmd != NULL);
    bool is_cd = strcmp(scommand_front(cmd),"cd") == 0;
    bool is_help = strcmp(scommand_front(cmd),"help") == 0;
    bool is_exit = strcmp(scommand_front(cmd),"exit") == 0;
    bool is_color = strcmp(scommand_front(cmd),"color") == 0;
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


