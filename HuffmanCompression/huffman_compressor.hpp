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
#pragma once
#include <algorithm>
#include <bitset>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unordered_map>
#include <vector>

#ifdef WIN32
    #include <windows.h>
#else
    #include <stdlib.h>
#endif

#include "huffman_exceptions.hpp"
#include "huffman_heap.hpp"
bool verbose = false;


class huffman_compressor {
private:        
    // -- Dependencias.
    // --------------------------------------------------------------------
    std::string arg;
    std::string path_file;
    std::string name;
    std::string ext;

    int rd_bytes; // Numero de bytes que seran leidos en el fichero.
    std::vector<huffman_heap*> freqs; // Tabla de frecuencias (vector).
    huffman_heap* root;
    std::unordered_map<char, std::string> codes; // Tabla de codificacion (hashmap).
    // --------------------------------------------------------------------

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Gets file size.
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    long get_file_size(std::string file) {
        struct stat stat_buf;
        long rc = stat(file.c_str(), &stat_buf);
        return rc == 0 ? stat_buf.st_size : -1;
    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Checks if file exists.
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    bool exists_file(std::string path_file) {
        struct stat tst;
        if (stat(path_file.c_str(), &tst) != 0 || !(tst.st_mode & S_IFREG)) 
            return false;
        else 
            return true;
    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Expands path-file.
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void expand_path_file(std::string rel_path) {
        char full_path[1024];
        #ifdef WIN32
            GetFullPathName(rel_path.c_str(), sizeof full_path, full_path, nullptr);
        #else
            realpath(rel_path.c_str(), full_path);
        #endif
        path_file = full_path;
    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Parses file path.
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void parse_path_file() {
        int fof = path_file.find_last_of("/\\");
        name = path_file.substr(fof + 1);
        fof = name.find_last_of('.');
        if (fof != std::string::npos) {
            ext = name.substr(fof);
            name = name.substr(0, fof);
        } else ext = "-";
    }

public:
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Constructors.                                                     --
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    huffman_compressor(std::string arg) : arg(arg) {
        if (!exists_file(arg)) throw file_not_found(arg);
        rd_bytes = 0;
        expand_path_file(arg);
        parse_path_file();
    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Inits huffman compressor.                                         --
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void init() {
        std::chrono::duration<double, std::milli> t_ex;
        auto checkpoint = std::chrono::high_resolution_clock::now();
        // -- Tabla de frecuencias.
        // --------------------------------------------------------------------
        frequences_table(); // Construccion de la tabla de las frecuencias de
                            // los caracteres que aparecen en el fichero.
        // --------------------------------------------------------------------
        if (verbose) {
            t_ex = std::chrono::high_resolution_clock::now() - checkpoint;
            std::cout << "----------------------------------------------------\n"
                      << "FREQUENCES TABLE : " << t_ex.count() << "ms\n"
                      << "----------------------------------------------------\n";
            for (auto &it : freqs) std::cout << it << "\n";
            std::cout << "----------------------------------------------------\n\n";
        }
        // --------------------------------------------------------------------


        // -- Monticulo de frecuencias de Huffman.
        checkpoint = std::chrono::high_resolution_clock::now();
        // --------------------------------------------------------------------
        full_huffman_heap();                        // Construccion del monticulo.
        std::vector<huffman_heap*>().swap(freqs);    // Liberamos el espacio que se le
                                                    // ha ido otorgando al vector a la 
                                                    // hora de construir el monticulo 
                                                    // ya que ahora esta vacio.
        // --------------------------------------------------------------------
        if (verbose) {
            t_ex = std::chrono::high_resolution_clock::now() - checkpoint;
            std::cout << "----------------------------------------------------\n"
                      << "HUFFMAN HEAP CONSTRUCION : " << t_ex.count() << "ms\n"
                      << "----------------------------------------------------\n";
            if (root != nullptr) root->print_huffman_heap(0, "R00T");
            else std::cout << "Empty heap.\n";
            std::cout << "----------------------------------------------------\n\n";
        }
        // --------------------------------------------------------------------

        // -- Tabla de codificacion.
        checkpoint = std::chrono::high_resolution_clock::now();
        // --------------------------------------------------------------------
        huffman_codes(root, "");    // Construccion de la tabla de codificacion 
                                    // con el monticulo de Huffman.
        // --------------------------------------------------------------------
        if (verbose) {
            t_ex = std::chrono::high_resolution_clock::now() - checkpoint;
            std::cout << "----------------------------------------------------\n"
                      << "HUFFMAN CODIFICATION TABLE : " << t_ex.count() << "ms\n"
                      << "----------------------------------------------------\n";
            for (auto &it:codes) std::cout << it.first << ":" << it.second << "\n";
            std::cout << "----------------------------------------------------\n\n";
        }
        // --------------------------------------------------------------------
    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Calculates the frequences tables from the given file.                 --
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void frequences_table() {
        std::ifstream in(arg);

        char ch;
        while (in >> std::noskipws >> ch) {
            auto it = find_if(freqs.begin(), freqs.end(), 
                [&ch](const huffman_heap* cf) { return cf->get_char() == ch; });
            if (it != freqs.end()) (*it)->inc_freq();
            else freqs.push_back(new huffman_heap(ch, 1));
            rd_bytes++;
        }
        if (verbose) std::cout << "READED BYTES: " << rd_bytes << "\n";

        sort(freqs.begin(), freqs.end(), 
            [](const huffman_heap *a, const huffman_heap *b){
                return a->get_freq() > b->get_freq();
            }
        );
    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Calculates the Huffman heap through a frequences table.           --
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void full_huffman_heap() {
        huffman_heap *_1, *_2;
        if (freqs.size() <= 1) root = freqs.back();
        else {
            _1 = pop_huffman_heap(freqs);
            _2 = pop_huffman_heap(freqs);
            _1 = new huffman_heap(_1,_2,_1->get_freq() + _2->get_freq());
            auto pos = find_if(freqs.begin(), freqs.end(), 
                [_1](const huffman_heap* s) { return s->get_freq() < _1->get_freq(); });
            freqs.insert(pos, _1);
            full_huffman_heap();
        } 
    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Calculates the code for each character through a huffman heap.    --
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void huffman_codes(huffman_heap* heap, std::string code) {
        if (heap->getLeft_son() == nullptr && heap->getRight_son() == nullptr)
            codes.insert({heap->get_char(), code});
        else {
            if (heap->getLeft_son() != nullptr)
                huffman_codes(heap->getLeft_son() , code + "0");
            if (heap->getLeft_son() != nullptr)
                huffman_codes(heap->getRight_son(), code + "1");
        }
    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Compress the given file through the characters code. Stores the   --
    // -- codes and the compression result itself in a file named <file>.huf -
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void compress(){
        if (!get_file_size(path_file)) throw empty_file(arg);

        std::string huf_file = name + ".huf";
        std::ofstream out(huf_file, std::ios::binary);
        out << ext << " " << rd_bytes << "\n";
        for (auto& it : codes) {
            std::string bitstring = it.second;
            int left = 0;
            std::vector<unsigned char> bytes;
            while (bitstring.length() != 0) {
                std::string the_byte = bitstring.substr(0,8);
                std::bitset<8> b(the_byte); 
                bitstring = bitstring.substr(the_byte.length(), bitstring.length());

                left = 8 - the_byte.length();
                bytes.push_back((unsigned char) (b << left).to_ulong());
            }
            ((it.first == '\n') ? out << "\\n" : out << it.first ) << bytes.size() 
                << left;
            for (auto &it: bytes) { out << it; bytes.pop_back(); } 
        }
        out << "\n";

        // Lee el fichero original caracter a caracter y escribe su codificacion 
        // correspondiente en el nuevo fichero.
        char ch;
        std::string bitstring = "";
        std::ifstream in(arg);
        while (in >> std::noskipws >> ch) {
            bitstring += codes.at(ch);
            if (bitstring.length() >= 8) {
                std::bitset<8> b(bitstring.substr(0,8));
                out << (unsigned char) b.to_ulong();
                bitstring = bitstring.substr(8,bitstring.length()-8);
            }
        }
        std::bitset<8> b(bitstring);
        out << (unsigned char) (b << (8-bitstring.length())).to_ulong();
    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Decompress the given file.                                        --
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void decompress() {
        if (ext.compare(".huf")) throw no_huffman_file(arg);
 
        int maxLength = 0;
        std::ifstream in(arg, std::ios::binary);
        unsigned char ch;
        std::string bitstring = "";

        // Obtener extension y tamaño original.
        getline(in, bitstring);
        int fof = bitstring.find_last_of(" ");
        std::string ext = bitstring.substr(0, fof);
        if (ext[0] == '-') ext = "";
        int bytes_size = atoi(bitstring.substr(fof+1).c_str());

        // Obtener tabla de codificacion.
        getline(in, bitstring);
        std::unordered_map<std::string, char> codes;
        while(bitstring.length() != 0) {
            int new_line = 0;

            char the_char;
            if (bitstring[0] == '\\' && bitstring[1] == 'n') {
                the_char = '\n';
                new_line++;
            } else the_char = bitstring[0];
            int nbytes = bitstring[1+new_line] - '0';
            int offset = bitstring[2+new_line] - '0';
            std::string code   = "";
            for (int i = 0; i < nbytes; i++) {
                std::bitset<8> bitchain(bitstring[3+new_line+i]);
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


        std::ofstream out(name + "_orig" + ext);
        long bytes_written = 0;
        while (bytes_written < bytes_size) {
            if (bitstring.length() >= maxLength) {
                int i = 1;
                std::unordered_map<std::string,char>::iterator it;
                for (; it == codes.end() && i <= maxLength; i++)
                    it = codes.find(bitstring.substr(0, i));

                bitstring = bitstring.substr(i-1, bitstring.length()-(i-1));
                out << it->second;
                bytes_written++;
                if (verbose) {
                    std::cout << "------\n";
                    std::cout << "CHAR  " << it->second << ":" << it->first << "\n";
                    std::cout << "LEFT  " << bitstring << "\n";
                    std::cout << "BYTES WRITTEN: " << bytes_written << "\n";
                }
            } else {
                in >> std::noskipws >> ch;
                std::bitset<8> bitchain(ch);
                bitstring += bitchain.to_string();
                if (verbose) {
                    std::cout << "------------------------------\n";
                    std::cout << "NEW   " << bitchain.to_string() << "\n";
                    std::cout << "SUM   " << bitstring << "\n";
                }
            }
        }
    }
};