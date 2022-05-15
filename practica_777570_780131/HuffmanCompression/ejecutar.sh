#!/bin/bash
# Author: Jorge Fenero Laborda (777570)
echo "Compilando los archivos"
g++ -std=c++11 main.cpp -o huf
echo "Ya se han compilado los archivos"
echo "Inicializando prueba 1 con fichero de texto Texto1.txt"
./prueba1.sh
echo "acabada prueba 1"
echo "Inicializando prueba 2 con fichero de texto Texto2.txt"
./prueba2.sh
echo "acabada prueba 2"
echo "Inicializando prueba 3 con fichero de texto Texto3.txt"
./prueba3.sh
echo "acabada prueba 3"
echo "FIN DEL SCRIPT"