#include "header.h"


#define HASH_MULTIPLIER 65599



static unsigned int SymTable_hash(const char *name);

void insert_library_functions(void);


void make_scope_inactive( int scope );


symbolList symbolTable[BUCKETS];


symbolList scopeList;


char *library_function[] = {
	
	"print", 
	"input", 
	"objectmemberkeys",
	"objecttotalmembers",
	"objectcopy",
	"totalarguments",
	"argument",
	"typeof",
	"srtonum",
	"sqrt",
	"cos",
	"sin"
};



void init_symbolTable()
{
     int i;
     
     for(i=0; i<BUCKETS; i++)
		
		symbolTable[i] = newSymbolList();
	
     scopeList = newSymbolList();
	
	insert_library_functions();
}




/*
WARNING
H eisagwgh ginetai xwris elegxo gia an yparxei idi.
Prepei na elegxoume prwta me tis lookup() prin eisagoume ena entry
*/
void insert_to_table( SymbolTableEntry *entry )
{
     if( entry == NULL )
	  return;
	
	int index;
	
	if( entry->isVariable)
		index = SymTable_hash( entry->value.varVal->name );
	else
		index = SymTable_hash( entry->value.funcVal->name );
	
	
	insert_to_list( &(symbolTable[ index ]->head) , entry, 0);
		
	
	SymbolTableEntry *scopeHead = get_scope_head(scopeList, Scope);
		
	if(scopeHead == NULL)
	{	
		/* 
		the scopeList does not have this scope!
		we have to insert a node with the new scope in scopeList
		*/
		SymbolTableEntry *theNewScopeNode = newEntry( userfunc, Scope, "", 0, 0, 0, 0, var_s );
		
		insert_to_list( &(scopeList->head), theNewScopeNode, 0 );
		
		scopeHead = theNewScopeNode;
				
	}
	insert_to_list( &(scopeHead->up) , entry, 1);
	
	
}







/*

Ektypwnei olo to symbolTable

*/
void print_table()
{
	puts("\n\n ============== Symbol Table ==============");
	SymbolTableEntry *scopeHead = scopeList->head;
	
	while( scopeHead != NULL )
	{
		printf("\n ==== SCOPE : %d ==== \n\n", scopeHead->isVariable);
		print_list( scopeHead );
		
		printf("\n");
			
		scopeHead = scopeHead->next;
	}
}



void insert_library_functions()
{
	
	/* Edw dimiourgoume to prwto Node tis scopeList gia scope=0 pou einai oles oi libfunc */
	SymbolTableEntry *theNewScopeNode ;
	theNewScopeNode = newEntry( userfunc, 0, "", 0, 0, 0, 0, var_s);
	

	insert_to_list( &(scopeList->head), theNewScopeNode, 0 );
	
	
	int i;
	
	for(i=0;i<12;i++)
	{
		SymbolTableEntry *entry ;
		entry= newEntry(libfunc, 0, library_function[i] , 0, 0, currscopeoffset(), currscopespace(), libraryfunc_s);
		inccurrscopeoffset();
		
		int index = SymTable_hash( library_function[i] );
		
		insert_to_list( &(symbolTable[ index ]->head) , entry, 0);
		
		insert_to_list( &(theNewScopeNode->up) , entry, 1);
	
	}
	
}




/*
Epitrefei : 
1 ean yparxei function me onoma name kai einai isActive=1
0 ean den yparxei.

kalei tin synartisi hash() me name
pairnei to index
psaxnei sto symbolTable thesh index


Elegxei ean ena onoma metavlitis (name) pou dilwsame kapou
sympiptei me ena onoma synartisis (viliothikis i xristi) 
pou einai energh (Active) se olo to symbolTable.

*/
int lookUp_function_global( const char *name )
{
	int index = SymTable_hash( name );
	
	SymbolTableEntry *listHead = symbolTable[ index ]->head;
	
	if(listHead==NULL)
		return 0;
	
	while( listHead != NULL )
	{
		/* IF is Function */
		if(listHead->isVariable==0)
		{
			if( strcmp( name , listHead->value.funcVal->name ) == 0 && listHead->isActive==1){
				/* SUCCESS*/
				return 1;
			}
		}
		
		listHead = listHead->next;
	}
	/* FAIL */
	return 0;
	
}




/*
Epitrefei : 
1 ean yparxei variable me onoma name *sto idio scope*
0 ean den yparxei.
*/
int lookUp_variable_local( const char *name )
{
	SymbolTableEntry *listHead = get_scope_head(scopeList, Scope );
	
	if(listHead==NULL)
		return 0;
	
	listHead = listHead->up;
	
	while( listHead != NULL )
	{	
		if( listHead->isVariable )
		{	if( strcmp( name , listHead->value.varVal->name ) == 0 && listHead->isActive==1 ){
				/* SUCCESS*/
				return 1;
			}
		}
		else
		{	if( strcmp( name , listHead->value.funcVal->name ) == 0 && listHead->isActive==1){
				/* SUCCESS*/
				return 1;
			}
		}
		listHead = listHead->up;
	}
	/* FAIL */
	return 0;
}


SymbolTableEntry *search_variable_local( const char *name )
{
	SymbolTableEntry *listHead = get_scope_head(scopeList, Scope );
	
	if(listHead==NULL)
		return 0;
	
	listHead = listHead->up;
	
	while( listHead != NULL )
	{	
		if( listHead->isVariable == 1 )
		{
		  if( strcmp( name , listHead->value.varVal->name ) == 0 && listHead->isActive==1 ){
				/* SUCCESS*/
				return listHead;
			}
		}
		else
		{	if( strcmp( name , listHead->value.funcVal->name ) == 0 && listHead->isActive==1){
				/* SUCCESS*/
				return listHead;
			}
		}
		listHead = listHead->up;
	}
	/* FAIL */
	return NULL;
}

/*
Epitrefei : 
1 ean yparxei variable me onoma name sta prohgoumena scopes
0 ean den yparxei.
*/
int lookUp_name_global( const char *name )
{
	int i;
	
	for(i=Scope; i>=0; i--)
	{
		SymbolTableEntry *listHead = get_scope_head(scopeList, i );	
		
		if(listHead==NULL)
			continue;
		
		listHead = listHead->up;
		
		
		
		while( listHead != NULL )
		{	
			if( listHead->isVariable ){
				if( (strcmp( name , listHead->value.varVal->name ) == 0) && (listHead->isActive==1)){
					/*SUCCESS*/
					return 1; 
				}
				
			}else{
				if( (strcmp( name , listHead->value.funcVal->name ) == 0 )&& (listHead->isActive==1)){
					/* SUCCESS*/
					return 1;
				}
			}		
			listHead = listHead->up;
		}
	}
	
	return 0;
}


int look_up_only_Scope0(const char *name){
	
	SymbolTableEntry *listHead = get_scope_head(scopeList, 0 );	
	
	
	if(listHead==NULL)
		return 0;
	
	listHead = listHead->up;
	
	
	while( listHead != NULL )
	{	
		if( listHead->isVariable ){
			if( strcmp( name , listHead->value.varVal->name ) == 0 ){
				/* SUCCESS*/
				return 1;
			}
		}			
		else{
			if( strcmp( name , listHead->value.funcVal->name ) == 0){
				/* SUCCESS*/
				return 1;
			}
		}
				
		listHead = listHead->up;
	}
	return 0;	
}

int look_up_only_LibFuncs(const char *name){
		int i;
		for(i=0; i<12; i++){
			if(strcmp(name,library_function[i])==0){
				return 1;
			}
		}
		return 0;
	
}


/*

Mark all variables in this scope as inactive.

*/
void make_scope_inactive( int scope )
{
	SymbolTableEntry *listHead = get_scope_head(scopeList, scope );
	
	if(listHead==NULL)
		return;
	
	listHead = listHead->up;
	
	while( listHead != NULL )
	{	
		if(listHead->isActive==0)
			return;
		
		listHead->isActive = 0;
		
		listHead = listHead->up;
	}
}




char *getRandomFuncName()
{
	int n = counterFuncName;
	const char *str = "$_Func_";
	char *name ;
	char *s=(char *)malloc(1000*sizeof(char));
	int i, sign, c, j;
	if((sign = n) <0)
	{
		n=-n;
	}
	i=0;
	do{
		s[i++] = n%10 + '0';
	}while((n/=10)>0);
	if(sign <0)
	{
		s[i++] = '-';
	}
	s[i] = '\0';
	for(i=0, (j=strlen(s)-1); (i<j); i++, j--)
	{
		c=s[i];
		s[i] = s[j];
		s[j] = c;
	}
	name=strdup(str);
	strcat(name, s);
	
	counterFuncName++;
	
	return name;
}


char *getRandomTempVar()
{
	int n = counterTempVar;
	const char *str = "$_tmp_";
	char *name ;
	char *s=(char *)malloc(1000*sizeof(char));
	int i, sign, c, j;
	if((sign = n) <0)
	{
		n=-n;
	}
	i=0;
	do{
		s[i++] = n%10 + '0';
	}while((n/=10)>0);
	if(sign <0)
	{
		s[i++] = '-';
	}
	s[i] = '\0';
	for(i=0, (j=strlen(s)-1); (i<j); i++, j--)
	{
		c=s[i];
		s[i] = s[j];
		s[j] = c;
	}
	name=strdup(str);
	strcat(name, s);
	
	counterTempVar++;
	
	return name;
}



static unsigned int SymTable_hash(const char *name)
{
     size_t ui;
     
     unsigned int uiHash = 0U;
     
     for (ui = 0U; name[ui] != '\0'; ui++)
	  uiHash = uiHash * HASH_MULTIPLIER + name[ui];
     
     return uiHash%BUCKETS;
}