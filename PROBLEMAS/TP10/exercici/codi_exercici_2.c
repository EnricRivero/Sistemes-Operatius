#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  int *a;

  a = malloc(1 * sizeof(int));

  printf("Escric a la posicio 0\n");
  a[0] = 11;

  free(a);

  printf("Torno a escriure a la posicio 0\n");
  a[0] = 22;

  return 0;
}
