#!/bin/bash
# Author: Jorge Fenero Laborda (777570)
echo "Inicializando compresion de Texto1.txt"
./huf -c Texto1.txt
echo "creado fichero comprimido Texto1.huf"
echo "Inicializando descompresion de Texto1.huf"
./huf -d Texto1.huf
echo "creado fichero descomprimido Texto1_orig.txt"