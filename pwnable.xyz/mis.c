#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(int argc, const char **argv, const char **envp)
{
  char s; // [rsp+10h] [rbp-A0h]
  uint64_t v5[3]; // [rsp+18h] [rbp-98h]
  int64_t v6; // [rsp+30h] [rbp-80h]
  int64_t v7; // [rsp+38h] [rbp-78h]
  int64_t v8; // [rsp+40h] [rbp-70h]
  uint64_t v9 = 0x28u;

  memset(&s, 0, 0x98uLL);
  *(uint64_t *)((char *)v5 + 7) = 3735928559LL;
  while ( (unsigned int) scanf("%ld %ld %ld", &v6, &v7, &v8) == 3 && v8 <= 9 && v8 >= -7 )
  {
    v5[v8 + 6] = v6 + v7;
    printf("v5[v8 + 6] addr: %p\n", (void *)&v5[v8 + 6]);
    printf("result %016lx\n", *(uint64_t *)((char *)v5 + 7));
  }
  if ( *(uint64_t *)((char *)v5 + 7) == 47244640437LL )
    printf("%s\n", "win!");
  return 0;
}