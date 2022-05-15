# ALGORITMIA BASICA 2021-22
## [Practica 1](https://github.com/ddevigner/ab21-22/tree/main/HuffmanCompression): Compresor de archivos mediante el algoritmo de Huffman
- Ficheros:
    - [main.cpp](https://github.com/ddevigner/ab21-22/blob/main/HuffmanCompression/main.cpp): contains the main programm, its features.
    - [huffman_compressor.hpp](https://github.com/ddevigner/ab21-22/blob/main/HuffmanCompression/huffman_compressor.hpp): contains the implementation of the different functions that implement the Huffman Algorithm (as calculate the frequences table, the Huffman heap, etc) and the features offered by the compressor (compress, decompress).
    - [huffman_heap.hpp](https://github.com/ddevigner/ab21-22/blob/main/HuffmanCompression/huffman_heap.hpp): implements a custom heap for the purpose of applying the Huffman Algorithm.
    - [huffman_exceptions.hpp](https://github.com/ddevigner/ab21-22/blob/main/HuffmanCompression/huffman_exceptions.hpp): exceptions for situations where the user is doing something that he shouldn't...

- Compilacion:
    ```bash
        g++ -std=c++11 main.cpp -o huf
    ```

- Utilizacion:
    ```bash
    ./huf [opciones]
    Opciones: 
        -c <file>       Comprime el archivo dado.
        -d <file>.huf   Descomprime el archivo huffman. 
        help            Muestra ayuda.
    ```

- Restricciones:
    - Solo funciona con ficheros.
    - A la hora de descomprimir, debe ser el fichero .huf, si no, no funcionará.


## [Practica 2](https://github.com/ddevigner/ab21-22/tree/main/VersionStorage): Almacen y gestor de versiones
- Archivos:
    - [main.cpp](https://github.com/ddevigner/ab21-22/blob/main/Version/main.cpp): contains the main program, its features.
    - [version_storage.hpp](https://github.com/ddevigner/ab21-22/blob/main/Version/version_storage.hpp): contains the implementation of the different functions that implement the different features offered by the version control storage.
    - [version_exceptions.hpp](https://github.com/ddevigner/ab21-22/blob/main/Version/version_exceptions.hpp): exceptions for situations where the user is doing something that he shouldn't...
    - [sequence_comparator.hpp](https://github.com/ddevigner/ab21-22/blob/main/Version/sequence_comparator.hpp): implements the sequence comparation algorithm.
    - [utils.hpp](https://github.com/ddevigner/ab21-22/blob/main/Version/utils.hpp): general purpose functions.

- Compilacion:
    ```bash
        g++ -std=c++11 main.cpp -o version
    ```

- Utilizacion
    ```bash
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
    ```

- Restricciones:
    - INICIALIZAR VERSION, SI NO NO FUNCIONA: ./version init
    - El comportamiento del programa con ficheros de texto con carácteres no ASCII (acentos, letras de diferentes alfabetos, etc) es impredecible.
    - El comportamiento con lineas mayores a 250 caracteres es un poco impredecible, no ha podido ser testeado del todo.
    - Usar los comandos como se disponen.


<!--Para la tabla de frecuencias se leerá el fichero pasado por parametro caracter a caracter, cada caracter nuevo se guardará en un nuevo nodo de monticulo de Huffman, de esta manera el programa ahorrará cierta computacion, cada aparicion de un caracter aumentará en uno su frecuencia y ademas, se guardará el numero de bytes reales leidos. En un punto posterior se explicará de manera mas detallada
las decisiones de los bytes leidos y los nodos de Huffman. La tabla de frecuencias se guarda en un vector ordenado por frecuencias, se probaron otras decisiones como una tabla hash o un diccionario, pero el ordenado del vector era mucho mas flexible y aceptaba estructuras propias con funciones lambda.

Se desarrolla una nueva estructura de solucion ad-hoc basada en este problema: un nodo de monticulo de Huffman (huffman_heap), decidido así debido a la flexibilidad que conlleva su utilizacion respecto a estructuras ya existentes que facilita su manipulacion.

Una vez se calcula la tabla de frecuencias, se decide crear el monticulo completo. El vector, ordenado de mayor a menor, contiene los diferentes nodos del monticulo, fue tomada esta decision ya que al ordenar el vector de mayor a menor, los dos ultimos elementos siempre serían las hojas del monticulo que habría que coger y al ser guardados directamente como nodos no era necesario crear objetos adicionales ya que estaban preparados para ser referenciados por un nodo padre posterior. El nuevo nodo conformado por los dos ultimos elementos del vector se introduce de manera ordenada para que siga cumpliendo el requisito.

Tras relacionar todos los nodos guardados en el vector, en este siempre quedará un unico nodo, siendo la raiz del monticulo, con el cual se calcularán los codigos de cada caracter. Existen dos tipos de nodos, uno rama, que no contiene caracter y otro hoja, que si lo contiene, cada codigo se calculará para cada nodo hoja y se guardará en una mapa no ordenado para que el acceso del mismo sea casi constante, ya que no se requiere de que los codigos esten ordenados.

Finalmente, en la compresion, por un parte, se guardará la extension del fichero original y el tamaño de los bytes leidos seguido de la tabla de codificaciones, y por otra, se volverá a leer el fichero pasado por parametro caracter a caracter pero esta vez se transformará cada caracter en su codigo guardado en el mapa no ordenado, con manipulacion de cadenas, se hace encajar los diferentes codigos para que queden escritos en un solo byte, es decir, en un byte, puede haber varias letras ya que los codigos son la representacion en bits. La compresion generará un fichero de extension .huf, perderlo significará perder el fichero original.

El funcionamiento de la decompresion es el contrario al de la compresion, primero se obtendrá la extension, guardada para poder generar el fichero original junto a su extension, y el numero de bytes leidos, guardado ya que como se ha mencionado anteriormente, cada letra es representada por una cadena de bits y es posible que esta codificacion deje bits de relleno al final del fichero que si se decodificará directamente saldrían caracteres adicionales que no se encontraban en el fichero original, por ello, con el numero de bytes reales, se puede verificar hasta que punto son caracteres reales del fichero original o son bits de relleno. Posteriormente, se obtiene la tabla de codificaciones y finalmente se lee el fichero caracter a caracter aplicando la decodificacion.

El segundo programa es un almacen y gestor de versiones que se encarga del seguimiento de ficheros y gestiona sus diferentes versiones.

Antes de utilizar version con todas sus funciones es imporante inicializarlo PORQUE SI NO NO FUNCIONARÁ, Y SI EL USUARIO QUE UTILICE EL PROGRAMA NO SABE PORQUE NO FUNCIONA SIGNIFICA QUE NO SE HA LEIDO EL README:

```bash
./version init
```

Tambien es posible eliminar toda traza del programa, sea carpeta de registro o ficheros de version, etc:

```bash
./version erase
```

El seguimiento de los ficheros se realiza mediante un fichero de registro que guarda cada fichero seguido y todas sus versiones. Se guarda el momento de registro y de ultima modificacion, la version actual y la ultima version guardada, si esta actualizado y el nombre y path absoluto del fichero:

> REGISTER_DATE, REGISTER_TIME, UPDATED, CURRENT_VERSION, LAST_VERSION, MODIFY_TIME, MODIFY_DATE, FILE
> \# VERSION_TIME, VERSION_DATE, VERSION_NAME, VERSION_DESCRIPTION.
> [...]

Es importante tener el nombre y path absoluto del fichero, ya que es la unica manera de poder verificar si un fichero ya ha sido registrado o no. El path absoluto y el nombre tienen un id unico en la misma maquina. 

Una vez terminado, se copia el fichero en la carpeta de registro y se guarda la ultima version datada.

Para la actualizacion de los ficheros se ha decidido leerlo linea por linea ya que se consideraba un punto intermedio entre leerlo palabra a palabra o caracter a caracter o bloque a bloque, mas soportable para el sistema. Para poder aliviar la carga de computo, se ha limitado el tamaño de linea a 250, es decir, si una linea es superior a los 250 caracteres, esta se procesará por bloques de 250 
caracteres y se evitaran matrices de gran tamaño (62500 elementos solo una linea de 250 caracteres). Por cada linea leida, se utilizará el algoritmo de comparacion de secuencias y los cambios se guardaran en un fichero de version asociado a la version a la que se va actualizar, en otras palabras, cada version tiene su propio fichero de cambios, debido a que la lectura de los ficheros se hacia mas sencilla. En los ficheros de version se guardan en pares de dos lineas de cambios, la primera guarda los cambios actuales de la version guardada a la nueva version, la segunda guarda los cambios necesarios para llegar a la version anterior:

>>@ linea cambios actuales de la version.
<<@ linea cambios a la version anterior.

La comparacion de cadenas se realiza mediante el algoritmo de comparacion de secuencias aplicado a programacion dinamica, para el calculo de costes se utilizaunicamente un vector ya que es posible recorrer los costes sin necesidad de guardar una matriz completa, respecto de las acciones, estas si se guardan en una matriz ya que no es posible determinar que acciones son las optimas sin la matriz. Respecto al resultado, este se obtiene mediante una cadena y ademas se ha implementado un modo comprimido de los cambios para asi acotarlos y evitar caracteres innecesarios:

- Si hay varias inserciones o sustituciones contiguas, no es necesario metercada insercion o sustitucion por separado, con tal de introducir desde que indicedebe realizarse y la cadena ya se tiene el dato:
    +H6 +e7 +l8 +l9 +o10    -> +6:Hello

- Si hay varias eliminacions contiguas, solo se requiere los indices inicial y final de los que se va a hacer la eliminacion:
    -7 -8 -9 -10 -11 -12    -> -7:12

Ademas, los cambios tambien vienen implementados con una funcion de analisis, que se encarga de analizar la situacion de las cadenas del fichero actualizado y del fichero guardado, si por ejemplo, alguna de las dos cadenas esta vacia o son iguales se evita el calculo de costes y acciones ya que estas se pueden sacar directamente sin necesidad de calcular nada.

Una vez calculados los cambios, se modifica el registro con el ultimo momento de modificacion, su version actual y su ultima version y se añade la nueva version, que permite tener un nombre personalizado (solo caracteres alfanumericos y "_") y una pequeña descripcion (max. 150 caracteres). Si se actualiza el fichero y la version actual es menor a la ultima version disponible, se perderan todas las versiones posteriores a la actual.

La restauracion funciona al contrario que la actualizacion, permite la restauracion hacia detras (version actual 3 -> 1), y hacia delante (version actual 1 -> 3), para ello, lee cada fichero de versiones correspondiente y aplica uno tras otro los cambios dependiendo de si la version a alcanzar es anterior o posterior. El aplicado de los cambios es con un simple algoritmo que procesa cadenas. Una vez restaurada la version, se actualiza el momento de modificacion y la version actual.

Los ficheros y versiones se guardan en estrcuturas propias para la facilitacion del computo, en versiones anteriores se modificaba directamente el fichero de registro obteniendo un codigo bastante mas complejo.-->
