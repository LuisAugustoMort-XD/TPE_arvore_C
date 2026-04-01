#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


typedef struct Produto {
    int id;
    char nome[50];
    float preco;
} Produto;


typedef struct NoID {
    Produto *prod;
    struct NoID *esq, *dir;
} NoID;


typedef struct NoPreco {
    Produto *prod;
    struct NoPreco *esq, *dir;
} NoPreco;


Produto* criarProduto(int id, char nome[], float preco) {
    Produto *p = (Produto*) malloc(sizeof(Produto));
    p->id = id;
    strcpy(p->nome, nome);
    p->preco = preco;
    return p;
}

int comparar(Produto *a, Produto *b) {
    if ( a -> preco < b -> preco) return -1;
    if ( a -> preco > b ->preco) return 1;

    if (a -> id < b->id) return -1;
    if (a -> id > b -> id) return 1;

    return 0;
}


NoID* inserirID(NoID *raiz, Produto *p) {
    if (raiz == NULL) {
        NoID *novo = (NoID*) malloc(sizeof(NoID));
        novo->prod = p;
        novo->esq = novo->dir = NULL;
        return novo;
    }

    if (p->id < raiz->prod->id)
        raiz->esq = inserirID(raiz->esq, p);
    else
        raiz->dir = inserirID(raiz->dir, p);

    return raiz;
}


NoPreco* inserirPreco(NoPreco *raiz, Produto *p) {
    if (raiz == NULL) {
        NoPreco *novo = (NoPreco*) malloc(sizeof(NoPreco));
        novo->prod = p;
        novo->esq = novo->dir = NULL;
        return novo;
    }

    if (comparar(p, raiz -> prod) < 0)
        raiz->esq = inserirPreco(raiz->esq, p);
    else
        raiz->dir = inserirPreco(raiz->dir, p);

    return raiz;
}


Produto* buscarID(NoID *raiz, int id) {
    if (raiz == NULL) return NULL;

    if (id == raiz->prod->id)
        return raiz->prod;
    else if (id < raiz->prod->id)
        return buscarID(raiz->esq, id);
    else
        return buscarID(raiz->dir, id);
}


void buscarFaixa(NoPreco *raiz, float min, float max) {
    if (raiz == NULL) return;

    if (raiz->prod->preco > min)
        buscarFaixa(raiz->esq, min, max);

    if (raiz->prod->preco >= min && raiz->prod->preco <= max)
        printf("%s - %.2f\n", raiz->prod->nome, raiz->prod->preco);

    if (raiz->prod->preco < max)
        buscarFaixa(raiz->dir, min, max);
}


void maisProximo(NoPreco *raiz, float alvo, Produto **melhor, float *dif) {
    if (raiz == NULL) return;

    float atual = fabs(raiz->prod->preco - alvo);

    if (atual < *dif) {
        *dif = atual;
        *melhor = raiz->prod;
    }

    if (alvo < raiz->prod->preco)
        maisProximo(raiz->esq, alvo, melhor, dif);
    else
        maisProximo(raiz->dir, alvo, melhor, dif);
}

NoID* menorNoID(NoID* raiz) {
    while (raiz -> esq != NULL)
        raiz = raiz -> esq;
    return raiz;
}

NoID* removerID(NoID* raiz, int id ) {
    if (raiz == NULL) return NULL;

    if (id < raiz -> prod -> id)
        raiz -> esq = removerID(raiz -> esq, id);
    else if (id > raiz -> prod -> id)
        raiz -> dir = removerID(raiz->dir,id);
    else {
        if (raiz -> esq == NULL) {
            NoID* temp = raiz ->dir;
            free(raiz);
            return(temp);
        }
        else if (raiz -> dir == NULL) {
            NoID* temp = raiz -> esq;
            free(raiz);
            return temp;
        }
        NoID* temp = menorNoID(raiz -> dir);
        raiz -> prod = temp -> prod;
        raiz -> dir = removerID(raiz -> dir, temp -> prod ->id);
    }
    return raiz;
}

NoPreco* removerPreco(NoPreco* raiz, Produto *p) {
    if (raiz == NULL) return NULL;

    int cmp = comparar(p, raiz->prod);

    if (cmp < 0)
        raiz->esq = removerPreco(raiz->esq, p);
    else if (cmp > 0)
        raiz->dir = removerPreco(raiz->dir, p);
    else {
        if (raiz->esq == NULL) {
            NoPreco* temp = raiz->dir;
            free(raiz);
            return temp;
        }
        else if (raiz->dir == NULL) {
            NoPreco* temp = raiz->esq;
            free(raiz);
            return temp;
        }

        NoPreco* temp = menorNoPreco(raiz->dir);
        raiz->prod = temp->prod;
        raiz->dir = removerPreco(raiz->dir, temp->prod);
    }

    return raiz;
}



void inserirProduto(NoID **idRoot, NoPreco **precoRoot, Produto *p) {
    *idRoot = inserirID(*idRoot, p);
    *precoRoot = inserirPreco(*precoRoot, p);
}

void removerProduto(NoID **raizID, NoPreco **raizPreco, int id) {
    Produto *p =  buscarID(*raizID, id);

    if (p == NULL) {
        printf("Produto nao encontrado! \n");
        return;
    }
    *raizID = removerID(*raizID, id);
    *raizPreco = removerPreco(*raizPreco, p);

    free(p);

    printf("Produto removido com sucesso!\n");
}


int main() {
    NoID *raizID = NULL;
    NoPreco *raizPreco = NULL;
int opcao;

    do {
        printf("\n====== MENU ======\n");
        printf("1 - Inserir produto\n");
        printf("2 - Buscar por ID\n");
        printf("3 - Buscar por faixa de preco\n");
        printf("4 - Buscar preco mais proximo\n");
        printf("5 - Remover produto\n");
        printf("0 - Sair\n");
        printf(" Escolha: \n");
        scanf("%d", &opcao);

        if (opcao == 1) {
            int id;
            char nome[50];
            float preco;

            printf("ID: ");
            scanf("%d", &id);
            getchar();
            printf("Nome: ");
            scanf("%[^\n]", nome);

            printf("Preco: ");
            scanf("%f", &preco);

            Produto *p = criarProduto(id, nome, preco);
            inserirProduto(&raizID, &raizPreco, p);

            printf("Produto inserido com sucesso! \n");
        }
        else if (opcao == 2) {
            int id;
            printf("Digite o ID: ");
            scanf("%d", &id);

            Produto *p = buscarID(raizID, id);

            if (p)
                printf("Encontrando: %s - %.2f\n", p -> nome, p -> preco);
            else
                printf("Prouto nao encontrado. \n");
        }
        else if (opcao == 3) {
            float min, max;
            printf("Preco minimo: ");
            scanf("%f",&min );

            printf("Preco maximo: \n");
            getchar();
            scanf("%f", &max);

            printf("produtos na faixa: \n");
            buscarFaixa(raizPreco, min, max);
        }
        else if (opcao == 4) {
            float alvo;
            printf("Digite o preco alvo:");
            scanf("%f", &alvo);

            Produto *melhor = NULL;
            float dif = 999999;

            maisProximo(raizPreco, alvo, &melhor, &dif);

            if (melhor)
                printf("Mais proximo; %s - %.2f\n", melhor->nome, melhor->preco);
            else
                printf("Nenhum produto encontrado");
        }

        else if ( opcao == 5) {
            int id;
            printf("ID para remover: ");
            scanf("%d", &id);

            removerProduto(&raizID, &raizPreco, id);
        }
    } while (opcao != 0);


    return 0;

}

