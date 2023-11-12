/**********************************************************
 * 
 * Trabalho 2 - Analise de Algoritmos 
 * Docente: Daniel Carlos Guimaraes Pedronette
 * 
 **********************************************************
 *
 * Integrantes: Jeferson Dietrich  <jp.dietrich@unesp.br>
 *              Larissa Ferrari  <larissa.ferrari@unesp.br>
 *              Murilo Venturato  <murilo.venturato@unesp.br>
 * 
 ***********************************************************/


//-------------------- Libraries ----------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-------------------- Constants ----------------------------
#define true 1
#define false 0
#define NIL -1
#define MAXKEYS 3


//-------------------- Structs ----------------------------
typedef struct
{
    char codCliente[12];
    char codVeiculo[8];
} CHAVE_PRIMARIA;

typedef struct
{
    CHAVE_PRIMARIA id;
    char nomeCliente[50];
    char nomeVeiculo[50];
    char dia[4];
} REGISTRO;

typedef struct
{
    int quantidadeChaves;
    CHAVE_PRIMARIA chaves[MAXKEYS];
    int filhos[MAXKEYS+1];
} PAGINA;

typedef struct
{
    int id;
    int rrn;
} CHAVE_PAGINA;


//-------------------- Main  ----------------------------
int main() {
    prtintf("É o orlas...");
    prtintf("É o o-r-l-a-s!");
    prtintf("É O ORLAAAAAS HAHAHAHA!");
}


//-------------------- Aux. Functions  ----------------------------
void insereNo(int rrn, CHAVE_PRIMARIA chave) {
/*

if CURRENT_RRN = NIL -- then past bottom of tree --
    PROMO_KEY : = KEY 
    PROMO_R_CHILD : = NIL 
    return PROMOTION -- promote ori ginal key and NIL --
else 
    read page at CURRENT_RRN into PAGE 
    search fo r KEY in PAGE
    let POS := the position where KEY occurs or should occur

if KEY found then 
    issue error message indicating duplicate key 
    return ERROR 

RETURN_VALUE := insert ( PAGE . CHILD[POS l , KEY , P_B_RRN , P_B_KEY ) 

if RETURN_VALUE == NO PROMOTION or ERROR then 
    return RETURN_VALUE 

elseif the re is space in PAGE fo r P_B_KEY then  
    insert P_B_KEY and P_B_RRN ( promoted from below ) in PAGE 
    return NO PROMOTION 

else
    split ( P_B_KEY , P_B_RRN , PAGE , PROMO_KEY , PROMO_R_CHILD , NEWPAGE ) 
    write PAGE to file at CURRENT_RRN 
    write NEWPAGE to file at rrn PROMO_R_CHILD 
    return PROMOTION -- promoting PROMO_KEY and PROMO_R_CHILD --
endi f 

*/
}

void insereRegistro() {

}

void buscaRegistro(int rrn, CHAVE_PRIMARIA chave, FILE* arquivo) {
/*

if RRN == NIL then
    return NOT FOUND
else
    read page RRN into PAGE
    look through PAGE for KEY, setting POS equal to the position where KEY occurs or should occur
    if KEY was found there
        FOUND_RRN := RRN
        FOUND_POS := POS
        return FOUND
    else -- follow CHILD reference to next level down --
        return search(PAGE.CHILD[POS], KEY, FOUND_RRN, FOUND_POS)
    endif
endif

*/
}

void insereRegistroIndex() {

}

void criaPagina() {

}

void lePagina() {

}

void buscaRegistroRRN() {

}

void buscaPaginaRRN() {

}

void criaArvore() {

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

void imprimeArvore(int rrn, FILE* arquivo) {
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
}

FILE* verificaArquivo(char* arquivo) {
    FILE* fp = fopen(arquivo, "rb");
    
    if( (fp == NULL) ) {
        printf("O arquivo %s não existe.", arquivo);
        exit(0);
    }    
    return fp;
}
