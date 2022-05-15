#!/bin/bash
# Author: Jorge Fenero Laborda (777570)
echo "Has hecho cambios en el fichero $1 y se van a actualizar para crear una nueva version"
./version update $1
echo "Se han actualizado los cambios y se ha creado una nueva version"