/******************************************************************************
 * 
 ******************************************************************************
 * Source:
 * https://stackoverflow.com/a/42114477/17824234 (borrar una linea n de un fichero).
 *****************************************************************************/
#include <algorithm>
#include <direct.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "SeqComparator.hpp"
#include "VersionExceptions.hpp"

#ifdef WIN32
    #include <windows.h>
#elif LINUX
    #include <stdlib.h>
#endif

class Version {
private: 
    const std::string REGISTER = "version/version_register";
    const std::string TMP = "version/tmp";

    std::string arg;
    std::string path_file;
    std::string name;
    std::string extension;
    std::string path;
    std::string version_id;
    int register_pos;

    bool exists;
    bool followed;
    bool updated;

    std::string register_time;
    std::string register_date;
    int current_version;
    int last_version;
    std::string last_modified_time;
    std::string last_modified_date;

    std::string timestamp() {
        time_t t; time(&t);
        char ts[256];
        strftime(ts, sizeof ts, "%X,%d-%m-%Y", gmtime(&t));
        return ts;
    }

    std::vector<std::string> tokenize(std::string scope, const std::string patron, bool extrict) {
        std::regex re(patron);
        std::sregex_token_iterator s(scope.begin(), scope.end(), re, -1), e;
        if (extrict) {
            std::vector<std::string> t;
            remove_copy_if(s, std::sregex_token_iterator(), back_inserter(t), 
                    [](std::string const &s) { return s.empty(); });
            return t;
        } else {
            return {s, e};
        }

    }

    void init() {
        path_file = ""; 
        name = ""; 
        extension = "";
        path = ""; 
        version_id = ""; 
        register_time = ""; 
        register_date = "";
        last_modified_time = "";
        last_modified_date = "";
        exists = false;
        followed = false;
        updated = false;
        register_pos = 0;
        current_version = 0;
        last_version = 0;
    }

    void expand_path_file() {
        char full_path[1024];
        #ifdef WIN32
            GetFullPathName(arg.c_str(), sizeof full_path, full_path, nullptr);
            std::ifstream test(full_path);
            test.close();
            exists = (test ? true : false); 
        #elif LINUX
            try {
                realpath(arg, full_path);
                exists = 1;
            } catch (no_such_file_or_directory e) {
                exists = 0;
            } 
        #endif
        path_file = full_path;
    }

    void get_register_info() {
        std::string reg = "";
        std::ifstream in(REGISTER, std::ios::binary);
        while (reg.find(path_file) == std::string::npos && getline(in, reg)) { if (reg[0] != '#') register_pos++; }
        if (reg.find(path_file) != std::string::npos) {
            std::vector<std::string> t = tokenize(reg, "[,]", false);
            register_time = t[0];
            register_date = t[1];
            followed = true;
            updated = (t[2][0] == '1' ? true : false);
            current_version = std::stoi(t[3]);
            last_version = std::stoi(t[4]);
            last_modified_time = t[5];
            last_modified_date = t[6];
        } else followed = false;
        in.close();
    }

    void get_register_info(int n) {
        std::string reg = "";
        std::ifstream in(REGISTER, std::ios::binary);
        while (getline(in, reg) && register_pos != n) { if (reg[0] != '#') register_pos++; }
        if (!in.eof()) {
            std::vector<std::string> t = tokenize(reg, "[,]", false);
            register_time = t[0];
            register_date = t[1];
            followed = true;
            updated = (t[2][0] == '1' ? true : false);
            current_version = std::stoi(t[3]);
            last_version = std::stoi(t[4]);
            last_modified_time = t[5];
            last_modified_date = t[6];
        } else followed = false;
        in.close();
    }

    void remove_register() {
        std::string line;
        std::ifstream in(REGISTER, std::ios::binary); getline(in, line);
        std::ofstream out(TMP, std::ios::binary);

        for (int n = 1; !in.eof(); n++) {
            if (n != register_pos) {
                out << line + "\n";
                while (getline(in, line) && line[0] == '#') out << line + "\n";
            } else
                while (getline(in, line) && line[0] == '#');
        }

        in.close(); out.close();
        remove(REGISTER.c_str());
        rename(TMP.c_str(), REGISTER.c_str());
    }

    void update_register(std::string ts, std::string version_name, std::string comment) {
        // -- Modified register info ------------------------------------------
        std::string modified_register = register_time + "," + register_date + ",1,"
            + std::to_string(++current_version) + "," + std::to_string(current_version) 
            + "," + ts + "," + path_file + "\n";

        // -- New version register --------------------------------------------
        std::string new_version = ts + ",";
        if (!version_name.empty()) new_version += version_name;
        else new_version += "Version_" + std::to_string(current_version);
        if (!comment.empty()) new_version += "," + comment; 

        std::string line;
        std::ifstream in(REGISTER, std::ios::binary); getline(in, line);
        std::ofstream out(TMP, std::ios::binary);
        for (int i = 1; !in.eof(); i++) {
            if (i != register_pos) {
                out << line + "\n";
                while (getline(in, line) && line[0] == '#') out << line + "\n";
            } else {
                out << modified_register + "\n";
                for (int j = 1; getline(in, line) && line[0] == '#'; j++)
                    if (j <= current_version-1) out << line + "\n";
                out << new_version + "\n";
            }
        }
        
        in.close(); out.close();
        remove(REGISTER.c_str());
        rename(TMP.c_str(), REGISTER.c_str());
    }

    void parse_file() {
        int aux = path_file.find_last_of("/\\");
        path = path_file.substr(0, aux);
        name = path_file.substr(aux+1);
        aux = name.find_last_of('.');
        if (aux != std::string::npos) {
            name = name.substr(0, aux);
            extension = name.substr(aux);
        }
    }

    void get_version_id() {
        std::vector<std::string> t = tokenize(path, "[\\\\/]", true);
        for (auto &i : t) version_id += i[0];
        version_id += "_" + name + extension;
    }

    void backup_file(std::string src_f, std::string dst_f, bool append = false, bool destroy = false) {
        
        std::ifstream src;
        if (!append) src = std::ifstream(src_f, std::ios::binary | std::ios_base::app);
        else src = std::ifstream(src_f, std::ios::binary);
        
        std::ofstream dst(dst_f, std::ios::binary);

        dst << src.rdbuf();
        src.close(); dst.close();
        if (destroy) remove(src_f.c_str());
    }

    void restore_changes(std::string changes) {
        //regex re("[@]");
        sregex_token_iterator begin(changes.begin(), changes.end(), regex("[@]"), {0});
        vector<string> lines;
        remove_copy_if(begin, sregex_token_iterator(), back_inserter(lines), [](std::string const &s) { return s.empty(); });
        
        int i = 0;
        std::string apply;
        std::ifstream sav("version/" + version_id, std::ios::binary);
        std::ofstream file(arg, std::ios::binary);
        for (auto &line : lines) {
            int aux = line.find_first_of(" "), n = stoi(line.substr(0, aux));
            for (; getline(sav, apply) && i != n ; i++) file << apply << "\n";
            line = line.substr(aux + 1);
            
            regex ch("[\\+\\-\\=]");
            sregex_token_iterator s(o2u.begin(), o2u.end(), ch, {-1, 0});
            vector<string> line_changes;
            remove_copy_if(s, sregex_token_iterator(), back_inserter(line_changes), [](std::string const &s) { return s.empty(); });

            for (auto i = line_changes.rbegin(); i != line_changes.rend(); i+=2) {
                int aux = (*i).find_first_of(":"), fst = stoi((*i).substr(0, aux));

                if ((*(i+1))[0] == '-') {
                    apply = apply.substr(0, fst-1);
                    if (aux == -1) apply.erase(fst-1,1);
                    else apply.erase(fst-1, stoi((*i).substr(aux+1)));
                }
                else {
                    string seq = (*i).substr(aux+1);
                    if ((*(i+1))[0] == '+') apply.insert(fst-1, seq);
                    else if ((*(i+1))[0] == '=') apply.replace(fst-1, seq.length(), seq);
                }
            }
        }
    }

    void beautify() {
        std::cout << "> #" << register_pos << " " << (name + extension) << " " << path << " " << register_time << " " << register_time 
                << " " << (updated ? "YES" : "NO") << " " << current_version << " " << last_version << " " << last_modified_time << " " << last_modified_date;
    }

    void print() {
    std::cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";
    std::cout << "|                 FILE INFO                 |\n";
    std::cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";
    std::cout << "> Given arg:\t\t" << arg << "\n";
    std::cout << "> Full path:\t\t" << path_file << "\n";
    std::cout << "> Only path:\t\t" << path << "\n";
    std::cout << "> Only name:\t\t" << name << "\n";
    std::cout << "> Only ext:\t\t" << extension << "\n";
    std::cout << "> Version id:\t\t" << version_id << "\n";
    std::cout << "> Register pos:\t\t" << register_pos << "\n";

    std::cout << "> Exists?\t\t" << (exists ? "YES" : "NO") << "\n";
    std::cout << "> Is followed?\t\t" << (followed ? "YES" : "NO") << "\n";
    std::cout << "> Is updated?\t\t" << (updated ? "YES" : "NO") << "\n";

    std::cout << "> Register time:\t" << register_time << "\n";
    std::cout << "> Register date:\t" << register_date << "\n";
    std::cout << "> Current version:\t" << current_version << "\n";
    std::cout << "> Last version:\t\t" << last_version << "\n";
    std::cout << "> Last update time:\t" << last_modified_time << "\n";
    std::cout << "> Last update date:\t" << last_modified_date << "\n";
    }

public:

    Version(int n) {
        init();
        expand_path_file();
        get_register_info(n);
        parse_file();
        get_version_id();
    }

    Version(std::string arg) : arg(arg) {
        //std::cout << "GETTING FILE. . .\n";
        init();
        expand_path_file();
        get_register_info();
        parse_file();
        get_version_id();
        // print();
    }

    static void Init() {
        struct stat info;
        if(stat("version", &info ) != 0 || info.st_mode & S_IFREG) {
            _mkdir("version");
            std::ofstream out("version/version_register", std::ios::binary);
        }
    }

    void Check() {
        // Sin implementar. Proximamente.
    }

    void Follow() {
        try {
            if (!exists) throw file_not_found(arg);
            if (followed) throw file_already_followed(arg);
        
            backup_file(path_file, "version/" + version_id);
            std::ofstream reg(REGISTER, std::ios::binary | std::ios_base::app);
            reg << timestamp() << ",1,1,1,,," <<  path_file << std::endl;
            reg.close();

        } catch (file_not_found e) {
            std::cerr << e.what() << std::endl;
        } catch (file_already_followed e) {
            std::cerr << e.what() << std::endl;
        }
    }

    static void Log() {
        Version v(0);
        for (int i = 1; v.followed; i++) {
            v.beautify();
            v = Version(i);
        }
    }

    void Remove() {
        try {
            if (!followed) throw file_not_followed(arg);
            delete_register();
            remove(("version/" + version_id).c_str());
            remove(("version/" + version_id + "_reg").c_str());

        } catch (file_not_followed e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void Restore(std::string version) {
        // Sin implementar. Proximamente.
    }

    void Restore(int version) {
        try {
            int src = current_version, dst = version, type = 0;
            if (current_version < version) { 
                if (version > last_version) throw version_not_found(version);
            } else if (current_version > version) {
                src = version; dst = current_version; type = 1;
            } else return;

            std::string otou, utoo;
            std::vector<std::string> changes;
            std::ifstream in("version/" + version_id + "_reg", std::ios::binary);
            for (int i = 2; src != dst; i++){
                if (i == (src+1)) {
                    src++;
                    getline(in, otou);
                    getline(in, utoo);
                    changes.push_back((!type ? otou : utoo));
                }
            }
            in.close();

            auto &b = (!type ? changes.begin() : changes.rbegin());
            auto &e = (!type ? changes.end() : changes.rend());
            for (; b != e; b++) {
                restore_changes(b);
            }

        } catch (version_not_found e) {
            std::cerr << e.what() << std::endl;
        }
        /*
        regex ch("[\\+\\-\\=]");
        sregex_token_iterator s(o2u.begin(), o2u.end(), ch, {-1, 0});
        vector<string> line_changes;
        remove_copy_if(s, sregex_token_iterator(), back_inserter(line_changes), [](std::string const &s) { return s.empty(); });

        cout << "ORIGINAL FILE: " << a << endl;
        string new_b = a;
        for (auto i = line_changes.rbegin(); i != line_changes.rend(); i+=2) {
            int aux = (*i).find_first_of(":"), fst = stoi((*i).substr(0, aux));

            if ((*(i+1))[0] == '-') {
                cout << "ELIMINAR CARACTER(ES) DESDE " << fst;
                new_b = a.substr(0, fst-1);
                if (aux == -1) new_b.erase(fst-1,1);
                else {
                    cout << " A " << stoi((*i).substr(aux+1));
                    new_b.erase(fst-1, stoi((*i).substr(aux+1)));
                }
            }
            else {
                string seq = (*i).substr(aux+1);
                if ((*(i+1))[0] == '+') {
                    cout << "INSERTAR CARACTERES EN " << fst;
                    new_b.insert(fst-1, seq);
                }
                else if ((*(i+1))[0] == '=') {
                    cout << "SUSTITUIR CARACTERES: ";
                    new_b.replace(fst-1, seq.length(), seq);
                } 
            }
            cout << endl << *i << endl;
            cout << "ASPECTO DE LINEA: " << new_b << endl;
        }
        cout << "UPDATED  FILE: " << b << endl;
        */
    }

   void Update(std::string version_name, std::string comment) {
        try {
            if (!followed) throw file_not_followed(arg);
            if (!exists) throw file_not_found(arg);

            std::string ts = timestamp();

            // -- Original file -----------------------------------------------
            std::string a, curr_changes = "";
            std::ifstream iA("version/" + version_id, std::ios::binary);
            std::ofstream oA("tmp_a", std::ios::binary);
            
            // -- Updated file ------------------------------------------------
            std::string b, undo_changes = "";
            std::ifstream iB(arg, std::ios::binary);
            std::ofstream oB("tmp_b", std::ios::binary);
            
            for (int l = 1; !iA.eof() || !iB.eof(); l++) {
                a = b = "";
                getline(iA, a); getline(iB, b);
                // Hay que mirarlo (!!!).
                if (!iA.eof() && a.empty()) a = "<nl>";
                if (!iB.eof() && b.empty()) b = "<nl>";
                // (!!!).

                // -- Cambios actuales ----------------------------------------
                SeqComparator AB(a, b);
                oA << AB.to_string();
                // ------------------------------------------------------------

                // -- Cambios para volver a una version anterior --------------
                SeqComparator BA(b, a);
                oB << BA.to_string();
                // ------------------------------------------------------------
            }
            iA.close(); oA.close();
            iB.close(); oB.close();

            backup_file("tmp_a", "version/" + version_id + "_reg", 1, 1);
            backup_file("tmp_b", "version/" + version_id + "_reg", 1, 1);

            if (!curr_changes.empty() || !undo_changes.empty())
                update_register(ts, version_name, comment);

        } catch (file_not_followed e) {
            std::cerr << e.what() << std::endl;
        } catch (file_not_found e) {
            std::cerr << e.what() << std::endl;
        }
    }
};