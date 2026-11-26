#include <bits/stdc++.h>
using namespace std;

struct Estado {
    string nome;
    bool final = false;
    set<string> tokens;
    map<char, set<int> > transicoes;
};

vector<Estado> afnd;
string alfabeto;
int contador = 0;

string novoNome() {

    contador++;


    if (contador == 19) {
        contador++;
    }


    string nome = "";


    int i = contador;

    while (i > 0) {

        int resto = (i - 1) % 26;

        char letra = 'A' + resto;

        nome = letra + nome;


        i = (i - 1) / 26;
    }

    return nome;
}

int novoEstado() {
    Estado e;
    e.nome = novoNome();
    afnd.push_back(e);
    int posicao = afnd.size() - 1;
    return posicao;
}

void criaSeta(int origem, char letra, int destino) {
    if (alfabeto.find(letra) == string::npos) {

        alfabeto = alfabeto + letra;
    }
    afnd[origem].transicoes[letra].insert(destino);
}

void adicionarToken(string token) {
    int atual = 0;
    for (char letra: token) {
        int novo = novoEstado();
        criaSeta(atual, letra, novo);
        atual = novo;
    }
    afnd[atual].final = true;
    afnd[atual].tokens.insert(token);
}


map<string, int> mapaGR;

string simboloInicial = "<S>";
bool inicialReferenciado = false;
int contadorGR = 0;



int pegarEstadoGR(string nomeGR) {
    if (nomeGR == simboloInicial && !inicialReferenciado) {
        return 0;
    }
    if (mapaGR.count(nomeGR) == 0) {
        mapaGR[nomeGR] = novoEstado();
    }
    return mapaGR[nomeGR];
}


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



void processarGramatica(string linha) {

    string linhaLimpa = "";
    for (int i = 0; i < linha.length(); i++) {
        if (linha[i] != ' ') {
            linhaLimpa += linha[i];
        }
    }


    int pos = linhaLimpa.find("::=");

    if (pos == -1) {
        return;
    }
    string origem = linhaLimpa.substr(0, pos);
    string regras = linhaLimpa.substr(pos + 3);


    string pedaco = "";
    for (int i = 0; i < regras.length(); i++) {
        if (regras[i] == '|') {

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
            pedaco = "";
        } else {
            pedaco += regras[i];
        }
    }


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


struct EstadoAFD {
    string nome;
    bool final = false;
    set<string> tokens;
    string conjunto;
    map<char, int> transicoes;
};

vector<EstadoAFD> afd;


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

        for (char letra: alfabeto) {
            if (afnd[proprio].transicoes.count(letra) > 0) {
                for (int destino: afnd[proprio].transicoes[letra]) {
                    criaSeta(0, letra, destino);
                }
            }
        }


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


    if (!arquivo) {
        cout << "nao foi possivel abrir o arquivo entrada.txt\n";
        exit(1);
    }

    string linha;
    bool primeiraLinha = true;
    vector<string> blocoGR;
    while (getline(arquivo, linha)) {
        if (primeiraLinha) {
            if (linha.size() >= 3 && linha.substr(0, 3) == "\xEF\xBB\xBF") {
                linha = linha.substr(3);
            }
            primeiraLinha = false;
        }
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

string textoConjunto(set<int> conjunto) {
    string texto = "[";
    for (int idAFND : conjunto) {
        if (texto != "[") {
            texto = texto + ",";
        }
        texto = texto + afnd[idAFND].nome;
    }
    texto = texto + "]";
    return texto;
}

void determinizar() {
    vector<set<int> > conjuntos;
    set<int> inicial;
    inicial.insert(0);
    conjuntos.push_back(inicial);

    EstadoAFD eInicial;
    eInicial.nome = gerarNomeAFD(0);
    eInicial.conjunto = textoConjunto(inicial);
    if (afnd[0].final) {
        eInicial.final = true;
        eInicial.tokens = afnd[0].tokens;
    }
    afd.push_back(eInicial);
    for (int i = 0; i < conjuntos.size(); i++) {
        for (char letra: alfabeto) {
            set<int> destino;


            for (int idAFND: conjuntos[i]) {
                if (afnd[idAFND].transicoes.count(letra) > 0) {
                    for (int d: afnd[idAFND].transicoes[letra]) {
                        destino.insert(d);
                    }
                }
            }

            if (destino.size() > 0) {

                int idDestino = -1;
                for (int j = 0; j < conjuntos.size(); j++) {
                    if (conjuntos[j] == destino) {
                        idDestino = j;
                        break;
                    }
                }


                if (idDestino == -1) {
                    idDestino = conjuntos.size();
                    conjuntos.push_back(destino);

                    EstadoAFD novo;
                    novo.nome = gerarNomeAFD(idDestino);
                    novo.conjunto = textoConjunto(destino);

                    for (int idAFND: destino) {
                        if (afnd[idAFND].final) {
                            novo.final = true;
                            for (string tok: afnd[idAFND].tokens) {
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


void reconstruirAFD(vector<bool> manter) {
    vector<int> novoIndice(afd.size(), -1);
    vector<EstadoAFD> novoAFD;


    for (int i = 0; i < afd.size(); i++) {
        if (manter[i]) {
            novoIndice[i] = novoAFD.size();
            EstadoAFD copia;
            copia.nome = gerarNomeAFD(novoIndice[i]);
            copia.final = afd[i].final;
            copia.conjunto = afd[i].conjunto;
            copia.tokens = afd[i].tokens;
            novoAFD.push_back(copia);
        }
    }


    for (int i = 0; i < afd.size(); i++) {

        if (novoIndice[i] == -1) {
            continue;
        }

        int origemNova = novoIndice[i];
        for (char letra: alfabeto) {
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

    if (afd.size() == 0) {
        return;
    }
    vector<bool> alcancavel(afd.size(), false);
    alcancavel[0] = true;

    bool mudou = true;
    while (mudou) {
        mudou = false;
        for (int i = 0; i < afd.size(); i++) {
            if (alcancavel[i]) {
                for (char letra: alfabeto) {
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

    if (afd.size() == 0) {
        return;
    }

    vector<bool> vivo(afd.size(), false);


    for (int i = 0; i < afd.size(); i++) {
        if (afd[i].final) {
            vivo[i] = true;
        }
    }


    bool mudou = true;
    while (mudou) {
        mudou = false;
        for (int i = 0; i < afd.size(); i++) {
            if (!vivo[i]) {
                for (char letra: alfabeto) {
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

    vivo[0] = true;
    reconstruirAFD(vivo);
}

void adicionarEstadoErro() {
    EstadoAFD erro;
    erro.nome = "_";
    erro.final = true;
    erro.tokens.insert("ERRO");

    afd.push_back(erro);
    int idErro = afd.size() - 1;


    for (int i = 0; i < afd.size(); i++) {
        for (char letra: alfabeto) {
            if (afd[i].transicoes.count(letra) == 0) {
                afd[i].transicoes[letra] = idErro;
            }
        }
    }
}

string rotuloLinha(int posicao, bool ehFinal, string nome) {
    string rotulo = "";
    if (posicao == 0) {
        rotulo = rotulo + "->";
    }
    if (ehFinal) {
        rotulo = rotulo + "*";
    }
    rotulo = rotulo + nome;
    return rotulo;
}


string celulaAFND(Estado e, char letra) {
    if (e.transicoes.count(letra) == 0) {
        return "-";
    }
    string texto = "";
    for (int destino : e.transicoes[letra]) {
        if (texto != "") {
            texto = texto + ", ";
        }
        texto = texto + afnd[destino].nome;
    }
    return texto;
}

void imprimirAFND() {
    int larguraRotulo = 5;
    int larguraCelula = 1;
    for (int i = 0; i < afnd.size(); i++) {
        string rotulo = rotuloLinha(i, afnd[i].final, afnd[i].nome);
        int tamanho = rotulo.size();
        if (tamanho > larguraRotulo) {
            larguraRotulo = tamanho;
        }
        for (char letra : alfabeto) {
            string celula = celulaAFND(afnd[i], letra);
            tamanho = celula.size();
            if (tamanho > larguraCelula) {
                larguraCelula = tamanho;
            }
        }
    }
    larguraRotulo = larguraRotulo + 2;
    larguraCelula = larguraCelula + 2;

    cout << "\ntabela do AFND:\n\n";
    cout << left << setw(larguraRotulo) << "delta";
    for (char letra : alfabeto) {
        cout << setw(larguraCelula) << letra;
    }
    cout << "\n";
    for (int i = 0; i < afnd.size(); i++) {
        cout << setw(larguraRotulo) << rotuloLinha(i, afnd[i].final, afnd[i].nome);
        for (char letra : alfabeto) {
            cout << setw(larguraCelula) << celulaAFND(afnd[i], letra);
        }
        cout << "\n";
    }


    cout << "\nestados finais do AFND:\n";
    for (int i = 0; i < afnd.size(); i++) {
        if (afnd[i].final) {
            cout << afnd[i].nome << ": ";
            for (string token : afnd[i].tokens) {
                cout << token << " ";
            }
            cout << "\n";
        }
    }
}

string rotuloAFD(int posicao) {
    string rotulo = rotuloLinha(posicao, afd[posicao].final, afd[posicao].nome);
    if (afd[posicao].conjunto != "") {
        rotulo = rotulo + " " + afd[posicao].conjunto;
    }
    return rotulo;
}
string celulaAFD(EstadoAFD e, char letra) {
    if (e.transicoes.count(letra) == 0) {
        return "-";
    }
    int destino = e.transicoes[letra];
    return afd[destino].nome;
}

void imprimirAFD() {

    int larguraRotulo = 5;
    int larguraCelula = 1;
    for (int i = 0; i < afd.size(); i++) {
        string rotulo = rotuloAFD(i);
        int tamanho = rotulo.size();
        if (tamanho > larguraRotulo) {
            larguraRotulo = tamanho;
        }
        for (char letra : alfabeto) {
            string celula = celulaAFD(afd[i], letra);
            tamanho = celula.size();
            if (tamanho > larguraCelula) {
                larguraCelula = tamanho;
            }
        }
    }
    larguraRotulo = larguraRotulo + 2;
    larguraCelula = larguraCelula + 2;

    cout << "\ntabela do AFD final:\n\n";

    cout << left << setw(larguraRotulo) << "delta";
    for (char letra : alfabeto) {
        cout << setw(larguraCelula) << letra;
    }
    cout << "\n";

    for (int i = 0; i < afd.size(); i++) {
        cout << setw(larguraRotulo) << rotuloAFD(i);
        for (char letra : alfabeto) {
            cout << setw(larguraCelula) << celulaAFD(afd[i], letra);
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

    imprimirAFND();

    determinizar();
    removerInalcancaveis();
    removerMortos();
    adicionarEstadoErro();
    imprimirAFD();

    return 0;
}
