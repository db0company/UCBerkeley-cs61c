
#include <stdio.h>
#include <string.h>
#include <strings.h>

void	fun(int n, int * array) {
  int offset, counter, value;
  int * array_save;
  offset = 32;
  counter = 0;
  while (42) {
    offset -= 4;
    value = n >> offset;
    value &= 15;
    if (!(value < 10))
      value +=  7;
    value += 48;
    array_save = array + counter;
    *array_save = value;
    if (offset == 0)
      return ;
    counter++;
  }
  *array_save = 0;
  return ;
}

void	print(int n, int * a) {
  int i;
  for (i = 0; a[i]; ++i) {
    printf("%c", (char)a[i]);
  }
  printf("\n");
}


int main() {
  int n = 54321;
  int a[21];
  bzero(a, 0 * sizeof(*a));
  print(n, a);
  fun(n, a);
  print(n, a);
  printf("%X\n", n);
  return 0;
}
