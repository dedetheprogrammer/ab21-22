#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;


struct node {
    node* father;
    char me;
    int freq;
};

node* heap(vector<pair<char,int>> freqs){


}

node* recursive_heap(vector<pair<char,int>> freqs, node* current){}

bool sortbysec(const pair<int,int> &a, const pair<int,int> &b) {
    return (a.second < b.second);
}

int main(int argc, char *argv[]) {
    char ch;

    vector<pair<char,int>> freqs;
    fstream fin(argv[1], fstream::in);
    while (fin >> noskipws >> ch) {
        auto it = find_if( freqs.begin(), freqs.end(), 
            [&ch](const std::pair<char,int>& elem){ return elem.first == ch;} );
        if (it != freqs.end()) it->second++;
        else freqs.push_back(make_pair(ch,1));
    }
    sort(freqs.begin(), freqs.end(), sortbysec);
    cout << "CARACTERES Y SUS FRECUENCIAS:\n";
    for (auto itr : freqs) cout << itr.first << " --> " << itr.second << endl;
}