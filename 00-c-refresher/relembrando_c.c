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

void adicao (void);
void subtracao (void);
void multiplicacao (void);
void divisao (void);
void raiz_quadrada (void);
void potencia (void);
void logaritmo (void);
void derivada (void);
void integral (void);

int main (void)
{
    int opcao;

    for (;;)
    {
        printf("1. Adicao\n");
        printf("2. Subtracao\n");
        printf("3. Multiplicacao\n");
        printf("4. Divisao\n");
        printf("5. Raiz Quadrada\n");
        printf("6. Potencia\n");
        printf("7. Logaritmo\n");
        printf("8. Derivada\n");
        printf("9. Integral\n");
        printf("10. Historico\n");
        printf("0. Sair\n");

        if (!ler_int("Escolha uma opcao: ", &opcao))
        {
            if (feof(stdin))
            {
                liberar_historico();
                printf("nEntrada encerrada. Memoria liberada. Tchau!\n");
                /*uma global = 0 mesma coisa que return 0, porem mais bonitinho rs*/
                return EXIT_SUCCESS;
            }
            printf("ERRO: entrada invalida. tú és burro!\n");
            continue;
        }

        switch (opcao)
        {
            case 1: edicao(); break;
            case 2: subtracao(); break;
            case 3: multiplicacao(); break;
            case 4: divisao(); break;
            case 5: raiz_quadrada(); break;
            case 6: potencia(); break;
            case 7: logaritmo(); break;
            case 8: derivada(); break;
            case 9: integral(); break;
            case 10: mostrar_historico(); break;
            case 0: liberar_historico(); printf("Tchau!VAI COM DEUS\n"); return EXIT_SUCCESS;
            default: printf("ERRO: TU ES BURRO! ESCOLHE CERTO!!\n");
        }
    }
    return 0;
}