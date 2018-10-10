#include <stdio.h>
#include <stdlib.h>

int key_counter = 0;

typedef struct Token {
   char *key;
   char type;
   char *lexem[];
   int lenght;
} token_t;  


void token_init(token_t *token);
void token_putchar(char *znak, token_t *token);
void set_token(int type, char *attribute);