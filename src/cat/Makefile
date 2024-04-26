CC = gcc -Wall -Werror -Wextra -std=c11

s21_cat:
	$(CC) cat.c -o s21_cat

clean:
	rm s21_cat

rebuild: clean s21_cat

test:
	sh test_func_cat.sh

test_leaks:
	sh test_leak_cat.sh

style:
	clang-format -style=Google -n cat.c