CC = gcc -Wall -Werror -Wextra -std=c11

s21_grep:
	$(CC) grep.c -o s21_grep

clean:
	rm s21_grep

rebuild: clean s21_grep

test:
	sh tests.sh

test_leaks:
	sh test_leaks.sh

style:
	clang-format -style=Google -n grep.c