#!/bin/bash
echo "Se va a restaurar la version $1 del fichero Texto1.txt"
./version restore Texto1.txt --version $1
echo "El fichero Texto1.txt esta ahora en la version $1"