#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // para o sleep.
#include <assert.h>
#include "def_funcoes.h"

int main () {
    Estado *topo = NULL;
    int jogo = 1;
    char c[100];
    char l[100];
    char **tabela = NULL;
    scanf("%99s %99s", l, c);
    int ll = atoi(l);
    int cc = atoi(c);
    if (l[0] < '0' || l[0] > '9') {
        char save[100];
        strcpy(save, c);
        strcat(save, ".txt");
        loadJogo(&cc, &ll, &tabela, save);
        int erro[5] = {0};
        verificaRegras(cc, ll, tabela, erro);
        push(&topo, cc, ll, tabela, -1, -1, 'l', erro);
    }
    else {
        tabela = malloc(ll * sizeof(char *));
        for (int i = 0; i < ll; i++) {
            tabela[i] = malloc(cc * sizeof(char));
            for (int j = 0; j < cc; j++) {
                scanf(" %c", &tabela[i][j]);
            }
        }
        int erro[5] = {0};
        push(&topo, cc, ll, tabela, -1, -1, 'i', erro); // inicializar a tabela
    }
    while (jogo == 1) {
        system("clear");
        printTabela(cc, ll, tabela);
        comando(&topo, &cc, &ll, &tabela, &jogo);
    }
    while (topo != NULL) {
        pop(&topo, cc, ll, tabela);
    }
    for (int i = 0; i < ll; i++) {
        free(tabela[i]);
    }
    free(tabela);

    return 0;
}
