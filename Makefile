CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O1 -fsanitize=address -fno-omit-frame-pointer -g -fprofile-arcs -ftest-coverage
GCOV_FLAGS = --branch-probabilities --all-blocks

.PHONY: jogo testar clean coverage

jogo:
	$(CC) $(CFLAGS) jogo.c -o jogo
	./jogo

testar:
	$(CC) $(CFLAGS) testes.c def_funções.c -o testar -lcunit
	./testar
	gcov testar-def_funções.c

coverage: testar
	gcov $(GCOV_FLAGS) def_funções.c

clean:
	rm -f jogo testar *.gcno *.gcda *.gcov
