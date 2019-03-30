/*
	Created by bla;
				++edx
*/
#include <stdio.h>
#include <string.h>
#include <openssl/md5.h>

#define MAX_NESTING 100
int bf(char *prog, char *result, int maxlen)
{
	int output_len = 0;
	char tape[4001];
	int edi=0, eip=0;
	unsigned long endless_loop_protection = 100000;
	int loopstart[MAX_NESTING + 1], depth = 0, state;

	memset(tape, 0, sizeof(tape));
	while(eip < strlen(prog) && --endless_loop_protection)
	{
		switch(prog[eip])
		{
			case '<':
				if(edi) --edi;
				break;
			case '>':
				if(edi<4000) ++edi;
				break;
			case '+':
				++tape[edi];
				break;
			case '-':
				--tape[edi];
				break;
			case '.':
				if (output_len < maxlen) result[output_len++] = tape[edi];
				break;
			case ',':
				/* not implemented */
				break;
			case '[':
				state=1;
				if(!tape[edi])
					while (state && ++eip < strlen(prog))
						if (prog[eip] ==']') --state;
						else if (prog[eip] == '[') ++state;
						else;
				else if (depth < MAX_NESTING)
					loopstart[++depth] = eip;
				break;
			case ']' :
				if(depth)
					if(tape[edi] == 0)
						--depth;
					else
						eip = loopstart[depth];
				break;
		}
		++eip;
	}

	result[output_len]=0;
	return output_len;
}


int main(int argc, char **argv, char **env)
{
	MD5_CTX prog1_md5;
	MD5_CTX prog2_md5;
	char prog1_hash[17];
	char prog2_hash[17];
	int i;
	char prog1_output[101];
	char prog2_output[101];
	int len1,len2;
	char *dropshell[] = {"/bin/sh", 0};

	if(argc!=3)
	{
		printf("USAGE:\n\t%s <prog1> <prog2>\n",argv[0]);
		return 1;
	}

	MD5_Init(&prog1_md5);
	MD5_Update(&prog1_md5,argv[1],strlen(argv[1]));
	MD5_Final(prog1_hash,&prog1_md5);

	MD5_Init(&prog2_md5);
	MD5_Update(&prog2_md5,argv[2],strlen(argv[2]));
	MD5_Final(prog2_hash,&prog2_md5);

	for (i = 0; i < 16; ++i)
		if (prog1_hash[i] != prog2_hash[i] && printf("Prog1 and Prog2 are too different\n"))
			return 1;

	len1=bf(argv[1],prog1_output,100);
	len2=bf(argv[2],prog2_output,100);

	printf("output prog1: %s\n",prog1_output);
	printf("output prog2: %s\n",prog2_output);

	if (len1 != len2 || memcmp(prog1_output,prog2_output,len1)) {
		if (!strcmp(prog1_output,"io.sts Rules!") && !strcmp(prog2_output,"io.sts Sucks!")){
			printf("congrats you did it\n");
			setresuid(geteuid(), geteuid(), geteuid());
			execve(dropshell[0],dropshell,0);
		} else
			printf("That's good but not entirely what I want to see\n");

	} else {
		printf("Sorry both programs output the same, there is no"
			"point in having two programs do the same task!\n");
	}
	return 0;
}
