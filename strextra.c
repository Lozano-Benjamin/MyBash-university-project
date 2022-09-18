#include <stdlib.h>    /* calloc()...                        */
#include <string.h>    /* strlen(), strncat, strcopy()...    */
#include <assert.h>    /* assert()...                        */
#include "strextra.h"  /* Interfaz                           */

char * strmerge(char *s1, char *s2) {
    char *merge=NULL;       /* Inicializacion del puntero de merge */
    size_t len_s1=strlen(s1);   /* Calculamos el largo de s1 */
    size_t len_s2=strlen(s2);   /* Calculamos el largo de s2 */
    assert(s1 != NULL && s2 != NULL);
    merge = calloc(len_s1 + len_s2 + 1, sizeof(char));  /* Inicializamos la nueva memoria con reserva del tam de s1 + s2 */
    strncpy(merge, s1, len_s1);         /* Copiamos s1 en merge, le pasamos su length  */
    merge = strncat(merge, s2, len_s2); /* Concatenamos s1 con s2 */
    assert(merge != NULL && strlen(merge) == strlen(s1) + strlen(s2));
    return merge;
}

char * strmergefree(char *s1, char *s2){
    assert(s1 != NULL && s2 != NULL);
    char *str1 = strmerge(s1, s2);  /* Utilizamos strmerge */
    free(s1);                   /* Liberamos la memoria que queda colgada. */
    return str1;
}