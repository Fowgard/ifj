#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#define MAXSTACK 1000


/* pomocné zásobníky */
typedef struct	{                          /* zásobník hodnot typu tBTNodePtr */
    int a[MAXSTACK];
    int top;
} tStack;


void SInit (tStack *S);
void SPush (tStack *S, int token_type);
int SPop (tStack *S);
bool SEmpty (tStack *S);