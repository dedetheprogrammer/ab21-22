#include <algorithm>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <time.h>
#include <vector>

#ifdef WIN32
    #include <windows.h>
    #include "color.hpp"
#endif

using namespace std;
/*
 * Encontrar elemento minimo de un vector.
 * https://www.includehelp.com/stl/find-the-minimum-smallest-element-of-a-vector.aspx#:~:text=To%20find%20a%20smallest%20or,element%20between%20the%20given%20range.
 */

enum Operation { INS, DEL, SUB, NON };
vector<vector<Operation>> t;

/*----- UTILS -----*/
//-- Generates a timestamp.
string TimeStamp() {
    time_t t = time(0);
    char time_str[80];
    strftime(time_str, sizeof(time_str), "%X %d-%m-%Y", localtime(&t));
    return string(time_str);
}

//-- Generates new file name with given path and file.
string GenerateFileName(string path, string file) {
    replace(path.begin(), path.end(), '\\', '/');
    istringstream ss(path);
    string token, name = "";

    while(getline(ss, token, '/')) name += token.at(0);
    return name + file.at(0);
}

//-- Copies file.
void SaveFile(string src_file, string dst_file) {
    ifstream src(src_file, ios::binary);
    ofstream dst("version/" + dst_file, ios::binary);
    dst << src.rdbuf();
}

//-- Help message.
void HelpLog() {
    cout << "\nusage -- version <option>\n\n";
    cout << "\tregister <file> ... Registers a new file.\n";
}


/*----- THE FAT THING -----*/
//-- Compares two sequences.
void Sequence_cmp(string a, string b) {
    int c_prev, min, aux;
    vector<int> c; 

    for (int j = 0; j < b.length(); j++) c.push_back(j);
    vector<Operation> v(a.length()-1);
    t = vector<vector<Operation>>(b.length()-1, v);
    for (int i = 1; i < a.length(); i++) {
        for (auto &it : c) cout << it << " ";
        cout << endl; 
        c_prev = c[0];
        for (int j = 1; j < c.size(); j++) {

            min = c[j] + 1;
            aux = c[j-1] + 1;
            t[i-1][j-1] = DEL;
            if (min > aux) {
                min = aux;
                t[i-1][j-1] = INS;
            }

            aux = c_prev + ((a[i] != b[j]) ? 1 : 0);
            if (min > aux) { 
                min = aux;
                t[i-1][j-1] = ((a[i] != b[j]) ? SUB : NON);
            }

            c_prev = c[j];
            c[j] = min;
            

            /*
            x[0] = c[j] + 1;
            x[1] = c[j-1] + 1;
            x[2] = c_prev + ((a[i] != b[j]) ? 1 : 0);

            // cout << "Index i: " << i << ", j: " << j << endl;
            // cout << "Comparing: " << a[i] << " & " << b[i] << endl;
            // cout << "X: " << x[0] << endl;
            // cout << "Y: " << x[1] << endl;
            // cout << "Z: " << x[2] << endl;

            c_prev = c[j];
            auto min = min_element(x.begin(), x.end());
            c[j] = *min;
            switch (min - x.begin()) {
            case 0: 
                t[i-1][j-1] = DEL; break;
            case 1:
                t[i-1][j-1] = INS; break;
            case 2:
                t[i-1][j-1] = (a[i] != b[j] ? SUB : NON); break;
            }*/
        }
        c[0] = i;
    }
    for (auto &it : c) cout << it << " ";
    cout << endl << endl;

    for (auto &v : t) {
        for (auto &it : v){ 
            if (it == INS) cout << "INS ";
            else if(it == DEL) cout << "DEL ";
            else if(it == SUB) cout << "SUB ";
            else cout << "NON ";
        }
        cout << endl;
    }
}

//-- Gets the solution.
void Resolve(int i, int j) {
    if (i == 0) for (int k = 1; k <= j; k++) cout << "Insertar" << endl;
    else if(j == 0) for(int k = 1; k <= i; k++) cout << "Borrar" << endl;
    else {
        cout << "i: " << i << ", j: " << j << endl;
        Operation op = t[i-1][j-1];
        if (op == INS) { cout << "Insertar" << endl; Resolve(i, j-1); }
        else if (op == DEL) { cout << "Borrar" << endl; Resolve(i-1, j); }
        else if (op == SUB) { cout << "Sustituir" << endl; Resolve(i-1, j-1); }
        else { cout << "Nada" << endl; Resolve(i-1, j-1); }
    }
}

//-- Compare two files.
void CompareFiles(string path_file) {

}


/*---- FILE STATUS -----*/
//-- Obtains 'file' basename.
string GetFileName(string file) {
    return file.substr(file.find_last_of("\\/") + 1);
}

//-- Recovers 'file' full path.
string GetFileFullPath(string file_path) {
    char path[1024];
    GetFullPathName(file_path.c_str(), 1024, path, nullptr);

    string full_path(path);
    return full_path.substr(0, full_path.find_last_of("\\/"));
}

//-- Checks if given file exists.
bool CheckFileExistance(string file_path) {
    ifstream test(file_path);
    test.close();
    return (test) ? true : false;
}

//-- Checks if given file is registered or not.
bool CheckFileRegister(string path, string file) {
    ifstream in("version/version_register", ios::binary);
    string line, rPath, rFile = "";
    while ((path.compare(rPath) != 0 || file.compare(rFile) != 0) && getline(in, line)) {
        stringstream ss(line);
        line = line.substr(line.find_first_of(" ") + 1);
        line = line.substr(line.find_first_of(" ") + 1);
        rPath = line.substr(0, line.find_last_of(" "));
        rFile = line.substr(line.find_last_of(" ") + 1);
    }
    in.close();
    return (path.compare(rPath) == 0 && file.compare(rFile) == 0) ? true : false;
}


/*----- REGISTER -----*/
//-- Register new file.
void RegisterFile(string file_path) {
    if (!CheckFileExistance(file_path)) {
        cout << "\nError -- file \'" << file_path << "\' does not exists.\n";
        return;
    }

    string file = GetFileName(file_path);
    string path = GetFileFullPath(file_path);
    if (CheckFileRegister(path, file)) {
        cout << "\nWarning -- file \'" << file << "\' under path \'" 
            <<  path << "\' was already registered.\n";
        return;
    }

    string reg_file = GenerateFileName(path, file);
    string ts = TimeStamp();

    SaveFile(file_path, reg_file);
    ofstream ver("version/" + reg_file + ".ver", ios::binary | ios_base::app);
    ver << TimeStamp() << " -- BASE VERSION --\n";
    ver.close();

    cout << "Registering file \'" << file << "\'...\n";
    ofstream out("version/version_register", ios::binary | ios_base::app);
    out << TimeStamp() << " " << path << " " << file << endl;
    out.close();
}

/*----- UPDATE -----*/
void UpdateFile(string file_path) {
    string file = GetFileName(file_path);
    string path = GetFileFullPath(file_path);
    if (!CheckFileRegister(path, file)) {
        cout << "\nwarning -- file \'" << file_path << "\'not registered.\n";
        return;
    }
    if (!CheckFileExistance(file_path)) {
        cout << "\nError -- file \'" << file_path << "\' not found. Did you removed it?\n";
        return;
    }

}

/*----- MAIN -----*/
int main(int argc, char* argv[]) {
    /*switch(argc-1) {
    case 0: case 1:
        HelpLog();
        break;
    case 2:
        if (strcmp(argv[1], "register") == 0) RegisterFile(argv[2]);
        else if (strcmp(argv[1], "update") == 0) UpdateFile(argv[2]);
        else {
            cerr << "Error -- unknown option \'" << argv[1] << "\'\n\n";
            HelpLog();
        }
        break;
    }*/
    Sequence_cmp("abbc","babb");
    Resolve(3,3);
}