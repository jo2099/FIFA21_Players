#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
// #include"parsing.hpp"
#include"parser.hpp"
#include"trie.hpp"
#include"HashTable.hpp"
#include <chrono>
#include<algorithm>
#include<sstream>

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

/*
CM
CDM
GK
CAM
RB
LB
LWB
CB
RM
LM
LW
RW
ST
RWB
CF
*/

#define TAM_HASH_PLAYERS 10000
#define TAM_HASH_AVALIACAO 1005007

#define NUM_POSITIONS 15

#define NAME_RATING "./files/rating.csv"



vector<int> intersectionOfVectors(const vector<vector<int>>& sortedVectors) {
    if (sortedVectors.empty()) {
        return {};  // Nenhum vetor para interseção
    }
    
    vector<int> result = sortedVectors[0];  // Começamos com o primeiro vetor
    
    for (size_t i = 1; i < sortedVectors.size(); ++i) {
        std::vector<int> tempResult;
        const std::vector<int>& currentVector = sortedVectors[i];
        
        size_t j = 0, k = 0;
        while (j < result.size() && k < currentVector.size()) {
            if (result[j] < currentVector[k]) {
                ++j;
            } else if (result[j] > currentVector[k]) {
                ++k;
            } else {
                tempResult.push_back(result[j]);
                ++j;
                ++k;
            }
        }
        
        result = tempResult;
        
        if (result.empty()) {
            break;  // Não há elementos em comum, podemos parar
        }
    }
    
    return result;
}


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


void generateSubstrings(const std::string& word, std::vector<std::string>& substrings) {
    int n = word.length();
    
    for (int len = 1; len <= n; ++len) {
        for (int i = 0; i <= n - len; ++i) {
            std::string substring = word.substr(i, len);
            substrings.push_back(substring);
        }
    }
}
void insere_substr(TRIE &trie,vector<string> &substr,int id)
{
    for(int i=0;i<substr.size();i++)
    {
        // if(id==20801)
        // {
        //     cout<<"inserindo "<<substr[i]<<endl;
        //     system("pause");
        // }
        trie.insere(trie.get_raiz(),substr[i],id,0);
    }
}



void printa_prefixo(TRIE &trie, HashTable<JOGADOR> &hash_id, string prefixo)
{
    vector<int> ids; 
    trie.get_jogadores_prefixo(trie.get_raiz(),prefixo,0,ids);
    for(int i=0;i<ids.size();i++)
    {
        cout<<"id: "<<ids[i]<<" nome: "<<hash_id.busca_jogador(ids[i]).nome<<" posicao: "<<hash_id.busca_jogador(ids[i]).posicoes[0]<<" rating: "<<hash_id.busca_jogador(ids[i]).avaliacao<<" count: "<<hash_id.busca_jogador(ids[i]).num_avaliacoes<<endl;
    }
}


vector<std::string> tokenize(const string& input, const string& delimiters) {
    vector<std::string> tokens;
    stringstream ss(input);
    string token;

    while (getline(ss, token)) {
        size_t pos = 0;
        while ((pos = token.find_first_of(delimiters)) != string::npos) {
            if (pos > 0) {
                tokens.push_back(token.substr(0, pos));
            }
            token.erase(0, pos + 1);
        }
        if (!token.empty() && token != " " && token != "\n" && token != "") {
            tokens.push_back(token);
        }
    }

    return tokens;
}

void processa_tags(HashTable<JOGADOR> &hash_id,TRIE &arvore) //insere as tags na hash e cria arquivo invertido binario
{
    
    ifstream arquivo_tags("./files/tags.csv");
    CsvParser parser_tags(arquivo_tags);
    string str_generica_tag;
    int id_jogador_generico;
    JOGADOR* generico_player;
    for(int i=0;i<4;i++)
        parser_tags.next_field();

    for(auto row:parser_tags)
    {
        id_jogador_generico=stoi(row[1]);
        str_generica_tag=row[2];
        
        generico_player=hash_id.busca_jogador_ref(id_jogador_generico);
        generico_player->tags.push_back(str_generica_tag);
        arvore.insere_ordenado(arvore.get_raiz(),str_generica_tag,id_jogador_generico,0);
    }

    arquivo_tags.close();
}

void preenche_hash_id(TRIE &trie, HashTable<JOGADOR> &hash_id,TRIE &substr)
{
    ifstream arquivo_players("./files/players.csv");
    CsvParser parser_players(arquivo_players);
    string str_generica;
    JOGADOR generico;
    vector<string> vetor_substr;
    for(int i=0;i<4;i++)
        parser_players.next_field();

    for(auto row:parser_players)
    {
        generico.id = stoi(row[0]);
        generico.nome = row[1];
        str_generica = row[2];
        generico.posicoes.clear();
        //retira espaços
        str_generica.erase(remove(str_generica.begin(), str_generica.end(), ' '), str_generica.end());

        generico.posicoes=tokenize(str_generica,",");
        hash_id.insere(generico);
        trie.insere(trie.get_raiz(),generico.nome, generico.id,0);
        // generateSubstrings(generico.nome,vetor_substr);
        // insere_substr(substr,vetor_substr,generico.id);
        vetor_substr.clear();        
    }
}

void atualiza_avaliacao(HashTable<JOGADOR> &hash_id, int id, float nota)
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

void preenche_hash_avaliacao(TRIE &trie, HashTable<JOGADOR> &hash_id, HashTable<USER> &hash_avaliacao)
{
    ifstream arquivo_notas(NAME_RATING);
    CsvParser parser_notas(arquivo_notas);
    JOGADOR jogador_generico;
    pair<int,float> par_generica;
    USER* pt_user;
    USER usuario_generico;
    int id_user;
    int i=0;
    int id_jogador_generico;
    float nota;
    vector<int> ids;
    for(int i=0;i<4;i++)
        parser_notas.next_field();
    for(auto row:parser_notas)
    {
        id_user = stoi(row[0]); //pega o id do usuario
        par_generica.first = stoi(row[1]); //pega o id do jogador
        par_generica.second = stof(row[2]); //pega a nota
        if(!hash_avaliacao.tem_user(id_user)) //se o usuario não estiver na hash
        {
            usuario_generico.num_avaliacoes++;
            // inserirOrdenado(usuario_generico.avaliacoes,make_tuple(stoi(row[1]),stoi(row[2])));
            // usuario_generico.avaliacoes.push_back(par_generica);
            hash_avaliacao.insere_user(usuario_generico);
            atualiza_avaliacao(hash_id,par_generica.first,par_generica.second);
        }
        else
        {
            pt_user=hash_avaliacao.busca_user_ref(usuario_generico.id_user);
            pt_user->num_avaliacoes++;
            // pt_user->avaliacoes.insert(par_generica);
            // inserirOrdenado(pt_user->avaliacoes,make_tuple(stoi(row[1]),stoi(row[2])));
            // pt_user->avaliacoes.push_back(par_generica);
            atualiza_avaliacao(hash_id,par_generica.first,par_generica.second);
            

        }
        usuario_generico.avaliacoes.clear();
        usuario_generico.num_avaliacoes=0;
    }
}

int position_to_index(const string& pos) {
	vector<string> all_positions = {"CAM", "CB", "CDM", "CF", "CM", "GK", "LB", "LM", "LW", "LWB", "RB", "RM", "RW", "RWB", "ST" };

	for (int i = 0; i < all_positions.size(); ++i) {
		if (all_positions[i] == pos)
			return i;
	}
	return -1;
}

void insert_players(vector<JOGADOR*> &players_in_position, JOGADOR* new_player){
    int start = 0;
    int end = players_in_position.size()-1;
    int insert_position = players_in_position.size();
 
    while (start <= end){
        int half = (start+end)/2;

        if(new_player->avaliacao > players_in_position.at(half)->avaliacao){
            insert_position = half;
            end = half-1;
        }
        else{
            start = half+1;
        }
    }

    players_in_position.insert(players_in_position.begin() + insert_position, new_player); 

}

void calcula_media(HashTable<JOGADOR> &hash_id, vector<JOGADOR*> players_in_position[],vector<HashTable<int>> &hash_posicoes) //faz a media das avaliações de cada jogador
{
    JOGADOR* pt_jg;

    JOGADOR* player;
    vector<int> player_positions_id;
    int size=0;

    for(int i=0;i<TAM_HASH_PLAYERS;i++) //percorre a tabela hash
    {
        if(hash_id.get_vetor(i).size()>=1)//se a posição não estiver vazia
        {
            for(int j=0;j<hash_id.get_vetor(i).size();j++) //percorre a lista
            {
                pt_jg=hash_id.busca_jogador_ref(hash_id.get_vetor(i)[j].id); //pega o ponteiro para o jogador
                if(pt_jg->num_avaliacoes>0)
                {
                    pt_jg->avaliacao = pt_jg->avaliacao/pt_jg->num_avaliacoes;

                    player = pt_jg;
                    // COLOCAR ESSA PARTE AQUI QUANDO FOR TESTAR NO RATING GRANDE

                     if(pt_jg->num_avaliacoes > 1000){
                        //cout << "jogador: " << player->nome << endl;
                        for(int k=0;k<player->posicoes.size();k++){    
                            //cout << player->posicoes[k] << endl;

                            // player_positions_id.push_back(position_to_index(player->posicoes[k]));
                            
                            int pos_id = position_to_index(player->posicoes[k]);

                            hash_posicoes[pos_id].insere(player->id);

                            //cout << player_positions_id.at(k) << endl;
                        }
                    
                        // for (int pos_id : player_positions_id){
                        //     if(pos_id < 0 || pos_id >= NUM_POSITIONS){
                        //         cout << "Invalid player position with";
                        //     }

                        //     insert_players(players_in_position[pos_id], pt_jg);
                        // }
                        
                        // player_positions_id.clear();
                    }               
                    

                    // for(int k=0;k<player->posicoes.size();k++){  //percorre as posições do jogador  
                    //     player_positions_id.push_back(position_to_index(player->posicoes[k]));  //pega o id da posição
                    // }
                    
                    // for (int pos_id : player_positions_id){
                    //     if(pos_id < 0 || pos_id >= NUM_POSITIONS){
                    //         cout << "Invalid player position with";
                    //     }
                    //     insert_players(players_in_position[pos_id], pt_jg); //insere o jogador na posição
                    // }
                        
                    // player_positions_id.clear();

                    if(pt_jg->avaliacao>5)
                    {
                        cout<<"erro nos calculos no jogador: "<<pt_jg->nome<<endl;
                    }
                }
            }
        }
    }
}

void answer_top_query(vector<JOGADOR*> &players_in_position, int n_top){

    JOGADOR* teste;

    if (n_top > players_in_position.size()){
        n_top = players_in_position.size();
    }

    for (int k=0; k < n_top; k++){
        teste = players_in_position.at(k);
        cout << "ID: " << teste->id << endl;
        cout << "Name: " << teste->nome << endl;
        cout << "Positions: ";
        for (int position = 0; position < teste->posicoes.size(); position++){
            if (position+1 == teste->posicoes.size()){
                cout << teste->posicoes[position]<<" ";
            }
            else
                cout << teste->posicoes[position]<<", ";
        }
        cout << endl;
        cout << "Rating: " << teste->avaliacao << endl;
        cout << "Count: " << teste->num_avaliacoes << endl;
        cout << endl;
    } 
}

int main()
{
    TRIE trie_ids;
    TRIE arvore_tags;
    TRIE arvore_prefixos;
    HashTable<JOGADOR> hash_id(TAM_HASH_PLAYERS);
    JOGADOR generico;
    HashTable<USER> hash_usuarios(TAM_HASH_AVALIACAO);
    string generica;
    string opcao;
    vector<int> ids;
    int id_user;
    //vector<vector<JOGADOR>> players_in_position;
    vector<JOGADOR*> players_in_position[NUM_POSITIONS]; //vetor de vetores de jogadores, cada vetor representa uma posição

    //vetor de tabelas hash para as posicoes, cada uma com TAM_HASH_PLAYERS posições
    vector<HashTable<int>> hash_posicoes(NUM_POSITIONS,HashTable<int>(TAM_HASH_PLAYERS));

    //JOGADOR* teste;

    auto t_start = std::chrono::high_resolution_clock::now();
    preenche_hash_id(trie_ids,hash_id,arvore_prefixos);
    auto t_end1 = std::chrono::high_resolution_clock::now();
    double time1 = std::chrono::duration<double, std::milli>(t_end1-t_start).count();
    cout << "Preprocessing time_hash_id: " << time1 << endl;

    auto t_start2 = std::chrono::high_resolution_clock::now();    
    preenche_hash_avaliacao(trie_ids,hash_id,hash_usuarios);
    auto t_end2 = std::chrono::high_resolution_clock::now();
    double time2 = std::chrono::duration<double, std::milli>(t_end2-t_start2).count();
    cout << "Preprocessing time_hash_avaliacao: " << time2 << endl;

    auto t_start3 = std::chrono::high_resolution_clock::now();
    calcula_media(hash_id, players_in_position,hash_posicoes);
    auto t_end3 = std::chrono::high_resolution_clock::now();
    double time3 = std::chrono::duration<double, std::milli>(t_end3-t_start3).count();
    cout << "Preprocessing time_calcula_media: " << time3 << endl;

    auto t_start4 = std::chrono::high_resolution_clock::now();
    processa_tags(hash_id,arvore_tags);
    auto t_end4 = std::chrono::high_resolution_clock::now();
    double time4 = std::chrono::duration<double, std::milli>(t_end4-t_start4).count();
    cout << "Preprocessing time_processa_tags: " << time4 << endl;


    //sort_players_positions(players_in_position);

    auto t_end = std::chrono::high_resolution_clock::now();
    double time = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    cout << "Preprocessing time: " << time << endl;
      
    do{
        cout << "Query [player,user,topN,tags,substr,quit]:\n";
        string opcao;
		getline(cin, opcao);

        if(opcao=="clear")
        {
            system("cls");
            continue;
        }
        // input example: player lionel finds all players with that prefix

        int first_space = opcao.find_first_of(' ');
        if(first_space == -1){
            cout << "Try again, input not specified\n";
            continue;
        } 

        // Separates the input
        string until_first_space = opcao.substr(0, first_space);
        string param_str = opcao.substr(first_space + 1);

        // to see all players, input ex: player ' ' -> space
        if (until_first_space == "player"){
            printa_prefixo(trie_ids,hash_id,param_str);
        }

        else if (until_first_space == "user"){
            int id_user = stoi(param_str);

            USER usuario=hash_usuarios.busca_user(id_user);
            JOGADOR jogador_generico;
            //coloca os ids avaliados pelo usuario em um vetor
            //printa os jogadores com esses ids
            if(usuario.id_user == -1)
            {
                cout<<"usuario nao encontrado"<<endl;
                continue;
            }
            // hash_usuarios.printa_user(usuario);
            for(int i=0;i<usuario.avaliacoes.size();i++) //percorre as avaliações do usuario
            {
                // jogador_generico=hash_id.busca_jogador(get<0>(usuario.avaliacoes[i]));
                cout<<"nome: "<<jogador_generico.nome;
                cout<<" global rating: "<<jogador_generico.avaliacao;
                cout<<" count: "<<jogador_generico.num_avaliacoes;
                // cout<<" rating: "<<get<1>(usuario.avaliacoes[i])<<endl;
            }
        }

        else if (until_first_space.find("top") == 0){
            string n_top_str = until_first_space.substr(3);
            int n_top = std::stoi(n_top_str);
            int pos_id=position_to_index(param_str);

            answer_top_query(players_in_position[pos_id], n_top);
        }

        else if (until_first_space == "tags"){
            // tags \o/
            cout<<"procurando tags ";
            vector<string> procura=tokenize(param_str,"'");
            for(int i=0;i<procura.size();i++)
            {
                if(procura[i]==" ")
                {
                    procura.erase(procura.begin()+i);
                }
                cout<<procura[i]<<" ";
            }
            cout<<endl;
            vector<vector<int>> ids_tags;
            vector<int> aux;
            vector<int>intersection;
            cout<<procura.size()<<endl;
            for(int i=0;i<procura.size();i++)
            {
                cout<<"procurando "<<procura[i]<<endl;
                arvore_tags.acha_palavra(arvore_tags.get_raiz(),procura[i],0,aux);
                ids_tags.push_back(aux);
                aux.clear();
            }
            intersection=intersectionOfVectors(ids_tags);
            ids_tags.clear();
            for(int i=0;i<intersection.size();i++)
            {
                cout<<"id: "<<intersection[i]<<" nome: "<<hash_id.busca_jogador(intersection[i]).nome<<endl;
            }
            intersection.clear();
            
        }
        else if(until_first_space=="substr")
        {
            vector<int> ids_substr;
            arvore_prefixos.acha_palavra(arvore_prefixos.get_raiz(),param_str,0,ids_substr);
            for(int i=0;i<ids_substr.size();i++)
            {
                cout<<"id: "<<ids_substr[i]<<" nome: "<<hash_id.busca_jogador(ids_substr[i]).nome<<endl;
            }
        }

        else if (until_first_space == "quit"){
            break;
        }

        else{
            cout << "Invalid input\n";
        }

    }while (opcao != "sair");
    cout << "Ending operation\n";
      
    

    cout<<"\noi\n";
    return 0;
}