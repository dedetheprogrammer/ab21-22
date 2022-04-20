/* ****************************************************************************
 * Author: Devid Dokash.
 * Date: 03/03/2022.
 * ****************************************************************************
 * Read file char by char:
    https://stackoverflow.com/a/12240035/17824234
 * Finding element if exists in structs vector (idea adapted to the code):
    https://stackoverflow.com/a/2031065/17824234
 * Inserting element into a sorted vector (idea adapted to the code):
    https://stackoverflow.com/a/47991118/17824234
 * Sorting a vector by second pair element (idea adapted to the code):
    https://www.geeksforgeeks.org/sorting-vector-of-pairs-in-c-set-1-sort-by-first-and-second/
 * Using functions as parameters:
    https://www.geeksforgeeks.org/passing-a-function-as-a-parameter-in-cpp/
 * Free up memory space reserved for vector:
    https://www.techiedelight.com/delete-vector-free-memory-cpp/
 * Remove file extension:
    https://stackoverflow.com/questions/6417817/easy-way-to-remove-extension-from-a-filename
 * Bitstring to byte:
    https://stackoverflow.com/a/26661731/17824234
 * Get file size:
    https://stackoverflow.com/a/6039648/17824234
 * ***************************************************************************/
#include <algorithm>
#include <bitset>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string.h>
#include <sys/stat.h>
#include <unordered_map>
#include <vector>
#include "HuffmanHeap.h"

using namespace std;

bool verbose = false;

/* ***************************************************************************/
void HelpLog(int err, string msg) {
    switch (err) {
    case 1:
        cout << "\nerror -- \"" << msg << "\" is not the huffman file...\n";
        break;
    case 2: 
        cout << "\nerror -- unable to open \"" << msg 
                << "\", check it again...\n";
        break;
    }
    cout << "\nhuf <options> <file_name>\n";
    cout << "\t-c   compress the file.\n";
    cout << "\t-d   decompress the file.\n";
}

long GetFileSize(string file) {
    struct stat stat_buf;
    long rc = stat(file.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

bool existsFile(string file) {
    ifstream fin(file);
    return fin.is_open();
}

/*****************************************************************************/
void FrequencesTable(string file, vector<HuffmanHeap*> &freqs, int &rd_bytes) {
    fstream fin(file, fstream::in);

    char ch;
    while (fin >> noskipws >> ch) {
        auto it = find_if(freqs.begin(), freqs.end(), 
            [&ch](const HuffmanHeap* cf) { return cf->getChar() == ch; });
        if (it != freqs.end()) (*it)->incFreq();
        else freqs.push_back(new HuffmanHeap(ch, 1));
        rd_bytes++;
    }
    cout << "READED BYTES: " << rd_bytes << endl;

    sort(freqs.begin(), freqs.end(), 
        [](const HuffmanHeap *a, const HuffmanHeap *b){
            return a->getFreq() > b->getFreq();
        });
}

/*****************************************************************************/
void FullHuffmanHeap(vector<HuffmanHeap*> *freqs) {
    HuffmanHeap *_1, *_2;
    if (freqs->size() > 1) {
        _1 = PopHuffmanHeap(freqs);
        _2 = PopHuffmanHeap(freqs);
        _1 = new HuffmanHeap(_1,_2,_1->getFreq() + _2->getFreq());
        auto pos = find_if(freqs->begin(), freqs->end(), 
            [_1](const HuffmanHeap* s) { return s->getFreq() < _1->getFreq(); });
        freqs->insert(pos, _1);
        FullHuffmanHeap(freqs);
    }
}

/*****************************************************************************/
/* ... */
void HuffmanCodes(unordered_map<char, string> &codes, HuffmanHeap* heap, 
        string code) {
    if (heap->getLeft_son() == nullptr && heap->getRight_son() == nullptr)
        codes.insert({heap->getChar(), code});
    else {
        if (heap->getLeft_son() != nullptr)
            HuffmanCodes(codes, heap->getLeft_son() , code + "0");
        if (heap->getLeft_son() != nullptr)
            HuffmanCodes(codes, heap->getRight_son(), code + "1");
    }
}

/* ***************************************************************************/

/* Codifica el archivo original, con la tabla de codificacion de los caracteres
 * calculada anteriormente, en un nuevo fichero "<nombre_fichero>_hfm". 
 * ***************************************************************************/
void HuffmanFileEncoding(string file, unordered_map<char,string>* codes, int *rd_bytes){
    // Obtiene el nombre de fichero sin la extension.
    // Indice del ultimo punto que indica la extension del fichero.
    size_t lastindex = file.find_last_of("."); 
    // Extrae la subcadena resultante de quitarle la extension al fichero.
    string huf_file = file.substr(0, lastindex) + ".huf";

    ofstream fout(huf_file, ios::binary);
    fout << file.substr(lastindex+1,file.length()) << " " << *rd_bytes << endl;
    for (auto& it : *codes) {
        string bitstring = it.second;
        int left = 0;
        vector<unsigned char> bytes;
        while (bitstring.length() != 0) {
            string the_byte = bitstring.substr(0,8);
            bitset<8> b(the_byte); 
            bitstring = bitstring.substr(the_byte.length(), bitstring.length());

            left = 8 - the_byte.length();
            bytes.push_back((unsigned char) (b << left).to_ulong());
        }
        ((it.first == '\n') ? fout << "\\n" : fout << it.first ) << bytes.size() 
            << left;
        for (auto &it: bytes) { fout << it; bytes.pop_back(); } 
    }
    fout << "\n";

    // Lee el fichero original caracter a caracter y escribe su codificacion 
    // correspondiente en el nuevo fichero.
    char ch;
    string bitstring = "";
    fstream fin(file, fstream::in);
    while (fin >> noskipws >> ch) {
        bitstring += codes->at(ch);
        if (bitstring.length() >= 8) {
            bitset<8> b(bitstring.substr(0,8));
            fout << (unsigned char) b.to_ulong();
            bitstring = bitstring.substr(8,bitstring.length()-8);
        }
    }
    bitset<8> b(bitstring);
    fout << (unsigned char) (b << (8-bitstring.length())).to_ulong();
}

/* ***************************************************************************/
void HuffmanFileDecoding(string file) {
    int maxLength = 0;
    
    //HuffmanCodesFile(file + ".dec", &codes, &ext, &left, &maxLength);
    ifstream fin(file + ".huf", ios::binary);
    unsigned char ch;
    string bitstring = "";

    // Obtener extension y tamaño original.
    getline(fin, bitstring);
    int dummy = bitstring.find_last_of(" ");
    string ext = bitstring.substr(0,dummy);
    int bytes_size = atoi(bitstring.substr(dummy+1,bitstring.length()).c_str());
    //cout << "EXTENSION ORIGINAL: " << ext << " Y TAMANIO " << bytes_size << "B\n";

    // Obtener tabla de codificacion.
    getline(fin, bitstring);
    unordered_map<string, char> codes;
    while(bitstring.length() != 0) {
        int new_line = 0;

        char the_char;
        if (bitstring[0] == '\\' && bitstring[1] == 'n') {
            the_char = '\n';
            new_line++;
        } else the_char = bitstring[0];
        int nbytes    = bitstring[1+new_line] - '0';
        int offset      = bitstring[2+new_line] - '0';
        string code   = "";
        for (int i = 0; i < nbytes; i++) {
            bitset<8> bitchain(bitstring[3+new_line+i]);
            code += bitchain.to_string();
        }
        //cout << "THE KEY " << the_char << " WITH ITS BYTES " << nbytes << 
        //    " AND THE OFFSET " << offset << endl;
        //cout << "\tTHE WHOLE BITCHAIN " << code << endl;
        //cout << "\tTHE ORIGINAL CODE  " << code.substr(0,code.length() - offset) << endl;
        code = code.substr(0, code.length()-offset);
        int length = code.length();
        if (length > maxLength) maxLength = length;
        codes.insert({code, the_char});
        bitstring = bitstring.substr(3+nbytes+new_line,bitstring.length());
    }
    //for (auto &it : codes) cout << it.second << "\t" << it.first << endl;
    
    
    fstream fout(file + "_orig." + ext, fstream::out);
    long bytes_written = 0;
    while (bytes_written < bytes_size) {
        if (bitstring.length() >= maxLength) {
            int i = 1;
            unordered_map<string,char>::iterator it;
            for (; it == codes.end() && i <= maxLength; i++)
                it = codes.find(bitstring.substr(0, i));
            
            bitstring = bitstring.substr(i-1, bitstring.length()-(i-1));
            fout << it->second;
            bytes_written++;
            if (verbose) {
                cout << "------" << endl;
                cout << "CHAR  " << it->second << ":" << it->first << endl;
                cout << "LEFT  " << bitstring << endl;
                cout << "BYTES WRITTEN: " << bytes_written << endl;
            }
        } else {
            fin >> noskipws >> ch;
            bitset<8> bitchain(ch);
            bitstring += bitchain.to_string();
            if (verbose) {
                cout << "------------------------------" << endl;
                cout << "NEW   " << bitchain.to_string() << endl;
                cout << "SUM   " << bitstring << endl;
            }
        }
    }
}

/* ***************************************************************************/
int main(int argc, char *argv[]) {
    if (argc != 3) {
        HelpLog(0,"");
        return -1;
    }

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto checkpoint = high_resolution_clock::now();
    chrono::duration<double, milli> texec;
    string file(argv[2]);

    if (!existsFile(file)) { HelpLog(2, file); return -1; }
    if (strcmp(argv[1],"-c") == 0){
        if(GetFileSize(file) == 0) { cerr << "\nwarning -- your file was empty, no file was generated...\n"; exit(0); }

        // -- Dependencias.
        // --------------------------------------------------------------------
        int readed_bytes = 0;               // Numero de bytes que seran leidos
                                            // en el fichero.
        vector<HuffmanHeap*> freqs;         // Tabla de frecuencias (vector).
        unordered_map<char, string> codes;  // Tabla de codificacion (hashmap).
        // --------------------------------------------------------------------

        // -- Tabla de frecuencias de los caracteres del fichero.
        if (verbose) checkpoint = high_resolution_clock::now();
        // --------------------------------------------------------------------
        FrequencesTable(file, freqs, readed_bytes); // Construccion de la tabla
                                                    // de las frecuencias de
                                                    // los caracteres que 
                                                    // aparecen en el fichero.
        // --------------------------------------------------------------------
        if (verbose) {
            texec = chrono::high_resolution_clock::now() - checkpoint;
            cout << "----------------------------------------------------\n";
            cout << "FREQUENCES TABLE : " << texec.count() << "ms\n";
            cout << "----------------------------------------------------\n";
            for (auto &it : freqs) cout << it << endl;
            cout << "----------------------------------------------------\n\n";
        }

        // -- Monticulo de frecuencias de Huffman.
        if (verbose) checkpoint = high_resolution_clock::now();
        // --------------------------------------------------------------------
        FullHuffmanHeap(&freqs);            // Construccion del monticulo.

        HuffmanHeap *root = freqs.back();   // Recuperamos la raiz del vector.

        vector<HuffmanHeap*>().swap(freqs); // Liberamos el espacio que se le
                                            // ha ido otorgando al vector a la 
                                            // hora de construir el monticulo 
                                            // ya que ahora esta vacio.
        // --------------------------------------------------------------------
        if (verbose) {
            texec = chrono::high_resolution_clock::now() - checkpoint;
            cout << "----------------------------------------------------\n";
            cout << "HUFFMAN HEAP CONSTRUCION : " << texec.count() << "ms\n";
            cout << "----------------------------------------------------\n";
            if (root != nullptr) root->PrintHuffmanHeap(0, "R00T");
            else cout << "Empty heap.";
            cout << "----------------------------------------------------\n\n";
        }

        // -- Tabla de codificacion.
        if (verbose) checkpoint = high_resolution_clock::now();
        // --------------------------------------------------------------------
        HuffmanCodes(codes, root, ""); // Construccion de la tabla de codifi-
                                        // cacion mediante el monticulo de 
                                        // Huffman.
        // --------------------------------------------------------------------
        if (verbose) {
            texec = chrono::high_resolution_clock::now() - checkpoint;
            cout << "----------------------------------------------------\n";
            cout << "HUFFMAN CODIFICATION TABLE : " << texec.count() << "ms\n";
            cout << "----------------------------------------------------\n";
            for (auto &it:codes) cout << it.first << ":" << it.second << endl;
            cout << "----------------------------------------------------\n\n";
        }

        // -- Codificacion del archivo.
        if (verbose) checkpoint = high_resolution_clock::now();
        // --------------------------------------------------------------------
        HuffmanFileEncoding(file, &codes, &readed_bytes); // Codificacion del 
                                                          // fichero con la 
                                                          // susodicha tabla.
        // --------------------------------------------------------------------
        if (verbose) {
            texec = chrono::high_resolution_clock::now() - checkpoint;
            cout << "----------------------------------------------------\n";
            cout << "HUFFMAN FILE ENCODING : " << texec.count() << "ms\n\n";
        }

        return 0;
    }
    if(strcmp(argv[1],"-d") == 0){
        size_t lastindex = file.find_last_of(".");
        string ext = file.substr(lastindex, file.length()-lastindex);

        if(ext.compare(".huf") != 0) HelpLog(1, file);
        else HuffmanFileDecoding(file.substr(0, lastindex));

        return 0;
    } 
    
    HelpLog(0, "");
}