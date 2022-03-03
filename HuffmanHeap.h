#pragma once
#include <ostream>
#include <string>
#include <vector>
/*
 * SOURCE:
 * Overloading operators << and >>:
    https://www.tutorialspoint.com/cplusplus/input_output_operators_overloading.htm
 *
 */


class HuffmanHeap {
private:
    HuffmanHeap* left_son;
    HuffmanHeap* right_son;
    char the_char;
    int freq;
public:
    HuffmanHeap(): left_son(nullptr), right_son(nullptr), the_char('\0'), 
        freq(0) {}

    HuffmanHeap(HuffmanHeap *left_son, HuffmanHeap *right_son, int freq):
        left_son(left_son), right_son(right_son), the_char('\0'), freq(freq) {}

    HuffmanHeap(char the_char, int freq): 
        left_son(nullptr), right_son(nullptr), the_char(the_char), freq(freq) {}

    ~HuffmanHeap(){
        if (left_son != nullptr) delete left_son;
        if (right_son != nullptr) delete right_son;
    }

    HuffmanHeap* getLeft_son() const {
        return left_son;
    }

    HuffmanHeap* getRight_son() const {
        return right_son;
    }

    char getChar() const {
        return the_char;
    }

    int getFreq() const {
        return freq;
    }

    void incFreq() {
        freq++;
    }

    void PrintHuffmanHeap(int level, std::string msg){
        for (int i = 0; i < level; i++) std::cout << "--";
        std::cout << msg << " " << the_char << ":" << freq << std::endl;

        if (left_son != nullptr) 
            left_son->PrintHuffmanHeap(level+1, " L" + std::to_string(level+1));
        if (right_son != nullptr) 
            right_son->PrintHuffmanHeap(level+1, " R" + std::to_string(level+1));
    }
};

HuffmanHeap* PopHuffmanHeap(std::vector<HuffmanHeap*> *freqs){
    HuffmanHeap *tmp = freqs->back();
    freqs->pop_back();
    return tmp;
}

std::ostream& operator<<(std::ostream& os, const HuffmanHeap& hf){
    os << hf.getChar() << ":" << hf.getFreq();
    return os;
}

std::ostream& operator<<(std::ostream& os, const HuffmanHeap* hf)
{
    os << hf->getChar() << ":" << hf->getFreq();
    return os;
}
