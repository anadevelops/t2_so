# Documentação - Processos e Tabelas de Páginas

## Visão Geral

Esta implementação simula o gerenciamento de memória por paginação, onde cada processo possui sua própria memória lógica e uma tabela de páginas que mapeia páginas lógicas para quadros físicos na memória.

## Estruturas Principais

### EntradaTabelaPagina
```c
typedef struct {
    int quadro_fisico;    // Índice do quadro físico onde a página está mapeada
    int presente;         // 1 se a página está presente na memória física, 0 caso contrário
    int modificada;       // 1 se a página foi modificada, 0 caso contrário
} EntradaTabelaPagina;
```

### Processo
```c
typedef struct {
    int id;                                    // ID único do processo
    int tamanho;                              // Tamanho da memória lógica em bytes
    unsigned char memoria_logica[TAMANHO_MAX_PROCESSO]; // Memória lógica do processo
    EntradaTabelaPagina tabela_paginas[MAX_PAGINAS_PROCESSO]; // Tabela de páginas
    int num_paginas;                          // Número de páginas utilizadas pelo processo
    int ativo;                                // 1 se o processo está ativo, 0 caso contrário
} Processo;
```

### GerenciadorProcessos
```c
typedef struct {
    Processo processos[MAX_PROCESSOS];
    int num_processos;
    int proximo_id;
} GerenciadorProcessos;
```

## Constantes Configuráveis

- `TAMANHO_MAX_PROCESSO`: 64 KB (tamanho máximo de memória lógica por processo)
- `MAX_PAGINAS_PROCESSO`: 16 páginas (64KB / 4KB por página)
- `MAX_PROCESSOS`: 10 processos simultâneos
- `TAMANHO_PAGINA`: 4 KB (definido em memoria.h)

## Funções Principais

### 1. Inicialização
```c
void inicializar_gerenciador_processos(GerenciadorProcessos *gp);
```
- Inicializa o gerenciador de processos
- Define todos os processos como inativos
- Inicializa a tabela de páginas de cada processo

### 2. Criação de Processos
```c
int criar_processo(GerenciadorProcessos *gp, MemoriaFisica *mf, int tamanho);
```
**Parâmetros:**
- `gp`: Ponteiro para o gerenciador de processos
- `mf`: Ponteiro para a memória física
- `tamanho`: Tamanho do processo em bytes (1 a 64KB)

**Retorno:**
- ID do processo criado (sucesso)
- -1 (falha)

**Funcionalidades:**
- Valida o tamanho do processo
- Gera dados aleatórios para a memória lógica
- Aloca quadros físicos necessários
- Copia dados da memória lógica para a física
- Monta a tabela de páginas

### 3. Alocação de Quadros
```c
int alocar_quadros_processo(GerenciadorProcessos *gp, MemoriaFisica *mf, int id_processo);
```
- Aloca quadros físicos para todas as páginas de um processo
- Atualiza a tabela de páginas com os quadros alocados
- Em caso de falha, libera os quadros já alocados

### 4. Cópia de Memória
```c
void copiar_memoria_logica_para_fisica(GerenciadorProcessos *gp, MemoriaFisica *mf, int id_processo);
```
- Copia os dados da memória lógica do processo para os quadros físicos alocados
- Calcula automaticamente quantos bytes copiar por página

### 5. Visualização
```c
void visualizar_tabela_paginas(GerenciadorProcessos *gp, int id_processo);
```
- Mostra a tabela de páginas de um processo específico
- Exibe estatísticas de páginas presentes e modificadas
- Formato tabular com informações detalhadas

### 6. Gerenciamento
```c
Processo* encontrar_processo(GerenciadorProcessos *gp, int id_processo);
int remover_processo(GerenciadorProcessos *gp, MemoriaFisica *mf, int id_processo);
void listar_processos(GerenciadorProcessos *gp);
```

## Exemplo de Uso

```c
#include "processo.h"

int main() {
    // Inicializa memória física e gerenciador de processos
    MemoriaFisica mf;
    GerenciadorProcessos gp;
    
    inicializar_memoria_fisica(&mf);
    inicializar_gerenciador_processos(&gp);
    
    // Cria processos
    int p1 = criar_processo(&gp, &mf, 8192);   // 8KB
    int p2 = criar_processo(&gp, &mf, 16384);  // 16KB
    
    // Lista processos
    listar_processos(&gp);
    
    // Visualiza tabela de páginas
    visualizar_tabela_paginas(&gp, p1);
    
    // Remove processo
    remover_processo(&gp, &mf, p2);
    
    return 0;
}
```

## Características da Implementação

### ✅ Funcionalidades Implementadas
- ✅ Criação de processos com tamanhos variáveis
- ✅ Geração automática de dados aleatórios
- ✅ Alocação dinâmica de quadros físicos
- ✅ Tabela de páginas completa com flags de controle
- ✅ Cópia automática de memória lógica para física
- ✅ Visualização detalhada de tabelas de páginas
- ✅ Remoção de processos com liberação de recursos
- ✅ Listagem de processos ativos
- ✅ Tratamento de erros robusto

### 🔧 Recursos Técnicos
- **Paginação**: Cada processo é dividido em páginas de 4KB
- **Mapeamento**: Tabela de páginas mapeia páginas lógicas → quadros físicos
- **Controle**: Flags para páginas presentes e modificadas
- **Memória**: Memória lógica separada para cada processo
- **Alocação**: Alocação dinâmica de quadros conforme necessário

### 📊 Estatísticas Disponíveis
- Número de páginas por processo
- Percentual de páginas presentes na memória
- Percentual de páginas modificadas
- Utilização de memória física
- Número de processos ativos

## Integração com o Sistema

Esta implementação se integra perfeitamente com:
- **memoria.h/memoria.c**: Gerencia a memória física
- **main.c**: Interface do usuário e menu principal

As funções estão prontas para serem usadas pela pessoa responsável pela interface (Giulia) para implementar o menu completo do sistema.

## Testes Realizados

O arquivo `teste_processo.c` demonstra:
- ✅ Criação de múltiplos processos
- ✅ Alocação correta de quadros
- ✅ Visualização de tabelas de páginas
- ✅ Remoção de processos
- ✅ Liberação de recursos
- ✅ Tratamento de erros
- ✅ Validação de parâmetros

## Próximos Passos

Para a integração final:
1. Incluir `processo.h` no `main.c`
2. Adicionar opções no menu para criar e visualizar processos
3. Integrar com as funções de visualização de memória
4. Implementar tratamento de erros no menu principal 