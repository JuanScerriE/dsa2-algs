#include <stdlib.h>
#include <string.h>

static inline int max(int x, int y) {
  return x < y ? y : x;
}

static inline int min(int x, int y) {
  return x < y ? x : y;
}

char *lcs(char *x, char *y) {
  int xi = 0;
  int yi = 0;
  int xj = 0;
  int yj = 0;
  int xl = strlen(x);
  int yl = strlen(y);
  char *cs = (char *)malloc(sizeof(char) * min(xl, yl));
  cs[0] = '\0';
  char *lcs = (char *)malloc(sizeof(char) * min(xl, yl));
  lcs[0] = '\0';

  int si = 0;

traverse_x:
  if (xi < xl) {
    while (xj < xl) {
      if (y[yi] == x[xj]) {
        cs[si++] = x[xj];
        
      }
    } 
  }
traverse_y:
  if (yi < yl) {
    while (yj < xl) {

    }
  }
}
