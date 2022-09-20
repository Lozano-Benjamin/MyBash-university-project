# Informe Lab 01 - Grupo 18
- Ayala Facundo (facundo.ayala@mi.unc.edu.ar)
- Bonfils Gastón Tomás (gastonbonfils@mi.unc.edu.ar)
- Longhi Heredia Fabrizio Mateo (fabrizio.longhi@mi.unc.ecu.ar)
- Lozano Benjamín (benjamin.lozano@mi.unc.edu.ar)

# Contenido

* [Introducción](#Introducción)
* [Guía de uso](#Guía-de-uso)
* [Metodología de trabajo](#Metodología-de-trabajo)
* [Módulos, implementaciones y técnicas](#Módulos-implementaciones-y-técnicas)
    * [Command](#Command)
    * [Parser y parsing](#Parser-y-Parsing)
    * [Builtin](#Builtin)
    * [Execute](#Execute)
    * [Strextra](#Strextra)
    * [myBash](#myBash)
* [Herramientas de programación](#Herramientas-de-programación)
* [Desarrollo](#Desarrollo)
    * [Solución de error de compatibilidad](#Solución-de-error-de-compatibilidad)


# Introducción
El laboratorio consistió en codificar una shell al estilo de bash (*Born Again Shell*) a la que denominamos *mybash*, utilizando el lenguaje C, a realizar en un periodo de aproximadamente 4 semanas.
 
# Guía de uso
Dirijase hacia el directorio donde tenga almacenado el proyecto. Luego desde la terminal ejecute el comando `make`. De esta manera  se compilara el proyecto. Se crearan archivos compilados y un ejecutable con el nombre `mybash`. Desde la terminal ejecute `./mybash` y podra hacer uso de nuestra bash. Para salir del programa use el comando `exit`.

# Metodología de trabajo
Para la realizacion del trabajo tomamos de base la metodología de cascada (*waterfall methodology*) ya que consideramos, a diferencia de las metodologias ágiles, que se adaptaba mejor a nuestras necesidades ya que esta metodologia sigue una estructura lineal acorde a lo que se nos aconsejo que sigamos. Esto es, principalmente a que no avanzabamos a otro modulo del proyecto hasta haber finalizado el anterior, tomando como guia la realizacion de los modulos más simples a los más complejos. 

Con respecto a la division de tareas y a la organizacion realizamos encuentros virtuales diarios, usando la plataforma *discord* en donde cada integrante comentaba en que pudo avanzar y si se encontraba con alguna dificultad. Por otro lado, para consultas mas casuales utilizamos la plataforma de mensajeria *telegram*

A la hora de realizar el primer módulo nos dividimos las funciones a completar ya que se trataban de implentaciones sencillas pero numerosas, lo hicimos así con el fin de que al momento de finalizar un compañero verifique lo realizado por el otro para así minimizar la cantidad de errores. 
Con respecto al segundo y tercer modulo lo realizamos dividiendonos en equipos de a dos para minimizar el tiempo ya que se trataban de modulos simples y cortos. Por último, para realizar el módulo `execute` repetimos el método usado para el primer módulo dividiendonos las funciones entre los integrantes.

Asímismo para el control de versionado utilizamos la plataforma *bitbucket*



# Módulos, implementaciones y técnicas

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
La tarea de estos modulos, como sus nombres lo indican, es la de recorrer el `stdin ` de manera lineal e ir tomando los comandos, argumentos, redirectores, pipes y operador de segundo plano para luego armar una instacia del tipo `pipeline` con la interpretacion de los datos de entrada. Cabe destacar que estos modulos no se encargan de verificar si los comandos simples o multiples son validos (dicha tarea esta asignada al modulo execute mediante las system calls) sino mas bien de que los comandos, argumentos, redirectores, pipes y operador de segundo plano respeten su estructura sintactica.


Estos modulos estan compuestos por 3 encabezados:
* `parser.h`
* `parsing.h`
* `parsing.c`

### `Parser.h`
En este encabezado se encuentra la interfaz del modulo `parser` la cual ya se encuentra implementada por la catedra y cuya implementacion se encuentra en los binarios `parser.o` y `lexer.o`.

### `Parsing.h`
En este encabezado se encuentra la interfaz del modulo `parsing` cuya implementacion se encuentra en el encabezado `parsing.c`.

### `Parsing.c`
En este encabezado se encuentran implementadas las funciones `parse_pipeline` y `scommand parse_scommand`.


La funcion `parse_scommand` es de caracter estatico y se encarga de armar una instancia del tipo `scommand` con sus respectivos comandos, argumentos y redirectores. En caso de que despues de un redirector no haya nada esto devuelve NULL ya que no estaria respetando la sintaxis correcta.

Por otro lado, la funcion `parse_pipeline` se encarga de crear una instancia del tipo pipeline, para esto, hace una llamada a la funcion `parse_scommand` para luego insertar dichos comandos a la pipeline. A su vez nos indica si se encontro un operador de background.


## Builtin
El módulo builtin se encarga de ejecutar los comandos internos que se quieran implementar. Nosotros implementamos los comandos principales los cuales son:
* `cd` para cambiar de directorio en el que trabaja
* `help` para mostrar información en pantalla sobre el MyBash
* `exit` para cerrar el programa  

Además decidimos implementar un comando para cambiar el color del `prompt` y de la terminal.
* `color` que dado un número se cambia los colores por otros de paletas hechas por nosotros  

A la hora de implementar estos comandos se modularizo la función `bultin_run` de manera tal que al detectar el comando que se pasa directamente se ejecuta una función `run_XX` donde XX es cd, help o exit.

### `cd`
La función `run_cd` se implemento utilizando la función `syscall` importada de la librería `<sys/syscall.h>`. Esta función toma una syscall, sus parametros y la ejecuta. La syscall `chdir` maneja por si sola la distincción entre path relativo y path absoluto. Nosotros agregamos un caso por si no se le pasa un path (sea `cd` y nada mas) para volver al directorio `home` del usuario.

### `help`
`run_help` no tiene mucho misterio, es simplemente un `printf` con un mensajito de ayuda.

### `exit`
Al principio no entendiamos la manera de implementar `run_exit` pero luego trabajando en `mybash.c` vimos la variable `quit` que manejaba el loop de parseo de pipelines. En `builtin.c` globalizamos la variable `quit` para que el `run_exit` simplemente la cambiara a `true` y luego se cortaba el bucle del `main` de `mybash.c`.

### `color`
Para poder cambiar el color del prompt primero cambiamos de lugar la función `show_prompt` del módulo [mybash](#myBash) trayendola al `builtin.c` de manera que se nos haga mas fácil trabajar.   
Para usar está función hay que llamarla como `color` seguida de un número del 0 al 5 el cual va a referenciar la paleta de colores que se usa. Para ver las posibles opciones de paletas con llamar `color` sin ningún parámetro se muestra la lista.

### `builtin_is_internal` y `builtin_alone`
El trabajo de estas dos funciones es verificar si el comando pasado es interno o no. Para implementar `bultin_is_internal` simplemente comparamos el primer elemento del `scommand` que se les pasa y veíamos si era igual cualquier comando interno. Para `bultin_alone` se usa la función anterior y además se chequea si el largo del `pipeline` es 1. 

## Execute
El `execute` es el módulo encargado de la ejecución de los comandos mediante el uso de syscalls. En un principio se provee la función `execute pipeline` la cual se encarga de ejecutar el pipeline distinguiendo si se ejecuta en foreground o background, si es simple o múltiple y si tiene redirección de entrada o sálida. Para encargarnos de esto modularizamos la función de la siguiente manera:  

* Primero se detecta si el pipeline se debe ejecutar en background o foreground con el uso de `execute_background` y `execute_foreground`.  
* Luego si es comando simple o pipeline utilizando `single_command_execution` y `multiple_command_execution`. Para tomar los argumentos del comando creamos la función `tomar_args` que pasa los argumentos del scommand a un array de strings.  
* Y por último si tiene redireccionamientos de entrada o salida usando `change_in` y `change_out`.   

Modularizamos el `execute` de esta manera para permitirnos poder trabajar sobre el módulo en momentos diferentes, pues nos dividimos las funciones entre los cuatro. 

Lamentablemente, solo pudimos implementar hasta un pipeline de dos comandos, como por ejemplo `ls -l | wc`  



## Strextra
En el módulo `strextra` se encuentran  dos funciones auxiliares. 

La primera de ellas es `strmerge` que ya viene implementada por la cátedra la cuál nos permite concatenar dos strings y nos devuelve el resultado en un string nuevo. 

La segunda de ellas es `strmergefree` la cuál implementamos nosotros. Ésta función hace un llamado a `strmerge` y libera la posición de memoria en dónde se encontraba el string original al que le concatenamos el segundo.

## myBash
Por último, el módulo `mybash.c` se encarga de ir parseando y ejecutando los pipilines mientras el usuario los va tipeando. Esta funcionalidad se encuentra en un bucle en donde sucede lo siguiente:  
* Se crea un nuevo `parser` donde se registra el input del usuario
* Se muestra un `prompt` con datos de usuario, hostname y path del directorio actual
* Se parsea el pipeline ingresado y se detecta si es nulo
    * Si es nulo, no se ejecuta y se espera al siguiente
    * Si no es nulo, se ejecuta y pipe se destruye instantaneamente
* Si en algún momento se ejecuta el comando `"exit"` cambiara la variable que mantiene el loop de parseo activo y se destruira el parser que registra los comandos para luego cerrar el programa
  
# Herramientas de programación
Utilizamos como editor de codigo a `Visual Estudio Code`, para el versionado de codigo utilizamos a `Git`, tambien para facilitar algunas veces utilizabamos el `source control` incluido en el editor previamente mencionado. Para compilar utilizabamos un archivo `makefile` incluido en el `kickstarter`. Para debuggear creamos archivos auxiliares que utilizaban todos las funciones del modulo a testear, compilabamos con el comando `gcc -Wall -Werror -Wextra -pedantic -std=gnu11 -c command.c $(pkg-config --cflags --libs glib-2.0) -lglib-2.0 && gcc -Wall -Werror -Wextra -pedantic -std=gnu11 -o self_command *.o main.c -lglib-2.0` donde el primer compilado es para las dependencias y el segundo para el archivo de test. Luego corriamos `gdb` para revisar los bugs. Luego otro tipo de analisis de errores se basaba en debuggear las funciones hasta que que pasen los test existentes en el archivo `makefile`.

# Desarrollo
En un principio había un problema de compilado (relacionado a la definición global de una variable) para algunos miembros del grupo. Este error es debido a una diferencia en la versión del compilador de los integrantes del grupo. Sin embargo, logramos solucionar este problema agregando la flag `-fcommon` al comando de compilado en el Makefile. En caso de que el error persista consultar la [solución](#Solución-de-error-de-compatibilidad).


## Solución de error de compatibilidad
Primero que todo verificar que se agregó la flag `-fcommon` de compilacion en el archivo *Makefile*. Si el error persiste entonces recurrir a lo siguiente:

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
Ahora debería compilar bien pero agrega un test fallido relacionado  a un `early exit`, además genera memory leaks pues no se llegan a destruir ciertos objetos como el pipe o el parser. 

