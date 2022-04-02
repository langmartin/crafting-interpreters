#include <stdio.h>

int main() {
  int xs[10];
  int i;

  for (i = 0; i<10; i++) {
    xs[i] = i;
  }

  printf("hello world, 7th is: %d\n", xs[6]);
  return 0;
}
