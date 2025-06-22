#include "memoria.h"
#include <string.h> // Para memset

void inicializar_memoria_fisica(MemoriaFisica *mf) {
    // Inicializa toda a memória física com zeros (opcional, mas boa prática)
    memset(mf->memoria, 0, TAMANHO_MEMORIA_FISICA);

    // Marca todos os quadros como livres (1)
    for (int i = 0; i < NUMERO_QUADROS; i++) {
        mf->quadro_livre[i] = 1;
    }
    printf("Memória física inicializada. Total de quadros: %d\n", NUMERO_QUADROS);
}

int alocar_quadro(MemoriaFisica *mf) {
    // Procura por um quadro livre
    for (int i = 0; i < NUMERO_QUADROS; i++) {
        if (mf->quadro_livre[i] == 1) {
            mf->quadro_livre[i] = 0; // Marca como ocupado
            printf("Quadro %d alocado.\n", i);
            return i; // Retorna o índice do quadro alocado
        }
    }
    printf("Erro: Não há quadros livres na memória física.\n");
    return -1; // Retorna -1 se não houver quadros livres
}

void liberar_quadro(MemoriaFisica *mf, int quadro_index) {
    if (quadro_index >= 0 && quadro_index < NUMERO_quadroS) {
        if (mf->quadro_livre[quadro_index] == 0) {
            mf->quadro_livre[quadro_index] = 1; // Marca como livre
            printf("Quadro %d liberado.\n", quadro_index);
        } else {
            printf("Atenção: Tentativa de liberar um quadro que já estava livre (quadro %d).\n", quadro_index);
        }
    } else {
        printf("Erro: Índice de quadro inválido para liberação (%d).\n", quadro_index);
    }
}

void escrever_na_memoria(MemoriaFisica *mf, int endereco_fisico, unsigned char valor) {
    if (endereco_fisico >= 0 && endereco_fisico < TAMANHO_MEMORIA_FISICA) {
        mf->memoria[endereco_fisico] = valor;
    } else {
        printf("Erro: Tentativa de escrita em endereço físico inválido (%d).\n", endereco_fisico);
    }
}

unsigned char ler_da_memoria(MemoriaFisica *mf, int endereco_fisico) {
    if (endereco_fisico >= 0 && endereco_fisico < TAMANHO_MEMORIA_FISICA) {
        return mf->memoria[endereco_fisico];
    } else {
        printf("Erro: Tentativa de leitura de endereço físico inválido (%d).\n", endereco_fisico);
        return 0; // Retorna 0 em caso de erro
    }
} 