#!/bin/bash
echo "Inicializando compresion de Texto2.txt"
./huf -c Texto2.txt
echo "creado fichero comprimido Texto2.huf"
echo "Inicializando descompresion de Texto2.huf"
./huf -d Texto2.huf
echo "creado fichero descomprimido Texto2_orig.txt"