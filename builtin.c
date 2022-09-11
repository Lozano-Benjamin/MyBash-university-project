/*
Cosas que quedan para hacer en esto
Terminar el exit
Mejorar el help
Ver si hay una mejor forma para modularizar
*/


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
    printf ("Ayuda para el help\n");

}

static void run_exit(scommand cmd) {
    printf ("Chau chau\n");
    quit= true;
}

bool builtin_is_internal(scommand cmd) {
    assert(cmd != NULL);
    bool is_cd = strcmp(scommand_front(cmd),"cd") == 0;
    bool is_help = strcmp(scommand_front(cmd),"help") == 0;
    bool is_exit = strcmp(scommand_front(cmd),"exit") == 0;
    return is_cd || is_help || is_exit;
}
/*
 * Indica si el comando alojado en `cmd` es un comando interno
 *
 * REQUIRES: cmd != NULL
 *
 */


bool builtin_alone(pipeline p){
    assert(p!=NULL);
    bool is_alone = pipeline_length(p) == 1 && builtin_is_internal(pipeline_front(p));
    
    return is_alone; 
}
/*
 * Indica si el pipeline tiene solo un elemento y si este se corresponde a un
 * comando interno.
 *
 * REQUIRES: p != NULL
 *
 * ENSURES:
 *
 * builtin_alone(p) == pipeline_length(p) == 1 &&
 *                     builtin_is_internal(pipeline_front(p))
 *
 *
 */

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
/*
 * Ejecuta un comando interno
 *
 * REQUIRES: {builtin_is_internal(cmd)}
 *
 * 
 * 
 * La forma de esto seria algo como
 * if == front(cd) 
 *  {run_cd}
 * if == front(exit)
 *  {run_exit}
 * .....
 */



