#include <stdio.h>
#include <stdbool.h>

typedef struct Estado { // guarda o numero da jogada, o comando executado, e a posição que foi alterada.
    char **tabela;
    int pos_x,pos_y,error[5];    // erro pode ser igual a 1 (erro 1), 2 (erro 2). pode ter mais que um erro, dai o array. pode ter 5 erros diferentes.
    char comando;
    struct Estado *prox;
} Estado;     

typedef struct {
    int x;
    int y;
} Coordenada;

void printTabela(int x, int y, char **tabela);
void push(Estado **topo, int x, int y, char **tabela, int pos_x, int pos_y, char comando, int error[5]);
void pop(Estado **topo, int x, int y, char **tabela);
int lerCoordenada (char a);
void inserirMaiuscula(Estado **topo,char a, int b, int x, int y, char **tabela);
void inserirHashtag(Estado **topo,char a, int b, int x, int y, char **tabela);
void loadJogo(int *x, int *y, char ***tabela, char *save);
void gravarJogo(int x, int y, char **tabela, char *save);
void desfazer(Estado **topo, int x, int y, char **tabela);
void verificarEstadoJogo(Estado *topo, int x, int y);
void verificaRegras(int x, int y, char **tabela, int erro[5]);
int mudouAlgo(Estado **topo, int x, int y, char **tabela);
void ajudar(Estado **topo,int x, int y, char **tabela);
void ajudarA(Estado **topo, int x, int y, char **tabela);
int estaresolvidoounao(int x, int y, char **tabela);
void resolver(Estado **topo, int x, int y, char **tabela);
void comando(Estado **topo, int *x, int *y, char ***tabela, int *jogo);
bool dentroDosLimites(int x, int y, int max_x, int max_y);
bool verificarConectividade(int x, int y, char **tabela);
void copiarTabela(int x, int y, char **origem, char **destino);