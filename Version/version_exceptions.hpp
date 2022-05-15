#pragma once
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

struct version_name_not_valid : public std::exception {

    std::string name;
    std::string msg;

    version_name_not_valid(std::string name) : name(name) {
        msg = "version: error: given name \'" + name + "\' contains invalid " +
            "symbols. Valid symbols: alphanumeric, \'_\'";
    } 

	const char * what () const throw () {
    	return msg.c_str();
    }
};

struct already_named_version : public std::exception {

    std::string msg;

    already_named_version(std::string file, std::string name) {
        msg = "version: error: there is already a version named \'" + name 
            + "\' in file \'" + file + "\'.";
    } 

	const char * what () const throw () {
    	return msg.c_str();
    }
};

struct too_long_description : public std::exception {

    too_long_description() {} 

	const char * what () const throw () {
    	return "version: error: descriptions have a max length of 150 symbols.";
    }
};

