CONSIDERACIONES SOBRE EL CODIGO:

El codigo permite obtener la version anterior de un fichero.

Se considera un estandar en el nombrado de los ficheros siendo nombre_vx.txt
donde nombre es el nombre del fichero y x es la version.

Para obtener los cambios se sigue el siguiente nombrado:
las lineas empiezan por # e indican que cada linea contiene los cambios de pasar
de una version a otra
La estructura de Inserccion es InumlineaLetransertadaPosicion
La estructura de Borrado es BnumlineaLetraborradaPosicion
La estructura de Sustitucion es SnumlineaLetraantiguaLetranuevaPosicion
La estructura de No hacer nada es Nnumlinea

El codigo no borra los ficheros de las versiones porque no deja usar la funcion remove
por lo que al pasar por ejemplo de la version 3 a la 1 apareceran el fichero
de la version 2 y 1.

El codigo realiza los cambios desde el final al principio por lo que hay que tener en
cuenta que al ir a una version anterior los cambios son los inversos a los que aparecen.
