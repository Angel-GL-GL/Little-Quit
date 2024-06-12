#include <stdio.h>
#include <malloc.h>
#include "hoc.h" 

Tela *crear(short int x, short int y){
    short int i = 0;
    Tela *t = (Tela *)malloc(sizeof(Tela));
    t->altura = y;
    t->longitud = x;
    t->patron = (unsigned char **)calloc(y, sizeof(unsigned char *));
    for (i = 0; i < y; i++)
        *(t->patron + i) = (unsigned char *)malloc(sizeof(unsigned char) * x);
    return t;
}

Tela *crearVacia(Tela *t){
    short int i = 0, j= 0;
    for (i = 0; i < t->altura; i++) for (j = 0; j < t->longitud; j++)t->patron[i][j] = ' ';
    return t;
}

void imprime(Tela *t)
{
    short int i = 0, j = 0;
    if (t == NULL)
        printf("No hay\n");
    else
        for (i = 0; i < t->altura; i++)
        {
            for (j = 0; j < t->longitud; j++)
                printf("%c ", t->patron[i][j]);
            printf("\n");
        };
    printf("\n");
}

Tela *rotar(Tela *t)
{
    Tela *nueva = crear(t->altura, t->longitud);
    short int i = 0, j = 0, auxX = 0, auxY = t->altura - 1;
    for (i = 0; i < t->altura; i++, auxX = 0, auxY--)
        for (j = 0; j < t->longitud; j++, auxX++)
        {

            // printf("old[%d][%d] -> new[%d][%d] = %c\n",i,j,auxX,auxY,t->patron[i][j]);
            if (t->patron[i][j] == '/')
                nueva->patron[auxX][auxY] = '\\';
            else if (t->patron[i][j] == '\\')
                nueva->patron[auxX][auxY] = '/';
            else if (t->patron[i][j] == '|')
                nueva->patron[auxX][auxY] = '-';
            else if (t->patron[i][j] == '-')
                nueva->patron[auxX][auxY] = '|';
            else
                nueva->patron[auxX][auxY] = t->patron[i][j];
        }
    return nueva;
}

Tela *coser(Tela *t1, Tela *t2)
{
    if (t1->altura == t2->altura)
    {
        short int i = 0, j = 0;
        char auxil = ' ';
        Tela *t = crear(t1->longitud + t2->longitud, t1->altura);

        for (i = 0; i < t1->altura; i++)
            for (j = 0; j < t1->longitud + t2->longitud; j++)
            {
                if (j < t1->longitud) auxil = t1->patron[i][j];
                else auxil = t2->patron[i][j - t1->longitud];
                t->patron[i][j] = auxil;
            }

        return t;
    }
    else
        return NULL;
}