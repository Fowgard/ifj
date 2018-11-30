#include "stack.h"
#include "priority.h"


void SInit (tStack *Stack ) 
/*   ------
** Inicializace zásobníku.
**/
{
	Stack->top = 0;  
}	

void SPush (tStack *Stack, int token_type)
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
{ 
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
  if (Stack->top==MAXSTACK) 
    printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
  else {  
		Stack->top++;  
		Stack->a[Stack->top]=token_type;
	}
}	

int SPop (tStack *Stack)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (Stack->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
		exit(99);
	}	
	else {
		return (Stack->a[Stack->top--]);
	}	
}

int STop (tStack *Stack)
/*         --------
** Vrati prvek z vrcholu zasobniku, ale neodstrani ho jako SPop.
**/
{
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (Stack->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
		exit(99);
	}	
	else {
		return (Stack->a[Stack->top]);
	}	
}

bool SEmpty (tStack *Stack)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
  return(Stack->top==0);
}	