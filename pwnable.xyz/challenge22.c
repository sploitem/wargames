#include <stdio.h>

int check(int v4, int v5)
{
	//if ( v4 <= 4918 && v5 <= 4918 )
	//{
		if ( v4 - v5 == 4919 )
		{
			return 0;
		}
	//}
	return 1;
}
int main(int argc, char const *argv[])
{
	unsigned int v4=4918;
	unsigned int v5=0;
	while (check(v4, v5--) && v5 <= 4918)
		;
	printf("%u %u %i \n", v4, v5, (v4 - v5));
	return 0;
}