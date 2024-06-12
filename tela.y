%{
#include   "hoc.h"
#include<string.h>
#define  code2(c1,c2)	    code(c1);   code(c2)
#define  code3(c1 ,c2,c3)   code(c1);   code(c2);  code(c3)
int indef;
void yyerror (char *s);
int yylex ();
void warning(char *s, char *t);
void execerror(char *s, char *t);
%}
%union   {
    Symbol  *sym;   
    Inst    *inst;  
    int narg;
}
%token  <sym> ROTAR COSER NUMBER PRINT VAR VARINT INDEF WHILE IF ELSE FOR STRING FUNCTION PROCEDURE RETURN FUNC PROC READ
%token <narg> ARG
%type   <inst>  stmt asgn expr exprnum stmtlist cond while if end for expf prlist begin
%type <sym> procname
%type <narg> arglist

%right	'='
%left	OR
%left	AND
%left	GT GE LT LE EQ NE 
%left   '+' '-'
%left   '*' '/'
%left   UNARYMINUS NOT
%right      '^'
%%

list:     /* nada */ 
	| list '\n'
	| list defn '\n'
	| list asgn '\n'	{ code2(pop1, STOP); return 1; }
	| list stmt '\n'	{ code(STOP); return 1; }
	| list expr '\n'	{ code2(print, STOP); return 1; }
	| list exprnum '\n'	{ code2(printnum, STOP); return 1; }
	| list error '\n'	{ yyerrok; }
	;

asgn: VAR '=' expr {$$=$3; code3(varpush,(Inst)$1,assign);}
	| ARG '=' expr {
		defnonly("$"); code2(argassign,(Inst)$1); $$=$3;}
	;

stmt:     expr   { code(pop1); }
	|     exprnum   { code(pop1); }
	| PRINT expr    { code(prexpr); $$ = $2;} 
	| PRINT exprnum    { code(printnum); $$ = $2;} 
	| while cond stmt end {
		($1)[1] = (Inst)$3;     
		($1)[2] = (Inst)$4; }   
	| if cond stmt end {    
		($1)[1] = (Inst)$3;      
		($1)[3] = (Inst)$4; }   
	| if cond stmt end ELSE stmt end {  
		($1)[1]   =   (Inst)$3;	
		($1)[2]   =   (Inst)$6;	
		($1)[3]   =   (Inst)$7;} 
	| for '(' expf ';' expf ';' expf ')' stmt end {
		($1)[1] = (Inst)$5; 
		($1)[2] = (Inst)$7; 
		($1)[3] = (Inst)$9; 
		($1)[4] = (Inst)$10;}
	| '{' stmtlist '}' { $$  =  $2; }
	| RETURN {defnonly("return"); code(procret);}
	| RETURN expr {defnonly("return");$$=$2;code(funcret);}
	| PROCEDURE begin '(' arglist ')' {
			$$=$2; code3(call, (Inst)$1, (Inst)$4);}
	| PRINT prlist { $$=$2; }
    ;

expf: expr {$$=$1;code(STOP);}
    | exprnum {$$=$1;code(STOP);}
    ;

cond:	'('   expr   ')'      {   code(STOP);  $$  =  $2;   }
	|	'('   exprnum   ')'      {   code(STOP);  $$  =  $2;   }
	;

while:	WHILE   {   $$ = code3(whilecode, STOP, STOP); }
	;

if:IF   { $$=code(ifcode); code3(STOP, STOP, STOP); }
	;

for:FOR { $$=code(forcode);code2(STOP, STOP);code2(STOP, STOP);}
	;

end: /* nada */{ code(STOP); $$ = progp; }
	;

stmtlist: /* nada */	{ $$ = progp; }
	| stmtlist '\n' 
	| stmtlist stmt
	;

exprnum: NUMBER {   $$ = code2(constpush, (Inst)$1); }
	|	VARINT '=' exprnum {$$=$3; code3(varpush,(Inst)$1,assignnum);}
	|   VARINT	{  $$ = code3(varpush, (Inst)$1, evalnum);}
	|exprnum GT exprnum  { code(gtn); }
	|exprnum GE exprnum  { code(gen); }
	|exprnum LT exprnum  { code(ltn); }
	|exprnum LE exprnum  { code(len); }
	|exprnum EQ exprnum  { code (eqn); }
	|exprnum NE exprnum  { code(nen); } 
   	| '(' exprnum ')'  { $$ = $2; }
   	| exprnum   '+'   exprnum {   code(add); }
   	| exprnum   '-'   exprnum {   code(sub); }
   	| exprnum   '*'   exprnum {   code(mul); }
   	| exprnum   '/'   exprnum {   code(divi); }
   	| exprnum   '^'   exprnum {   code(power); }
   	| '-' exprnum  %prec UNARYMINUS   { $$=$2; code(negate); }
	;
   	

expr:   VAR	{  $$ = code3(varpush, (Inst)$1, eval);}
	| asgn
	| ROTAR expr {code(rotar_); $$=$2;}
    | COSER '(' expr ',' expr ')' {$$=code(coser_);}
	| ARG {defnonly("$"); $$=code2(arg,(Inst)$1);}
	| FUNCTION begin '(' arglist ')'
		{ $$=$2; code3(call,(Inst)$1,(Inst)$4);}
	| READ '(' VAR ')' {$$ = code2(varread, (Inst)$3);}
	| '(' expr ')'  { $$ = $2; }
	|expr GT expr  { code(gt); }
	|expr GE expr  { code(ge); }
	|expr LT expr  { code(lt); }
	|expr LE expr  { code(le); }
	|expr EQ expr  { code (eq); }
	|expr NE expr  { code(ne); }
	|expr AND expr { code(and); }
	|expr OR expr  { code(or); }
	|NOT expr      { $$ = $2; code(not); }
	;

begin: /*nada*/ {$$=progp;}
	;

prlist: expr {code(prexpr);}
	|STRING {$$=code2(prstr, (Inst)$1);}
	|prlist ',' expr {code(prexpr);}
	|prlist ',' STRING {code2(prstr, (Inst)$3);}
	;

defn: FUNC procname {$2->type=FUNCTION; indef=1;}
	 	'('')' stmt {code(procret);define($2);indef=0;}
	| PROC procname {$2->type=PROCEDURE; indef=1;}
	 	'('')' stmt {code(procret);define($2);indef=0;}
	;

procname: VAR | FUNCTION | PROCEDURE
	;

arglist: /*nada*/ {$$=0;}
	|expr {$$=1;}
	|arglist ',' expr {$$=$1+1;}
	;

%%

/* fin de la gramática */ 
#include <stdio.h> 
#include <ctype.h> 

char    *progname; 
int    lineno = 1 ; 

#include <signal.h> 
#include <setjmp.h> 
jmp_buf begin; 
int    indef;
char   *infile;       /* nombre de archivo de entrada */ 
FILE    *fin;         /* apuntador a archivo de entrada */
char   **gargv;       /* lista global de argumentos */ 
int    gargc; 
int c;  /* global, para uso de warning() */

int yylex(){
while  ((c=getc(fin)) ==  ' ' ||  c ==   '\t')
          ;
if (c == EOF)
	return 0; 
if (c == '.' || isdigit(c)) {   /* número */
	double d;
	ungetc(c, fin); 
	fscanf(fin, "%lf", &d);
        //printf("val = < %g >", d);
	yylval.sym = installnum("", NUMBER, d);
	return NUMBER; 
}
if(c == '[' || (c>= 65 && c<=90) || c == ']'){
	Symbol *s;
	char sbuf[100], *p = sbuf; 
	do {
		if (p >= sbuf + sizeof(sbuf) - 1) { 
			*p = '\0'; 
			execerror("name too long", sbuf);
		}
	*p++ = c;
	} while ((c=getc(fin)) != ']'); 
	*p++ = c;
	*p = '\0'; 
        
	if ((s=lookupNum(sbuf)) == 0)
       		s=install(sbuf, INDEF, 0.0); 
        //printf("sbuf = < %s > tipo=(%d)", sbuf, s->type);
	yylval.sym = s;
	return VARINT;
}
if (isalpha(c)) {//ID 
	Symbol *s;
	char sbuf[100], *p = sbuf;
	if(islower(c)){
		 
	do {
		if (p >= sbuf + sizeof(sbuf) - 1) { 
			*p = '\0'; 
			execerror("name too long", sbuf);
		}
	*p++ = c;
        //putchar(c);
	} while ((c=getc(fin)) != EOF && isalnum(c)); 
	ungetc(c, fin); 
	*p = '\0'; 
        
	if ((s=lookup(sbuf)) == 0)
       		s=install(sbuf, INDEF, 0.0); 
        //printf("sbuf = < %s > tipo=(%d)", sbuf, s->type);
	yylval.sym = s;
	return s->type == INDEF ? VAR : s->type;
	} else {
		
	}
}
if (c == '$') { /* ¿argumento? */ 
	int n = 0; 
	while (isdigit(c=getc(fin)))
		n = 10 * n + c - '0'; 
	ungetc(c, fin); 
	if (n == 0)
		execerror("strange $...", (char *)0); 
	yylval.narg = n; 
	return ARG;
}
if (c == '"') { /* cadena entre comillas */ 
	char sbuf[100], *p, *emalloc(); 
	for (p = sbuf; (c=getc(fin)) != '"'; p++) { 
		if (c == '\n' || c == EOF)
			execerror("missing quote", ""); 
		if (p >= sbuf + sizeof(sbuf) - 1) { 
			*p = '\0';
			execerror("string too long", sbuf); 
		}
		*p = backslash(c); 
	}
	*p = 0;
	yylval.sym = (Symbol *)emalloc(strlen(sbuf)+1); 
	strcpy((char *)yylval.sym, sbuf); 
	return STRING;
}
	switch (c) {
	case '>':               return follow('=', GE, GT);
	case '<':               return follow('=', LE, LT);
	case '=':               return follow('=', EQ, '=');
	case '!':               return follow('=', NE, NOT);
	case '|':               return follow('|', OR, '|');
	case '&':               return follow('&', AND, '&');
	case '\n':              lineno++; return '\n';
	default:                return c; 
        }
}
int backslash(int   c ){/*tomar siguiente carácter con las \ interpretadas   */
   char *strchr( ) ;  /* strchr() en algunos sistemas */
   static char transtab[] = "b\bf\fn\nr\rt\t";
   if (c != '\\')
	return c;
   c = getc(fin);
   if (islower(c) && strchr(transtab, c)) 
	return strchr(transtab, c)[1];
   return c; 
}
int follow(expect, ifyes, ifno){ /* búsqueda hacia adelante para > -, etc. */ 
   int c = getc(fin);
   if (c == expect)
	return ifyes;
   ungetc(c, fin);
   return ifno; 
} 
void defnonly( char *s )     /* advertena la si hay definición i legal */
{
if (!indef)
	execerror(s, "used outside definition"); 
} 
void yyerror(char *s)      /* comunicar errores de tiempo de compilación */
{
warning(s, (char *)0); 
} 
execerror(char *s, char *t) /* recuperación de errores de tiempo de ejecución */
{
warning(s, t);
fseek( fin, 0L, 2);       /* sacar el resto del archivo */
longjmp(begin, 0); 
}

void fpecatch()      /* detectar errores por punto flotante */ 
{
execerror("floating point exception", (char *) 0); 
}		

void main(int argc, char **argv){  /* hoc6 */ 
   int i;
   void fpecatch();
progname = argv[0];
if (argc == 1) {        /* simular una lista de argumentos */ 
	static char *stdinonly[] = { "-" };
	gargv = stdinonly;
	gargc = 1; } 
else {
	gargv = argv+1;
	gargc = argc-1; 
}
init(); 
while (moreinput())
	run(); 
}                                                                              
int moreinput( ) {
if (gargc-- <= 0)
	return 0; 
if (fin && fin != stdin)
fclose(fin); 
infile = *gargv++; 
//printf("arch ent=(%s)\n",infile);
lineno = 1; 
if (strcmp(infile, "-") == 0) {
	fin = stdin;
	infile = 0; 
} else if ((fin=fopen(infile, "r")) == NULL) {
	fprintf(stderr, "%s: can't open %s\n" , progname, infile);
	return moreinput();
}
return 1;
}
void run()   /* ejecutar hasta el fin de archivo (EOF) */
{
setjmp(begin);
signal(SIGFPE,   fpecatch);
for   (initcode();   yyparse();   initcode()){
        //puts("ant execute");
	execute(progbase); 
 }
}
void warning(char *s, char *t)        /*   imprimir mensaje de advertencia   */
{
fprintf(stderr, "%s: %s", progname, s); 
if (t)
	fprintf(stderr, " %s", t); 
if (infile)
	fprintf(stderr, " in %s", infile); 
fprintf(stderr, " near line %d\n", lineno); 
while (c != '\n' && c != EOF)
c = getc(fin);  /* sacar el resto del renglón de entrada */
if (c == '\n')
	lineno++;
}
