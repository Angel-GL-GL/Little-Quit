#include "hoc.h"
#include "y.tab.h"
#include <math.h>

static struct
{
	char *name; /* Palabras clave */
	int kval;
} keywords[] = {
	"rotar", ROTAR,
	"coser", COSER,
	"proc",PROC,
	"func",FUNC,
	"if",IF,
	"else",ELSE,
	"while",WHILE,
	"print",PRINT,
	"for",FOR,
	"read",READ,
    "return",RETURN,
	0,0,
};

static struct
{
	char *name; /* variables clave */
	int kval;
} keyvar[] = {
	"[A]", VARINT,
	"[B]", VARINT,
	"[C]", VARINT,
	"[D]", VARINT,
	"[E]", VARINT,
	"[F]", VARINT,
	"[G]", VARINT,
	"[H]", VARINT,
	"[I]", VARINT,
	"[J]", VARINT,
	"[K]", VARINT,
	"[L]", VARINT,
	"[M]", VARINT,
	"[N]", VARINT,
	"[O]", VARINT,
	"[P]", VARINT,
	"[Q]", VARINT,
	"[R]", VARINT,
	"[S]", VARINT,
	"[T]", VARINT,
	"[U]", VARINT,
	"[V]", VARINT,
	"[W]", VARINT,
	"[X]", VARINT,
	"[Y]", VARINT,
	"[Z]", VARINT,
	0,0,
};

init() {
	short int i;
	double j = 0;
	Symbol *s;
	Tela *A = crear(2, 2), *B = crear(2, 2);

	for (i = 0; keywords[i].name; i++) install(keywords[i].name, keywords[i].kval, crearVacia(crear));
	for (i = 0; keyvar[i].name; i++, j++){ 
		installnum(keyvar[i].name, keyvar[i].kval, j);
	}

    *(*(A->patron)) = '/';
    *(*(A->patron + 1)) = *(*(A->patron) + 1) = *(*(A->patron + 1) + 1) = '*';

    *(*(B->patron)) = *(*(B->patron + 1)) = *(*(B->patron) + 1) = *(*(B->patron + 1) + 1) = '|';

	install("b",VAR,A);
	install("a",VAR,B);
}
