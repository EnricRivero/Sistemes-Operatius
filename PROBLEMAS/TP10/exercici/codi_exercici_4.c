#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  int *a;

  a = malloc(10 * sizeof(int));

  printf("Valor d'a[7]: %d\n", a[7]);

  free(a);

  return 0;
}
