#include <stdio.h>

int main(int argc, char const *argv[])
{
	char *ptr;
	if((ptr = getenv("PATH")) == NULL)
    {
      printf("[-] Can't find PATH.\n");
      return 1;
    }
    printf("%p\n", ptr);
	return 0;
}