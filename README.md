# ALGORITMIA BASICA 2021-22
## Practicas
- [Practica 1](https://github.com/ddevigner/ab21-22/tree/main/HuffmanCompression): compresor mediante el algoritmo de Huffman.
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

- Ficheros:
    - [HuffmanCompression.cpp](https://github.com/ddevigner/ab21-22/blob/main/HuffmanCompression/HuffmanCompression.cpp)
    - [HuffmanHeap.h](https://github.com/ddevigner/ab21-22/blob/main/HuffmanCompression/HuffmanHeap.h)
    - main.cpp: programa del compresor.
    - huffman_compressor.hpp: contiene las funciones de calculo de tabla de 
    frecuencias, monticulo, tabla de codificacion, compresion y decompresion.
    - huffman_heap.hpp: estructura propia para el monticulo de Huffman.
    - huffman_exceptions.hpp: excepciones que cubren situaciones que el usuario
    no deberia reproducir.
    - ejecutar.sh: script de compilacion y pruebas.
    - prueba[1-3].sh: scripts de pruebas individuales.
    - Texto[1-3].txt: ficheros de prueba.

    - Archivos:

    - Compilacion:
        ```bash
            g++ -std=c++11 HuffmanCompression.cpp -o huffman
        ```
    - Uso:
        - Compresion:
        ```bash
            ./huffman -c <file>
        ```
        - Decompresion:
        ```bash
            ./huffman -d <file>.huf
        ```
- [Practica 2](https://github.com/ddevigner/ab21-22/tree/main/VersionStorage): almacen gestor de versiones de ficheros mediante programacion dinamica.  
    - Archivos:
        - [main.cpp](https://github.com/ddevigner/ab21-22/blob/main/Version/main.cpp)
        - [Version.hpp](https://github.com/ddevigner/ab21-22/blob/main/Version/Version.hpp)
        - [VersionExceptions.hpp](https://github.com/ddevigner/ab21-22/blob/main/Version/VersionExceptions.hpp)
        - [SeqComparator.hpp](https://github.com/ddevigner/ab21-22/blob/main/Version/SeqComparator.hpp)
    - Compilacion:
        ```bash
            g++ -std=c++11 main.cpp -o version
        ```
    - Uso:
        - Seguir un nuevo fichero: 
        ```bash
            ./version follow <file>
        ```
