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
// 52584
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