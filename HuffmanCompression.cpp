#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "HuffmanHeap.h"

/* Implementation source:
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
 */

using namespace std;

struct Coding {
    char key;
    string code;
};

std::ostream& operator <<(std::ostream& os, const Coding& c){
    os << c.key << ":" << c.code;
    return os;
}

/*****************************************************************************/
void FrequencesTable(vector<HuffmanHeap*> *freqs, string file){
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


/*****************************************************************************/
void FullHuffmanHeap(vector<HuffmanHeap*> *freqs){
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
void HuffmanCodes(vector<Coding>* codes, HuffmanHeap* heap, string code) {
    if (heap->getLeft_son() == nullptr && heap->getRight_son() == nullptr) {
        auto pos = find_if(codes->begin(), codes->end(), 
            [code](const Coding &s) { 
                return s.code.length() >= code.length() && s.code >= code; });
        codes->insert(pos, Coding{heap->getChar(), code});
    } else {
        HuffmanHeap* son = heap->getLeft_son();
        if (son != nullptr) HuffmanCodes(codes, son, "0" + code);
        son = heap->getRight_son();
        if (son != nullptr) HuffmanCodes(codes, son, "1" + code);
    }
}


/* ***************************************************************************
 * Codifica el archivo original, con la tabla de codificacion de los caracteres
 * calculada anteriormente, en un nuevo fichero "<nombre_fichero>_hfm". 
 * ***************************************************************************/
void HuffmanEncodedFile(string file, vector<Coding> codes){
    // Obtiene el nombre de fichero sin la extension.
    // Indice del ultimo punto que indica la extension del fichero.
    size_t lastindex = file.find_last_of("."); 
    // Extrae la subcadena resultante de quitarle la extension al fichero.
    string rawname = file.substr(0, lastindex) + "_hfm"; 

    fstream fin(file, fstream::in);
    fstream fout(rawname, fstream::out);

    // HuffmanMagicNumbers: header especial para poder decodificar el archivo 
    // comprimido al archivo de origen (? - Hay que revisarlo).
    //for (auto it : codes) fout << it.code << it.key;

    // Lee el fichero original caracter a caracter y escribe su codificacion 
    // correspondiente en el nuevo fichero.
    char ch;
    string bit_str = "";
    while (fin >> noskipws >> ch) {
        auto it = find_if(codes.begin(), codes.end(), 
            [&ch](const Coding &cf) { return cf.key == ch; });
        bit_str += it->code;
        if (bit_str.length() >= 8) {
            bitset<8> b(bit_str.substr(0,8));
            fout << ((unsigned char)(b.to_ulong() & 0xFF));
            bit_str = bit_str.substr(8,bit_str.length()-8);
        }
    }
    bitset<8> b(bit_str);
    fout << ((unsigned char)(b.to_ulong() & 0xFF));
}

int main(int argc, char *argv[]) {
    string file(argv[1]);

    // Obtener la tabla de frecuencias.
    vector<HuffmanHeap*> freqs;
    FrequencesTable(&freqs, file);
    //cout << "\nCHARS AND FREQUENCIES:\n";
    //for (auto it:freqs) cout << *it << endl;

    // Obtener el monticulo de frecuencias de Huffman.
    FullHuffmanHeap(&freqs);
    HuffmanHeap *root = freqs.back();
    vector<HuffmanHeap*>().swap(freqs);

    //cout << "\nHUFFMAN HEAP:\n";
    //if (root != nullptr) root->PrintHuffmanHeap(0, "R00T");
    //else cout << "Empty heap.";

    // Obtener la tabla de codificacion.
    vector<Coding> codes;
    HuffmanCodes(&codes, root, "");
    //cout << "\nCODING TABLE:\n";
    //for (auto it:codes) cout << it << endl;  
    

    // Codificar el archivo.
    HuffmanEncodedFile(file, codes);
}