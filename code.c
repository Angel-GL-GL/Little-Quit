#include "hoc.h"
#include "y.tab.h"
#include "math.h"
#include <stdio.h>
#define NSTACK 256
static Datum stack[NSTACK];
static Datum *stackp;
#define NPROG 2000
Inst prog[NPROG];
Inst *progp;
Inst *pc;
Inst *progbase = prog;
int returning;
typedef struct Frame
{
   Symbol *sp;
   Inst *retpc;
   Datum *argn;
   int nargs;
} Frame;
#define NFRAME 10000
Frame frame[NFRAME];
Frame *fp;

void initcode()
{
   progp = progbase;
   stackp = stack;
   fp = frame;
   returning = 0;
}

void push(Datum d)
{
   if (stackp >= &stack[NSTACK])
      execerror("stack overflow", (char *)0);
   *stackp++ = d;
}

void pop1()
{
   if (stackp <= stack)
      execerror("stack underflow", (char *)0);
   --stackp;
}

Datum pop()
{
   if (stackp <= stack)
      execerror("stack underflow", (char *)0);
   return *--stackp;
}


void constpush( ){
Datum d;
d.valnumD  =  ((Symbol  *)*pc++)->u.valnum;
push(d);
}


void varpush()
{
   Datum d;
   d.sym = (Symbol *)(*pc++);
   push(d);
}

void whilecode()
{
   Datum d;
   Inst *savepc = pc;
   execute(savepc + 2);
   d = pop();
   while (d.dval)
   {
      execute(*((Inst **)(savepc)));
      execute(savepc + 2);
      d = pop();
   }
   pc = *((Inst **)(savepc + 1));
}

void ifcode()
{
   Datum d;
   Inst *savepc = pc;
   execute(savepc + 3);
   d = pop();
   if (d.dval)
      execute(*((Inst **)(savepc)));
   else if (*((Inst **)(savepc + 1)))
      execute(*((Inst **)(savepc + 1)));
   pc = *((Inst **)(savepc + 2));
}

void forcode()
{
   Datum d;
   Inst *savepc = pc;
   for (execute(savepc + 4), execute(*((Inst **)(savepc))), d = pop(); d.dval; execute(*((Inst **)savepc + 1)))
   {
      execute(*((Inst **)(savepc + 2)));
      execute(*((Inst **)(savepc)));
      d = pop();
   }
   pc = *((Inst **)(savepc + 3));
}

void eval()
{
   Datum d;
   d = pop();
   if (d.sym->type == INDEF)
      execerror("undefined variable",
                d.sym->name);
   d.val = d.sym->u.val;
   push(d);
}

void evalnum()
{
   Datum d;
   d = pop();
   if (d.sym->type == INDEF)
      execerror("undefined variable",
                d.sym->name);
   d.valnumD = d.sym->u.valnum;
   push(d);
}

void assign() /* asignar el valor superior al siguientevalor */
{
   Datum d1, d2;
   d1 = pop();
   d2 = pop();
   if (d1.sym->type != VAR && d1.sym->type != INDEF)
      execerror("assignment to non-variable", d1.sym->name);
   d1.sym->u.val = d2.val;
   d1.sym->type = VAR;
   push(d2);
}
void assignnum() /* asignar el valor superior al siguientevalor */
{
   Datum d1, d2;
   d1 = pop();
   d2 = pop();
   if (d1.sym->type != VARINT && d1.sym->type != INDEF)
      execerror("assignment to non-variable", d1.sym->name);
   d1.sym->u.valnum = d2.valnumD;
   d1.sym->type = VARINT;
   push(d2);
}

void print(){
   Datum d;
   d = pop();
   extern void imprime();
   imprime(d.val);
}

void printnum() {
Datum d;
d = pop();
printf("\t%.8g\n", d.valnumD);
}

void prexpr(){
   Datum d;
   print();
}

Inst *code(Inst f){
   Inst *oprogp = progp;
   if (progp > &prog[NPROG - 1])
      execerror("program too big", (char *)0);
   *progp++ = f;
   return oprogp;
}

void execute(Inst *p){ 
   for (pc = p; *pc != STOP;)
      (*pc++)();
}

void gt()
{
   Datum d1, d2;
   short int a,b;
   d2 = pop();
   d1 = pop();
   a = d1.val->altura * d1.val->longitud;
   b = d2.val->altura * d2.val->longitud;
   d1.dval = a > b ;   
   push(d1);
}

void lt()
{
   Datum d1, d2;
   short int a,b;
   d2 = pop();
   d1 = pop();
   a = d1.val->altura * d1.val->longitud;
   b = d2.val->altura * d2.val->longitud;
   d1.dval = a < b ;   
   push(d1);
}

void ge()
{
   Datum d1, d2;
   short int a,b;
   d2 = pop();
   d1 = pop();
   a = d1.val->altura * d1.val->longitud;
   b = d2.val->altura * d2.val->longitud;
   d1.dval = a >= b ;     
   push(d1);
}

void le()
{
   Datum d1, d2;
   short int a,b;
   d2 = pop();
   d1 = pop();
   a = d1.val->altura * d1.val->longitud;
   b = d2.val->altura * d2.val->longitud;
   d1.dval = a <= b ;   
   push(d1);
}

void eq()
{
   Datum d1, d2, band;
   d2 = pop();
   d1 = pop();
   band.dval = d1.val->altura == d2.val->altura && d1.val->longitud == d2.val->longitud;
   if (band.dval){
        short int i = 0, j = 0;
        for (i = 0; i < d1.val->altura && band.dval; i++)
            for (j = 0; j < d1.val->longitud && band.dval; j++)
                band.dval = d1.val->patron[i][j] == d2.val->patron[i][j];
    }
    
   push(band);
}

void ne()
{
   Datum d;
   eq();
   d = pop();
   if(d.dval) d.dval = 0;
   else d.dval = 1;
   push(d);
}

void and (){
   Datum d1, d2, band1, band2, daux1, daux2;
   extern Tela *crearVacia();
   d2 = pop();
   d1 = pop();
   daux1.val = crearVacia(crear(d1.val->longitud,d1.val->altura));
   daux2.val = crearVacia(crear(d2.val->longitud,d2.val->altura));
   push(d1);
   push(daux1);
   ne();
   band1 = pop();
   push(d2);
   push(daux2);
   ne();
   band2 = pop();
   band1.dval = band1.dval && band2.dval;
   push(band1);
}

void or (){
   Datum d1, d2, band1, band2, daux1, daux2;
   extern Tela *crearVacia();
   d2 = pop();
   d1 = pop();
   daux1.val = crearVacia(crear(d1.val->longitud,d1.val->altura));
   daux2.val = crearVacia(crear(d2.val->longitud,d2.val->altura));
   push(d1);
   push(daux1);
   ne();
   band1 = pop();
   push(d2);
   push(daux2);
   ne();
   band2 = pop();
   band1.dval = band1.dval || band2.dval;
   push(band1);
}

void not(){
   Datum d;
   d = pop();
   if(d.dval) d.dval = 0;
   else d.dval = 1;
   push(d);
}

void rotar_(){
   Datum d;
   d=pop();
   extern Tela* rotar();
   d.val = rotar(d.val);
   push(d);
}

void coser_(){
   Datum d1, d2;
   d2=pop();
   d1=pop();
   extern Tela* coser();
   d1.val = coser(d1.val,d2.val);
   push(d1);

}

void define(Symbol *sp){
   sp->u.defn = progbase; 
   progbase = progp;      
}

void call(){
   Symbol *sp = (Symbol *)pc[0]; 
                          
   if (fp++ >= &frame[NFRAME - 1])
      execerror(sp->name, "call  nested too deeply");
   fp->sp = sp;
   fp->nargs = (int)pc[1]; 
   fp->retpc = pc + 2;
   fp->argn = stackp - 1; 
   execute(sp->u.defn);
   returning = 0;
}

void ret(){
   int i;

   for (i = 0; i < fp->nargs; i++)
      pop(); 
   pc = (Inst *)fp->retpc;
   --fp;
   returning = 1;
}

void funcret(){
   Datum d;
   if (fp->sp->type == PROCEDURE)
      execerror(fp->sp->name, "(proc) returns value");
   d = pop(); 
   ret();
   push(d);
}

void procret(){
   if (fp->sp->type == FUNCTION)
      execerror(fp->sp->name, "(func) returns no value");
   ret();
}

Tela **getarg()
{
   int narg = (int)*pc++;
   if (narg > fp->nargs)
      execerror(fp->sp->name, "not enough arguments");
   return &fp->argn[narg - fp->nargs].val;
}

void arg()
{
   Datum d;
   d.val = *getarg();
   push(d);
}

void argassign()
{
   Datum d;
   d = pop();
   push(d); 
   *getarg() = d.val;
}

void prstr()
{
   printf("%s", (char *)*pc++);
}

void varread()
{
   Datum d;
   extern FILE *fin;
   Symbol *var = (Symbol *)*pc++;
Again:
   switch (fscanf(fin, "%lf", &var->u.val))
   {
   case EOF:
      if (moreinput())
         goto Again;
      d.val = var->u.val = crearVacia(crear(0,0));
      break;
   case 0:
      execerror("non-number read into", var->name);
      break;
   default:
      d.val = crearVacia(crear(0,0));
      break;
   }
   var->type = VAR;
   push(d);
}

void add(){
Datum d1, d2; 
d2 = pop(); 
d1 = pop(); 
d1.valnumD += d2.valnumD; 
push(d1);
}

void sub(){
Datum d1,  d2; 
d2  = pop(); 
d1  = pop(); 
d1.valnumD  -= d2.valnumD; 
push(d1);
}

void mul(){
Datum d1, d2;
d2 = pop(); 
d1 = pop(); 
d1.valnumD *= d2.valnumD; 
push(d1);
}
void divi( ){
Datum d1, d2;
d2 = pop();
if (d2.valnumD == 0.0)
execerror("division by zero", (char *)0);
d1 = pop(); 
d1.valnumD /= d2.valnumD; 
push(d1);
}
void negate(){
Datum d; 
d = pop(); 
d.valnumD  =  -d.valnumD; 
push(d);
}


void power(){
   Datum d1, d2;
   d2 = pop();
   d1 = pop();
   d1.valnumD = pow(d1.valnumD, d2.valnumD);
   push(d1);
}

void gtn() {
Datum d1,  d2;
d2 = pop();
d1 = pop();
d1.valnumD  =   (double)(d1.valnumD  > d2.valnumD);
push(d1);
}
void ltn(){
Datum d1,  d2;
d2 = pop();
d1 = pop();
d1.valnumD  =   (double)(d1.valnumD  < d2.valnumD);
push(d1);
}
void gen( ) {
Datum d1,  d2;
d2  = pop();
d1  = pop();
d1.valnumD   =   (double)(d1.valnumD  >= d2.valnumD);
push(d1) ;
}
void len() {
Datum d1,  d2;
d2   =  pop();
d1   =  pop();
d1.valnumD  =   (double)(d1.valnumD   <=  d2.valnumD);
push(d1);
}
void eqn( ) {
Datum d1,  d2;
d2  = pop();
d1  = pop();
d1.valnumD  =   (double) (d1.valnumD  ==  d2.valnumD);
push(d1);
}
void nen(){
Datum d1, d2;
d2 = pop();
d1 = pop();
d1.valnumD = (double)(d1.valnumD != d2.valnumD);
push(d1);
}