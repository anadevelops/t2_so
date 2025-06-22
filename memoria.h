#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdio.h>
#include <stdlib.h>

// Define o tamanho da memória física em bytes
#define TAMANHO_MEMORIA_FISICA (1024 * 1024) // 1 MB

// Define o tamanho de cada página/quadro em bytes
#define TAMANHO_PAGINA 4096 // 4 KB

// Calcula o número total de quadros na memória física
#define NUMERO_QUADROS (TAMANHO_MEMORIA_FISICA / TAMANHO_PAGINA)

// Estrutura para representar a memória física
typedef struct {
    unsigned char memoria[TAMANHO_MEMORIA_FISICA]; // A memória física como um array de bytes
    int quadro_livre[NUMERO_QUADROS];                // Um mapa de bits para rastrear quadros livres/ocupados (1 para livre, 0 para ocupado)
} MemoriaFisica;

// Protótipos das funções de gerenciamento da memória física

/**
 * @brief Inicializa a memória física, marcando todos os quadros como livres.
 * 
 * @param mf Ponteiro para a estrutura da Memória Física.
 */
void inicializar_memoria_fisica(MemoriaFisica *mf);

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

#endif // MEMORIA_H 