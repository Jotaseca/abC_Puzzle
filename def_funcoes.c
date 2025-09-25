#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // para o sleep.
#include <assert.h>
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
void printTabela(int x, int y, char **tabela) {
    for (int i = 0; i < y; i++) {
        for (int j = 0; j < x; j++) {
            printf("%c ", tabela[i][j]);
        }
        printf("\n");
    }
}
void push(Estado **topo, int x, int y, char **tabela, int pos_x, int pos_y, char comando, int error[5]) {
    Estado *novo = malloc(sizeof(Estado));
    novo->tabela = malloc(y * sizeof(char*));
    for (int i = 0; i < y; i++) {
        novo->tabela[i] = malloc(x * sizeof(char));
        memcpy(novo->tabela[i], tabela[i], x * sizeof(char)); // mais eficiente
    }
    novo->pos_x = pos_x;
    novo->pos_y = pos_y;
    novo->comando = comando;
    memcpy(novo->error, error, 5 * sizeof(int));
    novo->prox = *topo;
    *topo = novo;
}
void pop(Estado **topo, int x, int y, char **tabela) {
    if (*topo == NULL) return;
    Estado *temp = *topo;
    *topo = (*topo)->prox;
    if (*topo != NULL) {
        for (int i = 0; i < y; i++) {
            for (int j = 0; j < x; j++) {
                tabela[i][j] = (*topo)->tabela[i][j];
            }
        }
    }
    for (int i = 0; i < y; i++) {
        free(temp->tabela[i]);
    }
    free(temp->tabela);
    free(temp);
}
int lerCoordenada (char a) { // universal;
    int num = a - 'a';
    return num;
}
bool dentroDosLimites(int x, int y, int max_x, int max_y) {
    return x >= 0 && x < max_x && y >= 0 && y < max_y;
}
bool verificarConectividade(int x, int y, char **tabela) {// Função para verificar conectividade ortogonal entre casas brancas
    Coordenada inicio = {-1, -1};// Primeiro, encontramos a primeira casa branca para começar a busca
    for (int i = 0; i < y; i++) {
        for (int j = 0; j < x; j++) {
            if ((tabela[i][j] >= 'A' && tabela[i][j] <= 'Z') || (tabela[i][j] >= 'a' && tabela[i][j] <= 'z')) {
                inicio.x = j;
                inicio.y = i;
                break;
            }
        }
        if (inicio.x != -1) break; // Encontrei a primeira casa branca
    }
    if (inicio.x == -1) return true;// Se não encontrou nenhuma casa branca, consideramos como válido
    bool **visitado = malloc(y * sizeof(bool *));// Matriz para marcar casas visitadas
    for (int i = 0; i < y; i++) {
        visitado[i] = malloc(x * sizeof(bool));
        for (int j = 0; j < x; j++) {
            visitado[i][j] = false;
        }
    }
    Coordenada *fila = malloc(x * y * sizeof(Coordenada));
    int frente = 0, fim = 0;
    fila[fim++] = inicio;// Começamos pela primeira casa branca encontrada
    visitado[inicio.y][inicio.x] = true;
    int casasBrancasVisitadas = 0;
    int totalCasasBrancas = 0;// Contamos o total de casas brancas
    for (int i = 0; i < y; i++) {
        for (int j = 0; j < x; j++) {
            if  ((tabela[i][j] >= 'A' && tabela[i][j] <= 'Z') || (tabela[i][j] >= 'a' && tabela[i][j] <= 'z')) {
                totalCasasBrancas++;
            }
        }
    }// Movimentos ortogonais (cima, baixo, esquerda, direita)
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};// Executando a BFS
    while (frente < fim) {
        Coordenada atual = fila[frente++];// Se for uma casa branca, incrementamos o contador
        if ((tabela[atual.y][atual.x] >= 'A' && tabela[atual.y][atual.x] <= 'Z') || (tabela[atual.y][atual.x] >= 'a' && tabela[atual.y][atual.x] <= 'z')) {
            casasBrancasVisitadas++;
        }// Para cada direção (cima, baixo, esquerda, direita)
        for (int i = 0; i < 4; i++) {
            int nx = atual.x + dx[i];
            int ny = atual.y + dy[i];// Verificar se está dentro dos limites e se não foi visitado
            if (dentroDosLimites(nx, ny, x, y) && tabela[ny][nx] != '#' && !visitado[ny][nx]) {
                visitado[ny][nx] = true; // Marca como visitado somente ao processar
                fila[fim++] = (Coordenada){nx, ny};
            }
        }
    }
    for (int i = 0; i < y; i++) {// Liberação de memória
        free(visitado[i]);
    }
    free(visitado);
    free(fila);
    return casasBrancasVisitadas == totalCasasBrancas;// Verificamos se todas as casas brancas foram visitadas, se sim ta tudo certo, se não n está certo
}
void verificaRegras(int x, int y, char **tabela,int*erro) { // vamos tentar fazer esta cena universal.
    int i,j,k;
    for(i=0; i<x; i++) {
        for(j=0; j<y; j++) {
            // regra 1:
            if (!(tabela[j][i] == '#' ||( tabela[j][i] >= 'a' && tabela[j][i] <= 'z' )|| (tabela[j][i] >='A' && tabela[j][i] <= 'Z'))) { // verifica se é espaço. se for, dá erro.
                erro[0] = 1;
            }
            // regra 2:
            if((tabela[j][i] > 64) && (tabela[j][i] < 91)) { // verifica se é maiuscula. se for, verifica se existem outras maiusculas OU minusculas, do mesmo simbolo, na mesma linha ou coluna.
                for(k=0;k<y;k++) { // verifica na coluna.
                    if((k != j) && ((tabela[k][i] == tabela[j][i]) || (tabela[k][i] == tabela[j][i]+32))){erro[1] = 1;}
                }
                for(k=0;k<x;k++) { // verifica na linha.
                    if((k != i) && ((tabela[j][k] == tabela[j][i]) || (tabela[j][k] == tabela[j][i]+32))) {erro[1] = 1;}
                }
            }
            // regra 3: nao pode ter todos riscados juntos. regra semi-inventada.
            if(tabela[j][i] == '#') {
                // (tabela[j+1][i]=='#')||(tabela[j-1][i]=='#')||(tabela[j][i+1]=='#')||(tabela[j][i-1]=='#')
                if((j==0) && (i==0)) { // canto superior esquerdo.
                    if((tabela[j+1][i]=='#')||(tabela[j][i+1]=='#')) {erro[2] = 1;}                             //Cada casa contém um símbolo (uma letra inicialmente minúscula); ---- erro1ind0
                } else if((i==0) && ((j!=0) && (j+1 != y) )) { // parede esquerda
                    if((tabela[j+1][i]=='#')||(tabela[j-1][i]=='#')||(tabela[j][i+1]=='#')) {erro[2] = 1;}      //Em cada linha e coluna só pode existir uma única réplica de cada símbolo que é pintada a branco (coloca-se a letra em maiúsculas); --- erro2ind1
                } else if((i==0) && (j+1==y)) { // canto inferior esquerdo
                    if((tabela[j-1][i]=='#')||(tabela[j][i+1]=='#')) {erro[2] = 1;}                             //Todas as outras réplicas desse símbolo têm que ser riscadas (substituídas por um cardinal); --- erro3???ind2 acho que esta cena é consequencia do erro 2? mesmo assim vou deixar como erro.
                } else if((i!=0) && (j+1==y) && (i+1!=x)) { // parede inferior
                    if((tabela[j-1][i]=='#')||(tabela[j][i+1]=='#')||(tabela[j][i-1]=='#')) {erro[2] = 1;}      //Se uma casa está riscada, todas as casas vizinhas ortogonais têm que estar pintadas a branco; --- erro4ind3
                } else if((j+1==y)&&(i+1==x)) { // canto inferior direito
                    if((tabela[j-1][i]=='#')||(tabela[j][i-1]=='#')) {erro[2] = 1;}                             //É necessário existir um caminho ortogonal entre todas as casas brancas do tabuleiro. --- erro5ind4
                } else if((j!=0) && (j+1!=y) && (i+1==x)) { // parede direita
                    if((tabela[j+1][i]=='#')||(tabela[j-1][i]=='#')||(tabela[j][i-1]=='#')) {erro[2] = 1;}
                } else if((i+1==x) && (j==0)) { // canto superior direito
                    if((tabela[j+1][i]=='#')||(tabela[j][i-1]=='#')) {erro[2] = 1;}
                } else if((j==0) && (i+1!=x) && (i!=0)) { // parede superior
                    if((tabela[j+1][i]=='#')||(tabela[j][i+1]=='#')||(tabela[j][i-1]=='#')) {erro[2] = 1;}
                } else { // no meio. (tudo o resto).
                    if((tabela[j+1][i]=='#')||(tabela[j-1][i]=='#')||(tabela[j][i+1]=='#')||(tabela[j][i-1]=='#')) {erro[2] = 1;}
                }
            }// verificar se tem maiuscula á volta:// regra 4: meti bueda ifs por causa dos casos em que o gajo possa querer verificar fora dos arrays.
            if(tabela[j][i] == '#') {
                if((j==0) && (i==0)) { // canto superior esquerdo.
                    if(((tabela[j+1][i] <= 64) || (tabela[j+1][i] >= 91)) || ((tabela[j][i+1] <= 64) || (tabela[j][i+1] >= 91))) {erro[3] = 1;}
                } else if((i==0) && ((j!=0) && (j+1 != y) )) { // parede esquerda
                    if(((tabela[j-1][i] <= 64) || (tabela[j-1][i] >= 91)) || ((tabela[j+1][i] <= 64) || (tabela[j+1][i] >= 91)) || ((tabela[j][i+1] <= 64) || (tabela[j][i+1] >= 91))) {erro[3] = 1;}
                } else if((i==0) && (j+1==y)) { // canto inferior esquerdo
                    if(((tabela[j-1][i] <= 64) || (tabela[j-1][i] >= 91)) || ((tabela[j][i+1] <= 64) || (tabela[j][i+1] >= 91))) {erro[3] = 1;}
                } else if((i!=0) && (j+1==y) && (i+1!=x)) { // parede inferior
                    if(((tabela[j-1][i] <= 64) || (tabela[j-1][i] >= 91)) || ((tabela[j][i-1] <= 64) || (tabela[j][i-1] >= 91)) || ((tabela[j][i+1] <= 64) || (tabela[j][i+1] >= 91))) {erro[3] = 1;}
                } else if((j+1==y)&&(i+1==x)) { // canto inferior direito
                    if(((tabela[j-1][i] <= 64) || (tabela[j-1][i] >= 91)) || ((tabela[j][i-1] <= 64) || (tabela[j][i-1] >= 91))) {erro[3] = 1;}
                } else if((j!=0) && (j+1!=y) && (i+1==x)) { // parede direita
                    if(((tabela[j-1][i] <= 64) || (tabela[j-1][i] >= 91)) || ((tabela[j+1][i] <= 64) || (tabela[j+1][i] >= 91)) || ((tabela[j][i-1] <= 64) || (tabela[j][i-1] >= 91))) {erro[3] = 1;}
                } else if((i+1==x) && (j==0)) { // canto superior direito
                    if(((tabela[j+1][i] <= 64) || (tabela[j+1][i] >= 91)) || ((tabela[j][i-1] <= 64) || (tabela[j][i-1] >= 91))) {erro[3] = 1;}
                } else if((j==0) && (i+1!=x) && (i!=0)) { // parede superior
                    if(((tabela[j+1][i] <= 64) || (tabela[j+1][i] >= 91)) || ((tabela[j][i-1] <= 64) || (tabela[j][i-1] >= 91)) || ((tabela[j][i+1] <= 64) || (tabela[j][i+1] >= 91))) {erro[3] = 1;}
                } else { // no meio. (tudo o resto).
                    if(((tabela[j-1][i] <= 64) || (tabela[j-1][i] >= 91)) || ((tabela[j+1][i] <= 64) || (tabela[j+1][i] >= 91)) || ((tabela[j][i-1] <= 64) || (tabela[j][i-1] >= 91)) || ((tabela[j][i+1] <= 64) || (tabela[j][i+1] >= 91))) {erro[3] = 1;}
                }
            }
        }
    }
    if (verificarConectividade(x, y, tabela) == false) {
        erro[4] = 1;
    }
}
int estaresolvidoounao(int x, int y, char **tabela) { // da return 1 se estiver resolvido e da return 0 caso contrario.
    int a=1;
    for(int k=0; k<y; k++) {
        for(int l=0; l<x; l++) {
            if(!((tabela[k][l]>='A'&&tabela[k][l]<='Z')||(tabela[k][l]=='#'))) {
                a=0;
            }
        }
    }
    int erro[5] = {0};
    verificaRegras(x, y, tabela, erro);
    for (int i = 0; i < 5; i++) {
        if (erro[i] != 0) return 0; // Há erros, não resolvido
    }
    return a;
}
void inserirMaiuscula(Estado **topo,char a, int b, int x, int y, char **tabela) {
    int x_inp,y_inp;
    x_inp = lerCoordenada(a);
    y_inp = b-1;                                  //assume que a coordenada 1 é a primeira coordenada.
    if (tabela[y_inp][x_inp] != '#' && tabela[y_inp][x_inp] > 96 && tabela[y_inp][x_inp] < 123) {// verifica se pode meter maiuscula.
        tabela[y_inp][x_inp] = tabela[y_inp][x_inp] - 32;
    }
    int erro[5]={0};verificaRegras(x,y,tabela,erro);
    push(topo,x,y,tabela, x_inp, y_inp, 'b', erro);
}
void inserirHashtag (Estado **topo,char a, int b, int x, int y, char **tabela) {
    int x_inp,y_inp;
    x_inp = lerCoordenada(a);
    y_inp = b-1;                                //assume que a coordenada 1 é a primeira coordenada.
    tabela[y_inp][x_inp] = '#';
    int erro[5]={0};verificaRegras(x,y,tabela,erro);
    push(topo,x,y,tabela, x_inp, y_inp, 'r', erro);
}
void verificarEstadoJogo(Estado *topo, int x, int y, char** tabela) {
    if (topo == NULL) {printf("Nenhum estado disponível para verificação.\n");return;}
    printf("Erros no estado atual:\n");
    printf("---------------------------------------------------------------------\n");
    int erroEncontrado = 0;// Verifica cada tipo de erro no topo do stack.
    if (topo->error[0] == 1) {
        printf("Erro 1: Símbolo inválido ou não existente.\n");erroEncontrado = 1;
    }
    if (topo->error[1] == 1) {
        printf("Erro 2: Réplica de símbolo pintado na mesma linha ou coluna.\n");erroEncontrado = 1;
    }
    if (topo->error[2] == 1) {
        printf("Erro 3: Encontradas casas riscadas adjacentes.\n");erroEncontrado = 1;
    }
    if (topo->error[3] == 1) {
        printf("Erro 4: Casas adjacentes a símbolo riscado não pintadas.\n");erroEncontrado = 1;
    }
    if (topo->error[4] == 1) {
        printf("Erro 5: Não existe um caminho ortogonal entre as casas pintadas.\n");erroEncontrado = 1;
    }
    if (estaresolvidoounao(x,y,tabela)) {
        printf("Está resolvido!\n");
    }
    else if (!erroEncontrado) {
        printf("Não tem erros!\n");
    }
    printf("---------------------------------------------------------------------\n");
    printf("Pressione [ENTER] para continuar.\n");// Espera o usuário clicar Enter
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // Limpa o buffer de entrada
    getchar(); // Aguarda o Enter
}
void gravarJogo (int x, int y, char **tabela, char *save) {
    FILE *fptr;
    fptr = fopen(save, "w");
    fprintf(fptr, "%d %d\n", x, y);
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            fprintf(fptr, "%c", tabela[i][j]);
        }
        if (i != y-1) {
            fprintf(fptr, "\n");
        }
    }
    fclose(fptr);
}
void loadJogo(int *x, int *y, char ***tabela, char *save) {
    if (*tabela != NULL) {// Liberar a tabela antiga se existir
        for (int i = 0; i < *y; i++) {
            free((*tabela)[i]);
        }
        free(*tabela);
    }
    FILE *fptr = fopen(save, "r");
    if (fptr == NULL) {
        printf("Não existe o ficheiro.\n");
        return;
    }
    if (fscanf(fptr, "%d %d\n", y, x) != 2) {// Ler as novas dimensões y e x
        printf("Formato de arquivo inválido.\n");
        fclose(fptr);
        return;
    }
    *tabela = (char **)malloc(*y * sizeof(char *));
    for (int i = 0; i < *y; i++) {
        (*tabela)[i] = (char *)malloc(*x * sizeof(char));
    }
    int i = 0, j = 0;
    char c;
    while ((c = fgetc(fptr)) != EOF) {
        if (c == '\n' && j != 0) {
            i++;
            j = 0;
        } else if (i < *y && j < *x) {
            (*tabela)[i][j] = c;
            j++;
        }
    }
    fclose(fptr);
}
void desfazer(Estado **topo, int x, int y, char **tabela) {
    if ((*topo)->prox == NULL) {}
    else {
        pop(topo,x,y,tabela);
    }
}
void copiarTabela(int x, int y, char **origem, char **destino) { // cria tabela temporária
    for (int i = 0; i < y; i++) {
        memcpy(destino[i], origem[i], x * sizeof(char));
    }
}
void ajudar(Estado **topo,int x, int y, char **tabela) {
    int i,j,k;//riscar todas as letras iguais a uma letra branca na mesma linha e/ou coluna - funcao 1.
    // funcao 1:
    for(i=0; i<x; i++) {//pintar de branco todas as casas vizinhas de uma casa riscada - funcao 2.
        for(j=0; j<y;j++) {//pintar de branco uma casa quando seria impossível que esta fosse riscada, por isolar casas brancas - funcao 3.
            if(tabela[j][i] > 64 && tabela[j][i] < 91) {
                for(k=0;k<y;k++) { // coluna
                    if(k != j && (tabela[k][i] == tabela[j][i] || tabela[k][i] == tabela[j][i]+32)){
                        tabela[k][i] = '#';
                    }
                }
                for(k=0;k<x;k++) { // linha
                    if(k != i && (tabela[j][k] == tabela[j][i] || tabela[j][k] == tabela[j][i]+32)) {
                        tabela[j][k] = '#';
                    }
                }
            }
        }// neste caso temos de fazer 2 loops porque acho que dará problemas com as duas verificações a serem feitas ao mesmo tempo.
    }// funcao 2:
    for(i=0; i<x; i++) {
        for(j=0; j<y; j++) { // verifica os vizinhos com 9 casos possíveis
            if(tabela[j][i] == '#') {
                if(j==0 && i==0) { // canto superior esquerdo
                    if(tabela[j+1][i] > 96 && tabela[j+1][i] < 123) { tabela[j+1][i] -= 32; }
                    if(tabela[j][i+1] > 96 && tabela[j][i+1] < 123) { tabela[j][i+1] -= 32; }
                } else if(i==0 && j!=0 && j+1 != y) { // parede esquerda
                    if(tabela[j-1][i] > 96 && tabela[j-1][i] < 123) { tabela[j-1][i] -= 32; }
                    if(tabela[j+1][i] > 96 && tabela[j+1][i] < 123) { tabela[j+1][i] -= 32; }
                    if(tabela[j][i+1] > 96 && tabela[j][i+1] < 123) { tabela[j][i+1] -= 32; }
                } else if(i==0 && j+1==y) { // canto inferior esquerdo
                    if(tabela[j-1][i] > 96 && tabela[j-1][i] < 123) { tabela[j-1][i] -= 32; }
                    if(tabela[j][i+1] > 96 && tabela[j][i+1] < 123) { tabela[j][i+1] -= 32; }
                } else if(i!=0 && j+1==y && i+1!=x) { // parede inferior
                    if(tabela[j-1][i] > 96 && tabela[j-1][i] < 123) { tabela[j-1][i] -= 32; }
                    if(tabela[j][i-1] > 96 && tabela[j][i-1] < 123) { tabela[j][i-1] -= 32; }
                    if(tabela[j][i+1] > 96 && tabela[j][i+1] < 123) { tabela[j][i+1] -= 32; }
                } else if(j+1==y && i+1==x) { // canto inferior direito
                    if(tabela[j-1][i] > 96 && tabela[j-1][i] < 123) { tabela[j-1][i] -= 32; }
                    if(tabela[j][i-1] > 96 && tabela[j][i-1] < 123) { tabela[j][i-1] -= 32; }
                } else if(j!=0 && j+1!=y && i+1==x) { // parede direita
                    if(tabela[j-1][i] > 96 && tabela[j-1][i] < 123) { tabela[j-1][i] -= 32; }
                    if(tabela[j+1][i] > 96 && tabela[j+1][i] < 123) { tabela[j+1][i] -= 32; }
                    if(tabela[j][i-1] > 96 && tabela[j][i-1] < 123) { tabela[j][i-1] -= 32; }
                } else if(i+1==x && j==0) { // canto superior direito
                    if(tabela[j+1][i] > 96 && tabela[j+1][i] < 123) { tabela[j+1][i] -= 32; }
                    if(tabela[j][i-1] > 96 && tabela[j][i-1] < 123) { tabela[j][i-1] -= 32; }
                } else if(j==0 && i+1!=x && i!=0) { // parede superior
                    if(tabela[j+1][i] > 96 && tabela[j+1][i] < 123) { tabela[j+1][i] -= 32; }
                    if(tabela[j][i-1] > 96 && tabela[j][i-1] < 123) { tabela[j][i-1] -= 32; }
                    if(tabela[j][i+1] > 96 && tabela[j][i+1] < 123) { tabela[j][i+1] -= 32; }
                } else { // meio
                    if(tabela[j-1][i] > 96 && tabela[j-1][i] < 123) { tabela[j-1][i] -= 32; }
                    if(tabela[j+1][i] > 96 && tabela[j+1][i] < 123) { tabela[j+1][i] -= 32; }
                    if(tabela[j][i-1] > 96 && tabela[j][i-1] < 123) { tabela[j][i-1] -= 32; }
                    if(tabela[j][i+1] > 96 && tabela[j][i+1] < 123) { tabela[j][i+1] -= 32; }
                }
            }
            char **tabelaTemp = malloc(y * sizeof(char *)); // função 3
            for (int m = 0; m < y; m++) {
                tabelaTemp[m] = malloc(x * sizeof(char));
            }
            copiarTabela(x,y,tabela,tabelaTemp);
            tabelaTemp[j][i] = '#';
            if(verificarConectividade(x,y,tabelaTemp) == false) {
                if (tabela[j][i] != '#' && tabela[j][i] > 96 && tabela[j][i] < 123) {// verifica se pode meter maiuscula.
                    tabela[j][i] = tabela[j][i] - 32;
                }
            }
            for (int i = 0; i < y; i++) {
                free(tabelaTemp[i]);
            }
            free(tabelaTemp);
        }
    }
    int erro[5]={0}; verificaRegras(x, y, tabela, erro);
    push(topo, x, y, tabela, -1, -1, 'a', erro);
}
int mudouAlgo(Estado **topo, int x, int y) {
    if (*topo == NULL || (*topo)->prox == NULL) {
        return 0;
    }
    Estado *atual = *topo;
    Estado *anterior = atual->prox;
    for (int i = 0; i < y; i++) {
        for (int j = 0; j < x; j++) {
            if (atual->tabela[i][j] != anterior->tabela[i][j]) {
                return 1;
            }
        }
    }
    return 0;
}
void ajudarA(Estado **topo, int x, int y, char **tabela) {
    ajudar (topo,x,y,tabela);
    while (mudouAlgo(topo, x,y)) {
        ajudar (topo,x,y,tabela);
    }
}
void resolver(Estado **topo, int x, int y, char **tabela) {
    while((*topo)->prox != NULL) {
        pop(topo,x,y,tabela);
    }
    char **temp = malloc(y * sizeof(char *));
    for (int i = 0; i < y; i++) {
        temp[i] = malloc(x * sizeof(char));
    }
    for (int i = 0; i < y; i++) {
        for (int c = 0; c < x; c++) {
            copiarTabela(x,y,tabela,temp);
            temp[i][c] = temp[i][c] - 32; // meter maiuscula.
            ajudarA(topo, x, y, temp);
            if (estaresolvidoounao(x, y, temp)) {
                copiarTabela(x,y,temp,tabela);
                int erro[5]={0}; verificaRegras(x, y, tabela, erro);
                push(topo, x, y, tabela, -1, -1, 'R', erro);
                for (int i = 0; i < y; i++) free(temp[i]);
                free(temp);
                return;
            }
        }
    }
    for (int i = 0; i < y; i++) free(temp[i]);
    free(temp);
}
void comando(Estado **topo, int *x, int *y, char ***tabela, int *jogo){ // recebe o comando (letra).
    char a,b;int c;
    char save[100];
    scanf(" %c",&a); // recebe o comando.
    if(a == 'b') {
        scanf(" %c%d",&b,&c);
        inserirMaiuscula(topo,b,c,*x,*y,*tabela);  // inserir maiuscula.
    } else if (a == 'r') {
        scanf(" %c%d",&b,&c);
        inserirHashtag(topo,b,c,*x,*y,*tabela);    // inserir hashtag.
    } else if (a == 'v') {
        verificarEstadoJogo(*topo, *x, *y,*tabela);// verifica estado jogo.
    } else if (a == 'd') {
        desfazer(topo,*x,*y,*tabela);    // desfazer jogada.
    } else if (a == 'a') {
        ajudar(topo,*x,*y,*tabela);                // utiliza os mesmos principios lógicos da função verificar estado do jogo para ajudar o jogador, apenas nesta jogada.
    } else if (a == 'A') {
        ajudarA(topo,*x,*y,*tabela);
    } else if (a == 'R') {
        resolver(topo, *x,*y,*tabela);
    } else if (a == 'g') {
        scanf("%99s", save);
        strcat(save, ".txt");
        gravarJogo(*x, *y, *tabela, save);
    }
    else if (a == 'l') {
        char save[100];
        scanf("%99s",save);
        strcat(save, ".txt");
        loadJogo(x, y, tabela, save);
        int erro[5] = {0};
        verificaRegras(*x, *y, *tabela, erro);
        push(topo, *x, *y, *tabela, -1, -1, 'l', erro);
    }
    else if (a == 's') {
        *jogo = 0;
        while (*topo != NULL) {
            Estado *temp = *topo;
            *topo = (*topo)->prox;
            for (int i = 0; i < *y; i++) {
                free(temp->tabela[i]);
            }
            free(temp->tabela);
            free(temp);
        }
        return;                              // acaba com o loop principal e acaba o jogo.
    } else {
        printf("Comando invalido.\n");
        sleep(3);
    }
}
