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






void execute_pipeline(pipeline apipe) {


}
/*
 * Ejecuta un pipeline, identificando comandos internos, forkeando, y
 *   redirigiendo la entrada y salida. puede modificar `apipe' en el proceso
 *   de ejecución.
 *   apipe: pipeline a ejecutar
 * Requires: apipe!=NULL
 */
