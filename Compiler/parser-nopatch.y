%{
     #include "header.h"
     
     int matchParameter(char *param);
     
     int yyerror(char *yaccProvidedMessage);
     int yylex(void);
	
	void insert_to_elist( expr1 *e );
	void insert_to_elistStack(expr1 *e);
	
	int inFunction = 0;
	int inBlock = 0;
	int blockFun=0;
	int inLoop=0;

	int printRules = 0;
	int printTable = 0;
	int makeQuads = 0;
	int makeInstructions = 0;
	
	Stack stack;

	
	
	expr1 *elistStack = NULL;
	expr1 *eListHead = NULL;
	expr1 *eListTail = NULL;

	symbolList nestedFunctions = NULL;
	
     SymbolTableEntry *tmpEntry = NULL, *tmpEntry2 = NULL;
	
	
	void print_eList()
	{
		puts("\n\n  ============= eList ============= \n");
		expr1* tmp = eListHead;
		
		while(tmp!=NULL)
		{
			if(tmp->type == constnum_e )
				printf(" Value : %lf\n", tmp->numConst);
			else if(tmp->type == constchar_e)
				printf(" Name : %s\n", tmp->strConst );
			else
				printf(" Name : %s\n", tmp->sym->value.varVal->name );
			
				
			tmp = tmp->next;
		}
		puts("\n\n");
	}
	
	
	
%}


%defines "parser.h"

%output = "parser.c"

%expect 1



%union {
	
     char *stringValue;
     int intValue;
     double realValue;
	struct SymbolTableEntry *symb;
	struct expr *ex;
	struct stmt *statement;
}


%type <stringValue> returnstmt    
%type <intValue>  ifprefix elseprefix whilestart whilecond M N
%type <symb> funcdef 
%type <ex> lvalue member primary assignexpr call term objectdef const expr elist elist1 idlist idlist1 normcall callsuffix  methodcall indexed indexedelem

%type <statement> stmt statements forprefix ifstmt whilestmt forstmt block

%token <stringValue> ID
%token <stringValue> STRING
%token <intValue> INTEGER
%token <realValue> REAL


%token<stringValue> IF ELSE WHILE FOR FUNCTION RETURN BREAK CONTINUE
%token<stringValue> LOCAL TRUE FALSE
%token<stringValue> NIL ASSIGN ADD SUB MUL DIV PERCENT
%token<stringValue> EQUAL SMALLER BIGGER SMALLEREQUAL BIGGEREQUAL DIFFERENT OR AND NOT
%token<stringValue> INCREASE DECREASE
%token<stringValue> LEFT_CURLBRACKET RIGHT_BRACKET LEFT_BRACKET RIGHT_CURLBRACKET DOT
%token<stringValue>  LEFT_PARENTHESIS RIGHT_PARENTHESIS QUESTIONMARK COMMA UPDOWN_DOT DOUBLE_UPDOWN_DOT
%token<stringValue> DOUBLE_DOT




%left ASSIGN
%left OR
%left AND
%nonassoc EQUAL DIFFERENT
%nonassoc BIGGER SMALLER BIGGEREQUAL SMALLEREQUAL
%left ADD SUB
%left MUL DIV PERCENT
%right NOT INCREASE DECREASE
%nonassoc UMINUS
%left DOT DOUBLE_DOT
%left LEFT_BRACKET RIGHT_BRACKET
%left LEFT_PARENTHESIS RIGHT_PARENTHESIS



%start program


%%


program:	statements { if(printRules)  printf(" program: statements\n");   }
	       |  { if(printRules)  printf(" program: *empty*\n");  }
		;
	       
		
		
statements:	statements stmt 
				{
					//assert($1->breakList);
					//assert($$);
					//assert($2->breakList);

						$$->breakList = merge($1->breakList, $2->breakList);
						$$->contList = merge($1->contList, $2->contList);
				

					
					if(printRules)  printf(" statements: statements stmt\n");
				}
				
	       | stmt 
			{
				
				$$ = $1;
				if(printRules)  printf(" statements: stmt\n");
			}
		;

		
		
		
stmt:		expr QUESTIONMARK
				{
					//assert($$->sym);
					//assert($1->sym);
					$$ = malloc(sizeof(struct stmt));
					//$$ = $1->sym;
					if(printRules)  printf(" stmt: expr QUESTIONMARK\n");
				}
				
	       | ifstmt 
			{
				$$ = $1;
				
				if(printRules)  printf(" stmt: ifstmt\n");
			}
			
	       | whilestmt 
				{ 
					$$ = $1;
					if(printRules)  printf(" stmt: whilestmt\n");
				}
				
	       | forstmt
				{	
					$$ = $1;
					if(printRules)  printf(" stmt: forstmt\n");
				}
				
	       | returnstmt 
				{ 
					$$ = malloc(sizeof(struct stmt));
					
					$$->breakList=NULL;
					$$->contList=NULL;
					if(printRules)  printf(" stmt: returnstmt\n");
					if(inFunction==0)
					{
						yyerror("Return Statement out of function");
					}
			    }
			    
	       | BREAK QUESTIONMARK
			    { 
					if(inLoop==0)
					{
						yyerror("Break Statement out of loop");
					}
				$$ = malloc(sizeof(struct stmt));
				$$->breakList = newStack();
				push($$->breakList, nextquadlabel());
				emit(jump, NULL, NULL, NULL, 0, yylineno);
				
					
				if(printRules)  printf(" stmt: BREAK QUESTIONMARK\n");
			    }
	       | CONTINUE QUESTIONMARK 
			    {
					if(inLoop==0)
					{
						yyerror("Continue Statement out of loop");
					}
					
					$$ = malloc(sizeof(struct stmt));
					$$->contList = newStack();
					push($$->contList, nextquadlabel());
					emit(jump, NULL, NULL, NULL, 0, yylineno);
					
					if(printRules)  printf(" stmt: CONTINUE QUESTIONMARK\n");
			     }
	       | block 
			{
				$$ = $1;
				if(printRules)  printf(" stmt: block\n");
			}
			
	       | funcdef
			{
				$$ = malloc(sizeof(struct stmt));
				if(printRules)  printf(" stmt: funcdef\n");
			}
			
	       | QUESTIONMARK 
			{
				$$ = malloc(sizeof(struct stmt));
				$$->breakList=NULL;
				$$->contList=NULL;
				if(printRules)  printf(" stmt: QUESTIONMARK\n");
			}
			
	       ;
	      

	       
	       
expr:	assignexpr { $$ = $1; if(printRules)  printf(" expr: assignexpr\n");  }

	       | expr ADD expr
			{
			  $$ = expr_op(arithmexpr_e); 
			  $$->sym = newTemp(); 
			  emit(add, $1, $3, $$, 0, yylineno); 
			  if(printRules)  printf(" expr: expr ADD expr\n");
											}
	       | expr SUB expr 
				{
					$$ = expr_op(arithmexpr_e); 
					$$->sym = newTemp(); 
					emit(sub, $1, $3, $$, 0, yylineno); 
					if(printRules)  printf(" expr: expr SUB expr\n");
				}
	       
	       | expr MUL expr 
				{ 
					$$ = expr_op(arithmexpr_e); 
					$$->sym = newTemp(); 
					emit(mul, $1, $3, $$, 0, yylineno); 
					if(printRules)  printf(" expr: expr MUL expr\n"); }
					
	       | expr DIV expr 
			{ 
				$$ = expr_op(arithmexpr_e); 
				$$->sym = newTemp(); 
				emit(Div, $1, $3, $$, 0, yylineno); 
				if(printRules)  printf(" expr: expr DIV expr\n"); }
				
	       | expr PERCENT expr 
			{
				$$ = expr_op(arithmexpr_e); 
				$$->sym = newTemp(); 
				emit(mod, $1, $3, $$, 0, yylineno); 
				if(printRules)  printf(" expr: expr PERCENT expr\n"); }
				
	       | expr BIGGER expr 
			{
				$$ = expr_op(booleanexpr_e); 
				$$->sym = newTemp(); 
				emit(if_greater, $1, $3, NULL,nextquadlabel()+3, yylineno); 
				emit(assign,getExprConstBool("false"),NULL,$$,0,yylineno);
				emit(jump,NULL,NULL,NULL,nextquadlabel()+2,yylineno);
				emit(assign,getExprConstBool("true"),NULL,$$,0,yylineno);
				if(printRules)  printf(" expr: expr BIGGER expr\n"); }
				
	       | expr BIGGEREQUAL expr
			{ 
				$$ = expr_op(booleanexpr_e); 
				$$->sym = newTemp(); 
				emit(if_greateq, $1, $3,NULL,nextquadlabel()+3, yylineno); 
				emit(assign,getExprConstBool("false"),NULL,$$,0,yylineno);
				emit(jump,NULL,NULL,NULL,nextquadlabel()+2,yylineno);
				emit(assign,getExprConstBool("true"),NULL,$$,0,yylineno);
				if(printRules)  printf(" expr: expr BIGGEREQUAL expr\n"); }
				
	       | expr SMALLER expr 
			{
				$$ = expr_op(booleanexpr_e); 
				$$->sym = newTemp(); 
				emit(if_less, $1, $3, NULL, nextquadlabel()+3, yylineno); 
				emit(assign,getExprConstBool("false"),NULL,$$,0,yylineno);
				emit(jump,NULL,NULL,NULL,nextquadlabel()+2,yylineno);
				emit(assign,getExprConstBool("true"),NULL,$$,0,yylineno);
				if(printRules)  printf(" expr: expr SMALLER expr\n"); }
				
	       | expr SMALLEREQUAL expr 
			{
				$$ = expr_op(booleanexpr_e); 
				$$->sym = newTemp(); 
				emit(if_lesseq, $1, $3, NULL, nextquadlabel()+3, yylineno); 
				emit(assign,getExprConstBool("false"),NULL,$$,0,yylineno);
				emit(jump,NULL,NULL,NULL,nextquadlabel()+2,yylineno);
				emit(assign,getExprConstBool("true"),NULL,$$,0,yylineno);
				if(printRules)  printf(" expr: expr SMALLEREQUAL expr\n"); }
				
	       | expr EQUAL expr
			{
				$$ = expr_op(booleanexpr_e); 
				$$->sym = newTemp(); 
				emit(if_eq, $1, $3, NULL, nextquadlabel()+3, yylineno); 
				emit(assign,getExprConstBool("false"),NULL,$$,0,yylineno);
				emit(jump,NULL,NULL,NULL,nextquadlabel()+2,yylineno);
				emit(assign,getExprConstBool("true"),NULL,$$,0,yylineno);
				if(printRules)  printf(" expr: expr EQUAL expr\n"); }
				
	       | expr DIFFERENT expr 
			{ 
				$$ = expr_op(booleanexpr_e); 
				$$->sym = newTemp(); 
				emit(if_noteq, $1, $3, NULL, nextquadlabel()+3, yylineno); 
				emit(assign,getExprConstBool("false"),NULL,$$,0,yylineno);
				emit(jump,NULL,NULL,NULL,nextquadlabel()+2,yylineno);
				emit(assign,getExprConstBool("true"),NULL,$$,0,yylineno);
				if(printRules)  printf(" expr: expr DIFFERENT expr\n"); }
				
	       | expr AND expr
			{ 
				$$ = expr_op(booleanexpr_e); 
				$$->sym = newTemp(); 
				emit(and, $1, $3, $$, 0, yylineno); 
				if(printRules)  printf(" expr: expr AND expr \n"); }
				
	       | expr OR expr 
			{
				$$ = expr_op(booleanexpr_e); 
				$$->sym = newTemp(); 
				emit(or, $1, $3, $$, 0, yylineno); 
				if(printRules)  printf(" expr: expr OR expr\n"); }
				
		| term {
				
				$$ = $1; /**/if(printRules)  printf(" expr: term\n");
			}
	       ;

		  
		  
term:		LEFT_PARENTHESIS expr RIGHT_PARENTHESIS { $$ = $2; if(printRules)  printf(" term: LEFT_PARENTHESIS expr RIGHT_PARENTHESIS\n"); }
		 | SUB expr %prec UMINUS 
			{ 
				$$ = expr_op(arithmexpr_e); 
				$$->sym = newTemp(); 
				emit(uminus, $2, NULL, $$, 0, yylineno); 
				if(printRules)  printf(" term: SUB expr UMINUS\n"); }
				
		 | NOT expr 
			{
				$$ = expr_op(booleanexpr_e); 
				$$->sym = newTemp(); 
				emit(not, $2, NULL, $$, 0, yylineno); 
				if(printRules)  printf(" term: NOT expr\n"); }
				
		 | INCREASE lvalue 
			{
				
				if( $2->type == tableitem_e)
				{
					$$ = emit_iftableitem($2);
					
					emit(add, $$, getExprConstInt(1), $$, 0, yylineno);
					emit(tablesetelem, $2, $2->index, $$, 0, yylineno);
					
				}
				else
				{
					emit(add, $2, getExprConstInt(1), $2, 0, yylineno);
					$$ = expr_op(arithmexpr_e);
					$$->sym = newTemp();
					emit(assign, $$, NULL, $2, 0, yylineno);
				}
				
				if(printRules)  printf(" term: INCREASE lvalue\n"); }
				
		 | lvalue INCREASE 
			{
				$$ = expr_op(var_e);
				$$->sym = newTemp(); 
				
				if( $1->type == tableitem_e )
				{
					expr1 *e = emit_iftableitem($1);
					emit(assign, e, NULL, $$, 0, yylineno);
					emit(add, e, getExprConstInt(1), e, 0, yylineno);
					emit(tablesetelem, $1, $1->index, e, 0, yylineno);
				}
				else
				{
					emit(assign, $1, NULL, $$, 0, yylineno);
					emit(add, $1, getExprConstInt(1), $1, 0, yylineno);
				}
				
				if(printRules)  printf(" term: lvalue INCREASE\n"); }
				
		 | DECREASE lvalue 
			{ 
				if( $2->type == tableitem_e)
				{
					$$ = emit_iftableitem($2);
					
					emit(sub, $$, getExprConstInt(1), $$, 0, yylineno);
					emit(tablesetelem, $2, $2->index, $$, 0, yylineno);
					
				}
				else
				{
					emit(sub, $2, getExprConstInt(1), $2, 0, yylineno);
					$$ = expr_op(arithmexpr_e);
					$$->sym = newTemp();
					emit(assign, $2, NULL, $$, 0, yylineno);
				}
				if(printRules)  printf(" term: DECREASE lvalue\n"); }
				
		 | lvalue DECREASE
			{ 
				$$ = expr_op(var_e);
				$$->sym = newTemp(); 
				
				if( $1->type == tableitem_e )
				{
					expr1 *e = emit_iftableitem($1);
					emit(assign, e, NULL, $$, 0, yylineno);
					emit(sub, e, getExprConstInt(1), e, 0, yylineno);
					emit(tablesetelem, $1, $1->index, e, 0, yylineno);
				}
				else
				{
					emit(assign, $1, NULL, $$, 0, yylineno);
					emit(sub, $1, getExprConstInt(1), $1, 0, yylineno);
				}
				if(printRules)  printf(" term: lvalue DECREASE\n"); }
				
		| primary 
					{
						
						$$ = $1;
						if(printRules)  printf(" term: primary\n");
					}
		 ;
		 
		 
assignexpr:	lvalue
			{
				if ( $1->type == programfunc_e || $1->type == libraryfunc_e )
					yyerror("Function as left-value");
					

			} ASSIGN expr { 
				
				if($1->type == tableitem_e)
				{
					emit(tablesetelem, $1, $1->index, $4, 0, yylineno);
					$$ = emit_iftableitem($1);
					$$->type = assignexpr_e;
				}
				else
				{
					emit(assign, $4, (expr1*)0, $1, 0, yylineno);
					$$ = expr_op(assignexpr_e);
					$$->sym = newTemp();
					emit(assign, $1, (expr1*)0, $$, 0, yylineno);
				}
				
				if(printRules)  printf(" assignexpr: lvalue ASSIGN expr\n"); }
		    ;
		    
		    
primary:		lvalue 
		      { 	 $$ = emit_iftableitem($1);
				
				if(printRules)  printf(" primary: lvalue\n");
				}
		    | call  
			{
				
				  $$ = $1;
				  if(printRules)  printf(" primary: call\n");
			}
		    | objectdef 
					{
						$$ = $1;
						if(printRules)  printf(" primary: objectdef\n");
					}
		    | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS 
					{
						$$ = expr_op(programfunc_e);
						$$->sym = $2;
							
						if(printRules)  printf(" primary: LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS\n");
					}
		    | const { if(printRules)  printf(" primary: const\n"); }
		    ;
		    
		    
		    
lvalue:	  ID { 				
				if(printRules)  printf(" lvalue: ID\n");
				/*	if(printRules)  printf("--------------------------- infunction = %d, inblock=%d  --------------- \n\n",inFunction,blockFun);
				if(printRules)  printf("---------------------- lookUp_variable_local = %d --------------------- \n\n",lookUp_variable_local($1));
*/				if( lookUp_name_global( $1 ) == 0 ) {
					
					insert_to_table( newEntry((Scope==0)?global:local, 1, $1, Scope, yylineno, currscopeoffset(), currscopespace(), var_s));	
					
					inccurrscopeoffset();
				}
				else if(lookUp_variable_local($1)==0)
				{ 
					int tmpScope = Scope;
					Scope = Scope-blockFun;
/*					if(printRules)  printf("------------------------------ Scope - blockFun = %d \n",Scope);
if(printRules)  printf("------------------- lookUp_name_global = %d ---------------\n",lookUp_name_global( $1 ));
*/					  
				    if( lookUp_name_global( $1 ) == 1 && inFunction !=0 ) 
				    {
					    SymbolTableEntry * entry; 
					    while(Scope>0)
					    {
								entry=search_variable_local($1);
								if(entry){
								if(entry->isVariable)
									yyerror("ID not visible ");
								
								break;
								}
								Scope--;
					     }
					}
					
					Scope = tmpScope;
				} 
				int tmpScope = Scope;
				tmpEntry = NULL;
				while(Scope>=0){
					
					tmpEntry = search_variable_local($1);
					if(tmpEntry!=NULL)
						break;
					Scope--;
				}
				Scope = tmpScope;
				
				assert(tmpEntry);
				
				$$ = lvalue_expr(tmpEntry);

	}
				
	       | LOCAL ID { 
					if(printRules)  printf(" lvalue: LOCAL ID\n");
					if( lookUp_variable_local( $2 ) == 0 && look_up_only_LibFuncs($2)==0)
					{
						insert_to_table( newEntry(local, 1, $2, Scope, yylineno, currscopeoffset(), currscopespace(), var_s ) ); 
						inccurrscopeoffset();
					}
					else{
						yyerror("Redeclaration of ID.");
					}
						
						int tmpScope = Scope;
  
						tmpEntry = NULL;
						while(Scope>=0){
							
							tmpEntry = search_variable_local($2);
							if(tmpEntry!=NULL){    
								break;
							}
							Scope--;
						}
						Scope = tmpScope;
						assert(tmpEntry);
						$$ = lvalue_expr(tmpEntry);
						
			
		}
					
	       | DOUBLE_UPDOWN_DOT ID { 			int tmpScope = Scope;
								Scope = 0;
								tmpEntry = search_variable_local($2);
								if(printRules)  printf(" lvalue: DOUBLE_UPDOWN_DOT ID\n");
								if( look_up_only_Scope0( $2 ) == 0 )
								  { yyerror("Global ID not found");} 
								if(tmpEntry!=NULL && tmpEntry->isVariable ==0){
								     yyerror("Global Indetifier before function name");
								  }
								
								
								//assert(tmpEntry);
								Scope = tmpScope;
								
								$$ = lvalue_expr(tmpEntry);}
								
	       | member { 
			  if(printRules)  printf(" lvalue: member\n");
			  	}
	       ;
	       
	       
member:		lvalue DOT ID 
					{ 
						$$ = member_item($1, $3);	
						if(printRules)  printf(" member: DOT ID\n");
						/*	if( lookUp_function_global( $3 ) == 0 )
								yyerror("Function Not Found.");
								*/} 
							
		   | lvalue LEFT_BRACKET expr RIGHT_BRACKET 
				{
					$1 = emit_iftableitem($1);
					$$ = expr_op(tableitem_e);
					$$->sym = $1->sym;
					$$->index = $3;
					
					if(printRules)  printf(" member:lvalue LEFT_BRACKET expr RIGHT_BRACKET\n"); }
		   
		   | call DOT ID { $$=member_item($1,$3);
						if(printRules)  printf(" member: call DOT ID\n");
					/*	if( lookUp_variable_local( $3 ) == 0 )
							insert_to_table( newEntry(local, 1, $3, Scope, yylineno) );	*/ }
						
		   | call LEFT_BRACKET expr RIGHT_BRACKET 
				{
					
					$1 = emit_iftableitem($1);
					$$ = expr_op(tableitem_e);
					$$->sym = $1->sym;
					$$->index = $3;
					if(printRules)  printf(" member: call LEFT_BRACKET expr RIGHT_BRACKET\n");
				}
		   ;
		   
		   
call:		call normcall
						{
							$$ = make_call($1, $2->next);
							if(printRules)  printf(" call: call normcall\n");
						} 
		   | lvalue callsuffix  
						{
							if ($2->method == 1) 
							{
								expr1* self = $1;
								$1 = emit_iftableitem(member_item(self, $2->name));
								self->next = $2->next;
								$2->next = self;
							}
							
							$$ = make_call($1, $2->next);
							
							if(printRules)  printf(" call: lvalue callsuffix\n");
						}
		   | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS normcall 
							{
								expr1 *func = expr_op(programfunc_e);
								func->sym = $2;
								$$ = make_call(func, $4->next);
								
								if(printRules)  printf(" call: LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS normcall\n");
							}
		   ;
		   
		   
		   
callsuffix:	  normcall 
					{
						
						$$ = $1;
						
						if(printRules)  printf(" callsuffix: normcall\n");
					} 
		 | methodcall 
					{
						$$ = $1;
						if(printRules)  printf(" call: methodcall\n");
					} 
		 ;
		   
		   
normcall:	LEFT_PARENTHESIS
		{
					insert_to_elistStack(eListTail);
					insert_to_elistStack(eListHead);
					eListTail=NULL;
					eListHead=NULL;
		} elist RIGHT_PARENTHESIS
			{

				$$ = malloc(sizeof(expr1));

				//$$->next = $2;
				$$->next=eListTail;
				$$->method = 0;
				$$->name = NULL;

				if(elistStack!=NULL){
					eListHead = elistStack;
					elistStack=elistStack->next1;
				}
				else
					eListHead=NULL;

				
				if(elistStack!=NULL){
					eListTail= elistStack;
					elistStack=elistStack->next1;
				}
				else
					eListTail = NULL;

				
				if(printRules)  printf(" normcall: LEFT_PARENTHESIS elist RIGHT_PARENTHESIS\n");
			} 
		 ;
			 
			 
			 
methodcall:	DOUBLE_DOT ID normcall 
				{

					//$$ = malloc(sizeof(expr1));
					$$=$3;
					//$$->next = $3;
					$$->method = 1;
					$$->name = strdup($2);
					//$$->sym=$3->sym;
					if(printRules)  printf(" methodcall: DOUBLE_DOT ID normcall\n");
				} 
			 ;
			 

elist:   expr 
{  
			insert_to_elist( $1 );
			
			$$ = eListHead;
			
			if(printRules)  printf(" elist: expr\n");
		}
		
	  | expr elist1 
		{
			$1->next = eListHead;
			eListHead->previous=$1;
			$1->previous = NULL;
			eListHead = $1;
			//insert_to_elist($1);
			
			$$ = eListHead;
			
			if(printRules)  printf(" elist: expr elist1\n");
		}
		
	  |	{ 
		  $$ = NULL;
		  if(printRules)  printf(" elist: *empty*\n");
		} 
	  ;

elist1:  COMMA expr 
			{
				insert_to_elist( $2 );
				$$ = eListHead;
		
				if(printRules)  printf(" elist1: COMMA expr\n");
			}
			
		|elist1 COMMA expr 
			{ 
				insert_to_elist($3);
				$$ = eListHead;
	
				if(printRules)  printf(" elist1: elist1 COMMA expr\n");
			}
		;
		    
		    
objectdef:	LEFT_BRACKET
		{
			insert_to_elistStack(eListTail);
			insert_to_elistStack(eListHead);
			eListTail=NULL;
			eListHead=NULL;
		}
		elist RIGHT_BRACKET
		{
			expr1 *t = expr_op(newtable_e);
			t->sym = newTemp();
			emit(tablecreate, t, NULL, NULL, 0, yylineno);
			double i=0;
			
			eListHead = $3;
			
			expr1 *tmpHead = eListHead;
			while(tmpHead!=NULL)
			{
				emit(tablesetelem, t, getExprConstReal(i++), tmpHead, 0, yylineno);
				tmpHead = tmpHead->next;
			}
			$$ = t;
			
			
			
			if(elistStack!=NULL){
				eListHead = elistStack;
				elistStack=elistStack->next1;
			}
			else
				eListHead=NULL;
			
			
			if(elistStack!=NULL){
				eListTail= elistStack;
				elistStack=elistStack->next1;
			}
			else
				eListTail = NULL;
			
			
			$$->next=eListTail;
			
			if(printRules)  printf(" objectdef: LEFT_BRACKET elist RIGHT_BRACKET\n");
		}
		
		| LEFT_BRACKET {
			insert_to_elistStack(eListTail);
			insert_to_elistStack(eListHead);
			eListTail=NULL;
			eListHead=NULL;
		}indexed RIGHT_BRACKET
		{
			
			expr1* t = expr_op(newtable_e);
			t->sym = newTemp();
			emit(tablecreate, t,NULL, NULL, 0, yylineno);
			
			expr1 *tmp1 = $3, *tmp2 = $3->next;
			
			while( tmp1!=NULL && tmp2!=NULL )
			{
				emit(tablesetelem, t, tmp1, tmp2, 0, yylineno);
				
				tmp1 = tmp2->next;
				if(tmp1==NULL)
				{
					break;
				}
				tmp2 = tmp1->next;
			}
			$$ = t;
			if(elistStack!=NULL){
				eListHead = elistStack;
				elistStack=elistStack->next1;
			}
			else
				eListHead=NULL;
			
			
			if(elistStack!=NULL){
				eListTail= elistStack;
				elistStack=elistStack->next1;
			}
			else
				eListTail = NULL;
			
			
			$$->next=eListTail;
			
			if(printRules)  printf(" objectdef: LEFT_BRACKET indexed RIGHT_BRACKET\n");
		}
		;
		
		 
		 
indexedelem:	LEFT_CURLBRACKET expr UPDOWN_DOT expr RIGHT_CURLBRACKET 
			{
				
				insert_to_elist($2);
				insert_to_elist($4);
				
				$$ = eListHead;
				
				if(printRules)  printf(" indexedelem LEFT_CURLBRACKET  expr UPDOWN_DOT expr RIGHT_CURLBRACKET\n");
			}
			 ;		 

			

indexed:	indexedelem COMMA indexed
			{
				
				$$ = $3;
				if(printRules)  printf(" indexed: indexedelem COMMA indexed\n");
			}
			
	       | indexedelem 
			{	$$ = $1;
				if(printRules)  printf(" indexed: indexedelem\n");
			}
	       ;


block:		LEFT_CURLBRACKET 
				{ 	
					Scope++; 
					inBlock++; 
					blockFun++; 
				} 
			statements RIGHT_CURLBRACKET 
				{ 
					$$=$3;
					inBlock--; 
					blockFun--; 
					make_scope_inactive( Scope ); 
					Scope--; 
					if(printRules)  printf(" block: LEFT_CURLBRACKET statements RIGHT_CURLBRACKET\n");
					
				}
		  | LEFT_CURLBRACKET 
				{ 	
					Scope++; 
					inBlock++; 
					blockFun++; 
				} 
			RIGHT_CURLBRACKET 
				{
					$$=malloc(sizeof(struct stmt));
					$$->breakList=NULL;
					$$->contList=NULL;
					inBlock--; 
					blockFun--; 
					make_scope_inactive( Scope ); 
					Scope--; 
					if(printRules)  printf(" indexed: LEFT_CURLBRACKET RIGHT_CURLBRACKET\n");
				}
			 ;
			 
			 
funcdef:  FUNCTION ID {	
					if(lookUp_variable_local($2)==1)
					{	    yyerror("Redeclaration of Function's Name in the same scope");	}
					else if(look_up_only_LibFuncs($2)==1)
					{	    yyerror("Cannot shadow Library Functions");	}
					else
					{	
						$<symb>$= newEntry(userfunc, 0, $2, Scope, yylineno, currscopeoffset(), currscopespace(), programfunc_s);
						insert_to_table( $<symb>$ ); 
						$<symb>$->iaddress = nextquadlabel();
						emit(funcstart, lvalue_expr($<symb>$), NULL, NULL, 0 ,yylineno);
					
						insert_to_list( &(nestedFunctions->head), $<symb>$, 2 );

						inccurrscopeoffset();
					}
					push(stack, currscopeoffset());
					push(stack, inLoop);
					inLoop=0;
/*
					if( lookUp_name_global( $2 ) == 0 )
						insert_to_table( newEntry(userfunc, 0, $2, Scope, yylineno) );
					else
						yyerror("Redeclaration of Function's Name.");
*/				} 

		LEFT_PARENTHESIS 
		{	 enterscopespace();
			Scope++; 
			restorecurrscopeoffset(1);
		} 
		
		idlist RIGHT_PARENTHESIS
				{  Scope--; 
					enterscopespace(); 
					functionLocalOffset=1; 
					inFunction++; 
					inBlock--; 
					blockFun=0;
				}
				
		block 
			{ 	
				
				inFunction--;
				
				$<symb>$ = nestedFunctions->head;
				if(nestedFunctions->head!=NULL)
					nestedFunctions->head = nestedFunctions->head->next1;
				
				$<symb>$->totalLocals = currscopeoffset()-1; 
				exitscopespace();
				exitscopespace(); 
				inLoop = pop(stack);
				restorecurrscopeoffset(pop(stack)); 
				emit(funcend, lvalue_expr($<symb>$), NULL, NULL, 0, yylineno);
				inBlock++; 
				blockFun=0; 
				if(printRules)  printf(" funcdef: FUNCTION ID LEFT_PARENTHESIS idlist RIGHT_PARENTHESIS block\n");
			}
		
		| FUNCTION 
			{ 
			
				$<symb>$ =  newEntry(userfunc, 0, getRandomFuncName(), Scope, yylineno, currscopeoffset(), currscopespace(), programfunc_s);
				insert_to_table( $<symb>$ ); 
				$<symb>$->iaddress = nextquadlabel();
				emit(funcstart, lvalue_expr($<symb>$), NULL, NULL, 0 ,yylineno);
				//tmpEntry2 = $<symb>$;

				insert_to_list( &(nestedFunctions->head), $<symb>$, 2 );
				
				inccurrscopeoffset();
				push(stack, currscopeoffset()); 
				push(stack, inLoop);
				inLoop=0;
				enterscopespace(); 
				formalArgOffset = 1; 
			}
			
			LEFT_PARENTHESIS 
			{ Scope++; } 
			idlist RIGHT_PARENTHESIS 
				{  
					Scope--; 
					enterscopespace(); 
					functionLocalOffset=1; 
					inFunction++; inBlock--; blockFun=0;
				} 
		block {
			
				inFunction--;

				
				//$<symb>$ = tmpEntry2;

				$<symb>$ = nestedFunctions->head;
				
				if(nestedFunctions->head!=NULL)
					nestedFunctions->head = nestedFunctions->head->next1;

				
				$<symb>$->totalLocals = currscopeoffset()-1;
				exitscopespace(); 
				exitscopespace(); 
				inLoop = pop(stack);
				restorecurrscopeoffset(pop(stack));  
				emit(funcend, lvalue_expr($<symb>$), NULL, NULL, 0, yylineno);
				inBlock++; 
				blockFun=0;
				if(printRules)  printf(" funcdef: FUNCTION LEFT_PARENTHESIS idlist RIGHT_PARENTHESIS block\n");
			}
			 ;
			 
const:	INTEGER { $$ = getExprConstInt($1); if(printRules)  printf(" const: INTEGER\n"); }
	 | REAL { $$ = getExprConstReal($1); if(printRules)  printf(" const: REAL\n"); }
	 | STRING { $$ = getExprConstString($1); if(printRules)  printf(" const: STRING\n"); }
	 | NIL { $$ = getExprConstNill(); if(printRules)  printf(" const: NIL\n"); }
	 | TRUE { $$ = getExprConstBool("true"); if(printRules)  printf(" const: TRUE\n"); }
	 | FALSE { $$ = getExprConstBool("false"); if(printRules)  printf(" const: FALSE\n"); }
	 ;
			 
	 
	 
idlist:   ID {   
				if(printRules)  printf(" idlist: ID\n");
				if(look_up_only_LibFuncs($1)==0 && lookUp_variable_local($1)==0)
				{
					tmpEntry = NULL;
					tmpEntry =  newEntry(formal, 1, $1, Scope, yylineno, currscopeoffset(), currscopespace(), var_s);
					
					//expr1* lvalue_expr(SymbolTableEntry *sym);
					
				
					$$ = lvalue_expr(tmpEntry);
					
					insert_to_table( tmpEntry );
					inccurrscopeoffset();
				}
				else
					yyerror("Redeclaration of Library Function or Formal Argument.");
			}
			| ID 
				{
					if(look_up_only_LibFuncs($1)==0 && lookUp_variable_local($1)==0)
					{
						tmpEntry =  newEntry(formal, 1, $1, Scope, yylineno, currscopeoffset(), currscopespace(), var_s) ;
						
						insert_to_table( tmpEntry );
						inccurrscopeoffset();	
					}
					else
						yyerror("Redeclaration of Library Function or Formal Argument.");
				} idlist1 
				{
					tmpEntry = search_variable_local($1);
					if( tmpEntry != NULL)
					{
						struct expr *head = lvalue_expr(tmpEntry);
						head->next = $3;
						$3 = head;
						$$ = $3;
					}
					
					if(printRules)  printf(" idlist: ID idlist1\n");
				} 
			| { if(printRules)  printf(" idlist: *empty*\n"); }
		;
			 
idlist1:  COMMA ID {
				if(printRules)  printf(" idlist1: COMMA ID\n");
				if(look_up_only_LibFuncs($2)==0 && lookUp_variable_local($2)==0)
				{	
					tmpEntry = newEntry(formal, 1, $2, Scope, yylineno, currscopeoffset(), currscopespace(), var_s);					
					insert_to_table( tmpEntry );
					inccurrscopeoffset();	
					
					struct expr *head = lvalue_expr(tmpEntry);
					head->next = $$;
					$$=head;
					
				}
				else
					yyerror("Redeclaration of Library Function or Formal Argument."); 
				} 
					
	| idlist1 COMMA ID { 
					if(printRules)  printf(" idlist1: idlist1 COMMA ID\n");
					if(look_up_only_LibFuncs($3)==0 && lookUp_variable_local($3)==0)
					{
						tmpEntry = newEntry(formal, 1, $3, Scope, yylineno, currscopeoffset(), currscopespace(), var_s);
						insert_to_table( tmpEntry );
						inccurrscopeoffset();	
						
						struct expr *head = lvalue_expr(tmpEntry);
						head->next = $1;
						$1 = head;
						$$ = $1;
					}
					else
						yyerror("Redeclaration of Library Function or Formal Argument.");
				};
								 
	 
		    

ifstmt:		ifprefix stmt 
		  {
		      patchlabel($1,nextquadlabel());
			 $$ = malloc(sizeof(struct stmt));
			 
		   }
		
	 | ifprefix stmt elseprefix stmt
		   {
		      patchlabel($1,$3+1);
		      patchlabel($3,nextquadlabel());
		      if(printRules)  printf(" ifstmt: IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt ELSE stmt\n");
			 $$ = malloc(sizeof(struct stmt));
			 
			 $$->breakList = merge($2->breakList, $4->breakList);
			 $$->contList = merge($2->contList, $4->contList);
			 
			    }
	       ;
		  

ifprefix:	 IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS
		  {
		      emit(if_eq,getExprConstBool("true"),$3,NULL,nextquadlabel()+2,yylineno);
		      $$=nextquadlabel();
		      emit(jump,NULL,NULL,NULL,0,yylineno);
		      if(printRules)  printf("ifstmt: IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt\n");
		   };

elseprefix:	ELSE 
		  {
		      $$= nextquadlabel();
		      emit(jump,NULL,NULL,NULL,0,yylineno);
		  };




whilestmt:	whilestart whilecond stmt
				{
					emit(jump, NULL, NULL, NULL, $1, yylineno);
					patchlabel($2, nextquadlabel());
				//	assert($3->breakList);
					if($3->breakList!=NULL)
					{ 
						stackNode *tmp = $3->breakList->tail;
						
						while(tmp!=NULL)
						{
							patchlabel(tmp->number, nextquadlabel());
							tmp = tmp->previous;
						}
					}
						
					if($3->contList!=NULL)
					{
						stackNode *tmp = $3->contList->tail;
						
						while(tmp!=NULL)
						{
							patchlabel(tmp->number, $1);
							tmp = tmp->previous;
						}
					}
					
					inLoop--;
					$3->breakList=NULL;
					$3->contList=NULL;
					$$ = $3;
					if(printRules)  printf(" whilestmt: whilestart whilecond stmt\n");
					
				}
				;
				
whilecond:	LEFT_PARENTHESIS expr RIGHT_PARENTHESIS
				{
					inLoop++;
					emit(if_eq, getExprConstBool("true"),  $2, NULL,nextquadlabel()+2, yylineno);
					$$ = nextquadlabel();
					emit(jump, NULL, NULL, NULL, 0, yylineno);
					if(printRules)  printf(" whilecond: LEFT_PARENTHESIS expr RIGHT_PARENTHESIS\n");
					
				}
				;

whilestart:	WHILE 
				{
					$$ = nextquadlabel();
					if(printRules)  printf(" whilestart: WHILE\n");
				}
				;
	
N:		{	$$ = nextquadlabel();	emit(jump, NULL, NULL, NULL, 0, yylineno);	}
		;
		
M:		{	$$ = nextquadlabel();	}
		;
		

forprefix:	FOR LEFT_PARENTHESIS elist QUESTIONMARK M expr QUESTIONMARK
					{	
						$$ = malloc(sizeof(struct stmt));
						$$->test = $5;
						$$->enter = nextquadlabel();
						
						emit(if_eq, getExprConstBool("true"),$6 , NULL, 0, yylineno);	
					}
				;


forstmt:	forprefix N elist RIGHT_PARENTHESIS{inLoop++;} N stmt N 
			{	
				inLoop--;
				patchlabel($1->enter, $6+1);
				patchlabel($2, nextquadlabel());
				patchlabel($6, $1->test);
				patchlabel($8, $2+1);
				
				
				stackNode *tmp = NULL;
				if($7->breakList!=NULL)
					tmp = $7->breakList->tail;
				
				while(tmp!=NULL)
				{
					patchlabel(tmp->number, nextquadlabel());
					tmp = tmp->previous;
				}
				
				if($7->contList!=NULL)
					tmp = $7->contList->tail;
				else
					tmp = NULL;
				
				while(tmp!=NULL)
				{
					patchlabel(tmp->number, $2+1);
					tmp = tmp->previous;
				}
				$7->breakList=NULL;
				$7->contList=NULL;
				if(printRules)  printf(" forstmt: FOR LEFT_PARENTHESIS elist QUESTIONMARK expr QUESTIONMARK elist RIGHT_PARENTHESIS stmt\n");
			}
		    ;
     
returnstmt:	RETURN expr QUESTIONMARK 
			  {
			    emit(ret,$2,NULL,NULL,0,yylineno);
			    if(printRules)  printf(" returnstmt: RETURN expr QUESTIONMARK\n");
			  }
		| RETURN QUESTIONMARK 
			  {
			      emit(ret,NULL,NULL,NULL,0,yylineno);
			    if(printRules)  printf(" returnstmt: RETURN QUESTIONMARK\n");
			  }
		    ;

		    
		    
		    
%%

int yyerror(char *yaccProvidedMessage)
{
	if(printRules)  fprintf(stderr, "\n\n Line %d: %s , before token: %s\n", yylineno, yaccProvidedMessage, yytext);
     if(printRules)  fprintf(stderr, " ERROR\n\n");
	//return 0;
}




void insert_to_elist( expr1 *e )
{
	if(eListHead == NULL)
	{
		
		e->next = NULL;
		e->previous=NULL;
		eListHead = e;
		eListTail = e;
	}
	else
	{
		e->next = NULL;
		eListTail->next = e;
		e->previous=eListTail;
		eListTail = e;
	}

}

void insert_to_elistStack(expr1 *e)
{	if(e==NULL)
		return;
	if( elistStack == NULL )
	{
		elistStack = e;
		elistStack->next1=NULL;
	}
	else
	{
		e->next1 = elistStack;
		elistStack = e;
	}
}



int matchParameter(char *param)
{
	if(strcmp(param, "-r")==0)
	{
		printRules = 1;
		return 1;
	}
	else if(strcmp(param, "-t")==0)
	{
		printTable = 1;
		return 1;
	}
	
	else if(strcmp(param, "-q")==0)
	{
		makeQuads = 1;
		return 1;
	}
	else if(strcmp(param, "-i")==0)
	{
		makeInstructions = 1;
		return 1;
	}

	else if(param[0] == '-')
	{
		fprintf(stderr, "\n Unrecognised parameter : %s\n", param);
		return 1;
	}
	else
		return 0;
	
}


int main(int argc,char * argv[])
{

	/*
	
	===== Parameters =====
	
	-r  :  print Grammar Rules
	-t  :  print Symbol Table
	-q  :  print Quads
	
	*/

	
	if(argc >= 2 )
	{
		
		char *inputFile = NULL;
		int i;
		for(i=1;i<argc;i++)
		{
			if(!matchParameter(argv[i]))
				inputFile = strdup(argv[i]);
		}
		if(inputFile==NULL)
		{
			fprintf(stderr, "\n No input files \n\n");
			exit(1);
		}
		if( (yyin=fopen(inputFile,"r")) == NULL)
		{
			fprintf(stderr,"\n Could not open the file \"%s\"\n\n", inputFile);
			return 1;
		}
	}
	else
	{
		fprintf(stderr, "\n No input files \n\n");
		exit(1);
	}
	
	if(printRules)
		puts("\n\n ============== Grammar Rules ==============\n");
	
	Scope = 0;
	counterFuncName = 0;
	counterTempVar = 0;

	ij_head = (incomplete_jump*) 0;
	ij_total = 0;
	
	programVarOffset = 1;
	functionLocalOffset = 1;
	formalArgOffset = 1;
	scopeSpaceCounter = 1;
	
	nestedFunctions = newSymbolList();
	
	quads = (quad *) 0;
	
	total = 0;
	total_i = 0;
	currQuad = 0;
	currInstruction = 0;

	
	totalNumConsts = 0;
	totalStringConsts = 0;
	totalNamedLibFuncs = 0;
	totalUserFuncs = 0;
	totalUserVars = 0;

	numConsts = NULL;
	stringConsts = NULL;
	namedLibFuncs = NULL;
	userfuncs = NULL;
	userVars = NULL;

	instructions = (instruction *)0;
	
	init_symbolTable();
	
	stack = newStack();

	funcstack = newStackFunc();
	
	yyparse();
	
	if(printTable)
		print_table();
	

	if(printRules)
		puts("\n");

	if(makeQuads)
	{
		printQuads();
		puts("\n Quads was generated in file \"quads.txt\" ");
	}
	
	generateICodes();
	patch_incomplete_jumps();
	generateInstructions();
	if(makeInstructions)
	{
		FILE *fp;
		fp = fopen("instructions.txt", "w+");
		printInstructions(fp);
		puts(" Instructions was generated in file \"instructions.txt\" \n");
	}
	
	
	return 0;
}




