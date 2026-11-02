#include <bits/stdc++.h>
using namespace std;

struct Estado {
    string nome; // nome do estado q pode ser S, A,[A,H]
    bool final = false; // se o estado eh final equivale ao * da tablea
    map<char, set<int>> transicoes; //vao ser mias setas q saem do estado
};

vector<Estado> afnd; // lista de estados, entao afnd[0] eh o estado S (incial)
string alfabeto;
int contador = 0;

int main() {
    Estado s;
    s.nome = "S";
    s.transicoes['s'].insert(1);
    s.transicoes['s'].insert(3);
    afnd.push_back(s);

    cout << afnd[0].nome << " tem " << afnd[0].transicoes['s'].size() << " destinos com a letra s\n";

    return 0;

}