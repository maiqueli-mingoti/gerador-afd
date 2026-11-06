#include <bits/stdc++.h>
using namespace std;

struct Estado {
    string nome; // nome do estado na tabela tipo s ou a
    bool final = false; // se eh final equivale ao * da tabel
    set<string> tokens; // qual palavra reconhece no final
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

void criaSeta(int origem, char letra, int destino) {
    if (alfabeto.find(letra) == string:: npos) { // procura a letra se n cria nova
        alfabeto = alfabeto + letra;
    }
    afnd[origem].transicoes[letra].insert(destino);
}

void adicionarToken(string token) {
    int atual = 0;
    for (char letra : token) {
        int novo = novoEstado();
        criaSeta(atual, letra, novo);
        atual = novo;
    }
    afnd[atual].final = true;
    afnd[atual].tokens.insert(token);
}


map<string, int> mapaGR;

// funcao pra achar o numero do estado da gramatica
int pegarEstadoGR(string nomeGR) {
    if (nomeGR == "<S>") {
        return 0; // O S eh sempre o inicial q ta na posicao 0
    }
    // se nao existe ainda, cria um novo
    if (mapaGR.count(nomeGR) == 0) {
        mapaGR[nomeGR] = novoEstado();
    }
    return mapaGR[nomeGR];
}

// funcao pra processar a gramatica
void processarGramatica(string linha) {
    // tira todos os espacos 
    string linhaLimpa = "";
    for (int i = 0; i < linha.length(); i++) {
        if (linha[i] != ' ') {
            linhaLimpa += linha[i];
        }
    }

    // acha onde ta o "::=" e separa os lados
    int pos = linhaLimpa.find("::=");
    if (pos == -1) return; // se nao achou, ignora a linha

    string origem = linhaLimpa.substr(0, pos); // pega tudo antes do ::=
    string regras = linhaLimpa.substr(pos + 3); // pega tudo depois do ::=

    // vai ler letra por letra pra separar nos '|'
    string pedaco = "";
    for (int i = 0; i < regras.length(); i++) {
        if (regras[i] == '|') {
            // achou o pipe, entao o "pedaco" ta pronto pra virar seta
            if (pedaco == "\\epsilon") {
                int idOrigem = pegarEstadoGR(origem);
                afnd[idOrigem].final = true;
            } else {
                int idOrigem = pegarEstadoGR(origem);
                char letra = pedaco[0]; // a primeira coisa eh a letra
                string nomeDestino = pedaco.substr(1); // o resto 
                
                int idDestino;
                if (nomeDestino == "") { // se a regra for so uma letra tipo "a"
                    idDestino = novoEstado();
                    afnd[idDestino].final = true;
                } else {
                    idDestino = pegarEstadoGR(nomeDestino);
                }
                criaSeta(idOrigem, letra, idDestino);
            }
            pedaco = ""; // zera o pedaco pra ler o proximo
        } else {
            pedaco += regras[i]; // vai juntando as letras
        }
    }

    // salva o ultimo pedaco que sobrou depois do ultimo '|'
    if (pedaco != "") {
        if (pedaco == "\\epsilon") {
            int idOrigem = pegarEstadoGR(origem);
            afnd[idOrigem].final = true;
        } else {
            int idOrigem = pegarEstadoGR(origem);
            char letra = pedaco[0];
            string nomeDestino = pedaco.substr(1);
            
            int idDestino;
            if (nomeDestino == "") {
                idDestino = novoEstado();
                afnd[idDestino].final = true;
            } else {
                idDestino = pegarEstadoGR(nomeDestino);
            }
            criaSeta(idOrigem, letra, idDestino);
        }
    }
}


int main() {
    Estado inicial;
    inicial.nome = "S";
    afnd.push_back(inicial);
    adicionarToken("se");
    adicionarToken("senao");
    cout << "alfabeto: " << alfabeto << "\n";
    for (Estado e : afnd) {
        for (char letra : alfabeto) {
            if (e.transicoes.count(letra) > 0) {
                for (int destino : e.transicoes[letra]) {
                    cout << e.nome << " --" << letra << "--> " << afnd[destino].nome << "\n";
                }
            }
        }
        if (e.final) {
            for (string token : e.tokens) {
                cout << e.nome << " eh final e reconhece " << token << "\n";
            }
        }
    }

    return 0;
}