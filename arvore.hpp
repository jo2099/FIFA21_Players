#include<iostream>
#include<vector>

using namespace std;


template <typename T>

class RubroNegra
{
private:
    
    struct NO
    {
        T valor;
        NO *esquerda;
        NO *direita;
        NO *pai;
        bool cor; // 0 = preto, 1 = vermelho
    };

    NO *raiz;
    
public:
    RubroNegra(/* args */);
    ~RubroNegra();
    void inserir(T valor);
    // void remover(T valor);
    void imprimir();
    void imprimir(NO *no);
    void rotacaoEsquerda(NO *no);
    void rotacaoDireita(NO *no);
    // void rotacaoEsquerdaDireita(NO *no);
    // void rotacaoDireitaEsquerda(NO *no);
    void inserirCorrecao(NO *raiz, NO *no);
    NO *getRaiz() { return raiz; };
    int buscar(T valor);
    void retorna_valores(NO *no, vector<T> &vetor);
};

template <typename T>
RubroNegra<T>::RubroNegra(/* args */)
{
    raiz = NULL;
}

template <typename T>
RubroNegra<T>::~RubroNegra()
{
}


template <typename T>
void RubroNegra<T>::rotacaoDireita(NO *no)
{
    NO *aux = no->esquerda;
    no->esquerda = aux->direita;

    if (aux->direita != NULL)
    {
        aux->direita->pai = no;
    }

    aux->pai = no->pai;

    if (no->pai == NULL)
    {
        raiz = aux;
    }
    else if (no == no->pai->esquerda)
    {
        no->pai->esquerda = aux;
    }
    else
    {
        no->pai->direita = aux;
    }

    aux->direita = no;
    no->pai = aux;
}

template <typename T>
void RubroNegra<T>::rotacaoEsquerda(NO *no)
{
    NO *aux = no->direita;
    no->direita = aux->esquerda;

    if (aux->esquerda != NULL)
    {
        aux->esquerda->pai = no;
    }

    aux->pai = no->pai;

    if (no->pai == NULL)
    {
        raiz = aux;
    }
    else if (no == no->pai->esquerda)
    {
        no->pai->esquerda = aux;
    }
    else
    {
        no->pai->direita = aux;
    }

    aux->esquerda = no;
    no->pai = aux;
}

template <typename T>
void RubroNegra<T>::inserirCorrecao(NO *raiz, NO *no)
{
    if (no->pai == NULL)
    {
        no->cor = 0;
    }
    else if (no->pai->cor == 0)
    {
        return;
    }
    else if (no->pai->cor == 1)
    {
        NO *tio = NULL;
        NO *avo = NULL;

        while (no->pai != NULL && no->pai->cor == 1)
        {
            avo = no->pai->pai;
            if (avo->esquerda == no->pai)
            {
                tio = avo->direita;
                if (tio != NULL && tio->cor == 1)
                {
                    no->pai->cor = 0;
                    tio->cor = 0;
                    avo->cor = 1;
                    no = avo;
                }
                else
                {
                    if (no->pai->direita == no)
                    {
                        no = no->pai;
                        rotacaoEsquerda(no);
                    }
                    no->pai->cor = 0;
                    avo->cor = 1;
                    rotacaoDireita(avo);
                }
            }
            else
            {
                tio = avo->esquerda;
                if (tio != NULL && tio->cor == 1)
                {
                    no->pai->cor = 0;
                    tio->cor = 0;
                    avo->cor = 1;
                    no = avo;
                }
                else
                {
                    if (no->pai->esquerda == no)
                    {
                        no = no->pai;
                        rotacaoDireita(no);
                    }
                    no->pai->cor = 0;
                    avo->cor = 1;
                    rotacaoEsquerda(avo);
                }
            }
        }
    }
    raiz->cor = 0;
}


template<>
void RubroNegra<tuple<int,float>>::inserir(tuple<int,float> valor)
{
    NO *novo = new NO();
    novo->valor = valor;
    novo->esquerda = NULL;
    novo->direita = NULL;
    novo->pai = NULL;
    novo->cor = 1;

    if (raiz == NULL)
    {
        raiz = novo;
    }
    else
    {
        NO *atual = raiz;
        NO *anterior = NULL;

        while (atual != NULL)
        {
            anterior = atual;
            if (get<1>(valor) < get<1>(atual->valor))
            {
                atual = atual->esquerda;
            }
            else
            {
                atual = atual->direita;
            }
        }

        novo->pai = anterior;
        if (get<1>(valor) < get<1>(anterior->valor))
        {
            anterior->esquerda = novo;
        }
        else
        {
            anterior->direita = novo;
        }
    }

    inserirCorrecao(raiz, novo);
}

template <typename T>
void RubroNegra<T>::inserir(T valor)
{
    NO *novo = new NO();
    novo->valor = valor;
    novo->esquerda = NULL;
    novo->direita = NULL;
    novo->pai = NULL;
    novo->cor = 1;

    if (raiz == NULL)
    {
        raiz = novo;
    }
    else
    {
        NO *atual = raiz;
        NO *anterior = NULL;

        while (atual != NULL)
        {
            anterior = atual;
            if (valor < atual->valor)
            {
                atual = atual->esquerda;
            }
            else
            {
                atual = atual->direita;
            }
        }

        novo->pai = anterior;
        if (valor < anterior->valor)
        {
            anterior->esquerda = novo;
        }
        else
        {
            anterior->direita = novo;
        }
    }

    inserirCorrecao(raiz, novo);
}


template <typename T>
void RubroNegra<T>::imprimir(NO *no)
{
    if (no != NULL)
    {
        imprimir(no->esquerda);
        cout << no->valor << " ";
        imprimir(no->direita);
    }
}

template <typename T>
void RubroNegra<T>::imprimir()
{
    imprimir(raiz);
}

template <typename T>
int RubroNegra<T>::buscar(T valor)
{
    NO *atual = raiz;
    while (atual != NULL)
    {
        if (atual->valor == valor)
        {
            return 1;
        }
        else if (valor < atual->valor)
        {
            atual = atual->esquerda;
        }
        else
        {
            atual = atual->direita;
        }
    }
    return 0;
}

template <typename T>
void RubroNegra<T>::retorna_valores(NO *no, vector<T> &vetor)
{
    if (no != NULL)
    {
        //insere os valores em ordem decrescente(caminhamento central à direita)
        retorna_valores(no->direita, vetor);
        vetor.push_back(no->valor);
        retorna_valores(no->esquerda, vetor);
    }
}
