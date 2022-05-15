#include <exception>
#include <string>

struct file_not_found : public std::exception {

    std::string file; 
    std::string msg;

    file_not_found(std::string file) : file(file) {
        msg = ("huf: error: file \'" + file + "\' was not found.");
    }

	const char * what () const throw () {
    	return msg.c_str();
    }
};

struct empty_file : public std::exception {

    std::string file; 
    std::string msg;

    empty_file(std::string file) : file(file) {
        msg =  "\nhuf: warning -- file \'" + file + "\' was empty, no compression done...";
    }

	const char * what () const throw () {
    	return msg.c_str();
    }
};

struct no_huffman_file : public std::exception {

    std::string file; 
    std::string msg;

    no_huffman_file(std::string file) : file(file) {
        msg =  "\nhuf: error -- \'" + file + "\' is not a huffman file...";
    }

	const char * what () const throw () {
    	return msg.c_str();
    }
};

struct unknown_option : public std::exception {

    std::string option; 
    std::string msg;

    unknown_option(std::string option) : option(option) {
        msg =  "\nhuf: error: unknown option \'" + option + "\'.";
    }

	const char * what () const throw () {
    	return msg.c_str();
    }
};

struct bad_arguments_number : public std::exception {

    std::string option; 
    std::string msg;

    bad_arguments_number(std::string option) : option(option) {
        msg = ("version: error: option \'" + option + "\' requires a file to work");
    }

	const char * what () const throw () {
    	return msg.c_str();
    }
};