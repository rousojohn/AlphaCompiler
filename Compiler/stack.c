#include "header.h"

/*


###########################
PRADEIGMATA LEITOURGEIAS STOIVAS
##########################


Stack mystack = newStack();

push(mystack, 5);

int number = pop(mystack);

int number = top(mystack);

if(isEmpty(mystack))
	;


*/





Stack newStack()
{
	Stack s;
	
	if( (s = malloc(sizeof(stack1))) == NULL)
	{
		fprintf(stderr, "\nNo memory available\n");
		exit(-1);
	} 
	s->head = NULL;
	s->length = 0;
	
	return s;
}



int top(Stack s)
{
	if(s==NULL)
		return -1;
	return s->head->number;
}


void push(Stack s, int number)
{
	if(s==NULL)
		return;
	stackNode *entry;
	if( (entry = malloc(sizeof(stackNode))) == NULL)
	{
		fprintf(stderr, "\nNo memory available\n");
		exit(-1);
	} 
	entry->number = number;
	entry->next = NULL;
	entry->previous = NULL;
	
	if( s->head == NULL )
	{	
		s->head = entry;
		s->tail = entry;
		s->length++;
	}
	else
	{ 
		s->head->previous = entry;
		entry->next = s->head;  
		s->head = entry;
		s->length++;
	}
}


Stack merge(Stack a, Stack b)
{
	Stack result = newStack();
	
	if(a!=NULL)
	{
		stackNode *tmpHead = a->tail;
		while(tmpHead!=NULL)
		{
			push(result, tmpHead->number);
			tmpHead = tmpHead->previous;
		}
	}
	
	if(b!=NULL)
	{
		stackNode *tmpHead = b->tail;
		
		while(tmpHead!=NULL)
		{
			push(result, tmpHead->number);
			tmpHead = tmpHead->previous;
		}
	}
	return result;
}




int isEmpty(Stack s)
{
	if(s==NULL)
		return 1;
	if(s->head==NULL)
		return 1;
	else
		return 0;
}


/*
Ean i stoiva einai adeia epistrefei -1
*/
int pop(Stack s)
{
	if(s==NULL)
		return -1;
	if(isEmpty(s))
		return -1;
	int result = s->head->number;
	stackNode *tmp = s->head;
	s->head = s->head->next;
	free(tmp);
	return result;
}


int getLength(Stack s)
{
	if(s==NULL)
		return -1;
	return s->length;
}


void printStack(Stack s)
{
	stackNode *tmp = s->head;
	puts("\n\n === STACK === \n");
	while(tmp!=NULL)
	{
		printf("%d\n", tmp->number);
		tmp = tmp->next;
	}
}









StackFunc newStackFunc()
{
	StackFunc s;
	
	if( (s = malloc(sizeof(stack2))) == NULL)
	{
		fprintf(stderr, "\nNo memory available\n");
		exit(-1);
	}
	s->head = NULL;
	return s;
}

void push_func(StackFunc s, SymbolTableEntry *entry)
{
	if(s==NULL)
		return;
	stackNodeSt *entry2;
	if( (entry2 = malloc(sizeof(stackNodeSt))) == NULL)
	{
		fprintf(stderr, "\nNo memory available\n");
		exit(-1);
	}
	entry2->entry = entry;
	entry2->next = NULL;
	
	if( s->head == NULL )
	{
		s->head = entry2;
	}
	else
	{
		entry2->next = s->head;
		s->head = entry2;
	}
}

SymbolTableEntry *pop_func(StackFunc s)
{
	if(s==NULL)
		return NULL;
	if(isEmpty_func(s))
		return NULL;
	SymbolTableEntry *result = s->head->entry;
	stackNodeSt *tmp = s->head;
	s->head = s->head->next;
	free(tmp);
	return result;
}

SymbolTableEntry *top_func(StackFunc s)
{
	if(s==NULL)
		return NULL;
	return s->head->entry;
}


int isEmpty_func(StackFunc s)
{
	if(s==NULL)
		return 1;
	if(s->head==NULL)
		return 1;
	else
		return 0;
}












