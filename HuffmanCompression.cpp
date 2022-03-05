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
 * ***************************************************************************/
#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <string.h>
#include <vector>
#include "HuffmanHeap.h"

using namespace std;

bool verbose = false;

struct Coding {
    char key;
    string code;
};

std::ostream& operator <<(std::ostream& os, const Coding& c){
    os << c.key << ":" << c.code;
    return os;
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
void HuffmanCodes(vector<Coding>* codes, HuffmanHeap* heap, string code) {
    if (heap->getLeft_son() == nullptr && heap->getRight_son() == nullptr) {
        auto pos = find_if(codes->begin(), codes->end(), 
            [code](const Coding &s) { 
                return s.code.length() >= code.length() && s.code >= code; });
        codes->insert(pos, Coding{heap->getChar(), code});
    } else {
        HuffmanHeap* son = heap->getLeft_son();
        if (son != nullptr) HuffmanCodes(codes, son, code + "0");
        son = heap->getRight_son();
        if (son != nullptr) HuffmanCodes(codes, son, code + "1");
    }
}

string HuffmanCodesFile(string codes_file, vector<Coding> *codes, int *maxL) {
    fstream fin(codes_file, fstream::in);

    int sep;
    string ext, line, key, code;
    getline(fin, ext);
    while (getline(fin, line)) {
        sep = line.find_last_of(" ");
        key = line.substr(0, sep);
        code = line.substr(sep+1, line.length()-sep);
        if (key.compare("\\n") == 0) codes->push_back(Coding{'\n',code});
        else codes->push_back(Coding{key[0], code});

        int length = code.length();
        if (length > *maxL) *maxL = length;
    }
    return "." + ext;
}

void PrintHuffmanCodesTable(vector<Coding>* codes) {
    cout << "\nCODING TABLE:\n";
    for (auto it:*codes) cout << it << endl;  
    cout << endl;
}

/* ***************************************************************************
 * Codifica el archivo original, con la tabla de codificacion de los caracteres
 * calculada anteriormente, en un nuevo fichero "<nombre_fichero>_hfm". 
 * ***************************************************************************/
void HuffmanFileEncoding(string file, vector<Coding> codes){
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
        auto it = find_if(codes.begin(), codes.end(), 
            [&ch](const Coding &cf) { return cf.key == ch; });
        bitstring += it->code;
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
    fout_c << file.substr(lastindex+1, file.length()-lastindex) /* << " "
        << (8-bitstring.length())*/ << endl;
    for (auto it : codes) {
        bitset<8> code(it.code);
        if (it.key == '\n') fout_c << "\\n";
        else fout_c << it.key;
        fout_c << " " << it.code << "\n";
    }
}

/* ***************************************************************************/
void HuffmanFileDecoding(string file) {

    int maxLength = 0;
    vector<Coding> codes;
    string ext = HuffmanCodesFile(file + ".dec", &codes, &maxLength);

    ifstream fin(file + ".huf", ios::binary);
    fstream fout(file + "_rest" + ext, fstream::out);

    unsigned char ch;
    string bitstring = "";
    while (fin >> noskipws >> ch) {
        bitset<8> bitchain(ch);
        bitstring += bitchain.to_string();
        //cout << "------------------------------" << endl;
        //cout << "NEW   " << bitchain.to_string() << endl;
        //cout << "SUM   " << bitstring << endl;

        while (bitstring.length() >= maxLength) {
            int i = 1;
            vector<Coding>::iterator it;
            for (it = codes.end(); it == codes.end() && i <= maxLength; i++) {
                it = find_if(codes.begin(), codes.end(),
                    [&bitstring, &i](const Coding &cd) { 
                        return cd.code.compare(bitstring.substr(0, i)) == 0; 
                    });
            }
            
            bitstring = bitstring.substr(i-1, bitstring.length()-(i-1));
            fout << it->key;
            //cout << "------" << endl;
            //cout << "CHAR  " << it->key << endl;
            //cout << "LEFT  " << bitstring << endl;
        } 
    }
}

/* ***************************************************************************/
void HelpLog(int err) {
    switch (err) {
    case 1:
        cout << "\nerror -- this is not the huffman file...\n";
        break;
    }
    cout << "\nhuf <options> <file_name>\n";
    cout << "\t-c   compress the file.\n";
    cout << "\t-d   decompress the file.\n";
}

/* ***************************************************************************/
int main(int argc, char *argv[]) {
    if (argc != 3) {
        HelpLog(0);
        return -1;
    }

    string file(argv[2]);
    if (strcmp(argv[1],"-c") == 0){
        // Tabla de frecuencias.
        vector<HuffmanHeap*> freqs;
        FrequencesTable(file, &freqs);
        if (verbose) PrintFreqsTable(&freqs);

        // Monticulo de frecuencias de Huffman.
        FullHuffmanHeap(&freqs);
        HuffmanHeap *root = freqs.back();
        vector<HuffmanHeap*>().swap(freqs);
        if (verbose) PrintHuffmanHeap(root);

        // Tabla de codificacion.
        vector<Coding> codes;
        HuffmanCodes(&codes, root, "");
        if (verbose) PrintHuffmanCodesTable(&codes);

        // Codificacion del archivo.
        HuffmanFileEncoding(file, codes);

    } else {
        if(strcmp(argv[1],"-d") == 0){
            size_t lastindex = file.find_last_of(".");
            string ext = file.substr(lastindex, file.length()-lastindex);
            file = file.substr(0, lastindex);

            if(ext.compare(".huf") != 0) HelpLog(1);
            else HuffmanFileDecoding(file);

        } else HelpLog(0);
    }
}