#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// you may ignore the following two lines
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

int global = 12;

void a(int c) {
  int bar = 15;
  while (bar < 18)
    bar++;
}

bool test() {
  return true;
}

int main(int argc, char** argv) {
	int i;
	char *str = "hello, world!", ch;
	int j = 0;
	for (; j < argc; j++)
	  puts(argv[j]);

	int foo = 3;
	a(0);

	for (i = 0; i < strlen(str); i++)
		ch = str[i];

	printf("%s\n",str);

	return 0;
}
