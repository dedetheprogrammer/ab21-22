Dokash -, Devid 780131@unizar.es    a780131
Fenero Laborda, Jorge   777570@unizar.es    a777570

Compresor basado en el algoritmo de Huffman
El primer programa es un compresor de archivos basado en el algoritmo de Huffman.

Para la tabla de frecuencias se leerá el fichero pasado por parametro caracter 
a caracter, cada caracter nuevo se guardará en un nuevo nodo de monticulo de 
Huffman, de esta manera el programa ahorrará cierta computacion, cada aparicion 
de un caracter aumentará en uno su frecuencia y ademas, se guardará el numero de 
bytes reales leidos. En un punto posterior se explicará de manera mas detallada
las decisiones de los bytes leidos y los nodos de Huffman. La tabla de 
frecuencias se guarda en un vector ordenado por frecuencias, se probaron otras 
decisiones como una tabla hash o un diccionario, pero el ordenado del vector
era mucho mas flexible y aceptaba estructuras propias con funciones lambda.

Se desarrolla una nueva estructura de solucion ad-hoc basada en este problema:
un nodo de monticulo de Huffman (huffman_heap), decidido así debido a la 
flexibilidad que conlleva su utilizacion respecto a estructuras ya existentes
que facilita su manipulacion.

Una vez se calcula la tabla de frecuencias, se decide crear el monticulo 
completo. El vector, ordenado de mayor a menor, contiene los diferentes nodos
del monticulo, fue tomada esta decision ya que al ordenar el vector de mayor a 
menor, los dos ultimos elementos siempre serían las hojas del monticulo que
habría que coger y al ser guardados directamente como nodos no era necesario
crear objetos adicionales ya que estaban preparados para ser referenciados por 
un nodo padre posterior. El nuevo nodo conformado por los dos ultimos elementos
del vector se introduce de manera ordenada para que siga cumpliendo el 
requisito.

Tras relacionar todos los nodos guardados en el vector, en este siempre quedará 
un unico nodo, siendo la raiz del monticulo, con el cual se calcularán los 
codigos de cada caracter. Existen dos tipos de nodos, uno rama, que no contiene 
caracter y otro hoja, que si lo contiene, cada codigo se calculará para cada 
nodo hoja y se guardará en una mapa no ordenado para que el acceso del mismo sea
casi constante, ya que no se requiere de que los codigos esten ordenados.

Finalmente, en la compresion, por un parte, se guardará la extension del fichero 
original y el tamaño de los bytes leidos seguido de la tabla de codificaciones,
y por otra, se volverá a leer el fichero pasado por parametro caracter a 
caracter pero esta vez se transformará cada caracter en su codigo guardado 
en el mapa no ordenado, con manipulacion de cadenas, se hace encajar los 
diferentes codigos para que queden escritos en un solo byte, es decir, en un
byte, puede haber varias letras ya que los codigos son la representacion en 
bits. La compresion generará un fichero de extension .huf, perderlo significará
perder el fichero original.

El funcionamiento de la decompresion es el contrario al de la compresion, 
primero se obtendrá la extension, guardada para poder generar el fichero
original junto a su extension, y el numero de bytes leidos, guardado ya que como
se ha mencionado anteriormente, cada letra es representada por una cadena de 
bits y es posible que esta codificacion deje bits de relleno al final del fichero
que si se decodificará directamente saldrían caracteres adicionales que no se 
encontraban en el fichero original, por ello, con el numero de bytes reales, se
puede verificar hasta que punto son caracteres reales del fichero original o son
bits de relleno. Posteriormente, se obtiene la tabla de codificaciones y 
finalmente se lee el fichero caracter a caracter aplicando la decodificacion. 
 
- Utilizacion:
    huf [opciones]
    Opciones: 
        -c <file>       Comprime el archivo dado.
        -d <file>.huf   Descomprime el archivo huffman. 
        help            Muestra ayuda.

- Restricciones:
    - Solo funciona con ficheros.
    - A la hora de descomprimir, debe ser el fichero .huf, si no, no funcionará.

Almacen y gestor de versiones
El segundo programa es un almacen y gestor de versiones que se encarga del 
seguimiento de ficheros y gestiona sus diferentes versiones.



- Utilizacion
    version [opciones]
    Opciones:
        add <file>      Añade el fichero al registro.
        erase           Elimina la carpeta de version.
        help            Muestra ayuda.
        init            Inicializa version y crea la carpeta de registro.
        log [<file>]    Muestra los ficheros registrados o informacion sobre un 
                        fichero en especifico.
        remove <file>   Quita el fichero del registro.
        restore <file> --version <n>
                        Restaura la version n del fichero, sea posterior o 
                        anterior.
        see <file>      Muestra el contenido del fichero que tiene guardado version.
        update <file>   Actualiza el contenido del fichero. Si la version actual
                        es inferior a la ultima, se perderan todas las versiones
                        posteriores a la actual.

- Restricciones:
    - El comportamiento del programa con ficheros de texto con carácteres no 
    ASCII (acentos, letras de diferentes alfabetos, etc) es impredecible.