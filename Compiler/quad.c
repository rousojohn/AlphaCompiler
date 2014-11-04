#include "header.h"


void expand(void)
{
	assert(total==currQuad);
	quad* p = (quad*) malloc(NEW_SIZE);
	if(quads)
	{
		memcpy(p, quads, CURR_SIZE);
		free(quads);
	}
	quads = p;
	total += EXPAND_SIZE;
}



void emit( 
			iopcode op, 
			expr1* arg1,
			expr1* arg2,
			expr1* result,
			 int label,
			 int line
		)
{
	if(currQuad == total)
		expand();
	
	quad* p = quads+currQuad++;
	p->op = op;
	p->arg1 = arg1;
	p->arg2 = arg2;
	p->result = result;
	p->label = label;
	p->line = line;
}



enum scopespace_t currscopespace(void)
{
	if(scopeSpaceCounter==1)
		return programVar;
	else	if(scopeSpaceCounter%2 == 0)
		return formalArg;
	else
		return functionLocal;

}




unsigned int currscopeoffset(void)
{
	switch( currscopespace() )
	{
		case programVar : return programVarOffset;
		case functionLocal : return functionLocalOffset;
		case formalArg : return formalArgOffset;
		default: assert(0);
	}
}



void inccurrscopeoffset(void)
{
	switch(currscopespace())
	{
		case programVar : ++programVarOffset; break;
		case functionLocal : ++functionLocalOffset; break;
		case formalArg : ++formalArgOffset; break;
		default: assert(0);
	}
}

void enterscopespace(void)
{
	++scopeSpaceCounter;	
}


void exitscopespace(void)
{
	assert(scopeSpaceCounter>1);
	--scopeSpaceCounter;
}




void restorecurrscopeoffset(int number)
{
	switch( currscopespace() )
	{
		case programVar : programVarOffset=number; break;
		case functionLocal : functionLocalOffset=number; break;
		case formalArg : formalArgOffset = number; break;
		default: assert(0);
	}
}


unsigned int nextquadlabel(void)
{
	return currQuad; 
}


void patchlabel( unsigned int quadNo, unsigned int label )
{
	assert(quadNo < currQuad);
	quads[quadNo].label = label;
}




expr1* lvalue_expr(SymbolTableEntry *sym)
{
	assert(sym);

	expr1 *e = malloc(sizeof(expr1));
	memset(e, 0, sizeof(expr1));
	
	e->next = NULL;
	e->sym = sym;
	
	switch( sym->type_t )
	{
		case var_s:	e->type = var_e; break;
		case programfunc_s:	e->type = programfunc_e; break;
		case libraryfunc_s: e->type = libraryfunc_e; break;
		
		default:  assert(0);
	}
	
	return e;
}


expr1* asignexpr_expr()
{
	expr1 *e = malloc(sizeof(expr1));
	memset(e, 0, sizeof(expr1));
	
	e->next = NULL;
	
	e->type = assignexpr_e;
	
	return e;
	
}


SymbolTableEntry* newTemp()
{
	SymbolTableEntry *n;
	n = newEntry((Scope==0)?global:local, 1, getRandomTempVar(), Scope, yylineno, currscopeoffset(), currscopespace(), var_s);
	
	insert_to_table(n);	
	
	return n;
}



expr1 *expr_op(expr_t type)
{
	expr1 *e = malloc(sizeof(expr1));
	memset(e, 0, sizeof(expr1));
	e->next = NULL;
	e->type = type;
	return e;
}




expr1 *getExprConstInt(int n)
{
	expr1 *e = malloc(sizeof(expr1));
	memset(e, 0, sizeof(expr1));
	e->next = NULL;
	e->type = constnum_e;
	e->numConst = n; // # WARNING type-casting?
	return e;
}



expr1 *member_item(expr1 *lvalue, char *name)
{
	lvalue = emit_iftableitem(lvalue);
	expr1 *item = expr_op(tableitem_e);
	item->sym = lvalue->sym;
	item->index = getExprConstString(name);
	return item;
}


expr1 *getExprConstString(char *s)
{
	expr1 *e = expr_op(constchar_e);
	e->strConst = strdup(s);
	return e;
}


expr1 *getExprConstNill()
{
	expr1 *e = malloc(sizeof(expr1));
	memset(e, 0, sizeof(expr1));
	e->next = NULL;
	e->type = nil_e;
	e->strConst = NULL;
	return e;
}


expr1 *emit_iftableitem(expr1 *e)
{
	if(e->type != tableitem_e)
		return e;
	else
	{
		expr1 *result = expr_op(var_e);
		result->sym = newTemp();
		
		emit( tablegetelem , e, e->index, result, 0, yylineno);
		return result;
	}
}

expr1 *getExprConstReal(double i)
{
	expr1 *e = malloc(sizeof(expr1));
	memset(e, 0, sizeof(expr1));
	e->next = NULL;
	e->type = constnum_e;
	e->numConst = i; 
	return e;
}

expr1 *getExprConstBool(char * boolean)
{
	expr1 *e = malloc(sizeof(expr1));
	memset(e, 0, sizeof(expr1));
	e->next = NULL;
	e->type = constbool_e;
	if(strcmp(boolean, "true")==0)
	{
		e->boolConst = 1;
	}
	else
	{
		e->boolConst = 0;
	}
	return e;
}



expr1* make_call(expr1* lvalue, expr1* elist)
{
	expr1* func = emit_iftableitem(lvalue);
	expr1* tmphead = elist;
	
	int length = 0;

	while(tmphead!=NULL)
	{
		emit(param, tmphead, NULL, NULL, 0, yylineno);	
		tmphead = tmphead->previous;
	}	
	emit(call, func, NULL, NULL, 0, yylineno);
	expr1 *result = expr_op(var_e);
	
	result->sym = newTemp();
	emit(getretval,result, NULL, NULL, 0, yylineno);
	
	return result;
}



void checkminus(expr1 *e)
{
	if(e->type == constbool_e ||
		e->type == constchar_e ||
		e->type == nil_e ||
		e->type == newtable_e ||
		e->type == programfunc_e ||
		e->type == libraryfunc_e ||
		e->type == booleanexpr_e 
		)
	{
		fprintf(stderr, "Illegel expr to unary -\n\n");
		
	}
		
}





void backpatch(Stack s, int label)
{	if(s == NULL)
		return;
	stackNode *tmpHead = s->head;
	while(tmpHead!=NULL)
	{
		quads[ tmpHead->number ].label = label;
		tmpHead = tmpHead->next;
	}
}




char *getOpAsString(iopcode op)
{
	switch(op)
	{
		case assign: 	return "ASSIGN";
		case add:		return "ADD";
		case sub:		return "SUB";
		case mul:		return "MUL";
		case Div:		return "DIV";
		case or:		return "OR";
		case and:		return "AND";
		case mod:		return "MOD";
		case uminus:		return "UMINUS";
		case not:		return "NOT";
		case if_eq:		return "IF_EQ";
		case if_noteq:		return "IF_NOTEQ";
		case if_lesseq:		return "IF_LESSEQ";
		case if_greateq:	return "IF_GREATEQ";
		case if_less:		return "IF_LESS";
		case if_greater:	return "IF_GREATER";
		case call:		return "CALL";
		case param:		return "PARAM";
		case ret:		return "RET";
		case getretval:		return "GETRETVAL";
		case funcstart:		return "FUNCSTART";
		case funcend:		return "FUNCEND";
		case tablecreate:	return "TABLECREATE";
		case tablegetelem:	return "TABLEGETELEM";
		case tablesetelem:	return "TABLESETELEM";
		case jump:		return "JUMP";
		default: 		return "";
	}
}




void printQuads(void){
	int counter = 0;
	
	quad *p ;
	FILE *fp;
	fp = fopen("quads.txt","w+");
	
	while (counter<currQuad){
		
		p=quads+counter;
		fprintf(fp,"%d \t",counter);
		fprintf(fp,"%s \t",getOpAsString(p->op));
		switch(p->op){
			case assign:
				switch(p->arg1->type){
					case var_e:
					case tableitem_e:
					case programfunc_e:
					case libraryfunc_e:
					case arithmexpr_e:
					case assignexpr_e:
					case newtable_e:
						fprintf(fp,"%s \t",p->arg1->sym->value.varVal->name); break;
					case booleanexpr_e:
						fprintf(fp,"%s \t", p->arg1->sym->value.varVal->name);
// 						if(p->arg1->boolConst ==1){
// 							fprintf(fp,"TRUE\t");
// 						}
// 						else{
// 							fprintf(fp,"FALSE \t");
// 						}
						break;
					case constnum_e:
						fprintf(fp,"%lf \t", p->arg1->numConst);
						break;
					case constbool_e:
						if(p->arg1->boolConst ==1){
							fprintf(fp,"TRUE\t");
						}
						else{
							fprintf(fp,"FALSE \t");
						}
						break;
					case constchar_e:
						fprintf(fp,"%s \t", p->arg1->strConst);
						break;
					case nil_e:
						fprintf(fp,"NULL");
						break;
					default: assert(0);
				}
				switch(p->result->type){
					case var_e:
					case tableitem_e:
					case programfunc_e:
					case libraryfunc_e:
					case arithmexpr_e:
					case assignexpr_e:
					case newtable_e:
						fprintf(fp,"%s \t",p->result->sym->value.varVal->name);
						break;
					case booleanexpr_e:
						//if(p->result->boolConst ==1){
							
							//	fprintf(fp," TRUE\t");
							//}
							//else{
								fprintf(fp," %s \t", p->result->sym->value.varVal->name);
								// break;
								//fprintf(fp," FALSE \t");
								//}
								break;
					case constnum_e:
						fprintf(fp," %lf \t", p->result->numConst);
						break;
					case constbool_e:
						if(p->result->boolConst ==1){
							fprintf(fp," TRUE\t");
						}
						else{
							fprintf(fp," FALSE \t");
						}
						break;
					case constchar_e:
						fprintf(fp," %s \t", p->result->strConst);
						break;
					case nil_e:
						fprintf(fp," NULL");
						break;
					default: assert(0);
		}
		fprintf(fp,"\n");
		break;
					case add:
					case sub:
					case mul:
					case Div:
					case mod:
					case and:
					case or:
					case tablegetelem:
					case tablesetelem:
						switch(p->arg1->type){
							case var_e:
							case tableitem_e:
							case programfunc_e:
							case libraryfunc_e:
							case arithmexpr_e:
							case assignexpr_e:
							case newtable_e:
								fprintf(fp,"%s \t",p->arg1->sym->value.varVal->name);
								break;
							case booleanexpr_e:
								//if(p->arg1->boolConst ==1)
								//	fprintf(fp,"TRUE\t");
								//else
								//	fprintf(fp,"FALSE \t");
								//break;
								fprintf(fp,"%s \t",p->arg1->sym->value.varVal->name);
								break;
							case constnum_e:
								fprintf(fp,"%lf \t", p->arg1->numConst);
								break;
							case constbool_e:
								if(p->arg1->boolConst ==1)
									fprintf(fp,"TRUE\t");
								else
									fprintf(fp,"FALSE \t");
								break;
							case constchar_e:
								fprintf(fp,"%s \t", p->arg1->strConst);
								break;
							case nil_e:
								fprintf(fp,"NULL");
								break;
							default: assert(0);
						}
						switch(p->arg2->type){
							case var_e:
							case tableitem_e:
							case programfunc_e:
							case libraryfunc_e:
							case arithmexpr_e:
							case assignexpr_e:
							case newtable_e:
								fprintf(fp,"%s \t ",p->arg2->sym->value.varVal->name);
								break;
							case booleanexpr_e:
								//if(p->arg1->boolConst ==1)
								//	fprintf(fp,"TRUE\t");
								//else
								//	fprintf(fp,"FALSE \t");
								//break;
								fprintf(fp,"%s \t", p->arg2->sym->value.varVal->name);
								break;
							case constnum_e:
								fprintf(fp,"%lf \t", p->arg2->numConst);
								break;
							case constbool_e:
								if(p->arg2->boolConst ==1){
									fprintf(fp,"TRUE\t");
								}
								else{
									fprintf(fp,"FALSE \t");
								}
								break;
							case constchar_e:
								fprintf(fp,"%s \t", p->arg2->strConst);
								break;
							case nil_e:
								fprintf(fp,"NULL");
								break;
							default: assert(0);
						}
						switch(p->result->type){
							case var_e:
							case tableitem_e:
							case programfunc_e:
							case libraryfunc_e:
							case arithmexpr_e:
							case assignexpr_e:
							case newtable_e:
								fprintf(fp,"%s \t",p->result->sym->value.varVal->name);
								break;
							case booleanexpr_e:
								fprintf(fp," %s \t", p->result->sym->value.varVal->name);
// 								if(p->result->boolConst ==1)
// 									fprintf(fp," TRUE\t");
// 								else
// 									fprintf(fp," FALSE \t");
								break;
							case constnum_e:
								fprintf(fp," %lf \t", p->result->numConst);
								break;
							case constbool_e:
								if(p->result->boolConst ==1)
									fprintf(fp," TRUE\t");
								else
									fprintf(fp," FALSE \t");
								break;
							case constchar_e:
								fprintf(fp," %s \t", p->result->strConst);
								break;
							case nil_e:
								fprintf(fp," NULL");
								break;
							default: assert(0);
						}
						fprintf(fp,"\n");
						break;
							case uminus:
								switch(p->arg1->type){
									case var_e:
									case tableitem_e:
									case programfunc_e:
									case libraryfunc_e:
									case arithmexpr_e:
									case assignexpr_e:
									case newtable_e:
										fprintf(fp,"%s \t",p->arg1->sym->value.varVal->name);
										break;
									case booleanexpr_e:
										fprintf(fp,"%s \t",p->arg1->sym->value.varVal->name);
// 										if(p->arg1->boolConst ==1){
// 											fprintf(fp,"TRUE\t");
// 										}
// 										else{
// 											fprintf(fp,"FALSE \t");
// 										}
										break;
									case constnum_e:
										fprintf(fp,"%lf \t", p->arg1->numConst);
										break;
									case constbool_e:
										if(p->arg1->boolConst ==1){
											fprintf(fp,"TRUE\t");
										}
										else{
											fprintf(fp,"FALSE \t");
										}
										break;
									case constchar_e:
										fprintf(fp,"%s \t", p->arg1->strConst);
										break;
									case nil_e:
										fprintf(fp,"NULL");
										break;
									default: assert(0);
								}
								switch(p->result->type){
									case var_e:
									case tableitem_e:
									case programfunc_e:
									case libraryfunc_e:
									case arithmexpr_e:
									case assignexpr_e:
									case newtable_e:
										fprintf(fp,"%s \t",p->result->sym->value.varVal->name);
										break;
									case booleanexpr_e:
										fprintf(fp,"%s \t",p->result->sym->value.varVal->name);
// 										if(p->result->boolConst ==1)
// 											fprintf(fp," TRUE\t");
// 										else
// 											fprintf(fp," FALSE \t");
										break;
									case constnum_e:
										fprintf(fp," %lf \t", p->result->numConst);
										break;
									case constbool_e:
										if(p->result->boolConst ==1)
											fprintf(fp," TRUE\t");
										else
											fprintf(fp," FALSE \t");
										break;
									case constchar_e:
										fprintf(fp," %s \t", p->result->strConst);
										break;
									case nil_e:
										fprintf(fp," NULL");
										break;
									default: assert(0);
								}
								fprintf(fp,"\n");
								break;
									case not:
										switch(p->arg1->type){
											case var_e:
											case tableitem_e:
											case programfunc_e:
											case libraryfunc_e:
											case arithmexpr_e:
											case assignexpr_e:
											case newtable_e:
												fprintf(fp,"%s \t",p->arg1->sym->value.varVal->name);
												break;
											case booleanexpr_e:
												fprintf(fp,"%s \t",p->arg1->sym->value.varVal->name);
// 												if(p->arg1->boolConst ==1)
// 													fprintf(fp,"TRUE\t");
// 												else
// 													fprintf(fp,"FALSE \t");
												break;
											case constnum_e:
												fprintf(fp,"%lf \t", p->arg1->numConst);
												break;
											case constbool_e:
												if(p->arg1->boolConst ==1)
													fprintf(fp,"TRUE\t");
												else
													fprintf(fp,"FALSE \t");
												break;
											case constchar_e:
												fprintf(fp,"%s \t", p->arg1->strConst);
												break;
											case nil_e:
												fprintf(fp,"NULL");
												break;
											default: assert(0);
										}
										switch(p->result->type){
											case var_e:
											case tableitem_e:
											case programfunc_e:
											case libraryfunc_e:
											case arithmexpr_e:
											case assignexpr_e:
											case newtable_e:
												fprintf(fp,"%s \t",p->result->sym->value.varVal->name);
												break;
											case booleanexpr_e:
												fprintf(fp,"%s \t",p->result->sym->value.varVal->name);/*
												if(p->result->boolConst ==1)
													fprintf(fp," TRUE\t");
												else
													fprintf(fp," FALSE \t");*/
												break;
											case constnum_e:
												fprintf(fp," %lf \t", p->result->numConst);
												break;
											case constbool_e:
												if(p->result->boolConst ==1)
													fprintf(fp," TRUE\t");
												else
													fprintf(fp," FALSE \t");
												break;
											case constchar_e:
												fprintf(fp," %s \t", p->result->strConst);
												break;
											case nil_e:
												fprintf(fp," NULL");
												break;
											default: assert(0);
										}
										fprintf(fp,"\n");
										break;
											case if_eq:
											case if_noteq:
											case if_lesseq:
											case if_greateq:
											case if_less:
											case if_greater:
												switch(p->arg1->type){
													case var_e:
													case tableitem_e:
													case programfunc_e:
													case libraryfunc_e:
													case arithmexpr_e:
													case assignexpr_e:
													case newtable_e:
														fprintf(fp,"%s \t",p->arg1->sym->value.varVal->name);
														break;
													case booleanexpr_e:
														fprintf(fp,"%s \t",p->arg1->sym->value.varVal->name);/*
														if(p->arg1->boolConst ==1){
															fprintf(fp,"TRUE\t");
														}
														else{
															fprintf(fp,"FALSE \t");
														}*/
														break;
													case constnum_e:
														fprintf(fp,"%lf \t", p->arg1->numConst);
														break;
													case constbool_e:
														if(p->arg1->boolConst ==1)
															fprintf(fp,"TRUE\t");
														else
															fprintf(fp,"FALSE \t");
														break;
													case constchar_e:
														fprintf(fp,"%s \t", p->arg1->strConst);
														break;
													case nil_e:
														fprintf(fp,"NULL");
														break;
													default: assert(0);
												}
												switch(p->arg2->type){
													case var_e:
													case tableitem_e:
													case programfunc_e:
													case libraryfunc_e:
													case arithmexpr_e:
													case assignexpr_e:
													case newtable_e:
														fprintf(fp,"%s \t ",p->arg2->sym->value.varVal->name);
														break;
													case booleanexpr_e:
														fprintf(fp,"%s \t ",p->arg2->sym->value.varVal->name);
// 														if(p->arg2->boolConst ==1){
// 															fprintf(fp,"TRUE\t");
// 															
// 														}
// 														else{
// 															fprintf(fp,"FALSE \t");
// 														}
														break;
													case constnum_e:
														fprintf(fp,"%lf \t", p->arg2->numConst);
														break;
													case constbool_e:
														if(p->arg2->boolConst ==1){
															fprintf(fp,"TRUE\t");
														}
														else{
															fprintf(fp,"FALSE \t");
														}
														break;
													case constchar_e:
														fprintf(fp,"%s \t", p->arg2->strConst);
														break;
													case nil_e:
														fprintf(fp,"NULL");
														break;
													default: assert(0);
												}
												fprintf(fp,"%d \n",p->label);
												break;
													case call:
													case param:
													case getretval:
													case funcstart:
													case funcend:
													case tablecreate:
														switch(p->arg1->type){
															case var_e:
															case tableitem_e:
															case programfunc_e:
															case libraryfunc_e:
															case arithmexpr_e:
															case assignexpr_e:
															case newtable_e:
																//assert(p);
																//assert(p->arg1);
																//assert(p->arg1->sym);
																fprintf(fp,"%s \t",p->arg1->sym->value.varVal->name); break;
															case booleanexpr_e:
																fprintf(fp,"%s \t",p->arg1->sym->value.varVal->name); break;
// 																if(p->arg1->boolConst ==1)
// 																	fprintf(fp,"TRUE\t");
// 																else
// 																	fprintf(fp,"FALSE \t");
// 																break;
															case constnum_e:
																fprintf(fp,"%lf \t", p->arg1->numConst);
																break;
															case constbool_e:
																if(p->arg1->boolConst ==1)
																	fprintf(fp,"TRUE\t");
																else
																	fprintf(fp,"FALSE \t");
																break;
															case constchar_e:
																fprintf(fp,"%s \t", p->arg1->strConst);
																break;
															case nil_e:
																fprintf(fp,"NULL");
																break;
															default: assert(0);
														}
														fprintf(fp,"\n");
														break;
														case ret:
															if(p->arg1 !=NULL){
																switch(p->arg1->type){
																	case var_e:
																	case tableitem_e:
																	case programfunc_e:
																	case libraryfunc_e:
																	case arithmexpr_e:
																	case assignexpr_e:
																	case newtable_e:
																		//assert(p);
																		//assert(p->arg1);
																		//assert(p->arg1->sym);
																		fprintf(fp,"%s \t",p->arg1->sym->value.varVal->name); break;
																	case booleanexpr_e:
																		fprintf(fp,"%s \t",p->arg1->sym->value.varVal->name); break;
																		// 																if(p->arg1->boolConst ==1)
																		// 																	fprintf(fp,"TRUE\t");
																		// 																else
																		// 																	fprintf(fp,"FALSE \t");
																		// 																break;
																	case constnum_e:
																		fprintf(fp,"%lf \t", p->arg1->numConst);
																		break;
																	case constbool_e:
																		if(p->arg1->boolConst ==1)
																			fprintf(fp,"TRUE\t");
																		else
																			fprintf(fp,"FALSE \t");
																		break;
																	case constchar_e:
																		fprintf(fp,"%s \t", p->arg1->strConst);
																		break;
																	case nil_e:
																		fprintf(fp,"NULL");
																		break;
																	default: assert(0);
																}
																fprintf(fp,"\n");
																break;
															}
															case jump:
																fprintf(fp,"%d\n",p->label);
																break;
	}
	counter++;
}
fclose(fp);

}

