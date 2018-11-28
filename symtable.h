#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "token.h"


/* Maximální velikost pole pro implementaci
   vyhledávací tabulky. Řešené procedury však
   využívají pouze HTSIZE prvků pole (viz deklarace této proměnné).
*/
#define MAX_HTSIZE 101

/* typ klíče (například identifikace zboží) */
typedef char* tKey;

/* typ obsahu (například cena zboží) */
typedef struct tData{
	int type;
	lexem_t lexem;
	bool definovano;

}tData;

/*Datová položka TRP s explicitně řetězenými synonymy*/
 typedef struct tHTItem{
	tKey key;				/* klíč  */
	tData *data;				/* obsah */
	struct tHTItem* ptrnext;	/* ukazatel na další synonymum */
} tHTItem;

/* TRP s explicitně zřetězenými synonymy. */
typedef tHTItem* tHTable[MAX_HTSIZE];

/*    
   POZOR! Pro správnou funkci TRP musí být hodnota této proměnné prvočíslem.
*/
extern int HTSIZE;

/* Hlavičky řešených procedur a funkcí. */

int hashCode ( tKey key );

void htInit ( tHTable* ptrht );

tHTItem* htSearch ( tHTable* ptrht, tKey key );

void htInsert ( tHTable* ptrht, tKey key, tData *data );

tData* htRead ( tHTable* ptrht, tKey key );

void htDelete ( tHTable* ptrht, tKey key );

void htClearAll ( tHTable* ptrht );


