#include <stdio.h>
#include <stdlib.h>

typedef struct Tokens {
   char *type;
   char *attribute;
   int lenght;
} token_t;  


void token_init(token_t *token);
void token_putchar(char *znak, token_t *token);
void set_token(int type, char *attribute);