/******************************************************************************
 * File: Version.hpp
 * Description: implementation of a version manager storage.
 * Author: Devid Dokash
 * Version: v1.3
 ******************************************************************************
 * Source:
 * https://stackoverflow.com/a/42114477/17824234 (borrar una linea n de un fichero).
 *****************************************************************************/
#pragma once
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <stdio.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "sequence_comparator.hpp"
#include "VersionExceptions.hpp"
#include "utils.hpp"

#ifdef WIN32
    #include <direct.h>
    #include <windows.h>
#else
    #include <stdlib.h>
#endif

class version_storage {
    const int MAX_LENGTH = 250;
    const std::string PATH = ".data/";
    const std::string REGISTER = PATH + ".register";
    const std::string TMP = PATH + "tmp";

    std::string time;
    std::string date;

    struct File {
        std::string path_file = "";
        std::string name = "";
        std::string extension = "";
        std::string path = "";
        std::string version_id = "";
        int register_pos = 0;

        bool exists = false;
        bool followed = false;
        bool updated = false;

        std::string register_time = "";
        std::string register_date = "";
        int current_version = 0;
        int last_version = 0;
        std::string last_modified_time = "";
        std::string last_modified_date = "";

        struct Changes {
            std::string time;
            std::string date;
            std::string name;
            std::string description;

            Changes(std::string time, std::string date, std::string name, std::string description) {
                this->time = time;
                this->date = date;
                this->name = name;
                this->description = description;
            }

            Changes(std::vector<std::string> t) {
                time = t[0];
                date = t[1];
                name = t[2];
                description = t[3];
            }

            void print() {
                std::cout << "\t#[" << time << " " << date << "] " << name 
                    << ": " << (description.empty() ? "<empty>" : description) 
                    << std::endl; 
            }

            std::string to_string() {
                return "# " + time + "," + date + "," + name + "," + description;
            }

            bool operator==(const std::string& b) {
                return !name.compare(b);
            }
        };
        std::vector<Changes> vers;

        // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        // -- Constructors.
        // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        File() {}

        File(std::string path_file, int p, std::string time, std::string date) : path_file(path_file) {
            register_pos = p;
            register_time = time;
            register_date = date;
            current_version = last_version = 1;
            parse_path_file();
            get_version_id();
        }

        File(std::vector<std::string> t, int p) {
            path_file = t[7];
            register_pos = p;
            followed = true;
            updated = (t[2][0] == '1' ? true : false);
            register_time = t[0];
            register_date = t[1];
            current_version = std::stoi(t[3]);
            last_version = std::stoi(t[4]);
            last_modified_time = t[5];
            last_modified_date = t[6];
            parse_path_file();
            get_version_id();  
        }

        void update_file(std::string time, std::string date, std::string name, std::string comment) {
            for (; last_version != current_version; last_version) vers.pop_back();
            if (name.empty()) name = "VERSION_" + std::to_string(current_version+1); 
            vers.push_back(Changes(time, date, name, comment));
            last_modified_time = time; 
            last_modified_date = date;
            current_version++;
            last_version++;
        }

        void update_time(std::string time, std::string date) {
            last_modified_time = time;
            last_modified_date = date;
        }

        // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        // -- Parses file path.
        // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        void parse_path_file() {
            int fof = path_file.find_last_of("/\\");
            path = path_file.substr(0, fof);
            name = path_file.substr(fof + 1);
            fof = name.find_last_of('.');
            if (fof != std::string::npos) {
                extension = name.substr(fof);
                name = name.substr(0, fof);
            }
        }

        // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        // -- Generates file version id.
        // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        void get_version_id() {
            std::vector<std::string> t = tokenize(path, "[\\\\/]", true);
            for (auto &i : t) version_id += i[0];
            version_id += "_" + name + extension;
        }

        // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        // -- To string.
        // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        std::string to_string() {
            std::string f_str = register_time + "," + register_date + ",1," 
                + std::to_string(current_version) + "," 
                + std::to_string(last_version) + "," + last_modified_time 
                + "," + last_modified_date + "," + path_file + "\n";
            for (auto &c : vers) f_str += c.to_string() + "\n"; 
            return f_str;
        }

        // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        // -- Prints file.
        // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        void print() {
            std::cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";
            std::cout << "|                 FILE INFO                 |\n";
            std::cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";
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
            std::cout << "> Versions:\n";
            for (auto &v : vers) v.print(); 
        }

        // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        // -- Beautifies file.
        // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        void beautify() {
            std::cout << std::setw(4) << register_pos   // #.
                << std::setw(15) << version_id          // Id.
                << std::setw(15) << name                // File.
                << std::setw(7)  << (updated ? "YES" : "NO") 
                << std::setw(8)  << current_version     // Current version.
                << std::setw(8)  << last_version        // Last version.
                << path_file << "\n"                    // Path.
                << "  Follow: " << register_time + " " + register_date + "\n"
                << "  Modify: " << last_modified_time + " " + last_modified_date
                << std::endl;
        }

        std::string next_version_file() {
            return ".data/" + version_id + "_" + padding(current_version+1, '0', 5) 
                + "__";
        }

        std::vector<Changes>::iterator find_version(std::string name) {
            return std::find(vers.begin(), vers.end(), name);
        }


        bool operator==(const std::string& b) {
            return !path_file.compare(b);
        }
    };
    int n_files;
    std::vector<File> files;

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- FILE SYSTEM                                                       --
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Checks if file exists.
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    bool exists_file(std::string path_file) {
        struct stat tst;
        if (stat(path_file.c_str(), &tst) != 0 || !(tst.st_mode & S_IFREG)) return false;
        else return true;
    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Backups a file.
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void backup_file(std::string src_f, std::string dst_f, bool append = false, bool destroy = false) {
        std::ifstream src;
        if (!append) src = std::ifstream(src_f, std::ios_base::app);
        else src = std::ifstream(src_f);

        std::ofstream dst(dst_f);
        dst << src.rdbuf();

        src.close(); dst.close();
        if (destroy) remove(src_f.c_str());
    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Expands path-file.
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    std::string expand_path_file(std::string rel_path) {
        char full_path[1024];
        #ifdef WIN32
            GetFullPathName(rel_path.c_str(), sizeof full_path, full_path, nullptr);
        #else
            realpath(rel_path.c_str(), full_path);
        #endif
        return full_path;
    }


    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- REGISTER FUNCTIONS                                                --
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Parses the register file info for later manipulation.
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    std::vector<std::string> tokenize_register(std::string reg) {
        std::vector<std::string> t = tokenize(reg, "[,]", 0);
        for (int i = 8; i < t.size(); i++) t[7] += t[i];
        return t;
    }

    std::vector<std::string> tokenize_version(std::string ver) {
        int fof;
        std::vector<std::string> t;

        ver = ver.substr(2);
        for (int i = 0; i < 3; i++) {
            fof = ver.find_first_of(",");
            t.push_back(ver.substr(0, fof));
            ver = ver.substr(fof+1);
        }
        t.push_back(ver);
        return t;
    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Reads and saves register info.
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void read_register() {
        std::string line = "";
        std::ifstream in(REGISTER);
        while (getline(in, line)) {
            if (line[0] != '#')
                files.push_back(File(tokenize_register(line), ++n_files));
            else 
                files.back().vers.push_back(File::Changes(tokenize_version(line)));
        }
        in.close();
    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Rewrites register info.
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void rewrite_register() {
        std::ofstream out(REGISTER);
        for(auto &i : files) out << i.to_string();
        out.close();
    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Checks if file is followed.
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    std::vector<File>::iterator find_file(std::string arg) {
        return std::find(files.begin(), files.end(), arg);
    }

    void parse_version_timestamp(std::string ts) {
        int fof = ts.find_first_of(",");
        time = ts.substr(0,fof);
        date = ts.substr(fof+2);
    }

    bool valid_desc(std::string desc) {
        return (desc.length() < 150);
    }

    bool valid_name(std::string name) {
        if (name.empty()) return true;
        std::regex re("([A-Z]|[a-z]|_|[0-9])*");
        return std::regex_match(name, re);
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

    std::string pre_analysis(std::string a, std::string b) {
        if (!a.compare(b)) return "=";
        else if (!a.empty() && b.empty()) return "-1:" + std::to_string(a.length());
        else if (!b.empty() && a.empty()) return "+1:" + b;
        return "!";
    }

    void a_link_to_the_past(std::string changes_str, int& line, std::ifstream& sav, std::ofstream& dst) {
        int target = stoi(substr(changes_str, " ").substr(3));
        std::string mods_str = substr(changes_str, " ");
        std::vector<std::string> mods;
        if (mods_str[0] != '!') mods = tokenize(mods_str, ",", 1);

        std::string lwac; // Line with applied changes.
        std::vector<std::string> changes = tokenize(changes_str, "(\\+|\\-|\\=)", 0, 1);
        for (; getline(sav, lwac) && line != target; line++) dst << lwac << "\n";
        if (line != 1 && sav.eof()) lwac = "";
        for (auto c = changes.rbegin(); c != changes.rend(); c += 2) {
            int fof = (*c).find_first_of(":"), fst;
            std::string seq;

            if (fof != std::string::npos) {
                fst = stoi((*c).substr(0, fof))-1;
                seq = (*c).substr(fof + 1);
            } else {
                fst = stoi((*c))-1;
                seq = "1";
            }
  
            if ((*(c + 1))[0] == '-') lwac.erase(fst, stoi(seq) - fst);
            else {
                seq = unparse(mods, seq);
                if ((*(c + 1))[0] == '+') lwac.insert(fst, seq);
                else if ((*(c + 1))[0] == '=') lwac.replace(fst, seq.length(), seq);
            }
        }
        dst << lwac << "\n";
        line++;
    }

public:
    version_storage() : n_files(0) {
        parse_version_timestamp(timestamp());
        read_register();
    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Inits Version System: creates its folder.
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void Init() {
        struct stat v;
        if (stat(PATH.c_str(), &v) != 0 || v.st_mode & S_IFREG) {
            #ifdef WIN32
                _mkdir(PATH.c_str());
            #else
                mkdir(PATH.c_str(), mode_t(0755));
            #endif
            std::ofstream out(REGISTER);
        }
    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Adds new file to the register.
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void Follow(std::string arg) {
        try {
            if (!exists_file(arg)) throw file_not_found(arg);
            if (find_file(expand_path_file(arg)) != files.end()) 
                throw file_already_followed(arg);

            File f(expand_path_file(arg), files.size()+1, date, time);
            files.push_back(f);
            backup_file(arg, PATH + f.version_id);

            rewrite_register();
        } catch (file_not_found e) {
            std::cerr << e.what() << std::endl;
        } catch (file_already_followed e) {
            std::cerr << e.what() << std::endl;
        }
    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Displays followed files or information about one an especific line.
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void Log(std::string arg) {
        try {
            if (arg.empty()) {
                std::cout << std::left
                    << std::setw(4) << "#"
                    << std::setw(15) << "Version id"
                    << std::setw(15) << "File"
                    << std::setw(7) << "Upd*"
                    << std::setw(8) << "Curr*"
                    << std::setw(8) << "Last*"
                    << "Path\n";
                for (auto &i : files) i.beautify();
                std::cout << "\n+-+-+\n"
                    << " *Upd: all changes of file are saved. Not implemented.\n"
                    << " *Curr: current version of file in system.\n"
                    << " *Last: last version of file available in system.\n"
                    << "+-+-+\n";
            } else {
                auto f = find_file(expand_path_file(arg));
                if (f == files.end()) throw file_not_followed(arg);
                (*f).print();
            }
        } catch (file_not_followed e) {
            std::cerr << e.what() << std::endl;
        }
    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Removes the given file of being followed: remove its entry in the 
    // -- register and every file related (changes and sav).
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void Remove(std::string arg) {
        try {
            auto f = find_file(expand_path_file(arg)); 
            if (f == files.end()) throw file_not_followed(arg);
            files.erase(f);
            remove((PATH + f->version_id).c_str());
            for (int v = 2; v <= f->last_version; v++) {
                remove((PATH + f->version_id + "_" + padding(v,'0',5) 
                    + "__").c_str());
            }
            rewrite_register();
        } catch (file_not_followed e) {
            std::cerr << e.what() << std::endl;
        }
    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Restores given file's given version.
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void Restore(std::string arg, int version) {
        try {
            auto f = find_file(expand_path_file(arg));
            if (f == files.end()) throw file_not_followed(arg);
            if (f->current_version == version) return;

            int line = 1;
            std::string changes_file, otou, utoo;
            std::ifstream in(PATH + f->version_id);
            std::ofstream out(arg);

            // -- Later version -----------------------------------
            if (f->current_version < version) {
                if (version > f->last_version) throw version_not_found(version);
                for (; f->current_version < version; f->current_version++) {
                    changes_file = PATH + f->version_id + "_" 
                        + padding(f->current_version+1, '0', 5) + "__";
                    std::ifstream ch(changes_file);
                    for(; getline(ch, otou) && getline(ch, utoo);)
                        a_link_to_the_past(otou, line, in, out);
                    ch.close();
                }
                while(getline(in, changes_file)) out << changes_file << "\n";
            } else {
                for (; f->current_version > version; f->current_version--) {
                    changes_file = PATH + f->version_id + "_" 
                        + padding(f->current_version, '0', 5) + "__";
                    std::ifstream ch(changes_file); 
                    for(; getline(ch, otou) && getline(ch, utoo);)
                        a_link_to_the_past(utoo, line, in, out);
                    ch.close();   
                }
                while(getline(in, changes_file)) out << changes_file << "\n";
            }
            in.close();
            out.close();

            f->update_time(time, date);
            backup_file(arg, PATH + f->version_id);
            rewrite_register();

        } catch (file_not_followed e) {
            std::cerr << e.what() << std::endl;
        } catch (version_not_found e) {
            std::cerr << e.what() << std::endl;
        }

    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Displays the saved content of the given file if followed.
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void See(std::string arg) {
        try {
            auto i = find_file(expand_path_file(arg)); 
            if (i == files.end()) throw file_not_followed(arg);

            char key = '\0';
            std::string line;
            std::ifstream in(PATH + (*i).version_id);
            std::cout << "+-+-+\n";
            while (getline(in, line)) std::cout << line << std::endl;
            std::cout << "+-+-+\n";
        } catch (file_not_followed e) {
            std::cerr << e.what() << std::endl;
        }
    }

    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // -- Updates given file and saves its current changes respect to the saved 
    // -- one.
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void Update(std::string arg, std::string name, std::string description) {
        try {
            auto f = find_file(expand_path_file(arg));
            if (f == files.end()) throw file_not_followed(arg);
            if (!exists_file(arg)) throw file_not_found(arg);
            if (!valid_name(name)) throw version_name_not_valid(name);
            if (f->find_version(name) != f->vers.end()) throw already_named_version(arg, name);
            if (!valid_desc(description)) throw too_long_description();

            bool first_change = true, has_changes = false;
            std::string changes_file = PATH + f->version_id + "_" 
                + padding(f->current_version+1, '0', 5) + "__", aux;
            std::string otou, utoo, parsing;

            // -- Saved file --------------------------------------------------
            int n_a;
            std::string a;
            std::ifstream A(PATH + f->version_id);

            // -- Updated file ------------------------------------------------
            int n_b;
            std::string b;
            std::ifstream B(arg);

            std::ofstream out(changes_file, std::ios_base::app);
            for (int l = 1; !A.eof() || !B.eof(); l++) {
                a = b = otou = utoo = parsing = "";
                
                getline(A, a); 
                a = std::regex_replace(a, std::regex("\\r"), "");
                n_a = (!a.empty() ? std::ceil(double(a.length())/double(MAX_LENGTH)) : 1);

                getline(B, b);
                b = std::regex_replace(b, std::regex("\\r"), "");
                n_b = (!b.empty() ? std::ceil(double(b.length())/double(MAX_LENGTH)) : 1);

                for (int i = 1; i <= std::max(n_a, n_b); i++) {
                    std::string a_a = (i <= n_a ? a.substr((i-1)*MAX_LENGTH, i*MAX_LENGTH) : "");
                    std::string b_b = (i <= n_b ? b.substr((i-1)*MAX_LENGTH, i*MAX_LENGTH) : "");
                    aux = pre_analysis(a_a, b_b);
                    if (aux[0] != '=') {
                        has_changes = true;
                        if (aux[0] != '!') {
                            otou += aux;
                            utoo += pre_analysis(b_b, a_a);
                        } else {
                            sequence_comparator AB(a_a, b_b, true);
                            otou += AB.to_string(aux);
                            if (!aux.empty()) parsing += "," + aux;

                            sequence_comparator BA(b_b, a_a, true);
                            utoo += BA.to_string(aux);
                            if (!aux.empty()) parsing += "," + aux;
                        }
                    }
                }
                if (!otou.empty()) {
                    if (!first_change) out << "\n";
                    else first_change = false;
                    out << ">>@" << l << " " << (!parsing.empty() ? parsing : "!") << " " << otou << "\n";
                }
                if (!utoo.empty()) out << "<<@" << l << " " << (!parsing.empty() ? parsing : "!") << " " << utoo;
            }
            A.close();
            B.close();
            out.close();

            if (has_changes) {
                f->update_file(time, date, name, description);
                backup_file(arg, PATH + f->version_id);
                rewrite_register();
            } else remove(changes_file.c_str());

        } catch (file_not_followed e) { 
            std::cerr << e.what() << std::endl;
        } catch (file_not_found e) {
            std::cerr << e.what() << std::endl;
        } catch (version_name_not_valid e) {
            std::cerr << e.what() << std::endl;
        } catch (already_named_version e) {
            std::cerr << e.what() << std::endl;
        } catch (too_long_description e) {
            std::cerr << e.what() << std::endl;
        }
    }
};