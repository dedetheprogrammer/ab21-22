#!/bin/bash
# Author: Jorge Fenero Laborda (777570)
echo "Inicializando compresion de Texto3.txt"
./huf -c Texto3.txt
echo "creado fichero comprimido Texto3.huf"
echo "Inicializando descompresion de Texto3.huf"
./huf -d Texto3.huf
echo "creado fichero descomprimido Texto3_orig.txt"