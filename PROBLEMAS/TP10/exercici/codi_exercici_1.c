#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  int i;
  int *a;

  a = malloc(5 * sizeof(int));

  for(i = 0; i < 7; i++) {
      printf("Escric a la posicio %d\n", i);
      a[i] = i;
  }

  free(a);

  return 0;
}
