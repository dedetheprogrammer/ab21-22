#!/bin/bash
g++ -std=c++11 main.cpp -o version
echo "Initializing version storage. . ."
./version init
echo "Se va a comenzar el seguimiento de versiones del fichero Texto1.txt y del fichero Texto2.txt"
./version add Texto1.txt
./version add Texto2.txt
echo "Ahora cada vez que modifices el fichero tendras que ejecutar el script update.sh pasandole como argumento el nombre del fichero que se quiera actualizar"
echo "Si quieres recuperar una version tienes que ejecutar el script restore.sh pasandole como argumento la version que quieras y el nombre del fichero que quieras"
echo "Para el resto de opciones, ejecutar uno de los siguientes comandos:"
./version