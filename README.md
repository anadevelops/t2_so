# Trabalho 2: Gerenciamento de Memória com Paginação

## Como compilar

No terminal, execute:

```
gcc -Wall -Wextra -o t2_so main.c memoria.c processo.c
```

## Como executar

Após compilar, execute o programa com:

```
./t2_so
```

Esses comandos devem ser executados na raiz do projeto.

## Casos de teste
**Tamanho da memória:** 32 (mb)
**Tamanho da página/quadro:** 4096 (kb)
**Tamanho máx. de processo:** 32000000 (b)

São criados 8 quadros. Criando um processo com ID 1 e tamanho 16000000 (b) são ocupados 4 quadros.

Criando outro processo, com ID 2 e tamanho 12000000 (b) são ocupados 3 quadros.

Tentando criar um novo processo, com ID 3 e tamanho 12000000 (b), o programa identifica que serão necessários 3 quadros, mas ao tentar alocar verifica que há somente um quadro livre. O programa então retorna erros informando que não há quadros livres, portanto não foi possível criar o processo.

