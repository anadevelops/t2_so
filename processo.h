#ifndef PROCESSO_H
#define PROCESSO_H

#include "memoria.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Tamanho máximo de processo (potência de 2)
#define TAMANHO_MAX_PROCESSO (64 * 1024) // 64 KB

// Número máximo de processos suportados
#define MAX_PROCESSOS 10

// Estrutura para representar uma entrada na tabela de páginas
typedef struct {
    int quadro_fisico;    // Índice do quadro físico onde a página está mapeada
    int presente;         // 1 se a página está presente na memória física, 0 caso contrário
    int modificada;       // 1 se a página foi modificada, 0 caso contrário
} EntradaTabelaPagina;

// Estrutura para representar um processo
typedef struct {
    int id;                                    // ID único do processo
    int tamanho;                              // Tamanho da memória lógica em bytes
    unsigned char memoria_logica[TAMANHO_MAX_PROCESSO]; // Memória lógica do processo
    EntradaTabelaPagina *tabela_paginas;      // Tabela de páginas (alocada dinamicamente)
    int num_paginas;                          // Número de páginas utilizadas pelo processo
    int ativo;                                // 1 se o processo está ativo, 0 caso contrário
} Processo;

// Estrutura para gerenciar todos os processos
typedef struct {
    Processo processos[MAX_PROCESSOS];
    int num_processos;
    int proximo_id;
} GerenciadorProcessos;

// Protótipos das funções de gerenciamento de processos

/**
 * @brief Inicializa o gerenciador de processos.
 * 
 * @param gp Ponteiro para o gerenciador de processos.
 */
void inicializar_gerenciador_processos(GerenciadorProcessos *gp);

/**
 * @brief Cria um novo processo com valores aleatórios na memória lógica.
 * 
 * @param gp Ponteiro para o gerenciador de processos.
 * @param mf Ponteiro para a memória física.
 * @param id_processo ID específico do processo.
 * @param tamanho Tamanho do processo em bytes.
 * @param tamanho_pagina Tamanho da página em bytes.
 * @return ID do processo criado, ou -1 se falhar.
 */
int criar_processo(GerenciadorProcessos *gp, MemoriaFisica *mf, int id_processo, int tamanho, int tamanho_pagina);

/**
 * @brief Aloca quadros físicos para as páginas de um processo.
 * 
 * @param gp Ponteiro para o gerenciador de processos.
 * @param mf Ponteiro para a memória física.
 * @param id_processo ID do processo.
 * @return 1 se sucesso, 0 se falhar.
 */
int alocar_quadros_processo(GerenciadorProcessos *gp, MemoriaFisica *mf, int id_processo);

/**
 * @brief Copia dados da memória lógica para a memória física.
 * 
 * @param gp Ponteiro para o gerenciador de processos.
 * @param mf Ponteiro para a memória física.
 * @param id_processo ID do processo.
 */
void copiar_memoria_logica_para_fisica(GerenciadorProcessos *gp, MemoriaFisica *mf, int id_processo);

/**
 * @brief Visualiza a tabela de páginas de um processo.
 * 
 * @param gp Ponteiro para o gerenciador de processos.
 * @param id_processo ID do processo.
 */
void visualizar_tabela_paginas(GerenciadorProcessos *gp, int id_processo);

/**
 * @brief Encontra um processo pelo ID.
 * 
 * @param gp Ponteiro para o gerenciador de processos.
 * @param id_processo ID do processo.
 * @return Ponteiro para o processo, ou NULL se não encontrado.
 */
Processo* encontrar_processo(GerenciadorProcessos *gp, int id_processo);

/**
 * @brief Remove um processo e libera seus quadros.
 * 
 * @param gp Ponteiro para o gerenciador de processos.
 * @param mf Ponteiro para a memória física.
 * @param id_processo ID do processo.
 * @return 1 se sucesso, 0 se falhar.
 */
int remover_processo(GerenciadorProcessos *gp, MemoriaFisica *mf, int id_processo);

/**
 * @brief Lista todos os processos ativos.
 * 
 * @param gp Ponteiro para o gerenciador de processos.
 */
void listar_processos(GerenciadorProcessos *gp);

/**
 * @brief Gera dados aleatórios para a memória lógica de um processo.
 * 
 * @param processo Ponteiro para o processo.
 */
void gerar_dados_aleatorios_processo(Processo *processo);

/**
 * @brief Calcula o número de páginas necessárias para um tamanho de processo.
 * 
 * @param tamanho Tamanho do processo em bytes.
 * @param tamanho_pagina Tamanho da página em bytes.
 * @return Número de páginas necessárias.
 */
int calcular_num_paginas(int tamanho, int tamanho_pagina);

/**
 * @brief Libera a memória alocada para um processo.
 * 
 * @param processo Ponteiro para o processo.
 */
void liberar_processo(Processo *processo);

#endif // PROCESSO_H 