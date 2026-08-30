/*
 * Nome: Murilo Otavio Bertelli Cristiano
 * Curso: Engenharia de Computacao
 *
 * Atividade: Monitor de dados vitais utilizando processos no Linux.
 *
 * O programa cria tres processos filhos:
 * 1. Monitor de batimentos cardiacos;
 * 2. Monitor de saturacao de oxigenio;
 * 3. Monitor de temperatura corporal.
 */

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>


static int converter_iteracoes(const char *texto, int *resultado)
{
    char *fim;
    long valor;

    errno = 0;
    valor = strtol(texto, &fim, 10);

    if (fim == texto ||
        errno == ERANGE ||
        valor <= 0 ||
        valor > INT_MAX) {
        return 0;
    }

    while (isspace((unsigned char)*fim)) {
        fim++;
    }

    if (*fim != '\0') {
        return 0;
    }

    *resultado = (int)valor;
    return 1;
}

static int obter_iteracoes(int argc, char *argv[])
{
    int iteracoes;
    char entrada[100];

    if (argc >= 2) {
        if (!converter_iteracoes(argv[1], &iteracoes)) {
            fprintf(
                stderr,
                "Erro: a quantidade de iteracoes deve ser "
                "um numero inteiro positivo.\n"
            );

            exit(EXIT_FAILURE);
        }

        return iteracoes;
    }

    printf("O parametro com a quantidade de iteracoes e obrigatorio.\n");
    printf("Informe a quantidade de iteracoes: ");
    fflush(stdout);

    if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
        fprintf(stderr, "Erro ao ler a quantidade de iteracoes.\n");
        exit(EXIT_FAILURE);
    }

    if (!converter_iteracoes(entrada, &iteracoes)) {
        fprintf(
            stderr,
            "Erro: a quantidade de iteracoes deve ser "
            "um numero inteiro positivo.\n"
        );

        exit(EXIT_FAILURE);
    }

    return iteracoes;
}

static void preparar_gerador_aleatorio(void)
{
    unsigned int semente;

    semente = (unsigned int)time(NULL) ^ (unsigned int)getpid();
    srand(semente);
}

static int gerar_inteiro_aleatorio(int minimo, int maximo)
{
    return minimo + rand() % (maximo - minimo + 1);
}

static void monitorar_batimentos(int iteracoes)
{
    int leitura;

    preparar_gerador_aleatorio();

    for (leitura = 1; leitura <= iteracoes; leitura++) {
        int batimentos;
        const char *situacao;

        batimentos = gerar_inteiro_aleatorio(20, 140);

        if (batimentos < 50) {
            situacao = "ALERTA: batimento cardiaco baixo";
        } else if (batimentos > 90) {
            situacao = "ALERTA: batimento cardiaco alto";
        } else {
            situacao = "batimento cardiaco normal";
        }

        printf(
            "[BATIMENTOS | PID %ld | leitura %d/%d] "
            "%d bpm - %s\n",
            (long)getpid(),
            leitura,
            iteracoes,
            batimentos,
            situacao
        );

        fflush(stdout);

        if (leitura < iteracoes) {
            sleep(1);
        }
    }
}

static void monitorar_saturacao(int iteracoes)
{
    int leitura;

    preparar_gerador_aleatorio();

    for (leitura = 1; leitura <= iteracoes; leitura++) {
        int saturacao;
        const char *situacao;

        saturacao = gerar_inteiro_aleatorio(80, 100);

        if (saturacao < 90) {
            situacao = "ALERTA: saturacao baixa";
        } else {
            situacao = "saturacao normal";
        }

        printf(
            "[SATURACAO | PID %ld | leitura %d/%d] "
            "%d%% - %s\n",
            (long)getpid(),
            leitura,
            iteracoes,
            saturacao,
            situacao
        );

        fflush(stdout);

        if (leitura < iteracoes) {
            sleep(1);
        }
    }
}

static void monitorar_temperatura(int iteracoes)
{
    int leitura;

    preparar_gerador_aleatorio();

    for (leitura = 1; leitura <= iteracoes; leitura++) {
        int temperatura_decimos;
        double temperatura;
        const char *situacao;

        temperatura_decimos = gerar_inteiro_aleatorio(340, 410);
        temperatura = temperatura_decimos / 10.0;

        if (temperatura_decimos < 350) {
            situacao = "ALERTA: hipotermia";
        } else if (temperatura_decimos > 375) {
            situacao = "ALERTA: febre";
        } else {
            situacao = "temperatura normal";
        }

        printf(
            "[TEMPERATURA | PID %ld | leitura %d/%d] "
            "%.1f graus Celsius - %s\n",
            (long)getpid(),
            leitura,
            iteracoes,
            temperatura,
            situacao
        );

        fflush(stdout);

        if (leitura < iteracoes) {
            sleep(1);
        }
    }
}

int main(int argc, char *argv[])
{
    int iteracoes;
    int status;
    int ocorreu_erro;
    int indice;

    pid_t pid_batimentos;
    pid_t pid_saturacao;
    pid_t pid_temperatura;

    pid_t filhos[3];
    const char *nomes[3] = {
        "batimentos",
        "saturacao",
        "temperatura"
    };

    iteracoes = obter_iteracoes(argc, argv);

    setvbuf(stdout, NULL, _IOLBF, 0);

    printf(
        "Processo pai PID %ld: iniciando os monitores "
        "com %d iteracoes.\n",
        (long)getpid(),
        iteracoes
    );

    pid_batimentos = fork();

    if (pid_batimentos < 0) {

        perror("Erro ao criar o processo de batimentos");
        return EXIT_FAILURE;
    }

    if (pid_batimentos == 0) {

        monitorar_batimentos(iteracoes);

        exit(0);
    }

    pid_saturacao = fork();

    if (pid_saturacao < 0) {
        perror("Erro ao criar o processo de saturacao");

        waitpid(pid_batimentos, NULL, 0);
        return EXIT_FAILURE;
    }

    if (pid_saturacao == 0) {

        monitorar_saturacao(iteracoes);
        exit(0);
    }

    pid_temperatura = fork();

    if (pid_temperatura < 0) {
        perror("Erro ao criar o processo de temperatura");

        waitpid(pid_batimentos, NULL, 0);
        waitpid(pid_saturacao, NULL, 0);

        return EXIT_FAILURE;
    }

    if (pid_temperatura == 0) {

        monitorar_temperatura(iteracoes);
        exit(0);
    }

    filhos[0] = pid_batimentos;
    filhos[1] = pid_saturacao;
    filhos[2] = pid_temperatura;

    ocorreu_erro = 0;

    for (indice = 0; indice < 3; indice++) {
        if (waitpid(filhos[indice], &status, 0) == -1) {
            perror("Erro em waitpid");
            ocorreu_erro = 1;
            continue;
        }

        if (WIFEXITED(status)) {
            printf(
                "Processo pai: filho de %s, PID %ld, "
                "terminou com codigo %d.\n",
                nomes[indice],
                (long)filhos[indice],
                WEXITSTATUS(status)
            );
        } else {
            printf(
                "Processo pai: filho de %s, PID %ld, "
                "nao terminou normalmente.\n",
                nomes[indice],
                (long)filhos[indice]
            );
        }
    }

    printf("Processo pai: todos os monitores foram finalizados.\n");

    if (ocorreu_erro) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}