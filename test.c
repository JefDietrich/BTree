//-------------------- Libraries ----------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//-------------------- Constants ----------------------------
#define MAXKEYS 3
#define MINKEYS 2
#define NIL (-1)
#define NOKEY '@'

//--------------------  Structs  ----------------------------
typedef struct {
    int quantidadeNos;
    char chaves[MAXKEYS];
    int filhos[MAXKEYS+1];
} BTPAGE;

#define PAGESIZE sizeof(BTPAGE)

//--------------------  Functions  ----------------------------
bool inserirNaArvore (int rrn, char proximaChave, int *rrnPromovido, char *chavePromovida);
bool abrirArquivoIndice();
void fecharArquivoIndice();
int recuperarRrnRaiz ();
void atualizarIndiceComNovaRaiz(int rrnRaiz);
int criarArvore();
int buscarQuantidadePagina ();
void lerPagina (int rrn, BTPAGE *pagina);
void escrevePagina (int rrn, BTPAGE *pagina);
int criarRaiz(char chave, int esquerda, int direita);
void iniciarPagina(BTPAGE *pagina);
bool buscarNo (char chave, BTPAGE *pagina, int *posicao);
void inserirNaPagina(char chave, int rrnPromovido, BTPAGE *pagina);
void split(char chave, int filhoDireita, BTPAGE *paginaDividida, char *chavePromovida, int *filhoDireitaChavePromovida, BTPAGE *novaPagina);

FILE* indice;

//-------------------- Main  ----------------------------
int main() {
    bool foiPromovido;
    int rrnRaiz, rrnPromovido;
    char chavePromovida, proximaChave;

    if (abrirArquivoIndice()) {
        rrnRaiz = recuperarRrnRaiz();
    } else {
        rrnRaiz = criarArvore();
    }
    fecharArquivoIndice();

    do {
        abrirArquivoIndice();
        scanf(" %c", &proximaChave);          
        foiPromovido = inserirNaArvore(rrnRaiz, proximaChave, &rrnPromovido, &chavePromovida);
        if (foiPromovido)
            rrnRaiz = criarRaiz(chavePromovida, rrnRaiz, rrnPromovido);

        fecharArquivoIndice();
    } while (proximaChave != 'q');    
}

/**
 * rrn: OffSet da página atual
 * proximaChave: Chave a ser inserida
 * rrnPromovido: Ponteiro para a variável que receberá o RRN da página promovida.
 * chavePromovida: Ponteiro para a variável que receberá a chave promovida.
 */
bool inserirNaArvore (int rrn, char proximaChave, int *rrnPromovido, char *chavePromovida) {
    BTPAGE paginaAtual, novaPagina;     // novaPagina: Apenas se houver promoção
    bool encontrado, promovido;         
    int posicao, rrnPromovidoDeBaixo;   // rrnPromovidoDeBaixo: Vem da chamada recursiva
    char chavePromovidaDeBaixo;         // chavePromovidaDeBaixo: Vem da chamada recursiva

    // Está em uma folha da árvore (seria a inserção do primeiro item?)
    if (rrn == NIL) {
        *chavePromovida = proximaChave;
        *rrnPromovido = NIL;
        return true;
    }

    // Lê a página e procura pela chave a ser inserida
    lerPagina(rrn, &paginaAtual);
    encontrado = buscarNo (proximaChave, &paginaAtual, &posicao);
    if (encontrado) {
        printf ("Error: attempt to insert duplicate key: %c \n\007", proximaChave);
        return false;
    }

    // Recursão para a página filha
    promovido = inserirNaArvore(paginaAtual.filhos[posicao], proximaChave, &rrnPromovidoDeBaixo, &chavePromovidaDeBaixo);

    // Não houve recursão
    if (!promovido) {
        return false;
    }

    // Inserção na página atual, pois ainda há espaço livre
    if(paginaAtual.quantidadeNos < MAXKEYS) {
        inserirNaPagina(chavePromovidaDeBaixo, rrnPromovidoDeBaixo, &paginaAtual);
        escrevePagina(rrn, &paginaAtual);
        return false;
    } else { // Mas caso não haja espaço, é feito o split
        split(chavePromovidaDeBaixo, rrnPromovidoDeBaixo, &paginaAtual, chavePromovida, rrnPromovido, &novaPagina);
        escrevePagina(rrn, &paginaAtual);
        escrevePagina(*rrnPromovido, &novaPagina);
        return true;
    }
}

bool abrirArquivoIndice() {
    indice = fopen("indice.bin", "r+b");
    return (indice > 0);
}

void fecharArquivoIndice() {
    fclose(indice);
}

// le o header do arquivo da arvore-B e recupera RRN da pagina raiz
int recuperarRrnRaiz() {
    int rrnRaiz;
    fseek(indice, 0, SEEK_SET); 
    fread(&rrnRaiz, sizeof(int), 1, indice);    
    return rrnRaiz;
}

// atualiza o header do arquivo da arvore-B com RRN da nova pagina raiz
void atualizarIndiceComNovaRaiz(int rrnRaiz) {
    fseek(indice, 0, SEEK_SET); 
    fwrite(&rrnRaiz, sizeof(int), 1, indice);
}

// cria o arquivo da arvore-B e insere primeira chave na primeira pagina
int criarArvore() {
    char chave;
    indice = fopen("indice.bin", "w+b"); // "w+b" cria uma nova página, mesmo que exista. "a+b" cria somente se não existir

    // lembrar de escrever um header = -1 para a posição já existir e ser possível
    fseek(indice, 0, SEEK_SET);
    int header = -1;
    fwrite(&header, sizeof(int), 1, indice);
    fclose(indice);

    abrirArquivoIndice();
    scanf(" %c", &chave);
    return (criarRaiz(chave, NIL, NIL));
}

int buscarQuantidadePagina() {
    long addr; 
    fseek(indice, 0, SEEK_END);
    addr = ftell(indice) - sizeof(int);
    
    return ((int) addr / PAGESIZE);
}

void lerPagina (int rrn, BTPAGE *pagina) {
    long addr;
    addr = (long)rrn * (long)PAGESIZE + sizeof(int);
    fseek(indice, addr, SEEK_SET);
    fread(pagina, sizeof(BTPAGE), 1, indice);
}

void escrevePagina(int rrn, BTPAGE *pagina) {
    long addr;
    addr = (long)rrn * (long)PAGESIZE + sizeof(int);
    fseek(indice, addr, SEEK_SET);
    printf("Nova pagina escrita na posicao %d\n", rrn);
    fwrite(pagina, sizeof(BTPAGE), 1, indice);
}

int criarRaiz(char chave, int esquerda, int direita) {
    BTPAGE page;
    int rrn = buscarQuantidadePagina();
    iniciarPagina(&page);
    page.chaves[0] = chave;
    page.filhos[0] = esquerda;
    page.filhos[1] = direita;
    page.quantidadeNos = 1;
    escrevePagina(rrn, &page);
    atualizarIndiceComNovaRaiz(rrn);
    return(rrn);
}

void iniciarPagina(BTPAGE *pagina) {
    int j;

    for (j = 0; j < MAXKEYS; j++){
        pagina->chaves[j] = NOKEY;
        pagina->filhos[j] = NIL;
    }

    pagina->filhos[MAXKEYS] = NIL;
}

// busca na pagina para verificar se a chave existe (posicao encontrada) ou nao existe (posicao que deveria estar)
bool buscarNo(char chave, BTPAGE *pagina, int *posicao) {
    int i;
    for (i = 0; i < pagina->quantidadeNos && chave > pagina->chaves[i]; i++);
    *posicao = i;
    return (*posicao < pagina->quantidadeNos && chave == pagina->chaves[*posicao]);
}

// faz insercao ordenada na pagina
void inserirNaPagina(char chave, int rrnPromovido, BTPAGE *pagina) {
    int j;

    for(j = pagina->quantidadeNos; chave < pagina->chaves[j-1] && j > 0; j--){
        pagina->chaves[j] = pagina->chaves[j-1];
        pagina->filhos[j+1] = pagina->filhos[j];
    }

    pagina->quantidadeNos++;
    pagina->chaves[j] = chave;
    pagina->filhos[j+1] = rrnPromovido;
}

// /**
//  * key: chave a ser inserida
//  * r_child: filho a direita
//  * p_oldpage: pagina a ser dividida
//  * promo_key: chave que será promovida
//  * promo_r_child: filho direito da pomo_key
//  * p_newpage: página criada
//  */
void split(char chave, int filhoDireita, BTPAGE *paginaDividida, char *chavePromovida, int *filhoDireitaChavePromovida, BTPAGE *novaPagina) {
    int j;
    char auxChaves[MAXKEYS+1];
    int auxFilhos[MAXKEYS+2];

    for (j = 0; j < MAXKEYS; j++){
        auxChaves[j] = paginaDividida->chaves[j];
        auxFilhos[j] = paginaDividida->filhos[j];
    }
    auxFilhos[MAXKEYS] = paginaDividida->filhos[MAXKEYS];

    // Deslocando as chaves para inserir uma nova
    for (j = MAXKEYS; chave < auxChaves[j-1] && j > 0; j--){
        auxChaves[j] = auxChaves[j-1];
        auxFilhos[j+1] = auxFilhos[j];
    }
    auxChaves[j] = chave;
    auxFilhos[j+1] = filhoDireita;

    *filhoDireitaChavePromovida = buscarQuantidadePagina();    
    iniciarPagina(novaPagina);

    for (j = 0; j < MINKEYS; j++){
        paginaDividida->chaves[j] = auxChaves[j]; // posições: 0 e 1
        paginaDividida->filhos[j] = auxFilhos[j]; // posições: 0 e 1

        novaPagina->chaves[j] = auxChaves[j + MINKEYS]; // posições: 2 e 3
        novaPagina->filhos[j] = auxFilhos[j + MINKEYS]; // posições: 2 e 3

        if((j + MINKEYS) < MAXKEYS) {
            paginaDividida->chaves[j + MINKEYS] = NOKEY; // posições: 2 e 3
        }
        paginaDividida->filhos[j + MINKEYS] = NIL; // posições: 2 e 3
    }
    novaPagina->filhos[MINKEYS] = auxFilhos[j + MINKEYS]; // posição: 4
        
    paginaDividida->chaves[1] = NOKEY;

    novaPagina->quantidadeNos = (MAXKEYS + 1) - MINKEYS;
    paginaDividida->quantidadeNos = MINKEYS - 1;

    *chavePromovida = auxChaves[1];
}
