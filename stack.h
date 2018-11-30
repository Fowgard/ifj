#ifndef STACK_H
#define STACK_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#define MAXSTACK 1000


/* pomocné zásobníky */
typedef struct	{                          /* zásobník hodnot typu int */
    int a[MAXSTACK];
    int top;
} tStack;


void SInit (tStack *Stack);
void SPush (tStack *Stack, int token_type);
int SPop (tStack *Stack);
int STop (tStack *Stack);
bool SEmpty (tStack *Stack);

#endif