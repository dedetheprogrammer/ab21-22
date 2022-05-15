Dokash -, Devid 780131@unizar.es    a780131
Fenero Laborda, Jorge   777570@unizar.es    a777570

Compresor basado en el algoritmo de Huffman:
- Descripcion: el primer programa es un compresor de archivos basado en el 
algoritmo de Huffman. El compresor sacará de forma dinamica leyendo el fichero 
la tabla de frecuencias de las letras del fichero, el montículo con las 
frecuencias y los codigos asociados a cada letra.
 
- Utilizacion:
    huf [opciones]
    Opciones: 
        -c <file>       Comprime el archivo dado.
        -d <file>.huf   Descomprime el archivo huffman. 
        help            Muestra ayuda.

- Restricciones:
    - Solo funciona con ficheros.
    - A la hora de descomprimir, debe ser el fichero .huf, si no, no funcionará.

Almacen y gestor de versiones:
- Descripcion: el segundo programa es un almacen y gestor de versiones que se
encarga del seguimiento de ficheros y gestiona sus diferentes versiones.

- Utilizacion
    version [opciones]
    Opciones:
        follow <file>   Guarda el fichero y tiene constancia del mismo.
        help            Muestra ayuda.
        log [<file>]    Muestra todos los ficheros seguidos o informacion sobre 
                        uno especifico.
        remove <file>   Elimina toda constancia del fichero del gestor.
        restore <file> --version <n>
                        Recupera la version n del fichero.
        see <file>      Muestra el contenido del fichero que hay guardado en el 
                        gestor.
        update <file>   Actualiza el fichero si hay cambios.

- Restricciones:
    - El comportamiento del programa con ficheros de texto con carácteres no 
    ASCII (acentos, letras de diferentes alfabetos, etc) es impredecible.