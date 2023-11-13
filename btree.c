/**********************************************************
 * 
 * Projeto 3:   Indexação - Árvore B
 * Docente:     Orlando de Andrade Figueiredo
 * 
 **********************************************************
 *
 * Integrantes: Jeferson Dietrich   RA: 221154231
 *              Larissa Ferrari     RA: 
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
    int raiz = NIL, rrnPromovido, promovido;
    CHAVE_PAGINA chavePromovida;
    PAGINA* pagina;


/* Menu de Opções */
    int opcao, i, idPagina = 0;

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
                        arquivo = fopen("resultado.bin", "a+b");                        
                        int offSet = insereRegistro(&registrosInseridos[i - 1], arquivo);
                        fclose(arquivo);

                        CHAVE_PRIMARIA chavePrimaria;
                        strcpy(chavePrimaria.codCliente, registrosInseridos[i - 1].codCliente);
                        strcpy(chavePrimaria.codVeiculo, registrosInseridos[i - 1].codVeiculo);

                        CHAVE_PAGINA chavePagina;
                        chavePagina.id = chavePrimaria;
                        chavePagina.rrn = offSet;

                        arquivo = fopen("indice.bin", "rb+");   
                        if(arquivo == NULL) {
                            criaPagina(pagina, chavePagina, idPagina);
                        }  else {
                            arquivo = fopen("indice.bin", "wb+"); 
                            insereNo(chavePagina, pagina);
                        }

                        insereRegistroIndice(&chavePagina, arquivo);
                        fclose(arquivo);
                    }
                }
                break;

            case 2:
                printf("\nPesquisando por um cliente...\n");
                break;

            case 3:
                printf("\nListando todos os clientes...\n");
                arquivo = verificaArquivo("resultado.bin");
                imprimeArvoreEmOrdem(pagina, arquivo);

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
void criaPagina(PAGINA* pagina, CHAVE_PAGINA chavePagina, int id) {
    pagina->id = id;
    pagina->quantidadeChaves = 1;
    pagina->chaves[0] = chavePagina;
    pagina->ehFolha = true;
    id++;
}


//--------------------  Insertion Function  ----------------------------
void insereNo(CHAVE_PAGINA chave, PAGINA *arvoreB) {
    if(buscaRegistro(chave.id, arvoreB) != -1) {
        printf("Inserção negada, chave já existe no arquivo");
        return;
    }

    if (arvoreB->ehFolha) {
        for (int i = 0; i < arvoreB->quantidadeChaves; i++) {
            int comparacao = compararChaves(chave.id, arvoreB->chaves[i].id);
            
            if(comparacao < 0) { // chaveInserir menor
                arvoreB->quantidadeChaves++;                
                for (int j = arvoreB->quantidadeChaves - 1; i > i; j--) {
                    arvoreB->chaves[i] = arvoreB->chaves[i - 1];
                }                
                arvoreB->chaves[i] = chave;                
                
                if(arvoreB->quantidadeChaves > MAXKEYS) {
                    // split();
                    arvoreB->quantidadeChaves--;
                }
            }
        }   
    } else {
        int i;
        for (i = 0; i < arvoreB->quantidadeChaves; i++) {
            int comparacao = compararChaves(chave.id, arvoreB->chaves[i].id);
                    
            if(comparacao < 0) { // chaveInserir menor
                insereNo(chave, arvoreB->filhos[i]);
                return;
            }
        }
        insereNo(chave, arvoreB->filhos[i]); // insere no filho a direita
    }
}    


int insereRegistro(REGISTRO* novoRegistro, FILE* arquivo) {       
    if(arquivo == NULL) {
        arquivo = fopen("resultado.bin", "wb+");
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
    return 1;
}

void insereRegistroIndice(CHAVE_PAGINA* chavePagina, FILE* arquivo) {
    fseek(arquivo, SEEK_END, 0);
    fwrite(chavePagina, sizeof(CHAVE_PAGINA), 1, arquivo);
}


//--------------------   Search Functions   ----------------------------

int buscaRegistro(CHAVE_PRIMARIA chave, PAGINA* arvoreB) {
    if (arvoreB == NULL) {
        printf("Chave não encontrada no arquivo");
        return -1;
    }

    int i;
    for (i = 0; i < arvoreB->quantidadeChaves; i++) {
        int comparacao = compararChaves(chave, arvoreB->chaves[i].id);

        if (comparacao == 0) {            
            return arvoreB->chaves[i].rrn;
        }

        if (comparacao < 0) {
            break;
        }
    }

    if (!arvoreB->ehFolha) {
        return buscaRegistro(chave, arvoreB->filhos[i]);
    }

    printf("Chave não encontrada no arquivo");
    return -1;
}

void buscaRegistroRRN(int offSet, FILE* arquivo){
}

// void buscaRegistroRRN(int rrn, FILE* arquivo) {
//     fseek(arquivo, rrn, SEEK_SET);  // Move o ponteiro do arquivo para o RRN
//     REGISTRO registro;
    
//     // Lê o registro associado ao RRN
//     fread(&registro, sizeof(REGISTRO), 1, arquivo);

//     // Imprime os detalhes do registro
//     imprimirRegistro(&registro);
// }



//--------------------    Aux. Functions    ----------------------------
int compararChaves(CHAVE_PRIMARIA chave1, CHAVE_PRIMARIA chave2) {
    char id1[20], id2[20];
    sprintf(id1, "%s%s", chave1.codVeiculo, chave1.codCliente);
    sprintf(id2, "%s%s", chave2.codVeiculo, chave2.codCliente);

    return strcmp(id1, id2);
}


void split(CHAVE_PRIMARIA chave, int filhoDireita, PAGINA* paginaAntiga, CHAVE_PAGINA* chavePromovida, int chaveFilhaPromovida, PAGINA* novaPagina) {
    /*

    copy al l keys and pointers from PAGE into a working page that 
    can hold one extra key and child . 
    insert I_KEY and I_RRN into their proper places in the working page . 
    all ocate and initial ize a new page in the B-t ree file to hold NEWPAGE . 
    set PROMO_KEY to value of middl e key , which will be promoted after 
    the split . 
    set PROMO_R_CHILD to RRN of NEWPAGE . 
    copy keys and child pointers preceding PROMO_KEY from the working 
    page to PAGE . 
    copy keys and child pointers rol l owing PROMO_KEY from the working 
    page to NEWPAGE .

    */  
}

void imprimeArvoreEmOrdem(PAGINA* arvoreB, FILE* arquivo) {
    // Se a página for uma folha
    if (arvoreB->ehFolha) {
        // Percorre as chaves da página e imprime os registros associados a essas chaves
        for (int i = 0; i < arvoreB->quantidadeChaves; i++) {
            buscaRegistroRRN(arvoreB->chaves[i].rrn, arquivo);
        }
        // Retorna da função, encerrando a execução para esse nó
        return;
    } else {
        // Se a página não for uma folha, chama recursivamente a função para o primeiro filho
        imprimeArvoreEmOrdem(arvoreB->filhos[0], arquivo);
    }

    // Percorre as chaves da página e chama recursivamente a função para os filhos
    for (int i = 0; i < arvoreB->quantidadeChaves; i++) {
        // Imprime o registro associado à chave na posição i
        buscaRegistroRRN(arvoreB->chaves[i].rrn, arquivo);
        // Chama a função recursivamente para o filho da direita da chave i
        imprimeArvoreEmOrdem(arvoreB->filhos[i + 1], arquivo);
    }
}


// void imprimirRegistro(REGISTRO* registro) {
//     printf("Código do Cliente: %s\n", registro->id.codCliente);
//     printf("Código do Veículo: %s\n", registro->id.codVeiculo);
//     printf("Nome do Cliente: %s\n", registro->nomeCliente);
//     printf("Nome do Veículo: %s\n", registro->nomeVeiculo);
//     printf("Número de Dias: %d\n", registro->dia);
//     printf("--------------------------\n");
// }

/*

Função inOrderTraversal(No):
    Se No não for nulo:
        1. Chamar inOrderTraversal(No.filho[0])  -- Percorrer o primeiro filho (subárvore à esquerda) --

        2. Para cada i de 1 até No.numChaves:
            a. Imprimir No.chave[i]  -- Imprimir a chave atual --

            b. Chamar inOrderTraversal(No.filho[i])  -- Percorrer a subárvore à direita da chave atual --

        3. Chamar inOrderTraversal(No.filho[No.numChaves])  -- Percorrer o último filho (subárvore à direita) --

// Função para iniciar a travessia inOrder na árvore B
Função printInOrder(Tree):
    Chamar inOrderTraversal(Tree.raiz)

*/


FILE* verificaArquivo(char *arquivo)
{
    FILE *fp = fopen(arquivo, "rb");

    if (fp == NULL)
    {
        printf("O arquivo %s não existe.", arquivo);
        exit(0);
    }
    return fp;
}
