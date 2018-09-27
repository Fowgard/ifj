#include <stdio.h>

#define FILE_ERROR 5

int main(int argc, char *argv[])
{
	FILE *f;

	if (argc != 2) 
	{
		fprintf(stderr, "Nesparvny pocet argumentu.");
		exit(FILE_ERROR);
	}
	
	if (f = fopen(argv[1], "r") == NULL)
	{	
		fprintf(stderr, "Nepodarilo se otevrit soubor.");
		exit(FILE_ERROR);
	}

	
	printf("%s\n", "HELL");

	return 0;
}