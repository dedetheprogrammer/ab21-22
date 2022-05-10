#include <fstream>
#include <iostream>

#include "Version.hpp";

using namespace std;

void Help() {
    cout << "Usage: version [options]\n\n";
    cout << "Options:\n";
    cout << "  check [<file(s)>]\tDisplays followed files. If file is given, displays its local changes.\n";
    cout << "  follow <file>\tFollows given file(s).\n";
    cout << "  help\t\t\tVersion help, if not, what would it be?\n";
    cout << "  remove <file>\tStops following given file(s).\n";
    cout << "  restore <file> {--version <number> | --name <version_name> } \tRestores version n of given file.\n";
    cout << "  update <file(s)>\tUpdates given file(s).\n";
}

void option_follow(int argc, char* argv[]) {
    if (argc-1 != 2) throw bad_arguments_number("follow");
    Version v(argv[2]);
    v.Follow();
}

void option_log(int argc, char* argv[]) {
    if (argc-1 == 1) Version::Log();
    // else if(argc-1 == 2) {
    //  Version v(argv[2]);
    //  v.Log();
    // } 
    // else throw bad_option_usage();
}

void option_remove(int argc, char* argv[]) {
    if (argc-1 != 2) throw bad_arguments_number("remove");
    Version v(argv[2]);
    v.Remove();
}

void option_restore(int argc, char* argv[]) {
    id (argc-1 != 4) throw bad_arguments_number("restore");
    Version(argv[2]);
    if (!strcmp(argv[3],"--version")) v.Restore(std::stoi(argv[4]));
    else if(!strcmp(argv[3], "--name")) v.Restore(argv[4]);
    // else throw bad_option_usage();
}

void option_update(int argc, char* argv[]) {
    if (argc-1 == 4) {
        Version(argv[2]);
        if (!strcmp(argv[3], "--name")) v.Update(argv[4],"");
        // else throw bad_option_usage();
    } else if (argc-1 == 6) {
        Version(argv[2]);
        if (!strcmp(argv[3], "--name") && !strcmp(argv[5], "--comm"))
            v.Update(argv[4], argv[6]);
        // else throw bad_option_usage();
    }
}
 
int main(int argc, char* argv[]) {
    try {
        if (!(argc-1)) Help();
        else {
            if (!strcmp(argv[1], "help")) Help();
            else if (!strcmp(argv[1], "follow")) option_follow(argc, argv);
            else if (!strcmp(argv[1], "init")) Version::Init();
            else if (!strcmp(argv[1], "log")) option_log(argc, argv);
            else if (!strcmp(argv[1], "remove")) option_remove(argc, argv);
            else if (!strcmp(argv[1], "restore")) option_restore(argc, argv);
            else if (!strcmp(argv[1], "update")) option_update(argc, argv);
            else throw unknown_option(argv[1]);
        }
    } catch (unknow_option e) {
        std::cerr << e.what() << std::endl;
    } catch (bad_arguments_number e) {
        std::cerr << e.what() << std::endl;
    }
}
