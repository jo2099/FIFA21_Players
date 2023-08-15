#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include"parsing.hpp"
#include"trie.hpp"
#include"HashTable.hpp"
// #include"HashUsers.hpp"
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
#define TAM_HASH_AVALIACAO 4007

#define NUM_POSITIONS 15

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
        generateSubstrings(generico.nome,vetor_substr);
        insere_substr(substr,vetor_substr,generico.id);
        vetor_substr.clear();        
    }
}

void atualiza_avaliacao(HashTable<JOGADOR> &hash_id, int id, int nota)
{
    JOGADOR* pt_jg=hash_id.busca_jogador_ref(id);
    JOGADOR jogador_generico;
    jogador_generico=hash_id.busca_jogador(id);
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
        usuario_generico.id_user = stoi(row[0]); //pega o id do usuario
        if(hash_avaliacao.busca_user(usuario_generico.id_user).id_user == -1) //se o usuario não estiver na hash
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

void calcula_media(HashTable<JOGADOR> &hash_id) //faz a media das avaliações de cada jogador
{
    JOGADOR* pt_jg;
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
                    if(pt_jg->avaliacao>5)
                    {
                        cout<<"erro nos calculos no jogador: "<<pt_jg->nome<<endl;
                    }
                }
            }
        }
    }
}




/*
// -> ordena todas as posições por rating
void classify_positions(){

    // para todos os jogadores
        // se é NULL continue
        // se tem menos de 1000 ratings count continue

        // for(pos_id : player->position_ids) // -> pra cada posição que o jogador tem
            // testa se a posição é valida

            //coloca o player na lista ordenada da sua posição conforme o rating

    for (int id_player : class?.player_ids){ //-> player_ids = todos os jogadores lidos
        JOGADOR* player = hash_id.busca_jogador_ref(id_player);

        if(player == NULL) continue;
        if(player->avaliacao < 1000) continue;

        for(int pos_id : player->position_ids){
            if(pos_id < 0 || pos_id >= NUM_POSITIONS){
                cout << "Invalid player position with";
            }

            rating = player->avaliacao;
            class?.players_in_position[pos_id].*insere ordenado*(rating, id_user);
        }

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

void answer_top_position(int n_top, string param_str){

    //param_str.erase(remove(param_str.begin(), param_str.end(), ' '), param_str.end());
	//param_str.erase(remove(param_str.begin(), param_str.end(), '\''), param_str.end());

    int i_pos = position_to_index(param_str);
	if (i_pos == -1) {
		cout << "Did not find the position <" << param_str << ">.\n";
		return;
	}

    //se tem menos players que o n dado
    /*
    if (n_top > class?.players_in_position[i_pos].size()){
        n_top = class?.players_in_position[i_pos].size();
        cout << "There are only " << n_top << " players in this position (w/ +1000 ratings).\n";
    }
    */

    /*
    cout << "Top " << n_top << "of position '" << param_str << "':\n";
    for (int i = 0; i < n_top; ++i) {
        int id_player = class?.players_in_position[i_pos].[i];
        JOGADOR player = busca_jogador(id_player);
        printa_jogador(player);
    }

*/



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

    auto t_start = std::chrono::high_resolution_clock::now();
    preenche_hash_id(trie_ids,hash_id,arvore_prefixos);
    // system("Pause");
    preenche_hash_avaliacao(trie_ids,hash_id,hash_usuarios);
    calcula_media(hash_id);
    processa_tags(hash_id,arvore_tags);
    //classify_positions() 

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
            hash_usuarios.printa_user(usuario);
            for(int i=0;i<usuario.avaliacoes.size();i++) //percorre as avaliações do usuario
            {
                jogador_generico=hash_id.busca_jogador(get<0>(usuario.avaliacoes[i]));
                cout<<"nome: "<<jogador_generico.nome;
                cout<<" global rating: "<<jogador_generico.avaliacao;
                cout<<" count: "<<jogador_generico.num_avaliacoes;
                cout<<" rating: "<<get<1>(usuario.avaliacoes[i])<<endl;
            }
        }

        else if (until_first_space.find("top") == 0){
            string n_top_str = until_first_space.substr(3);
            int n_top = std::stoi(n_top_str);

            cout << n_top << endl;
            cout << param_str << endl;

           // answer_top_position(/*passar o vetor de todas as posições ordenadas*/, n_top, param_str);
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