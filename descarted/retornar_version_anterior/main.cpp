#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <time.h>
#include <vector>
#include <string>
#include <iterator>
#include <cstdlib>
#include <cstdio>
#include <stdio.h>

using namespace std;

/* Funcion para sacar la version de un fichero*/

int sacarVersion(string nombreFichero){
    string numero = nombreFichero.substr(nombreFichero.find_last_of("_") + 2, 1);
    return stoi(numero);
}

/* Funcion que dado un fichero y unos cambios, los deshace para volver a la version anterior */

string deshacerCambios(string nombreFichero, string cambios){
    fstream f;
    f.open(nombreFichero);
    string linea;
    vector<string> misLineas;

    while(getline(f, linea)){
        misLineas.push_back(linea);
    }

    string space_delimiter = " ";
    vector<string> losCambios;
    size_t pos = 0;

    while ((pos = cambios.find(space_delimiter)) != string::npos) {
        losCambios.push_back(cambios.substr(0, pos));
        cambios.erase(0, pos + space_delimiter.length());
    }

    losCambios.push_back(cambios);

    string delimitador = "#";
    linea = losCambios.back();

    string numLinea;
    int posicion;
    char letra1;
    char letra2;
    string posEnLinea;
    string lineaAprocesar;

    while(linea != delimitador){
        losCambios.pop_back();
        numLinea = "";
        posEnLinea = "";
        posicion = 1;
        switch(linea[0]){
            case 'I':
                while(isalpha(linea[posicion]) == 0){
                    numLinea = numLinea + linea[posicion];
                    posicion++;
                }
                letra1 = linea[posicion];
                posicion++;
                while(linea[posicion]){
                    posEnLinea = posEnLinea + linea[posicion];
                    posicion++;
                }
                lineaAprocesar = misLineas[stoi(numLinea)-1];
                lineaAprocesar.erase(stoi(posEnLinea)-1, 1);
                misLineas[stoi(numLinea)-1] = lineaAprocesar;
                break;
            case 'B':
                while(isalpha(linea[posicion]) == 0){
                    numLinea = numLinea + linea[posicion];
                    posicion++;
                }
                letra1 = linea[posicion];
                posicion++;
                while(linea[posicion]){
                    posEnLinea = posEnLinea + linea[posicion];
                    posicion++;
                }
                lineaAprocesar = misLineas[stoi(numLinea)-1];
                lineaAprocesar.insert(stoi(posEnLinea)-1, 1, letra1);
                misLineas[stoi(numLinea)-1] = lineaAprocesar;
                break;
            case 'S':
                while(isalpha(linea[posicion]) == 0){
                    numLinea = numLinea + linea[posicion];
                    posicion++;
                }
                letra1 = linea[posicion];
                posicion++;
                letra2 = linea[posicion];
                posicion++;
                while(linea[posicion]){
                    posEnLinea = posEnLinea + linea[posicion];
                    posicion++;
                }
                lineaAprocesar = misLineas[stoi(numLinea)-1];
                lineaAprocesar.erase(stoi(posEnLinea)-1, 1);
                lineaAprocesar.insert(stoi(posEnLinea)-1, 1, letra1);
                misLineas[stoi(numLinea)-1] = lineaAprocesar;
                break;
            default:
                ;
        }
        linea = losCambios.back();
    }

    f.close();
    //int result = remove(nombreFichero);

    ofstream f2;
    int nuevaVersion = sacarVersion(nombreFichero);
    nuevaVersion--;
    string nuevoNombre = "nombre_v" + to_string(nuevaVersion) + ".txt";
    cout << nuevoNombre << endl;
    f2.open(nuevoNombre);

    for(int i = 0; i<misLineas.size(); i++){
        f2 << misLineas[i] << endl;
    }

    f2.close();

    return nuevoNombre;
}

/* Funcion que crea un fichero con una version anterior al actual */

void retornarAnterior(string nombreFichero, string ficheroCambios, int version, int version_actual){
    ifstream f2;
    f2.open(ficheroCambios);

    string linea;
    vector<string> misLineas;
    int lineasAsacar = 0;
    while(getline(f2, linea)){
        if ((lineasAsacar >= version) && (lineasAsacar < version_actual)){
            misLineas.push_back(linea);
        }
        lineasAsacar++;
    }

    string nuevoNombre;

    while(!misLineas.empty()){
        string miLinea = misLineas.back();
        misLineas.pop_back();
        nuevoNombre = deshacerCambios(nombreFichero, miLinea);
        nombreFichero = nuevoNombre;
    }
    
    f2.close();

}

int main(){

    cout << "Introduzca el fichero del que quiere obtener una version anterior: " << flush;
    string nombreFichero;
    cin >> nombreFichero;

    cout << "Introduzca el fichero donde se encuentran los cambios: " << flush;
    string ficheroCambios;
    cin >> ficheroCambios;

    cout << "Introduzca una version anterior a la que pasar el fichero: " << flush;
    int version;
    cin >> version;
    //sacar version fichero actual
    int version_actual = sacarVersion(nombreFichero);
    if(version_actual > version){
        retornarAnterior(nombreFichero, ficheroCambios, version, version_actual);
    } else {
        cout << "No se puede realizar tal cambio" << endl;
    }

    // string prueba = "hola buenas que tal estamos";
    // prueba.insert(10, 1, 'a');
    // cout << prueba << endl;

    // prueba.erase(1, 1);
    // cout << prueba << endl;
    // prueba.insert(1, 1, 'h');
    // cout << prueba << endl;

    // prueba.erase(0, 1);
    // cout << prueba << endl;
    // prueba.insert(0, 1, 'y');
    // cout << prueba << endl;

    // prueba.erase(prueba.size()-1, 1);
    // cout << prueba << endl;
    // prueba.insert(prueba.size(), 1, 'f');
    // cout << prueba << endl;

    return 0;
}