#include<cstdlib>
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include"parsing.hpp"
#include"trie.hpp"
#include"HashTable.hpp"

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

ir lendo do arquivo de players e ir

*/

#define TAM_HASH 10000

int main()
{
    TRIE trie;
    ifstream arquivo_players("./files/players.csv");
    HashTable hash_id(TAM_HASH);
    JOGADOR generico;
    string generica;
    vector<int> ids;
    int indice;
    CsvParser parser_players(arquivo_players);
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
    // hash_id.printa_tabela_nomes();
    // trie.printa_arvore(trie.get_raiz(),0,generica);
    trie.get_jogadores_prefixo(trie.get_raiz(),"Joao",0,ids);
    for(int i=0;i<ids.size();i++)
        hash_id.printa_jogador(hash_id.busca_jogador(ids[i]));    

    

    cout<<"\noi\n";
    return 0;
}