
#include "header.h"



char *getTypeAsString(enum SymbolTableType type);

void print_this_entry(const char *name, const char *type, const int scope, const int isActive, const int line, const int offset);






SymbolTableEntry *newEntry(enum SymbolTableType type, int isVariable, const char *name, unsigned int scope, unsigned int line, unsigned int offset, enum scopespace_t space, enum symbol_t type_t )
{
     SymbolTableEntry *theEntry;
     
     if( (theEntry = malloc(sizeof(SymbolTableEntry))) == NULL)
     {
	  fprintf(stderr, "\nNo memory available\n");
	  exit(-1);
     }
     
     theEntry->isActive = 1;
     theEntry->type = type;
     theEntry->next = NULL;
     theEntry->up = NULL;
	theEntry->isVariable = isVariable;
	
	theEntry->offset = offset;
	theEntry->space = space;
	theEntry->type_t = type_t;
     
     if(isVariable)
     {
	  if( (theEntry->value.varVal = malloc(sizeof(Variable))) == NULL)
	  {
	       fprintf(stderr, "\nNo memory available\n");
	       exit(-1);
	  } 
	  theEntry->value.varVal->name = strdup(name);
	  theEntry->value.varVal->scope = scope;
	  theEntry->value.varVal->line = line;
     }
     else
     {
	  if( (theEntry->value.funcVal = malloc(sizeof(Function))) == NULL)
	  {
	       fprintf(stderr, "\nNo memory available\n");
	       exit(-1);
	  } 
	  theEntry->value.funcVal->name = strdup(name);
	  theEntry->value.funcVal->scope = scope;
	  theEntry->value.funcVal->line = line;
     }
     
    
	return theEntry;
}






symbolList newSymbolList()
{
     symbolList list;
     if( (list = malloc(sizeof(symbolList))) == NULL)
     {
	  fprintf(stderr, "\nNo memory available\n");
	  exit(-1);
     }
     list->head = NULL;
	
     return list;
}



/*

================= SHMEIWSEIS ===============

1. To entry pou pame na eisagoume den yparxei sti Lista (prepei na exoume kanei lookUp_list() pio prin gia na elegksoume)

2. Prepei na exoume dhmiourgisei prwta ena Node stin scopeList pou tha exei sto isActive tin timi tou scope. 
	
	
	
	gia orizontia list tin kaloume ws:
	
	
	px        insert_to_list(  &(symbolTable[i]->head) , .. );
	
	
	gia katheti list tin kaloume ws:
	
	
	px        insert_to_list(  &(scopeHead->up) , ..  );

*/


void insert_to_list(SymbolTableEntry **listHead, SymbolTableEntry *entry, int moveUP)
{
    	
     if( *listHead == NULL )     
	{	
		*listHead = entry;
	}
     else
     { 
		if(moveUP==1)	
			entry->up = *listHead;
		else if(moveUP==0)
			entry->next = *listHead;
		else
			entry->next1 = *listHead;
		
		*listHead = entry;
     }
}





/*

Pairnei enan akeraio scope kai afou psaxei stin scopeList
epistrefei ena deikti pros to Node pou exei tin katheti lista 
me ola ta symbola to table me auto to scope.
Diaforetika ean dn vrethei to Node me auto to scope 
(dld den yparxei tetoio scope) epistrefei NULL.

!!! elegxei to isVariable san arithmo gia scope

*/

SymbolTableEntry *get_scope_head(symbolList list, int scope )
{
	if(list == NULL)
	{	/* FAIL */
		return NULL;
	}
	
	SymbolTableEntry *head = list->head;
	
	while(head!=NULL)
	{
		if( head->isVariable == scope )
		{	/* SUCCESS */
			return head;
		}
		head = head->next;
	}
	
	/* FAIL */
	return NULL;
}








/*

================= SHMEIWSEIS ===============

!!! To *listHead einai deiktis sto prwto Node (dld ena SymbolTableEntry) tis listas pou psaxnoyme !!!



gia orizontia list tin kaloume ws:


px        print_list(  symbolTable[i]->head , .. );


gia katheti list tin kaloume ws:


px        print_list(  scopeHead->up , ..  );

*/

void print_list(SymbolTableEntry *listHead)
{
     if(listHead == NULL)
	{ // ABORTED
	  return;	
	}
	
	listHead = listHead->up;
	
	int enable = 1;
	
     while( listHead != NULL )
     {
		
		/* if the symbol is libfunc */
		if(listHead->type == 4 && enable)
		{	puts("\n\n ===== Library Functions =====\n");
			enable=0;
		}
		if( listHead->isVariable )
		{
			print_this_entry(listHead->value.varVal->name, getTypeAsString(listHead->type), listHead->value.varVal->scope, listHead->isActive, listHead->value.varVal->line, listHead->offset );
		}
		else
		{
			print_this_entry(listHead->value.funcVal->name, getTypeAsString(listHead->type), listHead->value.funcVal->scope, listHead->isActive, listHead->value.funcVal->line, listHead->offset );
		}	
		
		listHead = listHead->up; 
     }
}



void print_this_entry(const char *name, const char *type, const int scope, const int isActive, const int line, const int offset)
{
	printf(" name: %s", name);
	getGoodAligmentStr(name);
	
	printf(" type: %s", type);
	getGoodAligmentStr(type);
	
	printf(" scope: %d", scope);
	getGoodAligmentNum(scope);
	
	printf("   isActive: %d ", isActive);
	printf("\t  Line: %d", line);
	printf("\t  Offset: %d\n", offset);
}




char *getTypeAsString(enum SymbolTableType type)
{	
	switch(type)
	{
		case 0: return "Global Variable";
		
		case 1: return "Local Variable";
		
		case 2: return "Formal Variable";
		
		case 3: return "User Function";
		
		case 4: return "Library Function";
	
	}
	return "Undefined";
}



void getGoodAligmentStr(const char *s)
{
	int len = strlen(s);
	
	int starting_position = 22;
	
	if(len>=starting_position)
		return;
	
	int max = starting_position-len;
	int i;
	for(i=0;i<max;i++)
		printf(" ");
}



void getGoodAligmentNum(const int num)
{
	if(num<10)
		printf("   ");
	if(num>=10 && num<100)
		printf("  ");
	if(num>=100 & num<1000)
		printf(" ");
}






/*

void freeList(symbolList list)
{
     if(list == NULL)
	  //ABORTED
	  return;
     
     SymbolTableEntry *tmp1 = list->head;
     SymbolTableEntry *tmp2 = tmp1;

     while(tmp1!=NULL)
     {
	  tmp2 = tmp1->next;
	  free(tmp1);
	  tmp1 = tmp2;
     }
     free(list);
     list = NULL;
}

*/

