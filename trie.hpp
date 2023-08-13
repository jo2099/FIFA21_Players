#include<iostream>
#include<cstdio>
#include<fstream>
#include<string>
#include<vector>

/*  
    0123
    JOEL
*/
using namespace std;

class TRIE
{
private:
    struct trie_node
    {
        char letra=' ';
        vector<trie_node*> filhos={NULL};
        bool fim_palavra;
        vector<int> ids;
    };
    int num_palavras;
    int num_nodos;
    trie_node* raiz;
public:
    TRIE();
    ~TRIE();
    void insere(trie_node* raiz,string palavra, int id,int pos_char);
    void insere_ordenado(trie_node* raiz,string palavra, int id,int pos_char);
    bool tem_filho_char(trie_node* nodo, char letra); //retorna true se o nodo tem um filho com a letra
    void printa_arvore(trie_node* nodo,int indice,string generica);
    trie_node* get_raiz(){return raiz;};
    int get_num_palavras(){return num_palavras;};
    int get_num_nodos(){return num_nodos;};
    bool acha_palavra(trie_node* nodo, string palavra, int pos_char, vector<int> &ids);
    void get_jogadores(trie_node* nodo, vector<int> &jogadores);
    void printa_nodo(trie_node* nodo);
    trie_node* acha_prefixo(trie_node* nodo, string prefixo, int pos_char);
    trie_node* acha_nodo(trie_node* nodo, string palavra, int pos_char);
    void get_jogadores_prefixo(trie_node* nodo, string prefixo, int pos_char, vector<int> &jogadores);
};

template <typename T>
bool esta_vetor(vector<T> vetor, T elemento)
{
    for(int i=0;i<vetor.size();i++)
    {
        if(vetor[i] == elemento)
            return true;
    }
    return false;
}

bool esta_vetor_ordenado(vector<int> vetor, int elemento)
{
    //busca binaria para verificar se o elemento esta no vetor
    int inicio = 0;
    int fim = vetor.size()-1;
    int meio;

    while(inicio <= fim)
    {
        meio = (inicio+fim)/2;
        if(vetor[meio] == elemento)
        {
            return true;
        }
        else if(vetor[meio] < elemento)
        {
            inicio = meio+1;
        }
        else
        {
            fim = meio-1;
        }
    }
    return false;
}

void insere_binary_search(vector<int> &vetor,int elemento)
{
    //insere o elemento no vetor de forma ordenada
    int inicio = 0;
    int fim = vetor.size()-1;
    int meio;

    while(inicio <= fim)
    {
        meio = (inicio+fim)/2;
        if(vetor[meio] == elemento)
        {
            return;
        }
        else if(vetor[meio] < elemento)
        {
            inicio = meio+1;
        }
        else
        {
            fim = meio-1;
        }
    }
    vetor.insert(vetor.begin()+inicio,elemento);
    return;
}

TRIE::TRIE()
{
    raiz = new trie_node;
    raiz->letra = ' ';
    raiz->fim_palavra = false;
    num_palavras = 0;
    num_nodos = 1;
    raiz->ids.clear();
    raiz->filhos.clear();

}

TRIE::~TRIE()
{
    delete raiz;
}

void TRIE::printa_nodo(trie_node* nodo)
{
    cout<<"letra: "<<nodo->letra;
    cout<<" fim_palavra: "<<nodo->fim_palavra;
    cout<<" ids: ";
    for (int i = 0; i < nodo->ids.size(); i++)
    {
        cout<<nodo->ids[i]<<" ";
    }
    cout<<" filhos: ";
    for (int i = 0; i < nodo->filhos.size(); i++)
    {
        cout<<nodo->filhos[i]->letra<<" ";
    }
    cout<<endl;
}

bool TRIE::tem_filho_char(trie_node* nodo, char letra)
{
    for (int i = 0; i < nodo->filhos.size(); i++)
    {
        if (nodo->filhos[i]->letra == letra)
        {
            return true;
        }
        // system("pause");
    }
    return false;
}

void TRIE::insere(trie_node* raiz,string palavra,int id,int pos_char) //recebe a raiz da arvore, a palavra que deseja inserir e o id do jogador.pos_char é a posição do char que está sendo analisado
{
    if(pos_char == palavra.size()) //se pos_char for igual ao tamanho da palavra, significa que a palavra acabou e deve ser marcado o fim da palavra
    {
        raiz->fim_palavra = true;
        if(!esta_vetor(raiz->ids,id))
            raiz->ids.push_back(id);
        num_palavras++;
        return;
    }
    else if(tem_filho_char(raiz,palavra[pos_char])) //se o nodo tiver um filho com a letra da palavra, chama a função recursivamente para o filho e passa pos_char+1
    {
        for (int i = 0; i < raiz->filhos.size(); i++) //percorre os filhos do nodo
        {
            if(raiz->filhos[i]->letra == palavra[pos_char]) //se o filho tiver a letra da palavra, chama a função recursivamente para o filho e passa pos_char+1
            {
                insere(raiz->filhos[i],palavra,id,pos_char+1);
                return;
            }
        }
    }
    else //se o nodo não tiver um filho com a letra da palavra, cria um novo nodo e chama a função recursivamente para o novo nodo e passa pos_char+1
    {
        trie_node* novo = new trie_node;
        novo->letra = palavra[pos_char];
        novo->fim_palavra = false;
        novo->ids.clear();
        novo->filhos.clear();
        raiz->filhos.push_back(novo);
        num_nodos++;
        insere(novo,palavra,id,pos_char+1);
        return;
    }
    
}


void TRIE::insere_ordenado(trie_node* raiz,string palavra,int id,int pos_char) //recebe a raiz da arvore, a palavra que deseja inserir e o id do jogador.pos_char é a posição do char que está sendo analisado
{
    if(pos_char == palavra.size()) //se pos_char for igual ao tamanho da palavra, significa que a palavra acabou e deve ser marcado o fim da palavra
    {
        raiz->fim_palavra = true;
        if(!esta_vetor_ordenado(raiz->ids,id))
            insere_binary_search(raiz->ids,id);
        num_palavras++;
        return;
    }
    else if(tem_filho_char(raiz,palavra[pos_char])) //se o nodo tiver um filho com a letra da palavra, chama a função recursivamente para o filho e passa pos_char+1
    {
        for (int i = 0; i < raiz->filhos.size(); i++) //percorre os filhos do nodo
        {
            if(raiz->filhos[i]->letra == palavra[pos_char]) //se o filho tiver a letra da palavra, chama a função recursivamente para o filho e passa pos_char+1
            {
                insere_ordenado(raiz->filhos[i],palavra,id,pos_char+1);
                return;
            }
        }
    }
    else //se o nodo não tiver um filho com a letra da palavra, cria um novo nodo e chama a função recursivamente para o novo nodo e passa pos_char+1
    {
        trie_node* novo = new trie_node;
        novo->letra = palavra[pos_char];
        novo->fim_palavra = false;
        novo->ids.clear();
        novo->filhos.clear();
        raiz->filhos.push_back(novo);
        num_nodos++;
        insere_ordenado(novo,palavra,id,pos_char+1);
        return;
    }
    
}

void TRIE::printa_arvore(trie_node* nodo,int indice,string generica)
{
    if(nodo==NULL)
    {
        return;
    }
    if(nodo->fim_palavra)
    {   
        generica+=nodo->letra;
        cout<<generica<<endl;
        return;
    }
    else
    {
        generica+=nodo->letra;
        for(int i = 0; i < nodo->filhos.size(); i++)
        {
            printa_arvore(nodo->filhos[i],indice+1,generica);
        }
    }
    
    
}

bool TRIE::acha_palavra(trie_node* nodo, string palavra, int pos_char,vector<int> &ids) //funcao booleana que recebe a raiz da arvore, a palavra que deseja achar e a posicao do char que esta sendo analisado
{
    if(pos_char == palavra.size()) //se pos_char for igual ao tamanho da palavra, significa que a palavra acabou e deve ser marcado o fim da palavra
    {
        if(nodo->fim_palavra)
        {
            ids = nodo->ids;
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(tem_filho_char(nodo,palavra[pos_char])) //se o nodo tiver um filho com a letra da palavra, chama a função recursivamente para o filho e passa pos_char+1
    {
        for (int i = 0; i < nodo->filhos.size(); i++) //percorre os filhos do nodo
        {
            if(nodo->filhos[i]->letra == palavra[pos_char]) //se o filho tiver a letra da palavra, chama a função recursivamente para o filho e passa pos_char+1
            {
                return acha_palavra(nodo->filhos[i],palavra,pos_char+1,ids);
            }
        }
    }
    else //se o nodo não tiver um filho com a letra da palavra, retorna false
    {
        return false;
    }
    
}

TRIE::trie_node* TRIE::acha_prefixo(trie_node* nodo, string prefixo, int pos_char) //funcao que recebe a raiz da arvore, o prefixo que deseja achar e a posicao do char que esta sendo analisado e retorna o nodo que tem o ultimo char do prefixo
{
    if(pos_char == prefixo.size())
    {
        return nodo;
    }
    else if(nodo->filhos.size() == 0)
    {
        return NULL;
    }
    else
    {
        if(tem_filho_char(nodo,prefixo[pos_char]))
        {
            for (int i = 0; i < nodo->filhos.size(); i++)
            {
                if(nodo->filhos[i]->letra == prefixo[pos_char])
                {
                    return acha_prefixo(nodo->filhos[i],prefixo,pos_char+1);
                }
            }
        }
        else
        {
            return NULL;
        }
    }
    
}

void TRIE::get_jogadores(trie_node* nodo,vector<int> &jogadores)
{
    //retorna no vetor todos os ids dos jogadores a partir do nodo
    if(nodo->fim_palavra)
    {
        for (int i = 0; i < nodo->ids.size(); i++)
        {
            jogadores.push_back(nodo->ids[i]);
        }
    }
    for (int i = 0; i < nodo->filhos.size(); i++)
    {
        get_jogadores(nodo->filhos[i],jogadores);
    }
}

void TRIE::get_jogadores_prefixo(trie_node* nodo,string prefixo,int pos_char,vector<int> &jogadores)
{
    //localiza o nodo que representa o prefixo
    //printa prefixo+palavras
    trie_node* nodo_prefixo = acha_prefixo(nodo,prefixo,pos_char);
    // prefixo.pop_back(); //retira o ultimo char do prefixo pois ele ja foi analisado
    if(nodo_prefixo == NULL)
    {
        return;
    }
    else
    {
        get_jogadores(nodo_prefixo,jogadores);
    }
    
}

