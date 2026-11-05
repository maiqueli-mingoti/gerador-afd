#include <bits/stdc++.h>
using namespace std;

struct Estado {
    string nome; // nome do estado na tabela tipo s ou a
    bool final = false; // se eh final equivale ao * da tabel
    map<char, set<int>> transicoes; // setas q saem do estado
};

vector<Estado> afnd; // lista de estados e a posicao 0 eh o estado inicial
string alfabeto; // letras usadas q vao virar as colunas da tabela
int contador = 0; // qnts nomes ja foram criados

string novoNome() {
    // soma 1 pq vamos criar mais um nome
    contador++;

    // s eh a letra 19 e esse nome eh do estado inicial entao pula pro t
    if (contador == 19) {
        contador++;
    }

    // nome comeca vazio e as letras entram no while
    string nome = "";

    // copia do contador pq o i muda no while e o contador n pode mudar
    int i = contador;

    while (i > 0) {
        // acha a posicao da letra no alfabeto onde 0 eh a e 25 eh z
        int resto = (i - 1) % 26;

        char letra = 'A' + resto;

        nome = letra + nome;

        // se n faltar letra o i vira 0 e o while para
        i = (i - 1) / 26;
    }

    return nome;
}

int novoEstado() {
    Estado e;
    e.nome = novoNome();
    afnd.push_back(e);
    int posicao = afnd.size() - 1; // correçao de posicao
    return posicao;
}

int main() {
    Estado inicial;
    inicial.nome = "S";
    afnd.push_back(inicial);
    for (int i = 0; i < 30; i++) {
        novoEstado();
    }
    for (Estado e : afnd) {
        cout << e.nome << " ";
    }
    cout << "\n";
    return 0;
}