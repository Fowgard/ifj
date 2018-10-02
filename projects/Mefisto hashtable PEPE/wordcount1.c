#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
 
#define SLOVO_MAX 127
#define HTAB_VELIKOST 1000

typedef struct htab_listitem{
	char* key;
	unsigned int data;
	struct htab_listitem *next;
} htab_listitem;

typedef struct htab{
	unsigned int size;
	htab_listitem* array[];
} htab_t;

unsigned int htab_size(htab_t * t)
{
	return t->size;
}

unsigned int htab_bucket_count( htab_t *t)
{
	unsigned int citac = 0;
	for (int i = 0; i < HTAB_VELIKOST; ++i)
		if (t->array[i] != NULL)
			citac++;
	return citac;
}

unsigned int htab_hash_function(const char *str)
{
	unsigned int h=0;     // 32bit
	const unsigned char *p;
	for(p=(const unsigned char*)str; *p!='\0'; p++)
		h = 65599*h + *p;
	return h;
}

htab_listitem * htab_find(htab_t *t, const char *key)
{
	unsigned int index = htab_hash_function(key) % t->size;
	htab_listitem* item = t->array[index];

	while(item != NULL) 
	{
		if(strcmp(item->key, key) == 0)
			return item;

		item = item->next;
	}

	return item;
}

htab_listitem * htab_lookup_add(htab_t *t, const char *key)
{
	unsigned int index = htab_hash_function(key) % t->size;
	htab_listitem* item = htab_find(t, key);
	if(item == NULL)
	{

		htab_listitem* prev = NULL;
		while(item != NULL)
		{
			if (strcmp(item->key,key))
			{
				prev = item;
				break;
			}
			prev = item;
			item = item->next;
		}

		htab_listitem* add = (htab_listitem*) malloc(sizeof(htab_listitem));

		if(add != NULL)
		{
			add->key = malloc(sizeof(char)*(strlen(key)+1));
			add->key = strncpy(add->key, key, strlen(key)+1);
			add->next = NULL;
			item = add;
			if (prev != NULL && prev != item)
				prev->next = item;
			t->array[index] = item;
		}

	}
	return item;

}

bool htab_remove(htab_t *t, const char *key)
{
	unsigned int index = htab_hash_function(key) % t->size;
	htab_listitem* pom = htab_find(t,key);
	htab_listitem* item = t->array[index];
	if(pom != NULL)
	{
		htab_listitem* prev = t->array[index];
		while(item != NULL)
		{
			if (strcmp(item->key, key) == 0)
			{
				if (prev == item)
					t->array[index] = item->next;
				prev->next = item->next;

				free(item);
				break;
			}
			prev = item;
			item = item->next;
		}
		

		return true;
	}
	else
		return false;
	
}


void htab_clear(htab_t *t)
{
	htab_listitem* item = NULL;

	for(unsigned i = 0; i < t->size; i++)
	{	item = t->array[i];
		while(item != NULL)
		{
			htab_remove(t,item->key);
			item = item->next;
		}
	}
}



void htab_foreach(htab_t *t, void (*function)(htab_listitem *item))
{
	htab_listitem* item = NULL;

	for(unsigned i = 0; i < t->size; i++)
		for (item = t->array[i]; item != NULL; item = item->next)
			function(item);
			
		
}

void htab_free(htab_t* t)
{
	htab_clear(t);
	free(t);
}



htab_t* htab_init(unsigned size)
{
	htab_t* tab = malloc(size*sizeof(htab_listitem) + sizeof(htab_t));
	if(tab == NULL)
		return NULL;

	tab->size = size;

	for(unsigned i = 0; i < size; i++)
		tab->array[i] = NULL;

	return tab;
}

int get_word(char *s, int max, FILE *f)
{
	if (f == NULL)
		return -1;

	int i = 0;
	for (int c = 0; i < max && (c = fgetc(f)) != EOF && !isspace(c); i++)
		s[i] = c;

	s[i]= '\0';

	if (i == 0)
	 return EOF;

	return i;
}

void vypis(htab_listitem* item)
{
	printf("%s\t%d\n", item->key, item->data);
}

int main(int argc, char* argv[])
{
	htab_t* tabulka = htab_init(HTAB_VELIKOST);
	char slovo[SLOVO_MAX];
	htab_listitem* item;
	FILE * vstup = NULL;
	if (argc ==2)
	{
		vstup = fopen(argv[1], "r");
		if (vstup == NULL)
		{
			fprintf(stderr, "Error nepodaril se nacis soubor\n");
			return -1;
		}
	}
	else
		vstup = stdin;

	while(get_word(slovo, SLOVO_MAX, vstup) != EOF)
	{
		item = htab_lookup_add(tabulka,slovo);
		item->data++;
	}

	htab_foreach(tabulka, &vypis);
	
	htab_free(tabulka);

	return 0;
}