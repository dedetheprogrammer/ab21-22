#!/bin/bash
g++ -std=c++11 main.cpp -o version
echo "Initializing version storage. . ."
./version init
echo "Se va a comenzar el seguimiento de versiones del fichero Texto1.txt"
./version add Texto1.txt
echo "Ahora cada vez que modifices el fichero tendras que ejecutar el script update.sh"
echo "Si quieres recuperar una version tienes que ejecutar el script restore.sh pasandole como argumento la version que quieras"
echo "Para el resto de opciones, ejecutar uno de los siguientes comandos:"
./version