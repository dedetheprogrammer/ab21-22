/******************************************************************************
 * 
 ******************************************************************************
 * Source:
 * https://stackoverflow.com/a/42114477/17824234 (borrar una linea n de un fichero).
 *****************************************************************************/
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <stdio.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "SeqComparator.hpp"
#include "VersionExceptions.hpp"

#ifdef WIN32
    #include <direct.h>
    #include <windows.h>
#else
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
        time_t t; std::time(&t);
        char ts[256];
        std::strftime(ts, sizeof ts, "%X %Z,%d-%m-%Y", std::localtime(&t));
        return ts;
    }

    static std::vector<std::string> tokenize(std::string scope, const std::string patron, bool extrict, bool keep = false) {
        std::regex re(patron);
        if (keep) {
            std::sregex_token_iterator s(scope.begin(), scope.end(), re, {-1,0});
            std::vector<std::string> t;
            remove_copy_if(s, std::sregex_token_iterator(), back_inserter(t), 
                    [](std::string const &s) { return s.empty(); });
            return t;
        } else {
            std::sregex_token_iterator s(scope.begin(), scope.end(), re, -1), e;
            if (extrict) {
                std::vector<std::string> t;
                remove_copy_if(s, std::sregex_token_iterator(), back_inserter(t), 
                        [](std::string const &s) { return s.empty(); });
                return t;
            } else return {s, e};
        }
    }

    static std::string padding(int i, char c, int n) {
        std::string s = std::to_string(i);
        return std::string(n-s.length(), c).append(s);
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
        #else
            realpath(arg.c_str(), full_path);
        #endif
        path_file = full_path;
        struct stat info;
        if (stat(full_path, &info) != 0 || !(info.st_mode & S_IFREG)) 
            exists = 0;
        else
            exists = 1;
    }

    static std::vector<std::string> tokenize_register_info(std::string reg) {
        std::vector<std::string> t = tokenize(reg, "[,]", 0);
        for (int i = 8; i < t.size(); i++) t[7] += t[8];
        return t;
    }

    void get_register_info() {
        std::string reg = "";
        std::ifstream in(REGISTER, std::ios::binary);
        while (reg.find(path_file) == std::string::npos && getline(in, reg)) { 
            if (reg[0] != '#') register_pos++; 
        }
        if (reg.find(path_file) != std::string::npos) {
            std::vector<std::string> t = tokenize_register_info(reg);
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
        while (register_pos != n) 
            if (reg[0] != '#') { getline(in, reg); register_pos++; }
        if (register_pos == n && !reg.empty()) {
            std::vector<std::string> t = tokenize_register_info(reg);
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
        current_version++;
        std::string modified_register = register_time + "," + register_date + ",1,"
            + std::to_string(current_version) + "," + std::to_string(current_version) 
            + "," + ts + "," + path_file;

        // -- New version register --------------------------------------------
        std::string new_version = "# " + ts + ",";
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
                    if (j < current_version) out << line + "\n";
                out << new_version + "\n";
            }
        }
        
        in.close(); out.close();
        remove(REGISTER.c_str());
        rename(TMP.c_str(), REGISTER.c_str());
    }

    void restore_register(std::string ts) {
        // -- Modified register info ------------------------------------------
        current_version++;
        std::string modified_register = register_time + "," + register_date + ",1,"
            + std::to_string(current_version) + "," + std::to_string(current_version) 
            + "," + ts + "," + path_file;

        std::string line;
        std::ifstream in(REGISTER, std::ios::binary); getline(in, line);
        std::ofstream out(TMP, std::ios::binary);
        for (int i = 1; !in.eof(); i++) {
            if (i != register_pos) out << line + "\n";
            else out << modified_register + "\n";
            while (getline(in, line) && line[0] == '#') out << line + "\n";
        }
        
        in.close(); out.close();
        remove(REGISTER.c_str());
        rename(TMP.c_str(), REGISTER.c_str());
    }

    void parse_path_file() {
        int aux = path_file.find_last_of("/\\");
        path = path_file.substr(0, aux);
        name = path_file.substr(aux+1);
        aux = name.find_last_of('.');
        if (aux != std::string::npos) {
            name = name.substr(0, aux);
            extension = name.substr(aux);
        }
    }

    static void parse_path_file(std::string path_file, std::string* path, 
            std::string* name, std::string* ext) {
        int aux = path_file.find_last_of("/\\");
        *path = path_file.substr(0, aux);
        *name = path_file.substr(aux+1);
        aux = (*name).find_last_of('.');
        if (aux != std::string::npos && ext != nullptr) {
            *name = (*name).substr(0, aux);
            *ext = (*name).substr(aux);
        }
    }

    void get_version_id() {
        std::vector<std::string> t = tokenize(path, "[\\\\/]", true);
        for (auto &i : t) version_id += i[0];
        version_id += "_" + name + extension;
    }

    static std::string get_version_id(std::string path, std::string name) {
        std::string version_id = "";
        std::vector<std::string> t = tokenize(path, "[\\\\/]", true);
        for (auto &i : t) version_id += i[0];
        return version_id + "_" + name;
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
    Version() {
        init();
    }

    Version(int n) {
        init();
        expand_path_file();
        get_register_info(n);
        parse_path_file();
        get_version_id();
        //print();
    }

    Version(std::string arg) : arg(arg) {
        init();
        expand_path_file();
        get_register_info();
        parse_path_file();
        get_version_id();
        print();
    }

    static void Init() {
        struct stat info;
        if(stat("version", &info ) != 0 || info.st_mode & S_IFREG) {
            #ifdef WIN32
                _mkdir("version");
            #else
                //mode_t mode = 0755;
                mkdir("version", mode_t(0755));
            #endif
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
        std::string reg = "";
        std::ifstream in("version/version_register", std::ios::binary);
        std::cout << std::left
            << std::setw(4) << "#"
            << std::setw(15) << "Version id"
            << std::setw(15) << "File"
            << std::setw(7) << "Upd*"
            << std::setw(8) << "Curr*"
            << std::setw(8) << "Last*"
            << "Path\n";

        for (int i = 1; getline(in, reg); ) { 
            if (reg[0] != '#') {
                std::string name;
                std::vector<std::string> t = tokenize_register_info(reg);
                parse_path_file(t[7], &t[7], &name, nullptr);
                std::cout << std::setw(4) << i                          // #.
                    << std::setw(15) << get_version_id(t[7], name)      // Id.
                    << std::setw(15) << name                            // File.
                    << std::setw(7)  << (t[2][0] == '1' ? "YES" : "NO") 
                    << std::setw(8)  << t[3]    // Current version.
                    << std::setw(8)  << t[4]    // Last version.
                    << t[7] << "\n"             // Path.
                    << "  Follow: " << t[0] + " " + t[1] + "\n"
                    << "  Modify: " << t[5] + " " + t[6] + "\n";
                i++;
            }
        }
        std::cout << "\n--\n"
            << "*Upd: Update. all changes of file are saved. Not implemented.\n"
            << "*Curr: Current version. Current version of file in system.\n"
            << "*Last: Last version. Last version of file available in system.\n"
            << "\n--\n";
        in.close();
    }

    void Remove() {
        try {
            if (!followed) throw file_not_followed(arg);
            remove_register();
            remove(("version/" + version_id).c_str());
            remove(("version/" + version_id + "_" + 
                padding(current_version, '0', 5) + "__").c_str());
        } catch (file_not_followed e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void Restore(std::string version) {
        // Sin implementar. Proximamente.
    }


    std::string substr(std::string& s, std::string pattern) {
        int fof = s.find_first_of(pattern);
        if (fof != std::string::npos) {
            std::string sub = s.substr(0, fof);
            s = s.substr(fof+1);
            return sub;
        } else return "";
    }

    std::string unparse(std::vector<std::string> mods, std::string seq) {
        if (mods.size()) {
            int fof;
            std::string del = "<DEL>", add = "<ADD>", sub = "<SUB>";
            for (int i = 0; stoi(mods[i]) < seq.length(); i++) {
                fof = seq.find(del);
                if (fof != std::string::npos) {
                    mods.erase(mods.begin() + i);
                    seq.replace(fof, del.length(), "-");
                } else {
                    fof = seq.find(add);
                    if (fof != std::string::npos) {
                        mods.erase(mods.begin() + i);
                        seq.replace(fof, add.length(), "-");
                    } else {
                        fof = seq.find(sub);
                        if (fof != std::string::npos) {
                            mods.erase(mods.begin() + i);
                            seq.replace(fof, sub.length(), "-");
                        }
                    }
                }
            }
        }
        return seq;
    }

    void a_link_to_the_past(std::string changes_str, int& line, std::ifstream& sav, std::ofstream& dst) {
        // Los preliminares.
        int target = stoi(substr(changes_str, " ").substr(3));
        std::string mods_str = substr(changes_str, " ");
        std::vector<std::string> mods;
        if (mods_str[0] != '!') mods = tokenize(mods_str, ",", 1);

        std::string lwac; // Line with applied changes.
        std::vector<std::string> changes = tokenize(changes_str, "[\\+\\-\\=]", 0, 1);
        for (; getline(sav, lwac) && line != target; line++) dst << lwac << "\n";
        if (sav.eof()) lwac = "";
        for (auto c = changes.rbegin(); c != changes.rend(); c += 2) {
            int fof = (*c).find_first_of(":"), fst = stoi((*c).substr(0, fof));
            if ((*(c + 1))[0] == '-') {
                lwac = lwac.substr(0, fst - 1);
                if (fof == -1) lwac.erase(fst - 1, 1);
                else lwac.erase(fst - 1, stoi((*c).substr(fof + 1)));
            } else {
                std::string seq = unparse(mods, (*c).substr(fof + 1));
                if ((*(c + 1))[0] == '+') lwac.insert(fst - 1, seq);
                else if ((*(c + 1))[0] == '=') lwac.replace(fst - 1, seq.length(), seq);
            }
            dst << lwac << "\n";
        }
    }

    void Restore(int version) {
        try {
            int line = 0;
            std::string changes_file, otou, utoo, ts = timestamp();
            std::ifstream sav("version/" + version_id, std::ios::binary);
            std::ofstream dst("arg", std::ios::binary);

            // -- Si se quiere recuperar una version posterior.
            if (version > last_version) throw version_not_found(version);
            for (; current_version < version; current_version++) {
                changes_file = "version/" + version_id + "_" + 
                    padding(current_version, '0', 5) + "__";
                std::ifstream in(changes_file, std::ios::binary);
                for (; getline(in, otou) && getline(in, utoo); )
                    a_link_to_the_past(otou, line, sav, dst);
                in.close();
            }
 
            // -- Si se quiere recuperar una version anterior.
            for (; current_version > version; current_version--) {
                changes_file = "version/" + version_id + "_" + 
                    padding(current_version, '0', 5) + "__";
                std::ifstream in(changes_file, std::ios::binary);
                for (; getline(in, otou) && getline(in, utoo); )
                    a_link_to_the_past(utoo, line, sav, dst);
                in.close();
            }
            restore_register(ts);
            backup_file(arg, "version/" + version_id);

        } catch (version_not_found e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void See() {

    }

    void Update(std::string version_name, std::string comment) {
        try {
            if (!followed) throw file_not_followed(arg);
            if (!exists) throw file_not_found(arg);

            bool first_change = true;
            std::string changes_file = "version/" + version_id + "_" + 
                padding(current_version+1, '0', 5) + "__", ts = timestamp(), aux;

            // -- Saved file -----------------------------------------------
            std::string a;
            std::ifstream A("version/" + version_id, std::ios::binary);
            
            // -- Updated file ------------------------------------------------
            std::string b;
            std::ifstream B(arg, std::ios::binary);
            
            std::ofstream out(changes_file, std::ios::binary | std::ios_base::app);
            for (int l = 1; !A.eof() || !B.eof(); l++) {
                a = b = "";
                getline(A, a); getline(B, b);
                // Hay que mirarlo (!!!).
                a = std::regex_replace(a, std::regex("\\r"), "");
                b = std::regex_replace(b, std::regex("\\r"), "");
                if (!A.eof() && a.empty()) a = "<nl>";
                if (!B.eof() && b.empty()) b = "<nl>";
                // (!!!).

                // -- Cambios actuales ----------------------------------------
                SeqComparator AB(a, b);
                aux = AB.to_string();
                if (!aux.empty()) {
                    if (first_change) first_change = 0;
                    out << ">>@" << l << " " << aux << std::endl;   
                }
                // ------------------------------------------------------------

                // -- Cambios para volver a una version anterior --------------
                SeqComparator BA(b, a);
                aux = BA.to_string();
                if (!aux.empty()) out << "<<@" << l << " " << aux << std::endl;
                // ------------------------------------------------------------
            }
            A.close();
            B.close();
            out.close();

            if (!first_change) {
                update_register(ts, version_name, comment);
                backup_file(arg, "version/" + version_id);
            } else remove(changes_file.c_str());
            
        } catch (file_not_followed e) {
            std::cerr << e.what() << std::endl;
        } catch (file_not_found e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void operator=(const Version& v) {
        this->path_file = v.path_file; 
        this->name = v.name; 
        this->extension = v.extension;
        this->path = v.path; 
        this->version_id = v.version_id; 
        this->register_time = v.register_time; 
        this->register_date = v.register_date;
        this->last_modified_time = v.last_modified_time;
        this->last_modified_date = v.last_modified_date;
        this->exists = v.exists;
        this->followed = v.followed;
        this->updated = v.updated;
        this->register_pos = v.register_pos;
        this->current_version = v.current_version;
        this->last_version = v.last_version;
    }
};