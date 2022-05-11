/* ****************************************************************************
 * Author: Devid Dokash.
 * Date: 03/03/2022.
 * ****************************************************************************
 * Overloading operators << and >>:
    https://www.tutorialspoint.com/cplusplus/input_output_operators_overloading.htm
 * ***************************************************************************/
#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

enum Operation { INS, DEL, SUB, NON };
std::map<Operation,std::string> Names {
    {INS, "+"}, {DEL, "-"}, {SUB, "="}, {NON, ""}
};
bool verbose = false;

class SeqComparator {
private:

    int line;
    std::string A, B;
    std::vector<int> c;
    std::vector<std::vector<Operation>> t;
    struct change {
        Operation op;
        int begin, end;

        change() : op(NON), begin(0), end(0) {}
        change(Operation op, int n) : op(op), begin(n), end(n) {}
    } s;

    bool mode;
    std::string changes;

    bool Analysis() {
        if (!A.compare(B)) return 1;
        else if (!A.compare("<nl>")) {
            Save(!B.length() ? "-<nl>" : "+1:" + B);
            return 1;
        } else if (!B.compare("<nl>")) {
            Save(!A.length() ? "+<nl>" : "-1:" + std::to_string(A.length()));
            return 1;
        } else if (A.length() && !B.length()) {
            Save("-1:" + std::to_string(A.length()));
            return 1;
        } else if (B.length() && !A.length()) {
            Save("+1:" + B);
            return 1;
        }
        return 0;
    }

    void Compare() {
        int c_prev, tmp, aux;

        for (int i = 1; i <= A.length(); i++) {
            c_prev = c[0];
            c[0] = i;
            for (int j = 1; j < c.size(); j++) {
                // -- X -------------------------------------------------------
                tmp = c[j] + 1;
                t[i-1][j-1] = DEL;
                // ------------------------------------------------------------

                // -- Y -------------------------------------------------------
                aux = c[j-1] + 1;
                if (tmp > aux) { tmp = aux; t[i-1][j-1] = INS; }
                // ------------------------------------------------------------

                // -- Z -------------------------------------------------------
                aux = c_prev + (A[i-1] != B[j-1] ? 1 : 0);
                if (tmp > aux) { tmp = aux; t[i-1][j-1] = (A[i-1] != B[j-1] ? SUB : NON); }
                // ------------------------------------------------------------

                // -- C[i][j] = min(x, y, z) ----------------------------------
                c_prev = c[j];
                c[j] = tmp;
                // ------------------------------------------------------------
            }
        }
    }

    /*
    void Resolve(int i, int j) {
        if(!i && !j) return;
        else if(!i) { Resolve(0, j-1); std::cout << "+" << B[j-1] << i << " "; }
        else if(!j) { Resolve(i-1, 0); std::cout << "-" << i << " "; }
        else {
            switch(t[i-1][j-1]) {
            case DEL: Resolve(i-1, j); std::cout << "-" << i << " "; break;
            case INS: Resolve(i, j-1); std::cout << "+" << B[j-1] << i << " "; break;
            case SUB: Resolve(i-1, j-1); std::cout << "=" << B[j-1] << i << " "; break;
            case NON: Resolve(i-1, j-1);
            }
        }
    }*/

    void Resolve(int i, int j) {
        if(!i && !j) return;
        else if(!i) { Resolve(0, j-1); Write(INS, B[j-1], i+1); }
        else if(!j) { Resolve(i-1, 0); Write(DEL, '\0', i); }
        else if(t[i-1][j-1] == DEL) { Resolve(i-1, j); Write(DEL, '\0', i); }
        else if(t[i-1][j-1] == INS) { Resolve(i, j-1); Write(INS, B[j-1], i+1); }
        else if(t[i-1][j-1] == SUB) { Resolve(i-1, j-1); Write(SUB, B[j-1], i); }
        else Resolve(i-1, j-1);
    }

    void Write(Operation op, char c, int n) {
        if (s.op != op) {
            if (s.op != NON && s.op == DEL && s.begin != s.end) 
                Save(":" + std::to_string(s.end));
            s = change(op, n);
            Save(Names[op] + std::to_string(n));
            if (op != DEL) Save(":" + std::string(1, c));
        } else if (op == DEL) {
            if (n - s.end != 1) {
                if (s.begin != s.end) Save(":" + std::to_string(s.end));
                Save("-" + std::to_string(n));
                s = change(DEL, n);
            } else s.end++;
        } else if (op == INS) {
            if (n - s.begin) {
                Save("+" + std::to_string(n) + ":" + std::string(1, c));
                s = change(INS, n);
            } else Save(std::string(1, c));
        } else {
            if (n - s.end != 1) {
                Save("=" + std::to_string(n) + ":" + std::string(1, c));
                s = change(SUB, n);
            } else { s.end++; Save(std::string(1, c)); }
        }
    }

    void Save(std::string change) {
        if (!mode) std::cout << change;
        else changes += change;
    }

public:
    SeqComparator(std::string A, std::string B) : A(A), B(B) {
        // -- Initiating costs table ------------------------------------------
        for (int j = 0; j <= B.length(); j++) { c.push_back(j); }
        // --------------------------------------------------------------------

        // -- Initiating actions table ----------------------------------------
        t = std::vector<std::vector<Operation>>(A.length(), 
                std::vector<Operation>(B.length()));
        // --------------------------------------------------------------------
    }


    void print() {
        mode = 0;
        // -- Resolve sequence comparison -------------------------------------
        if (!Analysis()) {
            Compare();
            Resolve(A.length(), B.length());
        }
        // --------------------------------------------------------------------
        std::cout << "\n";
    }

    std::string to_string() {
        mode = 1;
        changes = "";
        // -- Resolve sequence comparison -------------------------------------
        if (!Analysis()) {
            Compare();
            Resolve(A.length(), B.length());
        }
        // --------------------------------------------------------------------
        return changes;
    }


};