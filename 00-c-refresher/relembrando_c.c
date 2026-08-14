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
double *alocar_doubles(size_t n);
int ler_double(const char *msg, double *destino);
int ler_int(const char *msg, int *destino);
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
            case 1: adicao(); break;
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

void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

double *alocar_doubles(size_t n) {
    double *v = (double *) calloc(n, sizeof(double));
    if (v == NULL) {
        printf("-> ERRO FATAL: memoria insuficiente!\n");
        liberar_historico();
        exit(EXIT_FAILURE);
    }
    return v;
}

int ler_double(const char *msg, double *destino) {
    printf("%s", msg);
    if (scanf("%lf", destino) != 1) {
        limpar_buffer();
        return 0;
    }
    limpar_buffer();
    return 1;
}

int ler_int(const char *msg, int *destino) {
    printf("%s", msg);
    if (scanf("%d", destino) != 1) {
        limpar_buffer();
        return 0;
    }
    limpar_buffer();
    return 1;
}


void registrar(const char *descricao, double resultado) {
    if (qtd_regs == capacidade) {
        size_t nova_cap = (capacidade == 0) ? 4 : capacidade * 2;
        registro *tmp = (registro *) realloc(historico, nova_cap * sizeof(registro));
        if (tmp == NULL) {
            printf("-> AVISO: sem memoria para o historico.\n");
            return;
        }
        historico  = tmp;
        capacidade = nova_cap;
    }

    
    historico[qtd_regs].descricao = (char *) malloc(strlen(descricao) + 1);
    if (historico[qtd_regs].descricao == NULL) {
        printf("-> AVISO: sem memoria para o historico.\n");
        return;
    }
    strcpy(historico[qtd_regs].descricao, descricao);
    historico[qtd_regs].resultado = resultado;
    qtd_regs++;
}

void mostrar_historico(void) {
    printf("\n---------- HISTORICO ----------\n");
    if (qtd_regs == 0) {
        printf("Nenhuma operacao realizada ainda.\n");
    } else {
        for (size_t i = 0; i < qtd_regs; i++) {
            printf("%2zu) %s = %.4lf\n", i + 1, historico[i].descricao,historico[i].resultado);
        }
        printf("(%zu registro(s) | capacidade alocada: %zu)\n", qtd_regs, capacidade);
    }
    printf("-------------------------------\n");
}

void liberar_historico(void) {
    for (size_t i = 0; i < qtd_regs; i++) {
        free(historico[i].descricao);
        historico[i].descricao = NULL;
    }
    free(historico);
    historico  = NULL;
    qtd_regs   = 0;
    capacidade = 0;
}

void imprimir_polinomio(const double *coef, int grau, const char *rotulo) {
    int primeiro = 1;
    printf("%s", rotulo);
    for (int i = grau; i >= 0; i--) {
        if (coef[i] == 0.0) continue;
        if (!primeiro) printf(" + ");
        if (i == 0)      printf("%.2lf", coef[i]);
        else if (i == 1) printf("%.2lf*x", coef[i]);
        else             printf("%.2lf*x^%d", coef[i], i);
        primeiro = 0;
    }
    if (primeiro) printf("0");
    printf("\n");
}

void adicao(void) {
    char buffer[128];
    double *v = alocar_doubles(2);          /* malloc ""Queiro"" rs */

    if (!ler_double("\nDigite o primeiro numero: ", &v[0]) ||
        !ler_double("Digite o segundo numero: ",   &v[1])) {
        printf("-> ERRO: valor invalido!\n");
        free(v);                            /* erro */
        return;
    }

    double r = v[0] + v[1];
    printf("-> Resultado: %.2lf + %.2lf = %.2lf\n", v[0], v[1], r);
    snprintf(buffer, sizeof(buffer), "%.2lf + %.2lf", v[0], v[1]);
    registrar(buffer, r);

    free(v);
    v = NULL;
}

void subtracao(void) {
    char buffer[128];
    double *v = alocar_doubles(2);

    if (!ler_double("\nDigite o numero base: ", &v[0]) ||
        !ler_double("Digite o numero a ser subtraido: ", &v[1])) {
        printf("-> ERRO: valor invalido!\n");
        free(v);
        return;
    }

    double r = v[0] - v[1];
    printf("-> Resultado: %.2lf - %.2lf = %.2lf\n", v[0], v[1], r);
    snprintf(buffer, sizeof(buffer), "%.2lf - %.2lf", v[0], v[1]);
    registrar(buffer, r);

    free(v);
    v = NULL;
}

void multiplicacao(void) {
    char buffer[128];
    double *v = alocar_doubles(2);

    if (!ler_double("\nDigite o primeiro fator: ", &v[0]) ||
        !ler_double("Digite o segundo fator: ",   &v[1])) {
        printf("-> ERRO: valor invalido!\n");
        free(v);
        return;
    }

    double r = v[0] * v[1];
    printf("-> Resultado: %.2lf * %.2lf = %.2lf\n", v[0], v[1], r);
    snprintf(buffer, sizeof(buffer), "%.2lf * %.2lf", v[0], v[1]);
    registrar(buffer, r);

    free(v);
    v = NULL;
}

void divisao(void) {
    char buffer[128];
    double *v = alocar_doubles(2);

    if (!ler_double("\nDigite o numerador: ",   &v[0]) ||
        !ler_double("Digite o denominador: ",   &v[1])) {
        printf("-> ERRO: valor invalido!\n");
        free(v);
        return;
    }

    if (v[1] == 0.0) {                      /* n existe divisao por zero, animal rs, i love u */
        printf("-> ERRO: nao e possivel dividir por zero!\n");
    } else {
        double r = v[0] / v[1];
        printf("-> Resultado: %.2lf / %.2lf = %.2lf\n", v[0], v[1], r);
        snprintf(buffer, sizeof(buffer), "%.2lf / %.2lf", v[0], v[1]);
        registrar(buffer, r);
    }

    free(v);
    v = NULL;
}

void raiz_quadrada(void) {
    char buffer[128];
    double *v = alocar_doubles(1);

    if (!ler_double("\nDigite o numero: ", &v[0])) {
        printf("-> ERRO: valor invalido!\n");
        free(v);
        return;
    }

    if (v[0] < 0.0) {                       /* tratamento de raiz negativa */
        printf("-> ERRO: nao existe raiz quadrada real de numero negativo!\n");
    } else {
        double r = sqrt(v[0]);
        printf("-> Resultado: raiz de %.2lf = %.2lf\n", v[0], r);
        snprintf(buffer, sizeof(buffer), "sqrt(%.2lf)", v[0]);
        registrar(buffer, r);
    }

    free(v);
    v = NULL;
}

void potencia(void) {
    char buffer[128];
    double *v = alocar_doubles(2);

    if (!ler_double("\nDigite a base: ",     &v[0]) ||
        !ler_double("Digite o expoente: ",   &v[1])) {
        printf("-> ERRO: valor invalido!\n");
        free(v);
        return;
    }
    if (v[0] == 0.0 && v[1] < 0.0) {
        printf("-> ERRO: zero elevado a expoente negativo e indefinido!\n");
    } else if (v[0] < 0.0 && v[1] != floor(v[1])) {
        printf("-> ERRO: base negativa com expoente fracionario nao tem resultado real!\n");
    } else {
        double r = pow(v[0], v[1]);
        printf("-> Resultado: %.2lf ^ %.2lf = %.2lf\n", v[0], v[1], r);
        snprintf(buffer, sizeof(buffer), "%.2lf ^ %.2lf", v[0], v[1]);
        registrar(buffer, r);
    }
    free(v);
    v = NULL;
}
void logaritmo(void) {
    char buffer[128];
    double *v = alocar_doubles(1);

    if (!ler_double("\nDigite o numero (log na base 10): ", &v[0])) {
        printf("-> ERRO: valor invalido!\n");
        free(v);
        return;
    }

    if (v[0] <= 0.0) {
        printf("-> ERRO: o logaritmo so e definido para numeros maiores que zero!\n");
    } else {
        double r = log10(v[0]);
        printf("-> Resultado: log10(%.2lf) = %.2lf\n", v[0], r);
        snprintf(buffer, sizeof(buffer), "log10(%.2lf)", v[0]);
        registrar(buffer, r);
    }

    free(v);
    v = NULL;
}

void derivada(void) {
    int grau;

    if (!ler_int("\nDigite o grau do polinomio (0 a 20): ", &grau) ||
        grau < 0 || grau > 20) {
        printf("-> ERRO: grau invalido!\n");
        return;
    }

    double *coef = alocar_doubles((size_t) grau + 1);   /* grau+1 coeficientes */

    for (int i = grau; i >= 0; i--) {
        char msg[64];
        snprintf(msg, sizeof(msg), "Coeficiente de x^%d: ", i);
        if (!ler_double(msg, &coef[i])) {
            printf("-> ERRO: coeficiente invalido!\n");
            free(coef);
            return;
        }
    }

    int grau_d = (grau > 0) ? grau - 1 : 0;
    double *deriv = alocar_doubles((size_t) grau_d + 1);

    for (int i = 1; i <= grau; i++)
        deriv[i - 1] = coef[i] * i;

    printf("\n");
    imprimir_polinomio(coef,  grau,   "   f(x)  = ");
    imprimir_polinomio(deriv, grau_d, "-> f'(x) = ");

    double x;
    if (ler_double("\nAvaliar f'(x) em x = ", &x)) {
        double r = 0.0;
        for (int i = grau_d; i >= 0; i--) r = r * x + deriv[i];
        printf("-> f'(%.2lf) = %.4lf\n", x, r);
    }

    free(coef);
    free(deriv);
    coef = NULL;
    deriv = NULL;
}


void integral(void) {
    int grau;

    if (!ler_int("\nDigite o grau do polinomio (0 a 20): ", &grau) ||
        grau < 0 || grau > 20) {
        printf("-> ERRO: grau invalido!\n");
        return;
    }

    double *coef = alocar_doubles((size_t) grau + 1);

    for (int i = grau; i >= 0; i--) {
        char msg[64];
        snprintf(msg, sizeof(msg), "Coeficiente de x^%d: ", i);
        if (!ler_double(msg, &coef[i])) {
            printf("-> ERRO: coeficiente invalido!\n");
            free(coef);
            return;
        }
    }

    int grau_i = grau + 1;
    double *integ = alocar_doubles((size_t) grau_i + 1);

    for (int i = 0; i <= grau; i++)
        integ[i + 1] = coef[i] / (i + 1);

    printf("\n");
    imprimir_polinomio(coef,  grau,   "   f(x)     = ");
    imprimir_polinomio(integ, grau_i, "-> F(x)     = ");
    printf("   (+ C)\n");

    double a, b;
    if (ler_double("\nLimite inferior a = ", &a) &&
        ler_double("Limite superior b = ",  &b)) {
        double fa = 0.0, fb = 0.0;
        for (int i = grau_i; i >= 0; i--) { fa = fa * a + integ[i];
                                            fb = fb * b + integ[i]; }
        printf("-> Integral de %.2lf a %.2lf = %.4lf\n", a, b, fb - fa);
    }

    free(coef);
    free(integ);
    coef = NULL;
    integ = NULL;
}
