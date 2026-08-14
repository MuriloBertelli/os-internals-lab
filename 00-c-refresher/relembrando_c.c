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

void limpar_buffer(void);
double *alocar_memoria(size_t n);
int ler_double(double *msg, double *destino);
int ler_int(int *valor);
void registrar(const char *msg, double resultado);
void mostrar_historico(void);
void liberar_historico(void);
void imprimir_polinomio(const double *coef, int grau, const char *rotulo);



int main ()
{
    return 0;
}