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
    string posicao;
    float avaliacao = 0;
    int num_avaliacoes = 0;
    vector<string> tags;
} JOGADOR;

class HashTable
{
private:
    int tamanho;
    vector<vector<JOGADOR>> tabela;
    int num_jogadores;

public:
    HashTable(int tamanho);
    int get_tamanho() { return tamanho; };
    int get_num_jogadores() { return num_jogadores; };
    int hash_function(int id);
    void insere_jogador(JOGADOR jogador);
    void printa_tabela();
    void printa_tabela_nomes();
    JOGADOR busca_jogador(int id);
    JOGADOR* busca_jogador_ref(int id);
    void remove_jogador(int id);
    void printa_jogador(JOGADOR player);
    ~HashTable();
};

HashTable::HashTable(int tamanho)
{
    this->tamanho = tamanho;
    tabela.resize(tamanho);
    num_jogadores = 0;
}

HashTable::~HashTable()
{
    for (int i = 0; i < tamanho; i++)
    {
        tabela[i].clear();
    }
    tabela.clear();
}

int HashTable::hash_function(int id)
{
    return id % tamanho;
}

void HashTable::insere_jogador(JOGADOR jogador)
{
    int indice = hash_function(jogador.id);
    tabela[indice].push_back(jogador);
    num_jogadores++;
}

JOGADOR* HashTable::busca_jogador_ref(int id)
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

void HashTable::printa_jogador(JOGADOR player)
{
    cout << "id: " << player.id << " nome: " << player.nome << " posicao: " << player.posicao << " avaliacao: " << player.avaliacao << " num_avaliacoes: " << player.num_avaliacoes << " tags: ";
    for (int i = 0; i < player.tags.size(); i++)
    {
        cout << player.tags[i] << " ";
    }
    cout << endl;
}

JOGADOR HashTable::busca_jogador(int id)
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

void HashTable::printa_tabela()
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

void HashTable::printa_tabela_nomes()
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

void HashTable::remove_jogador(int id)
{
    int indice = hash_function(id);
    for (int i = 0; i < tabela[indice].size(); i++) // procura o jogador
    {
        if (tabela[indice][i].id == id)
        {
            tabela[indice].erase(tabela[indice].begin() + i);
            num_jogadores--;
            return;
        }
    }
}