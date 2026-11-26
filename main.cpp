#include <bits/stdc++.h>
using namespace std;

struct Estado {
    string nome; // nome do estado na tabela tipo s ou a
    bool final = false; // se eh final aparece com asterisco na tabela
    set<string> tokens; // tokens q esse estado reconhece qnd eh final tipo se ou gr1
    map<char, set<int> > transicoes; // setas q saem do estado
};

vector<Estado> afnd; // lista de estados e a posicao 0 eh o estado inicial
string alfabeto; // letras usadas q vao virar as colunas da tabela
int contador = 0; // qnts nomes ja foram criados

string novoNome() {
    // soma 1 pq vamos criar mais um nome
    contador++;

    // s eh a letra 19 e esse nome eh do estado inicial entao pula p o t
    if (contador == 19) {
        contador++;
    }

    // nome comeca vazio e as letras entram no while
    string nome = "";

    // copia do contador pq o i muda no while e o contador n pode mudar
    int i = contador;

    while (i > 0) {
        // posicao da letra de a ate z onde 0 eh a e 25 eh z
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
    int posicao = afnd.size() - 1; // o estado novo entrou no fim entao a posicao dele eh o tamanho menos 1
    return posicao;
}

void criaSeta(int origem, char letra, int destino) {
    if (alfabeto.find(letra) == string::npos) {
        // se a letra ainda n ta no alfabeto coloca ela no fim
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

string simboloInicial = "<S>"; // simbolo inicial da gramatica atual q muda a cada bloco
bool inicialReferenciado = false;
int contadorGR = 0;

// devolve o estado do afnd de um nao terminal e cria um novo se ainda n existir
// o inicial usa o estado 0 direto qnd nenhuma regra aponta p ele
int pegarEstadoGR(string nomeGR) {
    if (nomeGR == simboloInicial && !inicialReferenciado) {
        return 0;
    }
    if (mapaGR.count(nomeGR) == 0) {
        mapaGR[nomeGR] = novoEstado();
    }
    return mapaGR[nomeGR];
}

// ve se o pedaco eh o vazio escrito de um dos jeitos aceitos
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


// processa uma linha da gramatica e cria as setas dela no afnd
void processarGramatica(string linha) {
    // tira todos os espacos
    string linhaLimpa = "";
    for (int i = 0; i < linha.length(); i++) {
        if (linha[i] != ' ') {
            linhaLimpa += linha[i];
        }
    }

    // acha onde ta o sinal de definicao e separa os lados
    int pos = linhaLimpa.find("::=");
    // se n achou o sinal a linha n eh regra entao ignora
    if (pos == -1) {
        return;
    }
    string origem = linhaLimpa.substr(0, pos); // lado esquerdo eh o nao terminal
    string regras = linhaLimpa.substr(pos + 3); // lado direito sao as alternativas

    // le caractere por caractere e separa as alternativas em cada barra
    string pedaco = "";
    for (int i = 0; i < regras.length(); i++) {
        if (regras[i] == '|') {
            // pedaco vazio aparece qnd tem duas barras seguidas ou barra logo depois do sinal e eh ignorado
            if (pedaco != "") {
                if (ehEpsilon(pedaco)) {
                    int idOrigem = pegarEstadoGR(origem);
                    afnd[idOrigem].final = true;
                } else {
                    int idOrigem = pegarEstadoGR(origem);
                    char letra = pedaco[0]; // o primeiro caractere eh o terminal
                    string nomeDestino = pedaco.substr(1); // o resto eh o nome do nao terminal de destino

                    int idDestino;
                    if (nomeDestino == "") {
                        // regra so com terminal tipo a cria um estado final novo
                        idDestino = novoEstado();
                        afnd[idDestino].final = true;
                    } else {
                        idDestino = pegarEstadoGR(nomeDestino);
                    }
                    criaSeta(idOrigem, letra, idDestino);
                }
            }
            pedaco = ""; // zera o pedaco p ler o proximo
        } else {
            pedaco += regras[i]; // vai juntando as letras
        }
    }

    // trata o ultimo pedaco q sobrou depois da ultima barra
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

// estado do afd onde cada letra leva p um so estado
struct EstadoAFD {
    string nome;
    bool final = false;
    set<string> tokens;
    string conjunto; // estados tipo [A,H]
    map<char, int> transicoes;
};

vector<EstadoAFD> afd;

// gera o nome do estado do afd pela posicao dele
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

        for (char letra: alfabeto) {
            if (afnd[proprio].transicoes.count(letra) > 0) {
                for (int destino: afnd[proprio].transicoes[letra]) {
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

    // se n abriu avisa e fecha o programa
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
            if ((origem == "<S>" || origem == "S") && blocoGR.size() > 0) {                processarBlocoGR(blocoGR);
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
// forma um estado tipo [A,H]
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

            // junta todos os destinos possiveis p essa letra
            for (int idAFND: conjuntos[i]) {
                if (afnd[idAFND].transicoes.count(letra) > 0) {
                    for (int d: afnd[idAFND].transicoes[letra]) {
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

                // se for conjunto novo cria um estado novo no afd
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

// deixa no afd so os estados marcados em manter e refaz os nomes deles
void reconstruirAFD(vector<bool> manter) {
    vector<int> novoIndice(afd.size(), -1);
    vector<EstadoAFD> novoAFD;

    // copia os estados q ficam e anota a nova posicao de cada um
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

    // refaz as transicoes apontando p as novas posicoes
    for (int i = 0; i < afd.size(); i++) {
        // estado removido n tem setas p refazer
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
    // seguranca caso o afd venha vazio
    if (afd.size() == 0) {
        return;
    }
    vector<bool> alcancavel(afd.size(), false);
    alcancavel[0] = true; // o estado inicial s sempre eh alcancavel

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
    // seguranca caso o afd venha vazio
    if (afd.size() == 0) {
        return;
    }

    vector<bool> vivo(afd.size(), false);

    // todo estado final ja eh vivo
    for (int i = 0; i < afd.size(); i++) {
        if (afd[i].final) {
            vivo[i] = true;
        }
    }

    // se um estado leva p um estado vivo ele tb eh vivo
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

    vivo[0] = true; // o s fica mesmo se for morto pq a tabela sempre comeca nele
    reconstruirAFD(vivo);
}

void adicionarEstadoErro() {
    EstadoAFD erro;
    erro.nome = "_"; // nome do estado de erro na tabela
    erro.final = true; // final como pede a apostila pq reporta erro
    erro.tokens.insert("ERRO");

    afd.push_back(erro);
    int idErro = afd.size() - 1;

    // toda celula vazia vai p o erro inclusive as do proprio erro
    for (int i = 0; i < afd.size(); i++) {
        for (char letra: alfabeto) {
            if (afd[i].transicoes.count(letra) == 0) {
                afd[i].transicoes[letra] = idErro;
            }
        }
    }
}
// comeco da linha com seta no inicial e asterisco nos finais igual a apostila
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

// celula do afnd com os destinos separados por virgula ou traco se n tiver seta
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

    // lista o q cada estado final reconhece
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
//  tipo A [A,H]
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
    // so pra alinhar a tabel
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
