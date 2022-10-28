## Informe 
- Está bien pero podría ser más sintético. Describan la funcionalidad más a grandes rasgos. Hay detalles que ya se saben por el enunciado o que no son demasiado relevantes. Sobre todo en la parte de modularización.
- Bien que en el informe aclaran lo de la variable global y como solucionarlo.
- Para ortografía recomiendo pasar `aspell`

## Repo 
- Parece todo bien
- Pocos commits de Fabrizio

## Código
- `define` para caracteres especiales (`"|"`, `" "` `">"`, etc.)
- Deberían tener consistencia idiomática
- La modularización en execute mezcla unidades de sentido. 
- La diferencia entre hacer `wait` o no en el padre es muy pequeña, se resuelve con un if en el padre y nada más. No divide en dos casos tan separados como hicieron ustedes.
- Para el cambio de color se podrían guardar las paletas en un arreglo en vez de hardcodearlas en la función.
- Mucho de lo que es comentario en el código deberían ser aclaraciones en el informe. Igual prefiero que el código esté comentado de más a que esté comentado de menos
- `execute_pipeline` es muy larga y se podría modularizar. Hay código repetido. Una opción es hacer una función que ejecute un comando simple.

## Funcionalidad
- Testeo 97%
- Forkean de más. Este error quizás tiene que ver con la calidad del código? Es difícil seguir la ejecución en un código tan enredado.
- El forkeo extra probablemente haga que waiteen de más.
- Con comandos mal formados `ls |`, `ls >` se debería printear un mensaje
- Punto estrella prompt y builtin extra (BOKE)
