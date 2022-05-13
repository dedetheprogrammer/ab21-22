# ALGORITMIA BASICA 2021-22
## Practicas
- [Practica 1](https://github.com/ddevigner/ab21-22/tree/main/HuffmanCompression): compresor mediante el algoritmo de Huffman.
    - Archivos:
        - [HuffmanCompression.cpp](https://github.com/ddevigner/ab21-22/blob/main/HuffmanCompression/HuffmanCompression.cpp)
        - [HuffmanHeap.h](https://github.com/ddevigner/ab21-22/blob/main/HuffmanCompression/HuffmanHeap.h)
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
        - [Version.hpp](https://github.com/ddevigner/ab21-22/blob/main/VersionStorage/VersionStorage.cpp)
        - [SeqComparator.hpp](https://github.com/ddevigner/ab21-22/blob/main/VersionStorage/VersionStorage.cpp)
        - [main.cpp](https://github.com/ddevigner/ab21-22/blob/main/VersionStorage/VersionStorage.cpp)
        - [VersionExceptions.hpp](https://github.com/ddevigner/ab21-22/blob/main/VersionStorage/VersionStorage.cpp)
    - Compilacion:
        ```bash
            g++ -std=c++11 main.cpp -o version
        ```
    - Uso:
        - Seguir un nuevo fichero: 
        ```bash
            ./version follow <file>
        ```
