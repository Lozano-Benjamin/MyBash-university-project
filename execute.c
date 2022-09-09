#include <endian.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>


#include "command.h"
#include "builtin.h"
#include "execute.h"

/*
Lista de tareas a tomar en cuenta:!
Modificar las redirecciones (in/out, dup2)
Ejecutar un comando simple (los fork() y execvp())
Ejecutar pipelines
Ejecutar en foreground y esas cosas

"La primera tarea del módulo de ejecución es reconocer entre comandos internos y externos, 
y decidir si invocar a una función interna o a la ejecución de procesos de manera externa.
"

Arrancar con comandos simples e ir escalando de a poco


*/

static char** tomar_args(scommand cmd) {        //funcion propia para tomar los argumentos de un comando
    assert(cmd != NULL);    

    unsigned int n = scommand_length(cmd);
    char ** argv = calloc(sizeof(char*), n+1);  //hago un array argv de strings (todo dinamico asi lo puedo retornar)
    for (unsigned int i = 0; i < n; i++) {      
        char* arg = scommand_front(cmd);
        argv[i] = arg;
        scommand_pop_front(cmd);                //voy poniendo los argumentos en el array 
    }
    argv[n] = NULL;             //ultimo elemento NULL, esto por como funciona el execvp

    return argv; //el array argv quedaria por ejemplo ["ls", "-l", NULL]
}


static void single_command_execution(scommand cmd) {
    assert(cmd != NULL);

    if (builtin_is_internal(cmd)) {     //distinccion de casos de si es interno el comando o no
        builtin_run(cmd);
    }

    else if (!scommand_is_empty(cmd)) {
        pid_t pid = fork();
        if (pid < 0) {
            printf("Error del fork en single_command_execution \n");
        }
        else if (pid == 0) {
            char** argv = tomar_args(cmd);
            execvp(argv[0], argv); //argv[0] carga el nombre del comando (ls por ejemplo) y argv todos los argumentos (incluye ls al inicio y NULL al final)
        }
        else {
            printf("soy el papi \n");
        }                        
    }                           
                                
}

static void multiple_command_execution(pipeline apipe) {
    //hacer en algun momento xd
    printf("jaja comando multiple \n");
}

static void execute_foreground(pipeline apipe) {
    //vemos si es simple o multiple
    if (pipeline_length(apipe) == 1) {
        single_command_execution(pipeline_front(apipe)); //le paso el comando solito que tiene
    }
    else {
        multiple_command_execution(apipe);
    }
}


/*Para poder hacer correr el background, voy a correr todo el proceso en un hijo mediante un fork */
static void execute_background(pipeline apipe){
    pid_t pid = fork();
    if (pid == -1) {
        perror("Error en el fork al ejecutarse en background.");
    }else if(pid == 0){
        //Aca va el codigo de ejecucion en background
    }else {
        /*Proceso padre, va a esperar a que se termine el fork de su hijo,
          entonces solo necesitamos hacer un Wait al primero porque internamente los
          demas procesos padre esperaran a sus hijos. */
          wait(NULL);
    }
}




void execute_pipeline(pipeline apipe) {

    if (pipeline_get_wait(apipe)) {
        execute_foreground(apipe);
    }
    else {
        execute_background(apipe);
    }
/*
primero ver si tiene un wait y despues ver su largo

        ES BACK O FORE?
    e simple    o      multiple?
ver dos casos:
    - comando simple (ejecuto  un solo comando)
    - pipeline doble (hago pipe)
*/

}
/*
 * Ejecuta un pipeline, identificando comandos internos, forkeando, y
 *   redirigiendo la entrada y salida. puede modificar `apipe' en el proceso
 *   de ejecución.
 *   apipe: pipeline a ejecutar
 * Requires: apipe!=NULL
 */
