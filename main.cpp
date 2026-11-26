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

string simboloInicial = "<S>"; // inicial
bool inicialReferenciado = false;
int contadorGR = 0;

// funcao pra achar o numero do estado da gramatica
int pegarEstadoGR(string nomeGR) {
    if (nomeGR == simboloInicial && !inicialReferenciado) {
        return 0;
    }
    if (mapaGR.count(nomeGR) == 0) {
        mapaGR[nomeGR] = novoEstado();
    }
    return mapaGR[nomeGR];
}

// pensei q talvez precisaria disso pra verificar o vazio escrito de diferentes formas
bool ehEpsilon(string pedaco) {
    if (pedaco == "ε") {
        return true;
    }

    if (pedaco == "\\epsilon") {
        return true;
    }

    if (pedaco == "&") {
        return true;
    }

    return false;
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
            if (ehEpsilon(pedaco)) {
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
        if (ehEpsilon(pedaco)) {
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

// estrutura do AFD cada letra leva para apenas 1 estado
struct EstadoAFD {
    string nome;
    bool final = false;
    set<string> tokens;
    map<char, int> transicoes;
};

vector<EstadoAFD> afd;

// gera nomes para os estados do AFD
string gerarNomeAFD(int num) {
    if (num == 0) {
        return "S";
    }
    if (num >= 19) {
        num++;
    }

    string nome = "";
    int i = num;
    while (i > 0) {
        int resto = (i - 1) % 26;
        char letra = 'A' + resto;
        nome = letra + nome;
        i = (i - 1) / 26;
    }
    return nome;
}

// processa uma gramatica inteira de uma vez
void processarBlocoGR(vector<string> linhas) {
    mapaGR.clear();
    contadorGR++;
    string primeira = linhas[0];
    simboloInicial = primeira.substr(0, primeira.find("::="));
    inicialReferenciado = false;
    for (int i = 0; i < linhas.size(); i++) {
        string direita = linhas[i].substr(linhas[i].find("::=") + 3);
        if (direita.find(simboloInicial) != string::npos) {
            inicialReferenciado = true;
        }
    }

    for (int i = 0; i < linhas.size(); i++) {
        processarGramatica(linhas[i]);
    }
    if (inicialReferenciado) {
        int proprio = mapaGR[simboloInicial];

        for (char letra : alfabeto) {
            if (afnd[proprio].transicoes.count(letra) > 0) {
                for (int destino : afnd[proprio].transicoes[letra]) {
                    criaSeta(0, letra, destino);
                }
            }
        }

        // se o inicial eh final o s tb fica final
        if (afnd[proprio].final) {
            afnd[0].final = true;
        }
    }
    for (int i = 0; i < afnd.size(); i++) {
        if (afnd[i].final && afnd[i].tokens.size() == 0) {
            afnd[i].tokens.insert("GR" + to_string(contadorGR));
        }
    }
}

void lerArquivo() {
    ifstream arquivo("entrada.txt");

    // se n abriu avisa
    if (!arquivo) {
        cout << "nao foi possivel abrir o arquivo entrada.txt\n";
        exit(1);
    }

    string linha;
    vector<string> blocoGR;
    while (getline(arquivo, linha)) {
        string semLixo = "";
        for (int i = 0; i < linha.length(); i++) {
            if (linha[i] != '\r' && linha[i] != '\t') {
                semLixo += linha[i];
            }
        }
        string semEspaco = "";
        for (int i = 0; i < semLixo.length(); i++) {
            if (semLixo[i] != ' ') {
                semEspaco += semLixo[i];
            }
        }

        int pos = semEspaco.find("::=");
        if (pos != -1) {
            string origem = semEspaco.substr(0, pos);
            if (origem == "<S>" && blocoGR.size() > 0) {
                processarBlocoGR(blocoGR);
                blocoGR.clear();
            }
            blocoGR.push_back(semEspaco);
        } else {
            if (blocoGR.size() > 0) {
                processarBlocoGR(blocoGR);
                blocoGR.clear();
            }
            if (semEspaco != "") {
                adicionarToken(semEspaco);
            }
        }
    }
    if (blocoGR.size() > 0) {
        processarBlocoGR(blocoGR);
    }

    arquivo.close();
}
void determinizar() {
    // guarda o conjunto de estados do AFND que forma cada estado do AFD
    vector<set<int>> conjuntos;

    // comeca com o conjunto {0} S
    set<int> inicial;
    inicial.insert(0);
    conjuntos.push_back(inicial);

    EstadoAFD eInicial;
    eInicial.nome = gerarNomeAFD(0);
    if (afnd[0].final) {
        eInicial.final = true;
        eInicial.tokens = afnd[0].tokens;
    }
    afd.push_back(eInicial);

    // conforme novos conjuntos entram no vector o for continua ate acabar
    for (int i = 0; i < conjuntos.size(); i++) {
        for (char letra : alfabeto) {
            set<int> destino;

            // junta todos os destinos possiveis para essa letra
            for (int idAFND : conjuntos[i]) {
                if (afnd[idAFND].transicoes.count(letra) > 0) {
                    for (int d : afnd[idAFND].transicoes[letra]) {
                        destino.insert(d);
                    }
                }
            }

            if (destino.size() > 0) {
                // verifica se esse conjunto ja foi criado antes
                int idDestino = -1;
                for (int j = 0; j < conjuntos.size(); j++) {
                    if (conjuntos[j] == destino) {
                        idDestino = j;
                        break;
                    }
                }

                // se for conjunto novo cria um estado novo no AFD
                if (idDestino == -1) {
                    idDestino = conjuntos.size();
                    conjuntos.push_back(destino);

                    EstadoAFD novo;
                    novo.nome = gerarNomeAFD(idDestino);

                    for (int idAFND : destino) {
                        if (afnd[idAFND].final) {
                            novo.final = true;
                            for (string tok : afnd[idAFND].tokens) {
                                novo.tokens.insert(tok);
                            }
                        }
                    }

                    afd.push_back(novo);
                }

                afd[i].transicoes[letra] = idDestino;
            }
        }
    }
}

// funcao auxiliar pra manter no AFD apenas os estados com manter[i] == true
void reconstruirAFD(vector<bool> manter) {
    vector<int> novoIndice(afd.size(), -1);
    vector<EstadoAFD> novoAFD;

    // copia os estados que ficam e anota a nova posicao de cada um
    for (int i = 0; i < afd.size(); i++) {
        if (manter[i]) {
            novoIndice[i] = novoAFD.size();
            EstadoAFD copia;
            copia.nome = gerarNomeAFD(novoIndice[i]);
            copia.final = afd[i].final;
            copia.tokens = afd[i].tokens;
            novoAFD.push_back(copia);
        }
    }

    // refaz as transicoes apontando para as novas posicoes
    for (int i = 0; i < afd.size(); i++) {
        if (novoIndice[i] == -1) continue;

        int origemNova = novoIndice[i];
        for (char letra : alfabeto) {
            if (afd[i].transicoes.count(letra) > 0) {
                int destAntigo = afd[i].transicoes[letra];
                if (novoIndice[destAntigo] != -1) {
                    novoAFD[origemNova].transicoes[letra] = novoIndice[destAntigo];
                }
            }
        }
    }

    afd = novoAFD;
}

void removerInalcancaveis() {
    if (afd.size() == 0) return;

    vector<bool> alcancavel(afd.size(), false);
    alcancavel[0] = true; // o estado inicial S sempre eh alcancavel

    bool mudou = true;
    while (mudou) {
        mudou = false;
        for (int i = 0; i < afd.size(); i++) {
            if (alcancavel[i]) {
                for (char letra : alfabeto) {
                    if (afd[i].transicoes.count(letra) > 0) {
                        int dest = afd[i].transicoes[letra];
                        if (!alcancavel[dest]) {
                            alcancavel[dest] = true;
                            mudou = true;
                        }
                    }
                }
            }
        }
    }

    reconstruirAFD(alcancavel);
}

void removerMortos() {
    if (afd.size() == 0) return;

    vector<bool> vivo(afd.size(), false);

    // todo estado final ja eh vivo
    for (int i = 0; i < afd.size(); i++) {
        if (afd[i].final) {
            vivo[i] = true;
        }
    }

    // se um estado leva pra um estado vivo ele tambem eh vivo
    bool mudou = true;
    while (mudou) {
        mudou = false;
        for (int i = 0; i < afd.size(); i++) {
            if (!vivo[i]) {
                for (char letra : alfabeto) {
                    if (afd[i].transicoes.count(letra) > 0) {
                        int dest = afd[i].transicoes[letra];
                        if (vivo[dest]) {
                            vivo[i] = true;
                            mudou = true;
                        }
                    }
                }
            }
        }
    }

    vivo[0] = true; // mantem o inicial
    reconstruirAFD(vivo);
}

void adicionarEstadoErro() {
    EstadoAFD erro;
    erro.nome = "_"; // estado de erro
    erro.final = true; // ultimo estado final 
    erro.tokens.insert("ERRO");

    afd.push_back(erro);
    int idErro = afd.size() - 1;

    // preenche todas as celulas vazias 
    for (int i = 0; i < afd.size(); i++) {
        for (char letra : alfabeto) {
            if (afd[i].transicoes.count(letra) == 0) {
                afd[i].transicoes[letra] = idErro;
            }
        }
    }
}

void imprimirAFD() {
    cout << "\ntabela do AFD final:\n\n";

    cout << "estado\t";
    for (char letra : alfabeto) {
        cout << letra << "\t";
    }
    cout << "\n";

    for (int i = 0; i < afd.size(); i++) {
        if (afd[i].final) {
            cout << "*";
        }
        cout << afd[i].nome << "\t";

        for (char letra : alfabeto) {
            if (afd[i].transicoes.count(letra) > 0) {
                int dest = afd[i].transicoes[letra];
                cout << afd[dest].nome << "\t";
            } else {
                cout << "-\t";
            }
        }
        cout << "\n";
    }

    cout << "\nestados finais do AFD:\n";
    for (int i = 0; i < afd.size(); i++) {
        if (afd[i].final) {
            cout << afd[i].nome << ": ";
            for (string tok : afd[i].tokens) {
                cout << tok << " ";
            }
            cout << "\n";
        }
    }
}

int main() {
    Estado inicial;
    inicial.nome = "S";
    afnd.push_back(inicial);

    lerArquivo();
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

    determinizar();
    removerInalcancaveis();
    removerMortos();
    adicionarEstadoErro();
    imprimirAFD();

    return 0;
}