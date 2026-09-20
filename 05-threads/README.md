# Atividade II — Produtor e consumidor

Implementação do enunciado `Atividade II - SC.docx`, de 18/09/2026,
do professor Jhonatan Geremias. O código está em `simpson.c`, em C17 com
POSIX threads (`pthread`).
A pasta `05-threads`.

## Compilar e executar

No Linux, a partir da raiz do projeto:

```bash
make 05-threads
./bin/05-threads/simpson
```

Ou, dentro desta pasta, diretamente com GCC:

```bash
gcc -std=c17 -Wall -Wextra -pthread simpson.c -o simpson
./simpson
```

No PowerShell do Windows, com MinGW/GCC que tenha suporte a POSIX threads:

```powershell
gcc -std=c17 -Wall -Wextra -pthread simpson.c -o simpson.exe
.\simpson.exe
```

Informe dois inteiros positivos: a capacidade da lata e o total de biscoitos.
Por exemplo, capacidade `3` e total `10`. Entradas inválidas são solicitadas
novamente. Se a entrada for encerrada ou a alocação falhar, o programa informa
o erro e termina.

## Como funciona

- A thread de Marge produz e coloca exatamente o total informado de biscoitos.
- A thread de Homer retira e consome exatamente esse mesmo total.
- Cada biscoito é representado por um número sequencial. A lata é um buffer
  circular que mantém a ordem de produção, reutilizando as posições liberadas.
- O buffer é alocado com `malloc`, com verificação do retorno e proteção contra
  overflow no cálculo do tamanho em bytes.
- Um `pthread_mutex_t` protege o buffer, seus índices e a quantidade ocupada.
- Duas variáveis de condição (`pthread_cond_t`) fazem Marge aguardar quando a lata está cheia
  e Homer aguardar quando está vazia. A espera libera o mutex, permitindo que
  a outra thread trabalhe. As condições são reavaliadas em laços `while`.
- `pthread_create` cria as duas threads, e `pthread_join` aguarda ambas.
  Somente depois o programa destrói os objetos de sincronização e libera o buffer
  com `free`. Também há encerramento seguro se a criação de uma thread falhar.

A ordem das mensagens pode variar entre execuções, pois depende do escalonamento
das threads. A ocupação deve permanecer entre zero e a capacidade informada,
e a lata deve terminar vazia. Não há espera ocupada nem necessidade de `sleep`.

## Entrega

O enunciado exige **um único arquivo, sem compactação**, com o nome e o curso
no comentário do cabeçalho. Conforme solicitado pelo professor, a cópia
`simpson.txt` contém exatamente o código de `simpson.c` e está pronta para
ser enviada ao Canvas. O prazo indicado no documento é **26/09/2026**.

O nome no cabeçalho foi aproveitado do exercício `02-processes/monitor.c`:
Murilo Otavio Bertelli Cristiano. Se a atividade for entregue em dupla,
acrescente também o nome do segundo estudante ao cabeçalho.

Após qualquer alteração no código, atualize a cópia de entrega:

```powershell
Copy-Item simpson.c simpson.txt
```

No Linux, use `cp simpson.c simpson.txt`. Para compilar o arquivo recebido
como `.txt` sem renomeá-lo, o GCC aceita `-x c` antes do nome do arquivo.
