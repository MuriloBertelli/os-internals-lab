#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct
{
    char *descricao;
    double resultado;
} registro;

static registro *historico = NULL;
static size_t qtd_regs = 0;
static size_t capacidade = 0;

int main ()
{
    return 0;
}