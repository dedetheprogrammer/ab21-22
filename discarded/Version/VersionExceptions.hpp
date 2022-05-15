#include <exception>
#include <string>

struct file_not_found : public std::exception {

    std::string file; 
    std::string msg;

    file_not_found(std::string file) : file(file) {
        msg = ("version: error: file \'" + file + "\' was not found.");
    }

	const char * what () const throw () {
    	return msg.c_str();
    }
};

struct file_not_followed : public std::exception {

    std::string file;
    std::string msg;

    file_not_followed(std::string file) : file(file)  {
        msg = ("version: error: file \'" + file + "\' not followed.");
    } 

	const char * what () const throw () {
    	return msg.c_str();
    }
};

struct file_already_followed : public std::exception {
    
    std::string file;
    std::string msg;

    file_already_followed(std::string file) : file(file)  {
        msg = ("version: warning: file \'" + file + "\' was already followed.");
    } 

	const char * what () const throw () {
    	return msg.c_str();
    }
};

struct version_not_found : public std::exception {
    
    std::string name;
    int n;

    std::string msg;

    version_not_found(std::string name) : name(name)  {
        msg = ("version: error: version under name \'" + name + "\' not found.");
    }

    version_not_found(int n) : n(n)  {
        msg = ("version: error: no version " + std::to_string(n) + " available");
    }

	const char * what () const throw () {
    	return msg.c_str();
    }
};

struct unknown_option : public std::exception {

    std::string option;
    std::string msg;

    unknown_option(std::string option) : option(option)  {
        msg = ("version: error: unknown option \'" + option + "\'.");
    } 

	const char * what () const throw () {
    	return msg.c_str();
    }
};

struct bad_arguments_number : public std::exception {

    std::string option;
    std::string msg;

    bad_arguments_number(std::string option) : option(option) {
        msg = ("version: error: \'" + option + "\' requires a file to work");
        if(!option.compare("follow")) msg = " --> follow <file>";
        else if(!option.compare("remove")) msg = " --> remove <file>";
    } 

	const char * what () const throw () {
    	return msg.c_str();
    }
};