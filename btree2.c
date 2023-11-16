/**********************************************************
 * 
 * Projeto 3:   Indexação - Árvore B
 * Docente:     Orlando de Andrade Figueiredo
 * 
 **********************************************************
 *
 * Integrantes: Jeferson Dietrich   RA: 221154231
 *              Larissa Ferrari     RA: 221151214
 *              Murilo Venturato    RA: 221153861
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

    resultado = fopen("resultado.bin", "a+b");
    BTPAGE pagina;
    int rrnRaiz;

    fclose(resultado);

/* Obtendo raiz || Criando índice */

    if ((indice = fopen("indice.bin", "r+b")) > 0) {
        rrnRaiz = recuperarRrnRaiz();
    } else {        
        rrnRaiz = criarArvore();
    }
    fclose(indice);

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
            /* Inserção */
            case 1:
                while (1)
                {
                    printf("\nDigite '0' para sair...");
                    printf("\nInforme um número de 1 a 10: ");
                    scanf("%d", &i);

                    if (i == 0)
                        break;
                    if (i < 1 || i > 10)
                    {
                        printf("Opção inválida!");
                    }                    
                    else
                    {                        
                        bool foiPromovido;
                        int rrnPromovido;                    
                        CHAVE_PRIMARIA chavePrimaria;
                        strcpy(chavePrimaria.codCliente, registrosInseridos[i-1].codCliente);
                        strcpy(chavePrimaria.codVeiculo, registrosInseridos[i-1].codVeiculo);
                        CHAVE_PAGINA chavePromovida, chave;
                        chave.id = chavePrimaria;

                        indice = verificaArquivo("indice.bin");                        
                        int offSet = buscaRegistroNaArvore(rrnRaiz, chave);

                        if(offSet == (-1)) {
                            resultado = verificaArquivo("resultado.bin");
                            int offSet = insereRegistro(registrosInseridos[i-1]);
                            fclose(resultado);
                            chave.rrn = offSet;

                            foiPromovido = inserirNaArvore(rrnRaiz, chave, &rrnPromovido, &chavePromovida);
                            if (foiPromovido)
                                rrnRaiz = criarRaiz(chavePromovida, rrnRaiz, rrnPromovido);
                        } else {
                            printf("+----- Chave duplicada: <%s%s> -----+\n", chave.id.codCliente, chave.id.codVeiculo);
                        }
                        fclose(indice);
                    }
                }
                break;

            /* Busca */
            case 2:
                while (1)
                {
                    printf("\nDigite '0' para sair...");
                    printf("\nInforme um número de 1 a 4: ");
                    scanf("%d", &i);

                    if (i == 0)
                        break;
                    if (i < 1 || i > 4)
                    {
                        printf("Opção inválida!");
                    }                    
                    else
                    {
                        printf("\nAcessando o arquivo com chaves para busca...\n");

                        CHAVE_PRIMARIA chavePrimaria;
                        strcpy(chavePrimaria.codCliente, buscaIndice[i-1].codCliente);
                        strcpy(chavePrimaria.codVeiculo, buscaIndice[i-1].codVeiculo);
                        CHAVE_PAGINA chavePagina;
                        chavePagina.id = chavePrimaria;
                        
                        indice = verificaArquivo("indice.bin");
                        
                        int offSet = buscaRegistroNaArvore(rrnRaiz, chavePagina);
                        fclose(indice);

                        if(offSet != (-1)) {
                            resultado = verificaArquivo("resultado.bin");
                            buscaRegistroRRN(offSet, resultado);
                            fclose(resultado);
                        } else {
                            printf("Chave não encontrada no índice!\n");
                        }
                    }
                }
                break;

            /* Listagem */
            case 3:
                printf("\nListando todos os clientes...\n\n");
                indice = verificaArquivo("indice.bin");  
                resultado = verificaArquivo("resultado.bin");                            
                imprimeArvoreEmOrdem(recuperarRrnRaiz());
                printf("\n");
                fclose(resultado);
                fclose(indice);
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

int insereRegistro(REGISTRO novoRegistro) {       
    char registro[130];
    sprintf(registro, "%s|%s|%s|%s|%s|",    novoRegistro.codCliente,
                                            novoRegistro.codVeiculo,
                                            novoRegistro.nomeCliente,
                                            novoRegistro.nomeVeiculo,
                                            novoRegistro.dia
    );

    int tamRegistro = strlen(registro);
    
    fseek(resultado, 0, SEEK_END);
    int offSet = ftell(resultado);    

    fwrite(&tamRegistro, sizeof(int), 1, resultado);    
    fwrite(registro, sizeof(char), tamRegistro, resultado);
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
    BTPAGE paginaAtual, novaPagina;
    bool encontrado, promovido;   
    int posicao, rrnPromovidoDeBaixo;
    CHAVE_PAGINA chavePromovidaDeBaixo;

    // Está em uma folha da árvore
    if (rrn == NIL) {
        *chavePromovida = proximaChave;
        *rrnPromovido = NIL;
        printf("+----- Chave Inserida: <%s%s> -----+\n", proximaChave.id.codCliente, proximaChave.id.codVeiculo);
        return true;
    }

    // Lê a página e procura pela chave a ser inserida
    lerPagina(rrn, &paginaAtual);
    encontrado = buscarNo (proximaChave, &paginaAtual, &posicao);
    if (encontrado) {
        
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
        return paginaAtual.chaves[posicao].rrn;
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
    puts(registroBuscado);
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
    sprintf(id1, "%s%s", chave1.id.codCliente, chave1.id.codVeiculo);
    sprintf(id2, "%s%s", chave2.id.codCliente, chave2.id.codVeiculo);

    return strcmp(id1, id2);
}

void split(CHAVE_PAGINA chave, int filhoDireita, BTPAGE *paginaDividida, CHAVE_PAGINA *chavePromovida, int *filhoDireitaChavePromovida, BTPAGE *novaPagina) {
    printf("+----- Divisão de Nó -----+\n");
    
    int j;
    CHAVE_PAGINA auxChaves[MAXKEYS+1];
    int auxFilhos[MAXKEYS+2];

    for (j = 0; j < MAXKEYS; j++){
        auxChaves[j] = paginaDividida->chaves[j];
        auxFilhos[j] = paginaDividida->filhos[j];
    }
    auxFilhos[MAXKEYS] = paginaDividida->filhos[MAXKEYS];

    // Deslocando as chaves para inserir uma nova
    for (j = MAXKEYS; (compararChaves(chave, auxChaves[j-1]) < 0) && j > 0; j--){
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
        paginaDividida->filhos[j + MINKEYS] = NIL; // posições: 2 e 3
    }
    novaPagina->filhos[MINKEYS] = auxFilhos[j + MINKEYS]; // posição: 4
        
    paginaDividida->chaves[1] = criaNoKey();

    novaPagina->quantidadeNos = (MAXKEYS + 1) - MINKEYS;
    paginaDividida->quantidadeNos = MINKEYS - 1;

    *chavePromovida = auxChaves[1];

    printf("+----- Chave Promovida: <%s%s> -----+\n", chavePromovida->id.codCliente, chavePromovida->id.codVeiculo);
}

void imprimeArvoreEmOrdem(int rrn) {
    BTPAGE paginaAtual;

    lerPagina(rrn, &paginaAtual);

    // Se a página for uma folha
    if (paginaAtual.filhos[0] == NIL) {
        // Percorre as chaves da página e imprime os registros associados a essas chaves
        for (int i = 0; i < paginaAtual.quantidadeNos; i++) {
            buscaRegistroRRN(paginaAtual.chaves[i].rrn, resultado);
        }
        // Retorna da função, encerrando a execução para esse nó
        return;
    }
    else {
        // Se a página não for uma folha, chama recursivamente a função para o primeiro filho
        imprimeArvoreEmOrdem(paginaAtual.filhos[0]);
    }

    // Percorre as chaves da página e chama recursivamente a função para os filhos
    for (int i = 0; i < paginaAtual.quantidadeNos; i++) {
        // Imprime o registro associado à chave na posição i
        buscaRegistroRRN(paginaAtual.chaves[i].rrn, resultado);
        // Chama a função recursivamente para o filho da direita da chave i
        imprimeArvoreEmOrdem(paginaAtual.filhos[i + 1]);
    }
}

FILE* verificaArquivo(char *arquivo) {
    FILE *fp = fopen(arquivo, "r+b");

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
    fwrite(pagina, sizeof(BTPAGE), 1, indice);
}

void atualizarIndiceComNovaRaiz(int rrnRaiz) {
    fseek(indice, 0, SEEK_SET); 
    fwrite(&rrnRaiz, sizeof(int), 1, indice);
}

CHAVE_PAGINA criaNoKey() {
    CHAVE_PAGINA noKey;
    CHAVE_PRIMARIA nullKey;
    char nullableKey[12] = "@@@@@@@@@@@";
    char nullableKey2[8] = "#######";
    strcpy(nullKey.codCliente, nullableKey);
    strcpy(nullKey.codVeiculo, nullableKey2);

    noKey.id = nullKey;
    noKey.rrn = -1;
    
    return noKey;
}