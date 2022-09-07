#include <endian.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h> //

#include "command.h"
#include "builtin.h"

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



static int command_execution(scommand cmd) {
    if (builtin_is_internal(cmd)) {
        builtin_run(cmd);
    }
    else {
        char *args[] = {"ls", "-l", NULL};       //ver manera de pasar los args del scommand a array para el exexvp
        execvp("ls",args);
    }
}



void execute_pipeline(pipeline apipe) {


}
/*
 * Ejecuta un pipeline, identificando comandos internos, forkeando, y
 *   redirigiendo la entrada y salida. puede modificar `apipe' en el proceso
 *   de ejecución.
 *   apipe: pipeline a ejecutar
 * Requires: apipe!=NULL
 */
