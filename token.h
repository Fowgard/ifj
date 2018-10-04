#include <stdio.h>
#include <stdlib.h>

typedef struct Tokens {
   //char *lexem; nahrazuji typem
   char *type;
   char *attribute;
   int lenght;
} token_t;  


void token_init(token_t *token);