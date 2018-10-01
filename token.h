#include "token.c"
typedef struct Tokens {
   char *lexem;
   int lenght;
} token_t;  


void token_init(token_t *token);