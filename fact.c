#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
int fact(int n) {
  kill(getpid(), SIGTRAP);
  if(n < 0) return -1;
  if(n == 0 || n == 1) return 1;
  return n * fact(n-1);
}
int main() {
  int k = 5;
  int fk = fact(k);
  printf("fact(%d) = %d\n", k, fk);
  return 0;
}
