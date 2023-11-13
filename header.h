//-------------------- Libraries ----------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


//-------------------- Constants ----------------------------
#define NIL -1
#define MAXKEYS 3


//--------------------  Structs  ----------------------------
typedef struct
{
    char codCliente[12];
    char codVeiculo[8];
} CHAVE_PRIMARIA;

typedef struct
{
    char codCliente[12];
    char codVeiculo[8];
    char nomeCliente[50];
    char nomeVeiculo[50];
    char dia[4];
} REGISTRO;

typedef struct 
{
    CHAVE_PRIMARIA id;
    int rrn;
} CHAVE_PAGINA;

typedef struct s_PAGINA {
    int id;
    int quantidadeChaves;
    CHAVE_PAGINA chaves[MAXKEYS+1];
    struct s_PAGINA* filhos[MAXKEYS+1];
    bool ehFolha;
} PAGINA;


//--------------------  Creation Functions  ----------------------------
void criaPagina(PAGINA* pagina, CHAVE_PAGINA chavePagina, int id);

//--------------------  Insertion Function  ----------------------------
void insereNo(CHAVE_PAGINA chave, PAGINA* arvoreB);
int insereRegistro(REGISTRO* novoRegistro, FILE* arquivo);
void insereRegistroIndice(CHAVE_PAGINA* chavePagina, FILE* arquivo);


//--------------------   Search Functions   ----------------------------
int buscaRegistro(CHAVE_PRIMARIA chave, PAGINA* arvoreB);
void buscaRegistroRRN(int offSet, FILE* arquivo);

//--------------------    Aux. Functions    ----------------------------
int compararChaves(CHAVE_PRIMARIA chave1, CHAVE_PRIMARIA chave2);
void split(CHAVE_PRIMARIA chave, int filhoDireita, PAGINA* paginaAntiga, CHAVE_PAGINA* chavePromovida, int chaveFilhaPromovida, PAGINA* novaPagina);
void imprimeArvoreEmOrdem(PAGINA* arvoreB, FILE* arquivo);
FILE* verificaArquivo(char *arquivo);



// Fazer função buscaRegistroRRN
// Fazer função split
// Fazer função para ler o arquivo indice ao iniciar o programa
// Arrumar main