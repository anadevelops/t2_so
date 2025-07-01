#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdio.h>
#include <stdlib.h>

// Estrutura para representar a memória física
typedef struct {
    unsigned char *memoria;           // Ponteiro para a memória física alocada dinamicamente
    int *quadro_livre;                // Um mapa de bits para rastrear quadros livres/ocupados (1 para livre, 0 para ocupado)
    int tamanho_memoria;              // Tamanho total da memória em bytes
    int tamanho_pagina;               // Tamanho de cada página/quadro em bytes
    int numero_quadros;               // Número total de quadros na memória física
} MemoriaFisica;

// Funções de gerenciamento da memória física

/**
 * @brief Cria e inicializa a memória física com tamanho e tamanho de quadro especificados pelo usuário.
 * 
 * @param tamanho_mb Tamanho da memória em megabytes.
 * @param tamanho_pagina_kb Tamanho de cada página/quadro em kilobytes.
 * @return Ponteiro para a estrutura da Memória Física alocada, ou NULL em caso de erro.
 */
MemoriaFisica* criar_memoria_fisica(int tamanho_mb, int tamanho_pagina_kb);

/**
 * @brief Inicializa a memória física, marcando todos os quadros como livres.
 * 
 * @param mf Ponteiro para a estrutura da Memória Física.
 */
void inicializar_memoria_fisica(MemoriaFisica *mf);

/**
 * @brief Libera a memória física alocada dinamicamente.
 * 
 * @param mf Ponteiro para a estrutura da Memória Física.
 */
void destruir_memoria_fisica(MemoriaFisica *mf);

/**
 * @brief Aloca um quadro livre na memória física.
 * 
 * @param mf Ponteiro para a estrutura da Memória Física.
 * @return O índice do quadro alocado, ou -1 se não houver quadros livres.
 */
int alocar_quadro(MemoriaFisica *mf);

/**
 * @brief Libera um quadro previamente alocado na memória física.
 * 
 * @param mf Ponteiro para a estrutura da Memória Física.
 * @param quadro_index O índice do quadro a ser liberado.
 */
void liberar_quadro(MemoriaFisica *mf, int quadro_index);

/**
 * @brief Escreve dados em um endereço físico específico.
 * 
 * @param mf Ponteiro para a estrutura da Memória Física.
 * @param endereco_fisico O endereço físico onde escrever.
 * @param valor O byte a ser escrito.
 */
void escrever_na_memoria(MemoriaFisica *mf, int endereco_fisico, unsigned char valor);

/**
 * @brief Lê um byte de um endereço físico específico.
 * 
 * @param mf Ponteiro para a estrutura da Memória Física.
 * @param endereco_fisico O endereço físico de onde ler.
 * @return O byte lido do endereço.
 */
unsigned char ler_da_memoria(MemoriaFisica *mf, int endereco_fisico);

/**
 * @brief Exibe informações sobre o estado atual da memória física.
 * 
 * @param mf Ponteiro para a estrutura da Memória Física.
 */
void exibir_status_memoria(MemoriaFisica *mf);

/**
 * @brief Exibe uma visualização visual dos quadros livres e ocupados.
 * 
 * @param mf Ponteiro para a estrutura da Memória Física.
 * @param quadros_por_linha Número de quadros a serem exibidos por linha (padrão: 32).
 */
void visualizar_quadros_memoria(MemoriaFisica *mf, int quadros_por_linha);

#endif // MEMORIA_H 