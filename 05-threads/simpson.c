/*
 * Nome: Murilo Otavio Bertelli Cristiano
 * Curso: Engenharia da Computacao
 * Disciplina: Sistemas de Computacao
 * Atividade II: Simpsons - Problema do Produtor e Consumidor
 *
 * Marge produz biscoitos e Homer os consome usando uma lata compartilhada.
 * Cada biscoito e representado por um numero, na ordem de producao.
 * Compilacao: gcc -std=c17 -Wall -Wextra -pthread simpson.c -o simpson
 */

#include <ctype.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t *biscoitos;
    size_t capacidade;
    size_t quantidade;
    size_t entrada;
    size_t saida;
    size_t total;
    int encerrar;

    /* O mesmo mutex protege o buffer, os indices e a quantidade ocupada. */
    pthread_mutex_t mutex;
    pthread_cond_t nao_cheia;
    pthread_cond_t nao_vazia;
} Lata;

/* pthread retorna o codigo do erro diretamente, sem depender de errno. */
static void verificar_pthread(int erro, const char *operacao)
{
    if (erro != 0) {
        fprintf(stderr, "Erro em %s: %s\n", operacao, strerror(erro));
        /* Se a sincronizacao falhar, encerra o processo sem liberar um buffer
         * que ainda possa estar sendo acessado por outra thread. */
        exit(EXIT_FAILURE);
    }
}

/* Le a linha inteira, aceitando inteiros positivos e espacos nas extremidades.
 * O limite evita overflow na conversao e no tamanho da alocacao. */
static int ler_positivo(const char *pergunta, size_t limite, size_t *valor)
{
    for (;;) {
        size_t numero = 0;
        int caractere;
        int tem_digito = 0;
        int espaco_final = 0;
        int valido = 1;
        int tem_entrada = 0;

        printf("%s", pergunta);
        fflush(stdout);
        while ((caractere = getchar()) != '\n' && caractere != EOF) {
            tem_entrada = 1;
            if (isspace((unsigned char)caractere)) {
                if (tem_digito) {
                    espaco_final = 1;
                }
            } else if (caractere >= '0' && caractere <= '9' && !espaco_final) {
                size_t digito = (size_t)(caractere - '0');
                tem_digito = 1;
                if (numero > limite / 10 ||
                    (numero == limite / 10 && digito > limite % 10)) {
                    valido = 0;
                } else if (valido) {
                    numero = numero * 10 + digito;
                }
            } else {
                valido = 0;
            }
        }

        if (ferror(stdin) || (caractere == EOF && !tem_entrada)) {
            fprintf(stderr, "\nErro: entrada encerrada antes de informar os valores.\n");
            return 0;
        }
        if (valido && tem_digito && numero > 0) {
            *valor = numero;
            return 1;
        }
        fprintf(stderr, "Valor invalido. Informe um inteiro entre 1 e %zu.\n", limite);
    }
}

static void *produzir(void *argumento)
{
    Lata *lata = argumento;

    for (size_t i = 0; i < lata->total; ++i) {
        size_t biscoito = i + 1; /* Marge produz o proximo biscoito. */
        verificar_pthread(pthread_mutex_lock(&lata->mutex), "pthread_mutex_lock");

        /* A espera libera o mutex e o readquire ao acordar. O while reavalia
         * a condicao, inclusive quando ocorre um despertar espurio. */
        while (lata->quantidade == lata->capacidade && !lata->encerrar) {
            printf("Marge: lata cheia, aguardando espaco.\n");
            verificar_pthread(pthread_cond_wait(&lata->nao_cheia, &lata->mutex),
                              "pthread_cond_wait");
        }
        if (lata->encerrar) {
            verificar_pthread(pthread_mutex_unlock(&lata->mutex), "pthread_mutex_unlock");
            return NULL;
        }

        lata->biscoitos[lata->entrada] = biscoito;
        lata->entrada = (lata->entrada + 1) % lata->capacidade;
        ++lata->quantidade;
        printf("Marge colocou o biscoito %zu | Lata: %zu/%zu\n",
               biscoito, lata->quantidade, lata->capacidade);

        /* Libera a regiao critica e avisa Homer que existe um biscoito. */
        verificar_pthread(pthread_mutex_unlock(&lata->mutex), "pthread_mutex_unlock");
        verificar_pthread(pthread_cond_signal(&lata->nao_vazia), "pthread_cond_signal");
    }
    return NULL;
}

static void *consumir(void *argumento)
{
    Lata *lata = argumento;

    for (size_t i = 0; i < lata->total; ++i) {
        verificar_pthread(pthread_mutex_lock(&lata->mutex), "pthread_mutex_lock");
        while (lata->quantidade == 0 && !lata->encerrar) {
            printf("Homer: lata vazia, aguardando biscoitos.\n");
            verificar_pthread(pthread_cond_wait(&lata->nao_vazia, &lata->mutex),
                              "pthread_cond_wait");
        }
        if (lata->encerrar) {
            verificar_pthread(pthread_mutex_unlock(&lata->mutex), "pthread_mutex_unlock");
            return NULL;
        }

        /* Buffer circular: o primeiro biscoito a entrar e o primeiro a sair. */
        size_t biscoito = lata->biscoitos[lata->saida];
        lata->saida = (lata->saida + 1) % lata->capacidade;
        --lata->quantidade;
        printf("Homer retirou e consumiu o biscoito %zu | Lata: %zu/%zu\n",
               biscoito, lata->quantidade, lata->capacidade);

        /* As mensagens tambem ficam protegidas pelo mutex para nao se misturarem. */
        verificar_pthread(pthread_mutex_unlock(&lata->mutex), "pthread_mutex_unlock");
        verificar_pthread(pthread_cond_signal(&lata->nao_cheia), "pthread_cond_signal");
    }
    return NULL;
}

/* Chamada somente quando nao ha mais threads acessando a lata. */
static void liberar_lata(Lata *lata)
{
    verificar_pthread(pthread_cond_destroy(&lata->nao_cheia), "pthread_cond_destroy");
    verificar_pthread(pthread_cond_destroy(&lata->nao_vazia), "pthread_cond_destroy");
    verificar_pthread(pthread_mutex_destroy(&lata->mutex), "pthread_mutex_destroy");
    free(lata->biscoitos);
}

int main(void)
{
    /* Os campos nao indicados sao inicializados com zero (ou ponteiro nulo). */
    Lata lata = {
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .nao_cheia = PTHREAD_COND_INITIALIZER,
        .nao_vazia = PTHREAD_COND_INITIALIZER
    };
    pthread_t marge;
    pthread_t homer;
    int erro;

    printf("Simpsons - Produtor e Consumidor\n");
    if (!ler_positivo("Capacidade da lata: ", SIZE_MAX / sizeof(*lata.biscoitos),
                      &lata.capacidade) ||
        !ler_positivo("Quantidade de biscoitos a produzir e consumir: ", SIZE_MAX,
                      &lata.total)) {
        liberar_lata(&lata);
        return EXIT_FAILURE;
    }

    /* Alocacao dinamica com malloc e verificacao do retorno, como no enunciado. */
    lata.biscoitos = malloc(lata.capacidade * sizeof(*lata.biscoitos));
    if (lata.biscoitos == NULL) {
        fprintf(stderr, "Erro: nao foi possivel alocar a memoria da lata.\n");
        liberar_lata(&lata);
        return EXIT_FAILURE;
    }

    printf("\nIniciando Marge e Homer com %zu biscoitos.\n", lata.total);
    erro = pthread_create(&marge, NULL, produzir, &lata);
    if (erro != 0) {
        fprintf(stderr, "Erro ao criar Marge: %s\n", strerror(erro));
        liberar_lata(&lata);
        return EXIT_FAILURE;
    }

    erro = pthread_create(&homer, NULL, consumir, &lata);
    if (erro != 0) {
        /* Se Homer nao iniciar, acorda Marge para ela poder encerrar mesmo
         * que esteja esperando espaco em uma lata cheia. */
        verificar_pthread(pthread_mutex_lock(&lata.mutex), "pthread_mutex_lock");
        lata.encerrar = 1;
        verificar_pthread(pthread_mutex_unlock(&lata.mutex), "pthread_mutex_unlock");
        verificar_pthread(pthread_cond_signal(&lata.nao_cheia), "pthread_cond_signal");
        verificar_pthread(pthread_join(marge, NULL), "pthread_join");
        fprintf(stderr, "Erro ao criar Homer: %s\n", strerror(erro));
        liberar_lata(&lata);
        return EXIT_FAILURE;
    }

    /* As duas threads executam concorrentemente. join aguarda cada uma delas. */
    verificar_pthread(pthread_join(marge, NULL), "pthread_join");
    verificar_pthread(pthread_join(homer, NULL), "pthread_join");

    printf("\nFinalizado: %zu biscoitos produzidos e %zu consumidos. Restam %zu na lata.\n",
           lata.total, lata.total, lata.quantidade);
    liberar_lata(&lata);
    return EXIT_SUCCESS;
}
