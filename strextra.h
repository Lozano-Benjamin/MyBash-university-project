#ifndef _STREXTRA_H_
#define _STREXTRA_H_


char * strmerge(char *s1, char *s2);
/*
 * Concatena las cadenas en s1 y s2 devolviendo nueva memoria (debe ser
 * liberada por el llamador con free())
 *
 * USAGE:
 *
 * merge = strmerge(s1, s2);
 *
 * REQUIRES:
 *     s1 != NULL &&  s2 != NULL
 *
 * ENSURES:
 *     merge != NULL && strlen(merge) == strlen(s1) + strlen(s2)
 *
 */
char * strmergefree(char *s1, char *s2);
/*  Concatena las cadenas s1 y s2 usando strmerge
    pero en este caso la diferencia es que ya libera memoria,
    es decir que no es necesario usar un free() con cada uso. */

#endif
