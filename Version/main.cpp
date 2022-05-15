#include <fstream>
#include <iostream>
#include <iomanip>

#include "version_storage.hpp"

using namespace std;

void Help() {
    std::cout << "Usage: version [options]\n"
        << "Options:\n" << std::left
        << std::setw(20) << "  add <file>" << "Follows given file.\n"
        << std::setw(20) << "  help" << "Help usage. What then?\n"
        << std::setw(20) << "  log" << "Displays every followed file.\n"
        << std::setw(20) << "  remove <file>" << "Remove given file.\n"
        << std::setw(50) << "  restore <file> --version <n>"
        << "Restores a version by number or by name.\n"
        << std::setw(20) << "  see <file>" << "Displays saved content of file.\n"
        << std::setw(50) << "  update <file> [--name <name>][--comm \"<desc>\"]"
        << "Updates a file, can give a name and a description.\n";
}

void option_follow(version_storage& vs, int argc, char* argv[]) {
    if (argc-1 != 2) throw bad_arguments_number("add");
    vs.Add(argv[2]);
}

void option_log(version_storage& vs, int argc, char* argv[]) {
    if (argc-1 != 1 && argc-1 != 2) throw bad_arguments_number("log");
    if (argc-1 == 1) vs.Log("");
    else vs.Log(argv[2]);
}

void option_remove(version_storage& vs, int argc, char* argv[]) {
    if (argc-1 != 2) throw bad_arguments_number("remove");
    vs.Remove(argv[2]);
}

void option_restore(version_storage& vs, int argc, char* argv[]) {
    if (argc-1 != 4) throw bad_arguments_number("restore");
    if (!strcmp(argv[3],"--version")) vs.Restore(argv[2], std::stoi(argv[4]));
    // else throw bad_option_usage();
}

void option_see(version_storage& vs, int argc, char* argv[]) {
    if (argc-1 != 2) throw bad_arguments_number("see");
    vs.See(argv[2]);
}

void option_update(version_storage& vs, int argc, char* argv[]) {
    if (argc-1 == 2) vs.Update(argv[2], "","");
    else if (argc-1 == 4) {
        if (!strcmp(argv[3], "--name")) vs.Update(argv[2], argv[4],"");
        // else throw bad_option_usage();
    } else if (argc-1 == 6) {
        if (!strcmp(argv[3], "--name") && !strcmp(argv[5], "--comm"))
            vs.Update(argv[2], argv[4], argv[6]);
        // else throw bad_option_usage();
    } 
}

int main(int argc, char* argv[]) {
    version_storage vs;
    try {
        if (!(argc-1)) Help();
        else {
            if (!strcmp(argv[1], "help")) Help();
            else if (!strcmp(argv[1], "add")) option_follow(vs, argc, argv);
            else if (!strcmp(argv[1], "init")) vs.Init();
            else if (!strcmp(argv[1], "log")) option_log(vs, argc, argv);
            else if (!strcmp(argv[1], "remove")) option_remove(vs, argc, argv);
            else if (!strcmp(argv[1], "restore")) option_restore(vs, argc, argv);
            else if (!strcmp(argv[1], "see")) option_see(vs, argc, argv);
            else if (!strcmp(argv[1], "update")) option_update(vs, argc, argv);
            else throw unknown_option(argv[1]);
        }
    } catch (unknown_option e) {
        std::cerr << e.what() << std::endl;
    } catch (bad_arguments_number e) {
        std::cerr << e.what() << std::endl;
    }
}