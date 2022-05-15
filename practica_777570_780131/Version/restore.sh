#!/bin/bash
# Author: Jorge Fenero Laborda (777570)
echo "Se va a restaurar la version $1 del fichero $2"
./version restore $2 --version $1
echo "El fichero $2 esta ahora en la version $1"