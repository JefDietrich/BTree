/**********************************************************
 * 
 * Projeto 3:   Indexação - Árvore B
 * Docente:     Orlando de Andrade Figueiredo
 * 
 **********************************************************
 *
 * Integrantes: Jeferson Dietrich   RA: 221154231
 *              Larissa Ferrari     RA: 221151214
 *              Murilo Venturato    RA: 
 * 
 ***********************************************************/

//-------------------- Libraries ----------------------------
#include "header.h"

//-------------------- Main  ----------------------------
int main() {
    FILE *arquivo;

/* Registros a serem inseridos */

    arquivo = verificaArquivo("insere.bin");
    REGISTRO registrosInseridos[10];
    fread(registrosInseridos, sizeof(REGISTRO), 10, arquivo);
    fclose(arquivo);

/* Registros que serão pesquisados */

    arquivo = verificaArquivo("busca.bin");
    CHAVE_PRIMARIA buscaIndice[10];
    fread(buscaIndice, sizeof(CHAVE_PRIMARIA), 10, arquivo);
    fclose(arquivo);

/* Variáveis para usar posteriormente */

    BTPAGE pagina;
    iniciarPagina(&pagina);

/* Menu de Opções */
    int opcao, i;

    do
    {
        printf("+------------------------------------------+");
        printf("\nSelecione uma das opções abaixo:\n\n");
        printf("(1) Inserir registros.\n");
        printf("(2) Buscar por um cliente.\n");
        printf("(3) Listar os dados de todos os clientes.\n");
        printf("(0) Sair do programa.\n");
        printf("\n+------------------------------------------+\n");
        printf("Opção: ");
        scanf("%d", &opcao);

        /* Rotina de cada opção */
        switch (opcao)
        {
            case 1:
                while (1)
                {
                    printf("\nDigite '0' para sair...");
                    printf("\nInforme um número de 1 a 8: ");
                    scanf("%d", &i);

                    if (i == 0)
                        break;
                    if (i < 1 || i > 8)
                    {
                        printf("Opção inválida!");
                    }                    
                    else
                    {
                        
                    }
                }
                break;

            case 2:
                printf("\nPesquisando por um cliente...\n");
                break;

            case 3:
                printf("\nListando todos os clientes...\n");
                arquivo = verificaArquivo("resultado.bin");
                imprimeArvoreEmOrdem(recuperarRrnRaiz(), arquivo);
                fclose(arquivo);
                break;

            case 0:
                printf("\nSaindo do programa...\n\n");
                break;

            default:
                break;
        }
    } while (opcao != 0);
}

//--------------------  Creation Functions  ----------------------------

// cria o arquivo da arvore-B
int criarArvore() {
    char chave;
    indice = fopen("indice.bin", "w+b"); 
    
    fseek(indice, 0, SEEK_SET);
    int header = -1;
    fwrite(&header, sizeof(int), 1, indice);
    
    fclose(indice);
    return header;
}

// cria raiz
int criarRaiz(CHAVE_PAGINA chave, int esquerda, int direita) {
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

// inicializa pagina vazia
void iniciarPagina(BTPAGE *pagina) {
    int j;

    for (j = 0; j < MAXKEYS; j++){
        pagina->chaves[j] = criaNoKey();
        pagina->filhos[j] = NIL;
    }

    pagina->filhos[MAXKEYS] = NIL;
}

//--------------------  Insertion Function  ----------------------------

int insereRegistro(REGISTRO* novoRegistro, FILE* arquivo) {       
    if(arquivo == NULL) {
        arquivo = fopen("resultado.bin", "w+b");
    }

    char registro[130];
    sprintf(registro, "%s|%s|%s|%s|%s|",    novoRegistro->codCliente,
                                            novoRegistro->codVeiculo,
                                            novoRegistro->nomeCliente,
                                            novoRegistro->nomeVeiculo,
                                            novoRegistro->dia
    );

    int tamRegistro = strlen(registro);
    
    fseek(arquivo, 0, SEEK_END);
    int offSet = ftell(arquivo);    

    fwrite(registro, sizeof(char), tamRegistro, arquivo);
    
    printf("\nChave %s%s inserida com sucesso!", novoRegistro->codCliente, novoRegistro->codVeiculo);
    return offSet;
}

void inserirNaPagina(CHAVE_PAGINA chave, int rrnPromovido, BTPAGE *pagina) {
    int j;
        
    for(j = pagina->quantidadeNos; compararChaves(chave, pagina->chaves[j-1]) < 0 && j > 0; j--){
        pagina->chaves[j] = pagina->chaves[j-1];
        pagina->filhos[j+1] = pagina->filhos[j];
    }

    pagina->quantidadeNos++;
    pagina->chaves[j] = chave;
    pagina->filhos[j+1] = rrnPromovido;
}

bool inserirNaArvore (int rrn, CHAVE_PAGINA proximaChave, int *rrnPromovido, CHAVE_PAGINA *chavePromovida) {
    BTPAGE paginaAtual, novaPagina;     // novaPagina: Apenas se houver promoção
    bool encontrado, promovido;         
    int posicao, rrnPromovidoDeBaixo;   // rrnPromovidoDeBaixo: Vem da chamada recursiva
    CHAVE_PAGINA chavePromovidaDeBaixo; // chavePromovidaDeBaixo: Vem da chamada recursiva

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
        printf ("Erro ao inserir, chave já existe: %s%s \n\007", proximaChave.id.codCliente, proximaChave.id.codVeiculo);
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

//--------------------   Search Functions   ----------------------------

int buscaRegistroNaArvore(int rrn, CHAVE_PAGINA chave) {
    BTPAGE paginaAtual;
    bool encontrado;         
    int posicao;

    if (rrn == NIL) {        
        return -1;
    }
    
    lerPagina(rrn, &paginaAtual);
    encontrado = buscarNo(chave, &paginaAtual, &posicao);
    
    if (encontrado) {
        return chave.rrn;
    }

    return buscaRegistroNaArvore(paginaAtual.filhos[posicao], chave);
}

void buscaRegistroRRN(int rrnArquivoResultado, FILE* arquivo) {
    int tamanhoRegistro;
    char registroBuscado[130];                            
    fseek(arquivo, rrnArquivoResultado, SEEK_SET);
    fread(&tamanhoRegistro, sizeof(int), 1, arquivo);
    fread(registroBuscado, sizeof(char), tamanhoRegistro, arquivo);
    registroBuscado[tamanhoRegistro] = '\0';
    printf("%s\n", registroBuscado);
}

bool buscarNo(CHAVE_PAGINA chave, BTPAGE *pagina, int *posicao) {
    int i;
    for (i = 0; i < pagina->quantidadeNos && compararChaves(chave, pagina->chaves[i]) > 0; i++);
    *posicao = i;
    return (*posicao < pagina->quantidadeNos && compararChaves(chave, pagina->chaves[*posicao]) == 0);
}

int recuperarRrnRaiz() {
    int rrnRaiz;
    fseek(indice, 0, SEEK_SET); 
    fread(&rrnRaiz, sizeof(int), 1, indice);    
    return rrnRaiz;
}

//--------------------    Aux. Functions    ----------------------------

int compararChaves(CHAVE_PAGINA chave1, CHAVE_PAGINA chave2) {
    char id1[20], id2[20];
    sprintf(id1, "%s%s", chave1.id.codVeiculo, chave1.id.codCliente);
    sprintf(id2, "%s%s", chave2.id.codVeiculo, chave2.id.codCliente);

    return strcmp(id1, id2);
}

void split(CHAVE_PAGINA chave, int filhoDireita, BTPAGE *paginaDividida, CHAVE_PAGINA *chavePromovida, int *filhoDireitaChavePromovida, BTPAGE *novaPagina) {
    int j;
    CHAVE_PAGINA auxChaves[MAXKEYS+1];
    int auxFilhos[MAXKEYS+2];

    for (j = 0; j < MAXKEYS; j++){
        auxChaves[j] = paginaDividida->chaves[j];
        auxFilhos[j] = paginaDividida->filhos[j];
    }
    auxFilhos[MAXKEYS] = paginaDividida->filhos[MAXKEYS];

    // Deslocando as chaves para inserir uma nova
    for (j = MAXKEYS; compararChaves(chave, auxChaves[j-1]) < 0 && j > 0; j--){
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
            paginaDividida->chaves[j + MINKEYS] = criaNoKey(); // posições: 2 e 3
        }

        if((j + MINKEYS + 1) < MAXKEYS) {
            paginaDividida->filhos[j + 1 + MINKEYS] = NIL; // posições: 3 e 4
        }
    }
    
    novaPagina->filhos[MINKEYS] = auxFilhos[j + 1 + MINKEYS]; // posição: 4
  
    novaPagina->quantidadeNos = MAXKEYS - MINKEYS + 1;
    paginaDividida->quantidadeNos = MINKEYS;

    *chavePromovida = auxChaves[MINKEYS - 1];
}

void imprimeArvoreEmOrdem(int rrn, FILE* arquivo) {
    BTPAGE paginaAtual;
    lerPagina(rrn, &paginaAtual);

    // Se a página for uma folha
    if (paginaAtual.filhos[0] == NIL) {
        // Percorre as chaves da página e imprime os registros associados a essas chaves
        for (int i = 0; i < paginaAtual.quantidadeNos; i++) {
            buscaRegistroRRN(paginaAtual.chaves[i].rrn, arquivo);
        }
        // Retorna da função, encerrando a execução para esse nó
        return;
    } else {
        // Se a página não for uma folha, chama recursivamente a função para o primeiro filho
        imprimeArvoreEmOrdem(paginaAtual.filhos[0], arquivo);
    }

    // Percorre as chaves da página e chama recursivamente a função para os filhos
    for (int i = 0; i < paginaAtual.quantidadeNos; i++) {
        // Imprime o registro associado à chave na posição i
        buscaRegistroRRN(paginaAtual.chaves[i].rrn, arquivo);
        // Chama a função recursivamente para o filho da direita da chave i
        imprimeArvoreEmOrdem(paginaAtual.filhos[i + 1], arquivo);
    }
}

FILE* verificaArquivo(char *arquivo) {
    FILE *fp = fopen(arquivo, "rb");

    if (fp == NULL) {
        printf("O arquivo %s não existe.", arquivo);
        exit(0);
    }
    return fp;
}

int buscarQuantidadePagina() {
    long addr; 
    fseek(indice, 0, SEEK_END);
    addr = ftell(indice) - sizeof(int);
    
    return ((int) addr / PAGESIZE);
}

// Lê a página e procura pela chave a ser inserida
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

void atualizarIndiceComNovaRaiz(int rrnRaiz) {
    fseek(indice, 0, SEEK_SET); 
    fwrite(&rrnRaiz, sizeof(int), 1, indice);
}

CHAVE_PAGINA criaNoKey() {
    CHAVE_PAGINA noKey;
    CHAVE_PRIMARIA nullKey;
    char nullableKey[1] = "@";
    strcpy(nullKey.codCliente, nullableKey);
    strcpy(nullKey.codVeiculo, nullableKey);

    noKey.id = nullKey;
    noKey.rrn = (-1);
    
    return noKey;
}