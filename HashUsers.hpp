#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include"parsing.hpp"

using namespace std;
using namespace aria::csv;
typedef struct user
{
    int id_user;
    int num_avaliacoes = 0;
    vector<tuple<int, float>> avaliacoes; // <id_player,nota> , Esse vetor deve ser ordenado pela nota
} USER;

class HashUser
{
private:
    int tamanho;
    vector<vector<USER>> tabela;
    int num_users;

public:
    HashUser(int tamanho);
    int get_tamanho() { return tamanho; };
    int get_num_users() { return num_users; };
    int hash_function(int id) { return id % tamanho; };
    void insere_user(USER user);
    void printa_tabela();
    USER busca_user(int id);
    USER* busca_user_ref(int id);
    void printa_user(USER user);
    ~HashUser();
};

HashUser::HashUser(int tamanho)
{
    this->tamanho = tamanho;
    tabela.resize(tamanho);
    num_users = 0;
}

HashUser::~HashUser()
{
    for (int i = 0; i < tamanho; i++)
    {
        tabela[i].clear();
    }
    tabela.clear();
}

void HashUser::insere_user(USER user)
{
    int indice = hash_function(user.id_user);
    tabela[indice].push_back(user);
    num_users++;
}

USER HashUser::busca_user(int id)
{
    int indice = hash_function(id);
    for (int i = 0; i < tabela[indice].size(); i++)
    {
        if (tabela[indice][i].id_user == id)
            return tabela[indice][i];
    }
    // cout << "User nao encontrado" << endl;
    USER user;
    user.id_user = -1;
    return user;
}

USER* HashUser::busca_user_ref(int id)
{
    int indice = hash_function(id);
    for (int i = 0; i < tabela[indice].size(); i++)
    {
        if (tabela[indice][i].id_user == id)
            return &tabela[indice][i];
    }
    // cout << "User nao encontrado" << endl;
    
    return NULL;
}

void HashUser::printa_user(USER user)
{
    cout << "id_user: " << user.id_user << " avaliacoes: ";
    for (int i = 0; i < user.avaliacoes.size(); i++)
    {
        cout << "<" << get<0>(user.avaliacoes[i]) << "," << get<1>(user.avaliacoes[i]) << "> ";
    }
    cout << endl;
}

// void swapTuples(tuple<int, float> &a, tuple<int, int> &b) {
//     tuple<int, float> temp = a;
//     a = b;
//     b = temp;
// }

// // Função para particionar o vetor para o Quicksort
// int partition(vector<tuple<int, float>> &arr, int low, int high) {
//     //escaolhe randomicamente um pivo
//     int random = low + rand() % (high - low);
//     swapTuples(arr[random], arr[high]);
//     int pivot = get<1>(arr[high]); // Escolhe o último elemento como pivô
//     int i = low - 1; // Índice do menor elemento

//     for (int j = low; j <= high - 1; j++) { // Percorre o vetor do início até o penúltimo elemento 
//         if (get<1>(arr[j]) >= pivot) { // Se o elemento atual for menor ou igual ao pivô
//             i++;
//             swapTuples(arr[i], arr[j]); // Troca o elemento atual com o elemento do índice i
//         }
//     }
//     swapTuples(arr[i + 1], arr[high]); // Troca o elemento do índice i + 1 com o pivô
//     return i + 1; // Retorna o índice do pivô
// }

// // Função Quicksort principal
// void quicksort(vector<tuple<int, float>> &arr, int low, int high) {
//     if (low < high) {
//         int pi = partition(arr, low, high);

//         quicksort(arr, low, pi - 1);
//         quicksort(arr, pi + 1, high);
//     }
// }

// void HashUser::ordena_avaliacoes(USER user)
// {
//     quicksort(user.avaliacoes, 0, user.avaliacoes.size() - 1);
// }

void inserirOrdenado(vector<tuple<int, float>>& vetor, tuple<int, float> novaTupla) {
    int inicio = 0;
    int fim = vetor.size() - 1;
    int posicaoInsercao = vetor.size();

    while (inicio <= fim) {
        int meio = (inicio + fim) / 2;

        if (get<1>(novaTupla) > get<1>(vetor[meio])) {
            posicaoInsercao = meio;
            fim = meio - 1;
        } else {
            inicio = meio + 1;
        }
    }

    vetor.insert(vetor.begin() + posicaoInsercao, novaTupla);
}

void HashUser::printa_tabela()
{
    for (int i = 0; i < tamanho; i++)
    {
        cout << "Indice " << i << ": ";
        for(int j=0;j<tabela[i].size();j++)
        {
            printa_user(tabela[i][j]);
        }
        cout << endl;
    }
}

// int main()
// {
//     USER generico;
//     USER nao_encontrado;
//     USER *ponteiro;
//     HashUser hash_user(1000);
//     ifstream arquivo("files/minirating.csv");
//     CsvParser parser_user(arquivo);

//     for(int i=0;i<4;i++)
//         parser_user.next_field();
//     for(auto row:parser_user)
//     {
//         generico.id_user = stoi(row[0]);
//         if(hash_user.busca_user(generico.id_user).id_user == -1)
//         {
//             generico.num_avaliacoes++;
//             inserirOrdenado(generico.avaliacoes, make_tuple(stoi(row[1]), stof(row[2])));
//             hash_user.insere_user(generico);
//         }
//         else
//         {
//             ponteiro = hash_user.busca_user_ref(generico.id_user);
//             (*ponteiro).num_avaliacoes++;
//             inserirOrdenado((*ponteiro).avaliacoes, make_tuple(stoi(row[1]), stof(row[2])));
//             generico=hash_user.busca_user(generico.id_user);
//         }
//         generico.avaliacoes.clear();
//     }

//     // hash_user.printa_user(hash_user.busca_user(82985));
//     hash_user.printa_tabela();
// }