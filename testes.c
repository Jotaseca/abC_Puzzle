#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>
#include "def_funcoes.h"

void test_lerCoordenada() {
    CU_ASSERT_EQUAL(lerCoordenada('a'), 0);
    CU_ASSERT_EQUAL(lerCoordenada('b'), 1);
    CU_ASSERT_EQUAL(lerCoordenada('z'), 25);
}

void test_push_and_pop() {
    Estado *topo = NULL;
    int x = 3, y = 3;
    char **tabela = malloc(y * sizeof(char *));
    for (int i = 0; i < y; i++) {
        tabela[i] = malloc(x * sizeof(char));
        for (int j = 0; j < x; j++) {
            tabela[i][j] = '.';
        }
    }

    int error[5] = {0};
    push(&topo, x, y, tabela, 1, 1, 'b', error);
    CU_ASSERT_PTR_NOT_NULL(topo);
    CU_ASSERT_EQUAL(topo->pos_x, 1);
    CU_ASSERT_EQUAL(topo->pos_y, 1);
    CU_ASSERT_EQUAL(topo->comando, 'b');

    pop(&topo, x, y, tabela);
    CU_ASSERT_PTR_NULL(topo);

    for (int i = 0; i < y; i++) {
        free(tabela[i]);
    }
    free(tabela);
}

void test_inserirMaiuscula() {
    Estado *topo = NULL;
    int x = 3, y = 3;
    char **tabela = malloc(y * sizeof(char *));
    for (int i = 0; i < y; i++) {
        tabela[i] = malloc(x * sizeof(char));
        for (int j = 0; j < x; j++) {
            tabela[i][j] = 'a';
        }
    }

    inserirMaiuscula(&topo, 'a', 1, x, y, tabela);
    CU_ASSERT_EQUAL(tabela[0][0], 'A');
    CU_ASSERT_PTR_NOT_NULL(topo);

    while (topo != NULL) {
        pop(&topo, x, y, tabela);
    }
    for (int i = 0; i < y; i++) {
        free(tabela[i]);
    }
    free(tabela);
}

void test_inserirHashtag() {
    Estado *topo = NULL;
    int x = 3, y = 3;
    char **tabela = malloc(y * sizeof(char *));
    for (int i = 0; i < y; i++) {
        tabela[i] = malloc(x * sizeof(char));
        for (int j = 0; j < x; j++) {
            tabela[i][j] = 'a';
        }
    }

    inserirHashtag(&topo, 'a', 1, x, y, tabela);
    CU_ASSERT_EQUAL(tabela[0][0], '#');
    CU_ASSERT_PTR_NOT_NULL(topo);

    while (topo != NULL) {
        pop(&topo, x, y, tabela);
    }
    for (int i = 0; i < y; i++) {
        free(tabela[i]);
    }
    free(tabela);
}

char** create_board(int x, int y, const char data[y][x]) {
    char** board = malloc(y * sizeof(char*));
    for (int i = 0; i < y; i++) {
        board[i] = malloc(x * sizeof(char));
        memcpy(board[i], data[i], x);
    }
    return board;
}

void free_board(char** board, int y) {
    for (int i = 0; i < y; i++) free(board[i]);
    free(board);
}

// Test Case 1: Erro 0 - Caracteres inválidos
void test_erro0_invalid_character(void) {
    const char layout[3][3] = {
        {'A', 'b', '!'},  // ! é inválido
        {'#', ' ', 'd'},   // espaço é inválido
        {'G', 'H', '@'}    // @ é inválido
    };
    
    char** board = create_board(3, 3, layout);
    int errors[5] = {0};
    
    verificaRegras(3, 3, board, errors);
    
    CU_ASSERT_EQUAL(errors[0], 1); // Deve detectar erro 0
    free_board(board, 3);
}

// Test Case 2: Erro 1 - Duplicatas na mesma linha
void test_erro1_duplicate_row(void) {
    const char layout[3][3] = {
        {'A', 'B', 'A'},  // Duas maiúsculas A
        {'C', 'D', 'E'},
        {'F', 'G', 'H'}
    };
    
    char** board = create_board(3, 3, layout);
    int errors[5] = {0};
    
    verificaRegras(3, 3, board, errors);
    
    CU_ASSERT_EQUAL(errors[1], 1); // Deve detectar erro 1
    free_board(board, 3);
}

// Test Case 3: Erro 1 - Duplicatas na mesma coluna
void test_erro1_duplicate_column(void) {
    const char layout[3][3] = {
        {'B', 'C', 'D'},
        {'b', 'E', 'F'},  // b minúsculo na mesma coluna que B
        {'G', 'H', 'I'}
    };
    
    char** board = create_board(3, 3, layout);
    int errors[5] = {0};
    
    verificaRegras(3, 3, board, errors);
    
    CU_ASSERT_EQUAL(errors[1], 1); // Deve detectar erro 1
    free_board(board, 3);
}

// Test Case 4: Erro 3 - Cardinal com vizinho inválido
void test_erro3_invalid_adjacent(void) {
    const char layout[3][3] = {
        {'#', 'b', 'C'},  // # adjacente a minúscula
        {'D', '#', 'F'},
        {'G', 'H', 'I'}
    };
    
    char** board = create_board(3, 3, layout);
    int errors[5] = {0};
    
    verificaRegras(3, 3, board, errors);
    
    CU_ASSERT_EQUAL(errors[3], 1); // Deve detectar erro 3
    free_board(board, 3);
}

// Test Case 5: Erro 3 - Cardinal com vizinhos válidos
void test_erro3_valid_adjacent(void) {
    const char layout[3][3] = {
        {'A', 'B', 'C'},
        {'D', '#', 'F'},  // # cercado por maiúsculas
        {'G', 'H', 'I'}
    };
    
    char** board = create_board(3, 3, layout);
    int errors[5] = {0};
    
    verificaRegras(3, 3, board, errors);
    
    CU_ASSERT_EQUAL(errors[3], 0); // Não deve detectar erro 3
    free_board(board, 3);
}

// Test Case 6: Erro 4 - Conectividade quebrada
void test_erro4_disconnected_areas(void) {
    const char layout[4][4] = {
        {'A', '#', 'B', 'C'},
        {'#', '#', '#', '#'},
        {'D', '#', 'E', 'F'},
        {'G', '#', 'H', 'I'}
    };
    
    char** board = create_board(4, 4, layout);
    int errors[5] = {0};
    
    verificaRegras(4, 4, board, errors);
    
    CU_ASSERT_EQUAL(errors[4], 1); // Deve detectar erro 4
    free_board(board, 4);
}

// Test Case 7: Erro 4 - Conectividade válida
void test_erro4_connected_areas(void) {
    const char layout[4][4] = {
        {'A', 'B', 'C', '#'},
        {'D', 'E', '#', 'F'},
        {'G', '#', 'H', 'I'},
        {'J', 'K', 'L', 'M'}
    };
    
    char** board = create_board(4, 4, layout);
    int errors[5] = {0};
    
    verificaRegras(4, 4, board, errors);
    
    CU_ASSERT_EQUAL(errors[4], 0); // Não deve detectar erro 4
    free_board(board, 4);
}

// Test Case 8: Casos de borda - Cantos
void test_edge_corner_cases(void) {
    const char layout[3][3] = {
        {'#', 'B', 'C'},  // Canto superior esquerdo
        {'D', 'E', 'F'},
        {'G', 'H', '#'}   // Canto inferior direito
    };
    
    char** board = create_board(3, 3, layout);
    int errors[5] = {0};
    
    // Configurar vizinhos válidos
    board[0][1] = 'B';  // Direita do primeiro #
    board[1][0] = 'D';  // Abaixo do primeiro #
    board[2][1] = 'H';  // Esquerda do último #
    board[1][2] = 'F';  // Acima do último #
    
    verificaRegras(3, 3, board, errors);
    
    CU_ASSERT_EQUAL(errors[3], 0); // Não deve ter erro 3
    free_board(board, 3);
}

// Teste para verificarConectividade - Conectividade válida
void test_verificarConectividade_valid(void) {
    const char layout[4][4] = {
        {'A', 'B', 'C', '#'},
        {'D', 'E', '#', 'F'},
        {'G', '#', 'H', 'I'},
        {'J', 'K', 'L', 'M'}
    };
    char** board = create_board(4, 4, layout);
    CU_ASSERT_TRUE(verificarConectividade(4, 4, board));
    free_board(board, 4);
}

// Teste para estaresolvidoounao - Resolvido
void test_estaresolvidoounao_resolved(void) {
    const char layout[3][3] = {
        {'A', '#', 'B'},
        {'#', 'C', '#'},
        {'D', '#', 'E'}
    };
    char** board = create_board(3, 3, layout);
    CU_ASSERT_FALSE(estaresolvidoounao(3, 3, board));
    free_board(board, 3);
}

// Teste para estaresolvidoounao - Não resolvido
void test_estaresolvidoounao_unresolved(void) {
    const char layout[3][3] = {
        {'A', 'b', 'B'},  // Minúscula ainda presente
        {'#', 'C', '#'},
        {'D', '#', 'E'}
    };
    char** board = create_board(3, 3, layout);
    CU_ASSERT_FALSE(estaresolvidoounao(3, 3, board));
    free_board(board, 3);
}

// Teste para verificarConectividade - Conectividade inválida
void test_verificarConectividade_invalid(void) {
    const char layout[4][4] = {
        {'A', '#', 'B', 'C'},
        {'#', '#', '#', '#'},
        {'D', '#', 'E', 'F'},
        {'G', '#', 'H', 'I'}
    };
    char** board = create_board(4, 4, layout);
    CU_ASSERT_FALSE(verificarConectividade(4, 4, board));
    free_board(board, 4);
}


// Teste para comando - Gravar e carregar jogo
void test_comando_gravar_carregar(void) {
    Estado *topo = NULL;
    int x = 3, y = 3, jogo = 1;
    char **tabela = create_board(3, 3, (const char[3][3]){
        {'a', 'b', 'c'},
        {'d', 'e', 'f'},
        {'g', 'h', 'i'}
    });

    // Gravar o jogo
    freopen("test_save.txt", "w", stdout); // Redireciona stdout para um arquivo
    comando(&topo, &x, &y, &tabela, &jogo);
    freopen("/dev/tty", "w", stdout); // Restaura stdout

    // Carregar o jogo
    comando(&topo, &x, &y, &tabela, &jogo);

    CU_ASSERT_EQUAL(tabela[0][0], 'a'); // Verifica se o jogo foi carregado corretamente

    free_board(tabela, y);
}

// Teste para comando - Sair do jogo
void test_comando_sair(void) {
    Estado *topo = NULL;
    int x = 3, y = 3, jogo = 1;
    char **tabela = create_board(3, 3, (const char[3][3]){
        {'a', 'b', 'c'},
        {'d', 'e', 'f'},
        {'g', 'h', 'i'}
    });
     
    // Simula o comando 's' para sair
       freopen("/dev/null", "w", stdout); // Redireciona stdout para /dev/null
       comando(&topo, &x, &y, &tabela, &jogo);
       freopen("/dev/tty", "w", stdout); // Restaura stdout
   
       CU_ASSERT_EQUAL(jogo, 0); // Verifica se o jogo foi encerrado
   
       free_board(tabela, y);
   }

Estado *criarEstadoInicial(int x, int y, char **tabela_original) {
    Estado *novo = malloc(sizeof(Estado));
    if (!novo) return NULL;

    // Alocar e copiar a tabela
    novo->tabela = malloc(y * sizeof(char *));
    for (int i = 0; i < y; i++) {
        novo->tabela[i] = malloc(x * sizeof(char));
        for (int j = 0; j < x; j++) {
            novo->tabela[i][j] = tabela_original[i][j];
        }
    }

    novo->pos_x = -1;
    novo->pos_y = -1;

    for (int i = 0; i < 5; i++)
        novo->error[i] = 0;

    novo->comando = 'I'; // 'I' de inicial
    novo->prox = NULL;

    return novo;
}

void test_resolver(void) {
    int x = 3, y = 3;

    // Tabuleiro de exemplo
    char *dados_iniciais[] = {
        "XOX",
        "OXO",
        "XOX"
    };

    // Alocar e copiar dados iniciais
    char **tabela = malloc(y * sizeof(char *));
    for (int i = 0; i < y; i++) {
        tabela[i] = malloc(x + 1);
        strcpy(tabela[i], dados_iniciais[i]);
    }

    // Criar estado inicial usando nova função atualizada
    Estado *topo = criarEstadoInicial(x, y, tabela);

    // Garantir que o estado foi criado corretamente
    CU_ASSERT_PTR_NOT_NULL(topo);
    CU_ASSERT_PTR_NOT_NULL(topo->tabela);
    CU_ASSERT_EQUAL(topo->comando, 'I');  // 'I' de inicial, por exemplo

    // Chamar resolver (depois do "update")
    resolver(&topo, x, y, tabela);

    // Verificar se há um novo estado no topo
    CU_ASSERT_PTR_NOT_NULL(topo);

    // Limpeza da memória
    Estado *aux;
    while (topo != NULL) {
        for (int i = 0; i < y; i++) free(topo->tabela[i]);
        free(topo->tabela);
        aux = topo;
        topo = topo->prox;
        free(aux);
    }
    for (int i = 0; i < y; i++) free(tabela[i]);
    free(tabela);
}

void test_mudouAlgo(void) {
    int x = 3, y = 3;

    // Criar tabela original
    char **tabela_original = malloc(y * sizeof(char *));
    for (int i = 0; i < y; i++) {
        tabela_original[i] = malloc(x * sizeof(char));
        for (int j = 0; j < x; j++) {
            tabela_original[i][j] = '.';  // Inicializa com '.' como valor padrão
        }
    }

    // Criar o estado inicial utilizando a função de criação
    Estado *topo = criarEstadoInicial(x, y, tabela_original);

    // Criar tabela modificada (mudança visível)
    char **tabela2 = malloc(y * sizeof(char *));
    for (int i = 0; i < y; i++) {
        tabela2[i] = malloc(x * sizeof(char));
        for (int j = 0; j < x; j++) {
            tabela2[i][j] = '.';  // Inicializa com '.' como valor padrão
        }
    }
    tabela2[1][1] = 'X'; // Modifica a tabela2 em uma posição

    // Push de um estado com tabela diferente
    int erros[5] = {0};
    push(&topo, x, y, tabela2, 1, 1, 'X', erros);

    // Verificar se houve alteração
    CU_ASSERT_EQUAL(mudouAlgo(&topo, x, y, topo->tabela), 1);

    // Libertar memória da tabela original e da tabela modificada
    for (int i = 0; i < y; i++) free(tabela_original[i]);
    free(tabela_original);

    for (int i = 0; i < y; i++) free(tabela2[i]);
    free(tabela2);

    // Libertar memória dos estados na pilha
    Estado *aux;
    while (topo != NULL) {
        for (int i = 0; i < y; i++) free(topo->tabela[i]);
        free(topo->tabela);
        aux = topo;
        topo = topo->prox;
        free(aux);
    }
}

void test_loadJogo() {
    FILE *fp = fopen("test_save.txt", "w");
    fprintf(fp, "2 2\nA B\nC D\n");
    fclose(fp);

    int x, y;
    char **tabela;
    loadJogo(&x, &y, &tabela, "test_save.txt");

    CU_ASSERT_EQUAL(x, 2);
    CU_ASSERT_EQUAL(y, 2);
    CU_ASSERT_EQUAL(tabela[0][0], 'A');
    free_board(tabela, y);
}

void test_desfazer(void) {
    Estado* topo = NULL;
    int x = 3, y = 3;

    // Criar o tabuleiro inicial
    char** tabela = create_board(3, 3, (const char[3][3]){
        {'x', 'A', 'A'},
        {'A', 'A', 'A'},
        {'A', 'A', 'A'}
    });
    int error[5] = {0};verificaRegras(x,y,tabela,error);
    push(&topo,x,y,tabela,-1,-1,'i',error);
    // Fazer uma mudança e guardar o estado
    inserirMaiuscula(&topo,'a',1,x,y,tabela);

    // Desfazer a mudança
    desfazer(&topo, x, y, tabela);

    // Verificar se voltou ao estado original
    CU_ASSERT_EQUAL(tabela[0][0], 'x');

    // Libertar o tabuleiro atual
    free_board(tabela, y);

    // Libertar o estado
    if (topo != NULL) {
        for (int i = 0; i < y; i++) {
            free(topo->tabela[i]);
        }
        free(topo->tabela);
        free(topo);
    }
}

void test_ajudar(void) {
    const char layout[3][3] = {
        {'A', 'b', 'A'},
        {'D', 'e', 'F'},
        {'G', 'h', 'I'}
    };
    char** board = create_board(3, 3, layout);
    Estado* topo = NULL;

    ajudar(&topo, 3, 3, board);

    // Verificar se as regras foram aplicadas
    CU_ASSERT_EQUAL(board[0][2], '#'); // Segundo 'A' deveria ter sido riscado

    // Libertar o tabuleiro original
    free_board(board, 3);

    // Libertar toda a pilha de estados
    while (topo != NULL) {
        Estado* temp = topo;
        topo = topo->prox;

        // Libertar cada linha da tabela do estado
        for (int i = 0; i < 3; i++) {
            free(temp->tabela[i]);
        }
        free(temp->tabela);
        free(temp);
    }
}

int main() {
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Jogo Tests", NULL, NULL);
    
    // Testes originais
    CU_add_test(suite, "Test lerCoordenada", test_lerCoordenada);
    CU_add_test(suite, "Test push and pop", test_push_and_pop);
    CU_add_test(suite, "Test inserirMaiuscula", test_inserirMaiuscula);
    CU_add_test(suite, "Test inserirHashtag", test_inserirHashtag);

    // Novos testes para verificaRegras
    CU_add_test(suite, "Test Erro0 - Caracter Inválido", test_erro0_invalid_character);
    CU_add_test(suite, "Test Erro1 - Duplicata Linha", test_erro1_duplicate_row);
    CU_add_test(suite, "Test Erro1 - Duplicata Coluna", test_erro1_duplicate_column);
    CU_add_test(suite, "Test Erro3 - Adjacência Inválida", test_erro3_invalid_adjacent);
    CU_add_test(suite, "Test Erro3 - Adjacência Válida", test_erro3_valid_adjacent);
    CU_add_test(suite, "Test Erro4 - Áreas Desconectadas", test_erro4_disconnected_areas);
    CU_add_test(suite, "Test Erro4 - Áreas Conectadas", test_erro4_connected_areas);
    CU_add_test(suite, "Test Bordas - Casos de Canto", test_edge_corner_cases);

    // Novos testes para verificarConectividade e estaresolvidoounao
    CU_add_test(suite, "Test verificarConectividade - Válido", test_verificarConectividade_valid);
    CU_add_test(suite, "Test verificarConectividade - Inválido", test_verificarConectividade_invalid);
    CU_add_test(suite, "Test estaresolvidoounao - Resolvido", test_estaresolvidoounao_resolved);
    CU_add_test(suite, "Test estaresolvidoounao - Não Resolvido", test_estaresolvidoounao_unresolved);
    CU_add_test(suite, "Test resolver", test_resolver);
    CU_add_test(suite, "Test mudouAlgo", test_mudouAlgo);
    CU_add_test(suite, "Test load", test_loadJogo);
    CU_add_test(suite, "Test desfazer", test_desfazer);
    CU_add_test(suite, "Test ajudar", test_ajudar);

    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}