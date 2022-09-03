

#include <stdbool.h>

#include "command.h"

//librerias para que ande las syscalls
#include <unistd.h> 
#include <sys/syscall.h>
#include <sys/types.h>

static void run_cd(scommand cmd) {
    //cd home/aqui
/*
Para el cd son tres casos (pues lo complicado lo maneja la syscall)
ver si los argumentos estan de mas (error)
ver si el path es correcto (ver si retorna error y printear)
andar normalito (aca todo lo complicado lo maneja la syscall)
ademas, cd (a secas) te manda a home

*/
    unsigned int n = scommand_length(cmd);
    if (n > 2) {    //cd algo basura
        printf('Muchos argumentos\n');
    }
    else if (n == 2) { //cd path
        scommand_pop_front(cmd);
        char* path = scommand_front(cmd);
        int err = syscall(SYS_chdir, path);
        if (err != 0) {
            printf('pucha, no se encontró el directorio\n');
        }
    }
    else if (n == 1) { //cd (a secas, te salta a home)
        int err = syscall(SYS_chdir, "./home");
        if (err != 0) {
            printf("No se como esto dio un error");
        }
    }
}

static void run_help() {

}

static void run_exit() {

}

bool builtin_is_internal(scommand cmd);
/*
 * Indica si el comando alojado en `cmd` es un comando interno
 *
 * REQUIRES: cmd != NULL
 *
 */


bool builtin_alone(pipeline p);
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

void builtin_run(scommand cmd);
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


