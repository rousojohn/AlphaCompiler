%{

#include "parser.h"
#include "header.h"

int comments_counter = 0;

int stringIsValid(char *s);
void changeString(char *s);
void shrinkErrorString(char *s);

%}

%option noyywrap
%option yylineno
%option outfile = "lexical_analyzer.c"


KEYWORD_IF "if"
KEYWORD_ELSE "else"
KEYWORD_WHILE "while"
KEYWORD_FOR "for"
KEYWORD_FUNCTION "function"
KEYWORD_RETURN "return"
KEYWORD_BREAK "break"
KEYWORD_CONTINUE "continue"
KEYWORD_AND "and"
KEYWORD_NOT "not"
KEYWORD_OR "or"
KEYWORD_LOCAL "local"
KEYWORD_TRUE "true"
KEYWORD_FALSE "false"
KEYWORD_NIL "nil"

OPERATOR_ASSIGN "="
OPERATOR_ADD "+"
OPERATOR_SUB "-"
OPERATOR_MUL "*"
OPERATOR_DIV "/"
OPERATOR_PERCENT "%"
OPERATOR_EQUAL "=="
OPERATOR_DIFFERENT "!="
OPERATOR_INCREASE "++"
OPERATOR_DECREASE "--"
OPERATOR_BIGGER ">"
OPERATOR_SMALLER "<"
OPERATOR_BIGGEREQUAL ">="
OPERATOR_SMALLEREQUAL "<="

LEFT_BRACKET "["
RIGHT_BRAKET "]"
LEFT_CURLBRACKET "{"
RIGHT_CURLBRACKET "}"
LEFT_PARENTHESIS "("
RIGHT_PARENTHESIS ")"
DOT "."
DOUBLE_DOT ".."
UPDOWN_DOT ":"
DOUBLE_UPDOWN_DOT "::"
QUESTIONMARK ";"
COMMA ","


REAL 		[0-9]+"."[0-9]+
INTEGER 	[0-9]+
ID		[a-zA-Z]+[a-zA-Z_0-9]*
STRING		\"([^\n"]*[\n]*(\\\")*)*\"
/*
STRING		\"([a-zA-Z_0-9 \n\t\\]*(\\\")*)*\"
/*STRING		\""."*\"*/
UN_VAR		[_0-9]+[a-zA-Z_0-9]*
STRING_EOF	\"([^\n"]*[\n]*(\\\")*)*

	
%x COMMENT
%x STR_ERROR

%%



{KEYWORD_IF}     		{  return IF;}
{KEYWORD_ELSE}   		 {   return ELSE;}
{KEYWORD_WHILE}  		 {   return WHILE;}
{KEYWORD_FOR}    		 {  return FOR;}
{KEYWORD_FUNCTION}   		 {  return FUNCTION;}
{KEYWORD_RETURN}   		{   return RETURN;}
{KEYWORD_BREAK}   		{  return BREAK;}
{KEYWORD_CONTINUE} 		 {   return CONTINUE;} 
{KEYWORD_AND}   		{  return AND;}
{KEYWORD_NOT}   		{   return NOT;}
{KEYWORD_OR}   			{   return OR;}
{KEYWORD_LOCAL}  		 {    return LOCAL;}
{KEYWORD_TRUE}   		{   return TRUE;}
{KEYWORD_FALSE}  		{   return FALSE;}
{KEYWORD_NIL}   		{   return NIL;}


{OPERATOR_ASSIGN}  		{ return ASSIGN;}
{OPERATOR_ADD}  		{ return ADD;}
{OPERATOR_SUB}  		{  return SUB;}
{OPERATOR_MUL}  		{  return MUL;}
{OPERATOR_DIV}   		{ return DIV;}
{OPERATOR_PERCENT}  		{  return PERCENT;}
{OPERATOR_EQUAL}  		{  return EQUAL;}
{OPERATOR_DIFFERENT} 		 { return DIFFERENT;}
{OPERATOR_INCREASE}  		{ return INCREASE;}
{OPERATOR_DECREASE}  		{ return DECREASE;}
{OPERATOR_BIGGER} 		 { return BIGGER;}
{OPERATOR_SMALLER}  		{  return SMALLER; }
{OPERATOR_BIGGEREQUAL}  	{  return BIGGEREQUAL; }
{OPERATOR_SMALLEREQUAL} 	 { return SMALLEREQUAL;}


{LEFT_BRACKET}  	{   return LEFT_BRACKET; }
{RIGHT_BRAKET} 		 {   return RIGHT_BRACKET; }
{LEFT_CURLBRACKET}  	{   return LEFT_CURLBRACKET; }
{RIGHT_CURLBRACKET} 	  {  return RIGHT_CURLBRACKET; }
{LEFT_PARENTHESIS} 	 {  return LEFT_PARENTHESIS; }
{RIGHT_PARENTHESIS} 	 {   return RIGHT_PARENTHESIS; }
{DOT}  			{   return DOT; }
{DOUBLE_DOT}  		{  return DOUBLE_DOT; }
{UPDOWN_DOT} 		 {   return UPDOWN_DOT; }
{DOUBLE_UPDOWN_DOT}  	 { return DOUBLE_UPDOWN_DOT; }
{QUESTIONMARK}		 {  return QUESTIONMARK; }
{COMMA}	 		 {   return COMMA; }





{STRING}  	 {   if( stringIsValid(yytext) )
		     {	
			 yylval.stringValue = strdup(yytext); 
			 return STRING; 
		     }
		     else  
			 return INVALID_TOKEN; 
	       }



{ID}  		{ yylval.stringValue = strdup(yytext); return ID; }


{INTEGER}  	{ yylval.intValue = atoi(yytext); return INTEGER; }
		
		
{REAL} 		{ yylval.realValue = atof(yytext); return REAL; }



{UN_VAR}  	{ return INVALID_TOKEN; }

<INITIAL><<EOF>>	{ return EOF; }

"\n"		{		}
	

" "


"\t"




"//".*   	{   }


"&&"	{ return AND; }
"||"	{ return OR; }
"!"	{ return NOT; }



"/*"		{ comments_counter++;  }	BEGIN(COMMENT);

<COMMENT>"/*"	{comments_counter++; }

<COMMENT>[^*/]*		{  }

<COMMENT><<EOF>>  {   return INVALID_TOKEN; }

<COMMENT>"*/"	{ comments_counter--;
			 if(comments_counter==0){
			  BEGIN(INITIAL);
			//  return COMMENTS;
			  }
			   
		}
		
.		{  return INVALID_TOKEN; }

{STRING_EOF}	{ shrinkErrorString(yytext); return ERROR;}

%%



int stringIsValid(char *s)
{
     int state = 0;
     int i = 1;

     while( s[i] != '\0')
    {
	  if(state == 0)
	  {    if(s[i] == '\\')
	       {	   
		    state = 1;
	       }
	  }
	  else 
	  {
	       if( s[i] != '\"' && s[i] != '\\' && s[i] != 'n' && s[i]!= 't' )
		    return 0;
	       else
		    state = 0;
	  }
	  
	  i++;
     }
     return 1;
}


/*replaces the escape charachters from a valid string*/
void changeString(char *s)
{
  char * tmp;
  int i=0,j=0;
  while (s[i]!='\0'){
    if(s[i]=='\\'){
      if(s[i+1]=='n'){
	  tmp[j]='\n';
	  j++;
	  i++;
      }
      else if (s[i+1]=='t'){
	  tmp[j]='\t';
	  j++;
	  i++;
      }
      else if (s[i+1]=='\"'){
	  tmp[j]='\"';
	  j++;
	  i++;
      }
      else if(s[i+1] == '\\'){
	  tmp[j]='\\';
	  j++;
	  i++;
      }
    }
    else{
	 tmp[j]=s[i];
	  j++;
    }
    i++;
      
  }
  tmp[j]='\0';
  i=0;
  while(tmp[i]!='\0'){
    s[i]=tmp[i];
    i++;
  }
  s[i]='\0';
}



void shrinkErrorString(char *s)
{
     s[11] = '.';
     s[12] = '.';
     s[13] = '.';
     s[14] = '\0';
}

/*

int alpha_yylex(alpha_token_t *yyval)
{
     int c = yylex();
     *yyval = tmp;
     return c;   
}

*/
