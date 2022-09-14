# Informe Lab 01
- Ayala Facundo
- Bonfils Gastón Tomás 
- Longhi Heredia Fabrizio Mateo
- Lozano Benjamín

# Introduccion
Se nos asigno la tarea de codificar una shell al estilo de bash (*Born Again Shell*) a la que denominamos *mybash*, utilizando el lenguaje C, a realizar en un periodo de aproximadamente 4 semanas, las cuales nos dedicamos a distribuirnos tareas e investigar acerca del funcionamiento del bash de linux para replicar su comportamiento. Tambien nos dedicamos a aprender a trabajar en equipo con git (algunos no habian usado nunca git), coordinarnos y convivir con diferentes formas de programar. Fue nuestro primer proyecto en equipo. Logramos realizar una shell funcional con un prompt mejorado esteticamente. Aunque es una version muy basica, esperemos les guste y sea útil. 


# Metodologia de trabajo
Para la realizacion del trabajo tomamos de base la metodologia de cascada (*waterfall methodology*) ya que consideramos, a diferencia de las metodologias agiles, que se adaptaba mejor a nuestras necesidades ya que esta metodologia sigue una estructura lineal acorde a lo que se nos aconsejo que sigamos. Esto es, principalmente a que no avanzabamos a otro modulo del proyecto hasta haber finalizado el anterior, tomando como guia la realizacion de los modulos mas simples a los mas complejos. 

Con respecto a la division de tareas y a la organizacion realizamos encuentros virtuales diarios, usando la plataforma *discord* en donde cada integrante comentaba en que pudo avanzar y si se encontraba con alguna dificultad.

Al momento de completar el primer modulo lo hicimos dividiendo las funciones a completar, para que luego otro integrante se encargue de verificar lo realizado por su compañero con fin de eliminar posibles errores. Con respecto al segundo y tercer modulo lo realizamos dividiendonos en equipos de a dos para minimizar el tiempo ya que se trataban de modulos simples y cortos. Por ultimo, para realizar el modulo `execute` repetimos el metodo usado para el primer modulo dividiendo las funciones entre los integrantes.

Asimismo para el control de versionado utilizamos la plataforma *bitbucket*




# Contenido

* [Modulos e implemetaciones](#Módulos-e-implementaciones)
    * [Command](#Command)
    * [Parser y parsing](#Parser-y-Parsing)
    * [Builtin](#Builtin)
    * [Execute](#Execute)
    * [myBash](#myBash)
* [Solución de error de compatibilidad](#Solución-de-error-de-compatibilidad)
    * [Versión que anda a la mitad](#Versión-que-anda-a-la-mitad)
    * [Versión que anda a todos pero con un test fallido](#Versión-que-anda-a-todos-pero-con-un-test-fallido)

# Módulos e implementaciones

## Command
El módulo command se encarga de implementar el TAD del `scommand` y del `pipeline`. Este módulo no fue complicado de hacer pues usamos las listas de la librería de `GLib` los cuales agilizaron la mayoría de funciones.
### scommand
Los comandos simples se implementaron como un struct de una lista `GSList` (lista enlazada) y con campos de `in` y `out` implementados como strings.

```c
struct scommand_s {
    GSList *comm_args;
    char *out;
    char *in;
};
```

### pipelines
Los pipelines se implementaron como un struct que contiene una lista `GSList` de `scommand_s` y un `bool` que indica si la pipeline se ejecuta en background o en foreground.
```c
struct pipeline_s {
    GSList* command_list;
    bool  wait;
};
```

## Parser y Parsing
La tarea de estos modulos, como sus nombres lo indican, es la de recorrer el `stdin ` de manera lineal e ir tomando los comandos, argumentos, redirectores, pipes y operador de segundo plano para luego armar una instacia del tipo `pipeline` con la interpretacion de los datos de entrada. Cabe destacar que estos modulos no se encargan de verificar si los comandos simples o multiples son validos (dicha tarea esta asignada al modulo execute mediante las system calls) sino mas bien de que los comandos, argumentos, redirectores, pipes y operador de segundo plano respeten su estructura sintactica.\

> [name=fernuco] Insertar ejemplo de comando valido para parsear y un comando invalido.

Estos modulos estan compuestos por 3 encabezados.
* `parser.h`
* `parsing.h`
* `parsing.c`

### `Parser.h`
En este encabezado se encuentra la interfaz del modulo `parser` la cual ya se encuentra implementada por la catedra y cuya implementacion se encuentra en los binarios `parser.o` y `lexer.o`

### `Parsing.h`
En este encabezado se encuentra la interfaz del modulo `parsing` cuya implementacion se encuentra en el encabezado `parsing.c`

### `Parsing.c`
En este encabezado se encuentran implementadas las funciones `parse_pipeline` y `scommand parse_scommand`.


La funcion `parse_scommand` es de caracter estatico y se encarga de armar una instancia del tipo `scommand` con sus respectivos comandos, argumentos y redirectores. En caso de que despues de un redirector no haya nada esto devuelve NULL ya que no estaria respetando la sintaxis correcta.

Por otro lado, la funcion `parse_pipeline` se encarga de crear una instancia del tipo pipeline, para esto, hace una llamada a la funcion `parse_scommand` para luego insertar dichos comandos a la pipeline. A su vez nos indica si se encontro un operador de background.


## Builtin
El módulo builtin se encarga de ejecutar los comandos internos que se quieran implementar. Nosotros decidimos implementar únicamente los comandos que se nos pidieron en un principio siendo ellos:
* `cd` para cambiar de directorio en el que trabaja
* `help` para mostrar información en pantalla sobre el MyBash
* `exit` para cerrar el programa\

A la hora de implementar estos comandos se modularizo la función `bultin_run` de manera tal que al detectar el comando que se pasa directamente se ejecuta una función `run_XX` donde XX es cd, help o exit.

### `cd`
La función `run_cd` se implemento utilizando la función `syscall` importada de la librería `<sys/syscall.h>`. Esta función toma una syscall, sus parametros y la ejecuta. La syscall `chdir` maneja por si sola la distincción entre path relativo y path absoluto. Nosotros agregamos un caso por si no se le pasa un path (sea `cd` y nada mas) para volver al directorio `home` del usuario.

### `help`
`run_help` no tiene mucho misterio, es simplemente un `printf` con un mensajito de ayuda.

### `exit`
Al principio no entendiamos la manera de implementar `run_exit` pero luego trabajando en `mybash.c` vimos la variable `quit` que manejaba el loop de parseo de pipelines. En `builtin.c` globalizamos la variable `quit` para que el `run_exit` simplemente la cambiara a `true` y luego se cortaba el bucle del `main` de `mybash.c`.

### `builtin_is_internal` y `builtin_alone`
El trabajo de estas dos funciones es verificar si el comando pasado es interno o no. Para implementar `bultin_is_internal` simplemente comparamos el primer elemento del `scommand` que se les pasa y veíamos si era tanto `"cd"`, `"help"` o `"exit"`. Para `bultin_alone` se usa la función anterior y además se chequea si el largo del `pipeline` es 1. 

## Execute
El `execute` es el módulo encargado de la ejecución de los comandos mediante el uso de syscalls. En un principio se provee la función `execute pipeline` la cual se encarga de ejecutar el pipeline distinguiendo si se ejecuta en foreground o background, si es simple o múltiple y si tiene redirección de entrada o sálida. Para encargarnos de esto modularizamos la función de la siguiente manera:  

* Primero se detecta si el pipeline se debe ejecutar en background o foreground con el uso de `execute_background` y `execute_foreground`.  
* Luego si es comando simple o pipeline utilizando `single_command_execution` y `multiple_command_execution`. Para tomar los argumentos del comando creamos la función `tomar_args` que pasa los argumentos del scommand a un array de strings.  
* Y por último si tiene redireccionamientos de entrada o salida usando `change_in` y `change_out`.   


Lamentablemente, solo pudimos implementar hasta un pipeline de dos comandos, como por ejemplo `ls -l | wc`  

## myBash
Por último, el módulo `mybash.c` se encarga de ir parseando y ejecutando los pipilines mientras el usuario los va tipeando. Esta funcionalidad se encuentra en un bucle en donde sucede lo siguiente:  
* Se crea un nuevo `parser` donde se registra el input del usuario
* Se muestra un `prompt` con datos de usuario, hostname y path del directorio actual
* Se parsea el pipeline ingresado y se detecta si es nulo
    * Si es nulo, no se ejecuta y se espera al siguiente
    * Si no es nulo, se ejecuta y pipe se destruye instantaneamente
* Si en algún momento se ejecuta el comando `"exit"` cambiara la variable que mantiene el loop de parseo activo y se destruira el parser que registra los comandos para luego cerrar el programa
  
### Prompt
En `mybash.c` también incluimos parte del código que se encarga de mostrar el prompt en la terminal antes de ejecutar cada comando. Para ello se definieron ciertos colores y se obtuvieron el path al directorio actual, el nombre del usuario y el nombre del host. 


# Lista de problemas y tests fallidos
## Bugs notables
* Se puede solucionar el error de la sintaxis inválida pero nuestro método hace que el enter solo (comando vacio) también salte en ese error (priorizamos estética en este caso)
* Al hacer un pipe el prompt se muestra mal (solo por esa ejecución)


## Tests
* [Exit normal no compila en ciertas maquinas pero exit corregido compila pero agrega un fallo de test](#Solución-de-error-de-compatibilidad)
* Hay dos tests fallidos que trata de pipes pero creemos que estan "cruzados" (se espera que el hijo ejecute el primer comando mientas que el padre ejecute el segundo pero el nuestro esta al reves)
* Hay un error de test relacionado al background y los hijos. Con cambiar un wait se soluciona, pero ese wait si se saca se bugea el prompt por el resto de la ejecución. (wait de linea 237 de execute.c)
* Un error del chdir que no terminamos de entender.
* Otro error del pipe que aún menos idea


# Solución de error de compatibilidad
En nuestro grupo, la mitad de los integrantes tuvo problemas de compilación por el declarado de `quit` como una variable global. El error es del estilo `multiple definition of variable`.

Este problema tiene una solución, pero agrega un test fallido. 

## Versión que anda a la mitad
Esta es la versión que trae problemas de compilado a la mitad del grupo pero no agrega errores de tests ni de memory leaks

En `bultin.h` hay que definir la variable global `quit` de la siguiente manera
```c
8 | bool quit;
```


Luego en `builtin.c` hay que definir `run_exit` como
```c
58 | static void run_exit(scommand cmd)  {
59 |     printf ("Chau chau!!! nos vemoooooos!!! c:\n");
60 |     //exit(2)
61 |     quit= true;
62 | }
```

Por último en `mybash.c` hay que definir `quit = false` dentro del main

```c 
30 | int main(int argc, char *argv[]) {
31 |    pipeline pipe;
32 |    Parser input; 
33 |    quit = false;    //solo cambiar esta linea
```

De esta manera el programa debería compilar en ciertas maquinas y no tendrá errores de test o memory leaks.


## Versión que anda a todos pero con un test fallido
Esta versión compila a todos pero agrega un test fallido y memory leaks.

En `builtin.c` se debe definir `run_exit` como
```c
58 | static void run_exit(scommand cmd)  {
59 |     printf ("Chau chau!!! nos vemoooooos!!! c:\n");
60 |     exit(2);
61 |    //quit = true;
62 | }
```
comentando la asignación de `quit = true;`

Luego hay que comentar la aparicion de `quit` como variable global
en `builtin.h` comentando
```c 
8 | //bool quit;
```

Por último en `mybash.c` hay que cambiar `quit = false` por `bool quit = false`
```c 
30 | int main(int argc, char *argv[]) {
31 |    pipeline pipe;
32 |    Parser input; 
33 |    bool quit = false;    //solo cambiar esta linea
```
Ahora debería compilar en todas las maquinas pero agrega un test fallido relacionado  a un `early exit`, además genera memory leaks pues no se llegan a destruir ciertos objetos como el pipe o el parser. 

