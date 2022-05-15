/* ****************************************************************************
 * Author: Devid Dokash.
 * Date: 03/03/2022.
 * ****************************************************************************
 * Overloading operators << and >>:
    https://www.tutorialspoint.com/cplusplus/input_output_operators_overloading.htm
 * ***************************************************************************/
#pragma once
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

class huffman_heap {
private:
    huffman_heap* left_son;
    huffman_heap* right_son;
    char the_char;
    int freq;
public:
    huffman_heap(): left_son(nullptr), right_son(nullptr), the_char('\0'), 
        freq(0) {}

    huffman_heap(huffman_heap *left_son, huffman_heap *right_son, int freq):
        left_son(left_son), right_son(right_son), the_char('\0'), freq(freq) {}

    huffman_heap(char the_char, int freq): 
        left_son(nullptr), right_son(nullptr), the_char(the_char), freq(freq) {}

    ~huffman_heap(){
        if (left_son != nullptr) delete left_son;
        if (right_son != nullptr) delete right_son;
    }

    huffman_heap* getLeft_son() const { return left_son; }

    huffman_heap* getRight_son() const { return right_son; }

    char get_char() const { return the_char; }

    int get_freq() const { return freq; }

    void inc_freq() { freq++; }

    void print_huffman_heap(int level, std::string msg){
        for (int i = 0; i < level; i++) std::cout << "--";
        std::cout << msg << " " << the_char << ":" << freq << std::endl;

        if (left_son != nullptr) 
            left_son->print_huffman_heap(level+1, " L" + std::to_string(level+1));
        if (right_son != nullptr) 
            right_son->print_huffman_heap(level+1, " R" + std::to_string(level+1));
    }
};

huffman_heap* pop_huffman_heap(std::vector<huffman_heap*> &freqs){
    huffman_heap *tmp = freqs.back();
    freqs.pop_back();
    return tmp;
}

std::ostream& operator<<(std::ostream& os, const huffman_heap& hf){
    os << hf.get_char() << ":" << hf.get_freq();
    return os;
}

std::ostream& operator<<(std::ostream& os, const huffman_heap* hf)
{
    os << hf->get_char() << ":" << hf->get_freq();
    return os;
}
