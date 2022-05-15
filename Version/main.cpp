#include <fstream>
#include <iostream>
#include <iomanip>

#include "version_storage.hpp"

using namespace std;

void Help() {
    std::cout << "Usage: version [options]\n"
        << "Options:\n" << std::left
        << std::setw(20) << "  add <file>" << "Follows given file.\n"
        << std::setw(20) << "  erase" << "Delete every file and the directory of version.\n"
        << std::setw(20) << "  help" << "Help usage. What then?\n"
        << std::setw(20) << "  init" << "Initializes version. Required for working.\n"
        << std::setw(20) << "  log" << "Displays every followed file.\n"
        << std::setw(20) << "  remove <file>" << "Remove given file.\n"
        << std::setw(35) << "  restore <file> --version <n>"
        << "Restores a version by number or by name.\n"
        << std::setw(20) << "  see <file>" << "Displays saved content of file.\n"
        << std::setw(50) << "  update <file> [--name <name>][--comm \"<desc>\"]"
        << "Updates a file, can give a name and a description.\n";
}

void option_add(version_storage& vs, int argc, char* argv[]) {
    if (argc-1 != 2) throw bad_option_usage("add");
    vs.Add(argv[2]);
}

void option_erase(version_storage& vs, int argc) {
    if (argc-1 != 1) throw bad_option_usage("erase");
    vs.Erase();
}

void option_init(int argc) {
    if (argc-1 != 1) throw bad_option_usage("init");
    version_storage vs(1);
    vs.Init();
}

void option_help(int argc) {
    if (argc-1 != 1) throw bad_option_usage("help");
    Help();
}

void option_log(version_storage& vs, int argc, char* argv[]) {
    if (argc-1 == 1) vs.Log("");
    else if (argc-1 == 2) vs.Log(argv[2]);
    else throw bad_option_usage("log");
}

void option_remove(version_storage& vs, int argc, char* argv[]) {
    if (argc-1 != 2) throw bad_option_usage("remove");
    vs.Remove(argv[2]);
}

void option_restore(version_storage& vs, int argc, char* argv[]) {
    if (argc-1 != 4 || strcmp(argv[3],"--version")) throw bad_option_usage("restore");
    vs.Restore(argv[2], std::stoi(argv[4]));
}

void option_see(version_storage& vs, int argc, char* argv[]) {
    if (argc-1 != 2) throw bad_option_usage("see");
    vs.See(argv[2]);
}

void option_update(version_storage& vs, int argc, char* argv[]) {
    if (argc-1 == 2) vs.Update(argv[2], "","");
    else if (argc-1 == 4) {
        if (!strcmp(argv[3], "--name")) vs.Update(argv[2], argv[4],"");
        else throw bad_option_usage("update");
    } else if (argc-1 == 6) {
        if (!strcmp(argv[3], "--name") && !strcmp(argv[5], "--comm"))
            vs.Update(argv[2], argv[4], argv[6]);
        else throw bad_option_usage("update");
    } else throw bad_option_usage("update");
}

int main(int argc, char* argv[]) {
    try {
        if (!(argc-1)) Help();
        else {
            if (!strcmp(argv[1], "init")) option_init(argc);
            else if (!strcmp(argv[1], "help")) option_help(argc);
            else {
                version_storage vs;
                if (!strcmp(argv[1], "add")) option_add(vs, argc, argv);
                else if (!strcmp(argv[1], "erase")) option_erase(vs, argc);
                else if (!strcmp(argv[1], "log")) option_log(vs, argc, argv);
                else if (!strcmp(argv[1], "remove")) option_remove(vs, argc, argv);
                else if (!strcmp(argv[1], "restore")) option_restore(vs, argc, argv);
                else if (!strcmp(argv[1], "see")) option_see(vs, argc, argv);
                else if (!strcmp(argv[1], "update")) option_update(vs, argc, argv);
                else throw unknown_option(argv[1]);
            }
        }
    } catch (unknown_option e) {
        std::cerr << e.what() << std::endl;
    } catch (bad_option_usage e) {
        std::cerr << e.what() << std::endl;
    } catch (version_not_installed e) {
        std::cerr << e.what() << std::endl;
    }
}