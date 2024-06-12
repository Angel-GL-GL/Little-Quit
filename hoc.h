#include <string.h>
#define STOP (Inst)0

struct tela{
    short int longitud, altura;
    unsigned char **patron;
};

typedef struct tela Tela;

typedef struct Symbol
{
    char *name;
    short type;
    union
    {
        Tela *val;
        double valnum;
        void (*defn)(void);
        Tela (*ptr)(Tela);
        char *str;
    } u;
    struct Symbol *next;
} Symbol;
Symbol *install(), *installnum(), *lookup(), *lookupNum();

typedef union Datum
{
    Tela *val;
    Symbol *sym;
    double dval;
    double valnumD;
} Datum;

typedef void (*Inst)(void);

extern Datum pop();
extern Inst prog[];
extern Inst *progbase;
extern Inst *progp;
extern Inst *code();
extern void pop1();
extern void prexpr();
extern void prstr();
extern void gt();
extern void lt();
extern void eq();
extern void ge();
extern void le();
extern void ne();
extern void and();
extern void or();
extern void not();
extern void ifcode();
extern void whilecode();
extern void forcode();
extern void call();
extern void arg();
extern void argassign();
extern void funcret();
extern void procret();
extern void rotar_();
extern void coser_();
extern void assign(), assignnum(), varpush(), print(), varread(), eval(), constpush(), evalnum(), printnum();
extern void add(), sub(), mul(), divi(), negate(), power(), gtn(), ltn(), eqn(), gen(), len(), nen();
void execute(Inst *p);

Tela *crear(short int x, short int y);
Tela *crearVacia(Tela *t);
void imprime(Tela *t);
Tela *rotar(Tela *t);
Tela *coser(Tela *t1, Tela *t2);
