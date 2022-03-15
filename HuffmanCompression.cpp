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
    int rc = stat(file.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

bool existsFile(string file) {
    ifstream fin(file);
    return fin.is_open();
}

/*****************************************************************************/
void FrequencesTable(string file, vector<HuffmanHeap*> *freqs) {
    fstream fin(file, fstream::in);

    char ch;
    while (fin >> noskipws >> ch) {
        auto it = find_if(freqs->begin(), freqs->end(), 
            [&ch](const HuffmanHeap* cf) { return cf->getChar() == ch; });
        if (it != freqs->end()) (*it)->incFreq();
        else freqs->push_back(new HuffmanHeap(ch, 1));
    }

    sort(freqs->begin(), freqs->end(), 
        [](const HuffmanHeap *a, const HuffmanHeap *b){
            return a->getFreq() > b->getFreq();
        });
}

void PrintFreqsTable(vector<HuffmanHeap*> *freqs){
    cout << "\nCHARS AND FREQUENCIES:\n";
    for (auto it:*freqs) cout << *it << endl;
    cout << endl;
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

void PrintHuffmanHeap(HuffmanHeap* root) {
    cout << "\nHUFFMAN HEAP:\n";
    if (root != nullptr) root->PrintHuffmanHeap(0, "R00T");
    else cout << "Empty heap.";
    cout << endl;
}

/*****************************************************************************/
void HuffmanCodes(unordered_map<char, string>* codes, HuffmanHeap* heap, string code) {
    if (heap->getLeft_son() == nullptr && heap->getRight_son() == nullptr) {
        codes->insert({heap->getChar(), code});
    } else {
        HuffmanHeap* son = heap->getLeft_son();
        if (son != nullptr) HuffmanCodes(codes, son, code + "0");
        son = heap->getRight_son();
        if (son != nullptr) HuffmanCodes(codes, son, code + "1");
    }
}

void HuffmanCodesFile(string codes_file, unordered_map<string, char> *codes, 
        string *ext, int *left, int *maxL) {
    fstream fin(codes_file, fstream::in);

    int sep;
    string line;
    getline(fin, line);
    if (line.compare("") != 0) {
        sep = line.find_last_of(" ");
        *ext = "." + line.substr(0, sep);
        *left = atoi((line.substr(sep+1, line.length()-sep)).c_str());
    }

    while (getline(fin, line)) {
        sep = line.find_last_of(" ");
        string key = line.substr(0, sep);
        string code = line.substr(sep+1, line.length()-sep);
        if (key.compare("\\n") == 0) codes->insert({code,'\n'});
        else codes->insert({code,key[0]});

        int length = code.length();
        if (length > *maxL) *maxL = length;
    }
}

/*void PrintHuffmanCodesTable(unordered_map<char, string>* codes) {
    cout << "\nCODING TABLE:\n";
    for (auto &it:*codes) cout << it << endl;  
    cout << endl;
}*/

/* ***************************************************************************
 * Codifica el archivo original, con la tabla de codificacion de los caracteres
 * calculada anteriormente, en un nuevo fichero "<nombre_fichero>_hfm". 
 * ***************************************************************************/
void HuffmanFileEncoding(string file, unordered_map<char,string>* codes){
    // Obtiene el nombre de fichero sin la extension.
    // Indice del ultimo punto que indica la extension del fichero.
    size_t lastindex = file.find_last_of("."); 
    // Extrae la subcadena resultante de quitarle la extension al fichero.
    string rawname = file.substr(0, lastindex);

    // Lee el fichero original caracter a caracter y escribe su codificacion 
    // correspondiente en el nuevo fichero.
    char ch;
    string bitstring = "";
    fstream fin(file, fstream::in);
    ofstream fout(rawname + ".huf", ios::binary);
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

    // HuffmanMagicNumbers: header especial para poder decodificar el archivo 
    // comprimido al archivo de origen (? - Hay que revisarlo).
    fstream fout_c(rawname + ".dec", fstream::out);
    fout_c << file.substr(lastindex+1, file.length()-lastindex) << " "
        << (8-bitstring.length()) << endl;
    for (auto& it : *codes) {
        bitset<8> code(it.second);
        if (it.first == '\n') fout_c << "\\n";
        else fout_c << it.first;
        fout_c << " " << it.second << "\n";
    }
}

/* ***************************************************************************/
void HuffmanFileDecoding(string file) {
    int maxLength = 0, left = 0;
    unordered_map<string,char> codes;
    string ext;
    
    HuffmanCodesFile(file + ".dec", &codes, &ext, &left, &maxLength);

    ifstream fin(file + ".huf", ios::binary);
    fstream fout(file + "_orig" + ext, fstream::out);

    unsigned char ch;
    string bitstring = "";
    long readed_bits = 0;
    long size = (GetFileSize(file + ".huf") * 8 ) - left;
    while (readed_bits < size) {
        if (bitstring.length() >= maxLength) {
            int i = 1;
            unordered_map<string,char>::iterator it;
            for (; it == codes.end() && i <= maxLength; i++)
                it = codes.find(bitstring.substr(0, i));
            
            bitstring = bitstring.substr(i-1, bitstring.length()-(i-1));
            fout << it->second;
            readed_bits += (i-1);
            if (verbose) {
                cout << "------" << endl;
                cout << "CHAR  " << it->second << endl;
                cout << "LEFT  " << bitstring << endl;
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

    string file(argv[2]);
    if (!existsFile(file)) {
        HelpLog(2, file);
        return -1;
    }

    if (strcmp(argv[1],"-c") == 0){
        if(GetFileSize(file) == 0) {
            cerr << "\nwarning -- your file was empty, no file was generated...\n";
            exit(0);
        }

        // Tabla de frecuencias.
        vector<HuffmanHeap*> freqs;
        auto t1 = chrono::high_resolution_clock::now();
        FrequencesTable(file, &freqs);
        auto t2 = chrono::high_resolution_clock::now();
        chrono::duration<double, std::milli> ms_double = t2 - t1;
        cout << "FREQUENCES TABLE: " << ms_double.count() << "ms" << endl;

        // Monticulo de frecuencias de Huffman.
        t1 = chrono::high_resolution_clock::now();
        FullHuffmanHeap(&freqs);
        HuffmanHeap *root = freqs.back();
        vector<HuffmanHeap*>().swap(freqs);
        t2 = chrono::high_resolution_clock::now();
        ms_double = t2 - t1;
        cout << "FULL HUFFMAN HEAP: " << ms_double.count() << "ms" << endl;

        // Tabla de codificacion.
        t1 = chrono::high_resolution_clock::now();
        unordered_map<char, string> codes;
        HuffmanCodes(&codes, root, "");
        t2 = chrono::high_resolution_clock::now();
        ms_double = t2 - t1;
        cout << "HUFFMAN CODES: " << ms_double.count() << "ms" << endl;

        // Codificacion del archivo.
        t1 = chrono::high_resolution_clock::now();
        HuffmanFileEncoding(file, &codes);
        t2 = chrono::high_resolution_clock::now();
        ms_double = t2 - t1;
        cout << "HUFFMAN FILE ENCODING: " << ms_double.count() << "ms" << endl;
    } else {
        if(strcmp(argv[1],"-d") == 0){
            size_t lastindex = file.find_last_of(".");
            string ext = file.substr(lastindex, file.length()-lastindex);

            if(ext.compare(".huf") != 0) HelpLog(1, file);
            else HuffmanFileDecoding(file.substr(0, lastindex));
        } else HelpLog(0, "");
    }
}