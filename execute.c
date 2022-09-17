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



static int change_in(scommand cmd) {
    assert(cmd != NULL);
    char* in = scommand_get_redir_in(cmd);
    if(in != NULL){
        int file = open(in, O_RDONLY, S_IRUSR);
        if (file < 0){
            printf("pucha, no se encontró el archivo de input :c \n");
            exit(EXIT_FAILURE);
        }
        int res = dup2(file, STDIN_FILENO);
        if(res < 0){
            printf("Error redir\n");
            exit(EXIT_FAILURE);
        }
        int fd_close = close(file);
        if (fd_close <0){               // si el cierre da error (no deberia)
            printf ("Error close\n");
        }
    }
    return(EXIT_SUCCESS);
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

static void exec_single(scommand cmd) {
    if (scommand_get_redir_in(cmd) != NULL) {   //veo si tiene redir_in
        change_in(cmd);
    }
    if (scommand_get_redir_out(cmd) != NULL) {  //veo si tiene redir_out
        change_out(cmd);
    }

    char** argv = tomar_args(cmd);
    int errorcito = execvp(argv[0], argv); //argv[0] carga el nombre del comando (ls por ejemplo) y argv todos los argumentos (incluye ls al inicio y NULL al final)
    if (errorcito < 0) {
        printf("pucha, no se encontró el comando :(\n");
        exit(EXIT_FAILURE);
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
        else if (pid == 0) {    //forkeo
            exec_single(cmd);
        }
        else {
            //printf("soy el papi \n");
        }                        
    }                           
}                        

    



static void multiple_command_execution(pipeline apipe) {
    /*este multiple solo puede ejecutar hasta dos comandos
    * sea eso un comando del estilo 
    * ls -l | wc 
    */

    scommand fst_command = pipeline_front(apipe);  //guardo el primer comando
    assert(fst_command != NULL);
    pipeline_pop_front(apipe);
    scommand snd_command = pipeline_front(apipe);   //guardo el segundo comando
    assert(snd_command != NULL);


    /*
    * Coreccion del execute
    * La idea es un fork por comando
    * suerte porfa
    */

   int p[2];
   int err_pipe = pipe(p);
   if (err_pipe < 0) {printf("error al pipear");}

   pid_t pid = fork();
   if (pid < 0) {
        printf("error del tenedor \n");
   }
   else if (pid == 0) {
        int err_dup = dup2(p[1],1);
        if (err_dup < 0) {
            printf("error en el dup2 \n");
            exit(EXIT_FAILURE);
        }
        else {
            close(p[0]);
            close(p[1]);
            exec_single(fst_command); //el hijo se muere en el execvp
            //exit(EXIT_SUCCESS);
            printf("no se deberia llegar a este print \n");
        }
   }

   pid_t pid2 = fork();
   if (pid2 < 0) {
        printf("Error con el tenedor \n");
   }
   else if (pid2 == 0) {
        int err_dup = dup2(p[0],0);
        if (err_dup < 0) {
            printf("error en el dup2 \n");
            exit(EXIT_FAILURE);
        }
        else {
            close(p[0]);
            close(p[1]);
            exec_single(snd_command); //el hijo se muere en el execvp
            printf("no se deberia llegar a este print \n");
        }
   }

   else /*(pid2 > 0)*/ {
        //el padre ni idea
        close(p[0]);
        close(p[1]);
        // wait(NULL); //este wait no tiene sentido pues no espera a nadie
   }
}


static void execute_foreground(pipeline apipe) {
    //vemos si es simple o multiple
    if (pipeline_length(apipe) == 1) {
        single_command_execution(pipeline_front(apipe)); //le paso el comando solito que tiene
    }
    else if (pipeline_length(apipe) == 2){
        multiple_command_execution(apipe);              //ejecuto el comando multiple
        //wait(NULL); //este wait hace que se muestre bien el prompt pero hace que ande mal la otra cosa
    }
    else {
        printf("Lo sentimos, solo hasta dos comandos pipeados )): (ejemplo: ls -l | wc) \n");
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

        int dup2_result= dup2(p_lect, 0); //redireccion 
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
        //wait(NULL); //este wait hace que el prompt se muestre bien pero HACE QUE NO ANDE EL BACKGROUND LOL XD
    }
}




void execute_pipeline(pipeline apipe) {

    /* execute_pipline funciona primero chequeando si el comando es fore o back (ground)
    * luego de ver su wait, se ve su largo
    *   de ahi se ejecutan cada uno
    * 
    */
    uint n = pipeline_length(apipe);
    if (pipeline_get_wait(apipe)) {
        execute_foreground(apipe);
        wait(NULL);    //este wait hace que el prompt se muestre antes del comando
    }
    else {
        execute_background(apipe); //el back se ejecuta con el fore, una flasheada
        wait(NULL); //ESTE WAIT CORRIGÉ EL ERROR DEL WAIT DE LA LINEA 303
    }
    if (n == 2) {
        wait(NULL); //este hace que no se bugee el prompt en pipes , espero que no genere bugs
    } //es una solucion medio ad hoc, no lo voy a negar, pero soluciona un test relacionado a la cantidad de waits
}
