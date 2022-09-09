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

#include "tests/syscall_mock.h" //libreria para que los tests funcionen correctamente
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



static void change_in(scommand cmd) {

}

static void change_out(scommand cmd) {
    assert (cmd !=NULL);

    char * out_r = scommand_get_redir_out(cmd);

    if (out_r != NULL){
        int fd_out = open (out_r, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR); //crea como lectura y escritura, le da permiso al usuario

        if (fd_out < 0){                //si fd da error
            printf ("Error en fd\n");
        }

        int redir_out = dup2 (fd_out,1); //redireccion del file descriptor

        if (redir_out < 0){              //si la redireccion da error
            printf ("Error redir\n");
        }   

        int fd_close= close (fd_out);   //cierre del file desecriptor
        
        if (fd_close <0){               // si el cierre da error (no deberia)
            printf ("Error close\n");
        }
    }
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
            if (scommand_get_redir_in(cmd) != NULL) {
                change_in(cmd);
            }
            if (scommand_get_redir_out(cmd) != NULL) {
                change_out(cmd);
            }

            char** argv = tomar_args(cmd);
            int errorcito = execvp(argv[0], argv); //argv[0] carga el nombre del comando (ls por ejemplo) y argv todos los argumentos (incluye ls al inicio y NULL al final)
            if (errorcito < 0) {
                printf("pucha, no se encontro el comando :(\n");
            }
        }
        else {
            //printf("soy el papi \n");
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
        /*Pipe es una funcion que comunica dos procesos mediante un array 
        de dos dimensiones, AKA: int pipe(int pipefd[2]);, por lo que voy a trabajar con eso.
        Tambien voy a usar el array como archivo vacio, y voy a cerrar su punta de escritura*/
        int pipefd[2];
        int pipe_result = pipe(pipefd);
        if (pipe_result == -1) {
            perror("Error en el pipe al ejecutarse en background");
            exit(EXIT_FAILURE);  //Se cierra y da un status de fallo
        }
        int p_lect = pipefd[0];
        int p_esc = pipefd[1];
        close(p_esc);

        int dup2_result= dup2(p_lect, STDIN_FILENO); //Averiguando esto es parecido al fd de STDIN sobre archivos.
        if (dup2_result == -1) {
            perror("Error en dup2 al ejecutarse en background");
            exit(EXIT_FAILURE); //Se cierra y da un status de fallo
        }
        execute_foreground(apipe);
        exit(EXIT_SUCCESS);
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
        wait(NULL);    //este wait hace que el prompt se muestre antes del comando
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
