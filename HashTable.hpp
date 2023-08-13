#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

// LEMBRETE: usar numero primo como tamanho de tabela hash

typedef struct jogador
{
    int id;
    string nome;
    vector<string> posicoes;
    float avaliacao = 0;
    int num_avaliacoes = 0;
    vector<string> tags;
} JOGADOR;
template <typename T>
class HashTable
{
private:
    int tamanho;
    vector<vector<T>> tabela;
    int num_elementos;

public:
    
    HashTable(int tamanho);
    int get_tamanho() { return tamanho; };
    int get_num_elements() { return num_elementos; };
    int hash_function(int id);
    void insere_jogador(JOGADOR jogador);
    void printa_tabela();
    void printa_tabela_nomes();
    JOGADOR busca_jogador(int id);
    JOGADOR* busca_jogador_ref(int id);
    void remove_jogador(int id);
    void printa_jogador(JOGADOR player);
    vector<JOGADOR> get_vetor(int indice) { return tabela[indice];}
    ~HashTable();
    int hash_function(string str);
    void insere_string(string str);
};

template <typename T>
HashTable<T>::HashTable(int tamanho)
{
    this->tamanho = tamanho;
    tabela.resize(tamanho);
    num_elementos= 0;
}

template <typename T>
HashTable<T>::~HashTable()
{
    for (int i = 0; i < tamanho; i++)
    {
        tabela[i].clear();
    }
    tabela.clear();
}

template <>
int HashTable<JOGADOR>::hash_function(int id)
{
    return id % tamanho;
}

template <>
void HashTable<JOGADOR>::insere_jogador(JOGADOR jogador)
{
    int indice = hash_function(jogador.id);
    tabela[indice].push_back(jogador);
    num_elementos++;
}

template <>
JOGADOR* HashTable<JOGADOR>::busca_jogador_ref(int id)
{
    int indice = hash_function(id);
    for (int i = 0; i < tabela[indice].size(); i++)
    {
        if (tabela[indice][i].id == id)
        {
            return &tabela[indice][i];
        }
    }
    return NULL;
}

template <>
void HashTable<JOGADOR>::printa_jogador(JOGADOR player)
{
    cout<<"ID: "<<player.id<<" ";
    cout<<"Nome: "<<player.nome<<" ";
    cout<<"Posicoes: ";
    for(int i=0;i<player.posicoes.size();i++)
    {
        cout<<player.posicoes[i]<<" ";
    }
    cout<<"Avaliacao: "<<player.avaliacao<<" ";
    cout<<"Num avaliacoes: "<<player.num_avaliacoes<<" ";
    cout<<"Tags: ";
    for(int i=0;i<player.tags.size();i++)
    {
        cout<<player.tags[i]<<" ";
    }
    cout<<endl;
}

template <>
JOGADOR HashTable<JOGADOR>::busca_jogador(int id)
{
    int indice = hash_function(id);
    for (int i = 0; i < tabela[indice].size(); i++)
    {
        if (tabela[indice][i].id == id)
        {
            return tabela[indice][i];
        }
    }
    JOGADOR jogador;
    jogador.id = -1;
    return jogador;
}

template <>
void HashTable<JOGADOR>::printa_tabela()
{
    for (int i = 0; i < tamanho; i++)
    {
        if (tabela[i].size() != 0)
        {
            cout << "indice " << i << ": ";
            for (int j = 0; j < tabela[i].size(); j++)
            {
                printa_jogador(tabela[i][j]);
            }
            cout << endl;
        }
    }
}

template <>
void HashTable<JOGADOR>::printa_tabela_nomes()
{
    for(int i=0;i<tamanho;i++)
    {
        if(tabela[i].size()!=0)
        {
            cout<<"indice "<<i<<": ";
            for(int j=0;j<tabela[i].size();j++)
            {
                cout<<tabela[i][j].nome<<" -> ";
            }
            cout<<endl;
        }
    }
}

template <>
void HashTable<JOGADOR>::remove_jogador(int id)
{
    int indice = hash_function(id);
    for (int i = 0; i < tabela[indice].size(); i++) // procura o jogador
    {
        if (tabela[indice][i].id == id)
        {
            tabela[indice].erase(tabela[indice].begin() + i);
            num_elementos--;
            return;
        }
    }
}

//para strings
template<>
int HashTable<string>::hash_function(string str)
{
    int soma = 0;
    for (int i = 0; i < str.size(); i++)
    {
        soma += str[i];
    }
    num_elementos++;
    return soma % tamanho;
}

template <>
void HashTable<string>::insere_string(string str)
{
    int indice = hash_function(str);
    tabela[indice].push_back(str);
}