/* ****************************************************************************
 * File: Huffman compressor main.
 * Author: Devid Dokash.
 * Date: 03/03/2022.
 * ***************************************************************************/
#include <iomanip>
#include <iostream>

#include "huffman_compressor.hpp"

using namespace std;

void Help() {
    std::cout << "Usage: huf [options]\n"
        << "Options:\n" << std::left
        << std::setw(20) << "  -c <file>" << "Compress the given file.\n"
        << std::setw(20) << "  -d <file>.huf" << "Decompress the given file.\n"
        << std::setw(20) << "  help" << "Help usage.\n";
}

void compress_option(int argc, char *argv[]) {
    if (argc-1 != 2) throw bad_arguments_number("-c");
    huffman_compressor hfc(argv[2]);
    hfc.init();
    hfc.compress();
}

void decompress_option(int argc, char *argv[]) {
   if (argc-1 != 2) throw bad_arguments_number("-d");
   huffman_compressor hfc(argv[2]);
   hfc.init();
   hfc.decompress();
}

// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// -- Main                                                                  --
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int main(int argc, char *argv[]) {  
    try {
        if (!(argc-1)) Help();
        else if (!strcmp(argv[1], "help")) Help();
        else if (!strcmp(argv[1], "-c")) compress_option(argc, argv);
        else if (!strcmp(argv[1], "-d")) decompress_option(argc, argv);
        else throw unknown_option(argv[1]);
    } catch (file_not_found e) {
        std::cerr << e.what() << std::endl;
    } catch (no_huffman_file e) {
        std::cerr << e.what() << std::endl;
    } catch (empty_file e) {
        std::cerr << e.what() << std::endl;
    }
}