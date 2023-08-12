#include<cstdlib>
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include"parsing.hpp"
#include"trie.hpp"
#include"HashTable.hpp"
#include"HashUsers.hpp"

using namespace std;
using namespace aria::csv; //parser

/*

organização:

1. dados
    - usar o parser pra ler o csv
    https://github.com/AriaFallah/csv-parser/blob/master/parser.hpp

2. pesquisa
    1. por nome (prefixo): $ player <nome>
        - aparecer todos os jogadores que tem aquele prefixo (se tiver espaço incluir também? tipo jo elmano e joel?)
            - mostrar id, nome, posição (que pode ser uma ou mais), avaliação e contador de avaliação

    2. por avaliação de usuário: $ <user> <nota>
        - aparecer no máx 20 jogadores avaliados pelo user
        ? acho que é pra pegar todos os jogadores com nota >= a <nota> e colocar em ordem decrescente de acordo com a nota

    3. por posição: $ top<n> <posição>
        - <n> é quantos jogadores tem que aparecer
        - jogadores com no mínimo 1000 avaliações
        - ordenado de maior pra menor avaliação

    4. por tags: $ tags <list of tags>
        - mostrar todos os jogadores com aquelas tags, não tem ordem especificada

pré-processamento:
    - 2.1 e 2.3: calcular a quantidade e média de avaliações de cada jogador -> HASH?
    - 2.2: separar todos os jogadores avaliados por cada usuário -> B-TREE pra cada usuário? acho complicado demais
                                                                 -> HASH? 

    não é explícito que tem que fazer isso no pré-processamento, e como tem limite de tempo, 
    tem que ver se vale a pena fazer aqui ou durante a pesquisa mesmo
    - 2.4: separar tags e quais jogadores cada uma tem -> PATRICIA? deixa no último nodo uma lista com o id dos jogadores

3. implementação
    - fase 1: inicialização e construção de estruturas em até 3 minutos, menos de 1 minuto é +10% de nota
    - fase 2: consulta

    extra: consulta extras e interface gráfica é +20% de nota extra

4. entrega
    - os dois tem que tá presentes
    - explicar a 2
    - rodar \o/


observações: 
dados a serem guardados para cada jogador
    - id
    - nome
    - posição
    - avaliação(media das avaliações)
    - contador de avaliações
    - tags

nome->arvore trie
id->hash e arvore trie(nos nodos terminadores)
posição->hash
avaliação->hash
contador de avaliações->hash

*/

#define TAM_HASH_PLAYERS 10000
#define TAM_HASH_AVALIACAO 4007
//FAZER TABELA HASH PARA USARIOS QUE AVALIAM JOGADORES
// void calcula_avaliacao(TRIE &trie,HashTable &hash_id,/*HASH_USARIOS hash*/)
// {
//     ifstream arquivo_notas("./files/minirating.csv");
//     CsvParser parser_notas(arquivo_notas);
//     JOGADOR generico;
//     vector<int> ids;
//     int user_id,player_id,nota;
//     for(int i=0;i<4;i++)
//         parser_notas.next_field();
//     for(auto row:parser_notas)
//     {

//     }
// }

void printa_prefixo(TRIE &trie, HashTable &hash_id, string prefixo)
{
    vector<int> ids; 
    trie.get_jogadores_prefixo(trie.get_raiz(),prefixo,0,ids);
    for(int i=0;i<ids.size();i++)
    {
        hash_id.printa_jogador(hash_id.busca_jogador(ids[i]));
    }
}

void preenche_hash_id(TRIE &trie, HashTable &hash_id)
{
    ifstream arquivo_players("./files/players.csv");
    CsvParser parser_players(arquivo_players);
    JOGADOR generico;
    for(int i=0;i<4;i++)
        parser_players.next_field();

    for(auto row:parser_players)
    {
        generico.id = stoi(row[0]);
        generico.nome = row[1];
        generico.posicao = row[2];
        hash_id.insere_jogador(generico);
        trie.insere(trie.get_raiz(),generico.nome, generico.id,0);
        
    }
}

void atualiza_avaliacao(HashTable &hash_id, int id, int nota)
{
    JOGADOR* pt_jg=hash_id.busca_jogador_ref(id);
    if(pt_jg)
    {
        pt_jg->avaliacao += nota;
        pt_jg->num_avaliacoes++;

    }
    else
    {
        cout<<"ponteiro NULL"<<endl;
    }
}

void preenche_hash_avaliacao(TRIE &trie, HashTable &hash_id, HashUser &hash_avaliacao)
{
    ifstream arquivo_notas("./files/minirating.csv");
    CsvParser parser_notas(arquivo_notas);
    JOGADOR jogador_generico;
    USER* pt_user;
    USER usuario_generico;
    vector<int> ids;
    int user_id,player_id,nota;
    for(int i=0;i<4;i++)
        parser_notas.next_field();
    for(auto row:parser_notas)
    {
        usuario_generico.id_user = stoi(row[0]);
        if(hash_avaliacao.busca_user(usuario_generico.id_user).id_user == -1)
        {
            usuario_generico.num_avaliacoes++;
            inserirOrdenado(usuario_generico.avaliacoes,make_tuple(stoi(row[1]),stoi(row[2])));
            hash_avaliacao.insere_user(usuario_generico);
            atualiza_avaliacao(hash_id,stoi(row[1]),stoi(row[2]));
        }
        else
        {
            pt_user=hash_avaliacao.busca_user_ref(usuario_generico.id_user);
            pt_user->num_avaliacoes++;
            inserirOrdenado(pt_user->avaliacoes,make_tuple(stoi(row[1]),stoi(row[2])));
            atualiza_avaliacao(hash_id,stoi(row[1]),stoi(row[2]));
        }
        usuario_generico.avaliacoes.clear();
    }
}

int main()
{
    TRIE trie;
    HashTable hash_id(TAM_HASH_PLAYERS);
    JOGADOR generico;
    HashUser hash_usuarios(TAM_HASH_AVALIACAO);
    string generica;
    string opcao;
    vector<int> ids;
    int id_user;

    preenche_hash_id(trie,hash_id);
    // calcula_avaliacao(trie,hash_id,hash_avaliacao);
    preenche_hash_avaliacao(trie,hash_id,hash_usuarios);

    do
    {
        cin>>opcao;
        if(opcao == "player")
        {
            getline(cin,generica);
            generica.erase(0,1); //tira o espaço
            printa_prefixo(trie,hash_id,generica);
        }
        else if(opcao == "user")
        {
            cin>>id_user;
            hash_usuarios.printa_user(hash_usuarios.busca_user(id_user));
        }

    } while (opcao!="sair");
      

    

    cout<<"\noi\n";
    return 0;
}