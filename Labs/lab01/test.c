#include <stdlib.h>

#define	abs(val)	((val >= 0) ? val : -val)
#define	RADIX	"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"

size_t		nbrlen(int input, unsigned int radix) {
  size_t	i;

  i = 1;
  if (input < 0)
    ++i;
  while (radix > 1 && (input /= radix))
    ++i;
  return i;
}

void	itoa(int input, char * buffer, unsigned int radix) {
  size_t	i;

  i = nbrlen(input, radix) - 1;
  buffer[i] = '\0';
  if (input < 0)
    buffer[0] = '-';
  while ((input)) {
      buffer[i] = RADIX[abs(input) % radix];
      input /= radix;
      --i;
    }
}

char *		itoadup(int input, unsigned int radix) {
  char *buffer;

  if (radix <= 1
      || radix >= (int)sizeof(RADIX)
      || !(buffer = malloc(sizeof(*buffer)
			   * ((nbrlen(input, radix) - 1) + 2))))
    return NULL;
  itoa(input, buffer, radix);
  return buffer;
}

void		printbin(unsigned int nb) {
  char * str;
  str = itoadup(nb, 2);
  puts(str);
  free(str);
}


