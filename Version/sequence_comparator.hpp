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

class sequence_comparator {
private:

    bool compressed_mode = false;
    bool mode = false;
    struct action {
        Operation op;
        std::string seq;
        int begin, end;
        bool wiped;


        action(): op(NON), begin(0), end(0) {}
        action(Operation op, char c, int n) : op(op), begin(n), end(n) {
            seq = (c != '\0' ? std::string(1, c) : "");
            wiped = false;
        }

        std::string to_string() {
            wiped = true;
            std::string s = Names[op] + std::to_string(begin);
            if (op == DEL) { if(begin != end) s += ":" + std::to_string(end); }
            else s += ":" + seq;
            return s;
        }
    } a;

    std::string pa_str;
    std::string ch_str;

    std::string A, B;
    std::vector<int> c;
    std::vector<std::vector<Operation>> t;

    void compare() {
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
    }
    */

    void resolve(int i, int j) {
        if(!i && !j) return;
        else if(!i) { resolve(0, j-1); solution(INS, B[j-1], i+1); }
        else if(!j) { resolve(i-1, 0); solution(DEL, '\0', i); }
        else if(t[i-1][j-1] == DEL) { resolve(i-1, j); solution(DEL, '\0', i); }
        else if(t[i-1][j-1] == INS) { resolve(i, j-1); solution(INS, B[j-1], i+1); }
        else if(t[i-1][j-1] == SUB) { resolve(i-1, j-1); solution(SUB, B[j-1], i); }
        else resolve(i-1, j-1);
    }

    void write(Operation op, char c, int n) {
        std::string seq = (c != '\0' ? std::string(1,c) : "");
        if (!mode) std::cout << Names[op] << seq << n << " ";
        else ch_str += Names[op] + seq + std::to_string(n) + " ";
    }

    void solution(Operation op, char c, int n) {
        if (compressed_mode) {
            if (a.op == NON) a = action(op, c, n);
            else if (a.op != op) {
                ch_str += a.to_string();
                a = action(op, c, n);
            }
            else if (op == DEL) {
                if (n - a.end != 1) {
                    ch_str += a.to_string();
                    a = action(DEL, c, n);
                } else a.end++;
            } else if (op == INS) {
                if (n - a.begin) {
                    ch_str += a.to_string();
                    a = action(INS, c, n);
                } else a.seq += parse(a.end - a.begin, c);
            } else if (op == SUB) {
                if (n - a.end != 1) {
                    ch_str += a.to_string();
                    a = action(SUB, c, n);
                } else { a.end++; a.seq += parse(a.end - a.begin, c); }
            }
        } else {
            write(op, c, n);
        }
    }

    std::string parse(int i, char c) {
        if (c == '-') { pa_str += std::to_string(i) + ","; return "<DEL>"; }
        else if(c == '+') { pa_str += std::to_string(i) + ","; return "<ADD>"; }
        else if(c == '=') { pa_str += std::to_string(i) + ","; return "<SUB>"; }
        else return std::string(1,c);
    }

public:
    sequence_comparator(std::string A, std::string B, bool compress) : A(A), B(B) {
        compressed_mode = compress;

        // -- Initiating costs table ------------------------------------------
        for (int j = 0; j <= B.length(); j++) { c.push_back(j); }
        // --------------------------------------------------------------------

        // -- Initiating actions table ----------------------------------------
        t = std::vector<std::vector<Operation>>(A.length(), 
                std::vector<Operation>(B.length()));
        // --------------------------------------------------------------------
    }

    void print() {
        compare();
        resolve(A.length(), B.length());
        std::cout << "\n";
    }

    std::string to_string(std::string& parsing) {
        mode = true;
        pa_str = "";
        compare();
        resolve(A.length(), B.length());

        parsing = pa_str;
        ch_str += (!a.wiped ? a.to_string() : "");
        return ch_str;
    }
};