#include "memoria.h"
#include <string.h> // Para memset

MemoriaFisica* criar_memoria_fisica(int tamanho_mb, int tamanho_pagina_kb) {
    // Converte megabytes para bytes
    int tamanho_bytes = tamanho_mb * 1024 * 1024;
    // Converte kilobytes para bytes
    int tamanho_pagina_bytes = tamanho_pagina_kb * 1024;
    int numero_quadros = tamanho_bytes / tamanho_pagina_bytes;
    
    // Validação dos parâmetros
    if (tamanho_pagina_kb <= 0) {
        printf("Erro: Tamanho da pagina deve ser maior que zero.\n");
        return NULL;
    }
    
    if (numero_quadros <= 0) {
        printf("Erro: Memoria muito pequena para o tamanho de pagina especificado.\n");
        return NULL;
    }
    
    // Aloca a estrutura da memória física
    MemoriaFisica *mf = (MemoriaFisica*)malloc(sizeof(MemoriaFisica));
    if (!mf) {
        printf("Erro: Falha ao alocar estrutura da memoria fisica.\n");
        return NULL;
    }
    
    // Aloca a memória física
    mf->memoria = (unsigned char*)malloc(tamanho_bytes);
    if (!mf->memoria) {
        printf("Erro: Falha ao alocar memoria fisica de %d MB.\n", tamanho_mb);
        free(mf);
        return NULL;
    }
    
    // Aloca o mapa de quadros livres
    mf->quadro_livre = (int*)malloc(numero_quadros * sizeof(int));
    if (!mf->quadro_livre) {
        printf("Erro: Falha ao alocar mapa de quadros livres.\n");
        free(mf->memoria);
        free(mf);
        return NULL;
    }
    
    // Inicializa os campos da estrutura
    mf->tamanho_memoria = tamanho_bytes;
    mf->tamanho_pagina = tamanho_pagina_bytes;
    mf->numero_quadros = numero_quadros;
    
    printf("Memoria fisica criada com sucesso:\n");
    printf("- Tamanho: %d MB (%d bytes)\n", tamanho_mb, tamanho_bytes);
    printf("- Tamanho da pagina: %d KB (%d bytes)\n", tamanho_pagina_kb, tamanho_pagina_bytes);
    printf("- Numero de quadros: %d\n", numero_quadros);
    
    return mf;
}

void inicializar_memoria_fisica(MemoriaFisica *mf) {
    if (!mf || !mf->memoria || !mf->quadro_livre) {
        printf("Erro: Memoria fisica nao foi criada corretamente.\n");
        return;
    }
    
    // Inicializa toda a memória física com zeros (opcional, mas boa prática)
    memset(mf->memoria, 0, mf->tamanho_memoria);

    // Marca todos os quadros como livres (1)
    for (int i = 0; i < mf->numero_quadros; i++) {
        mf->quadro_livre[i] = 1;
    }
    printf("Memoria fisica inicializada. Total de quadros: %d\n", mf->numero_quadros);
}

void destruir_memoria_fisica(MemoriaFisica *mf) {
    if (mf) {
        if (mf->memoria) {
            free(mf->memoria);
        }
        if (mf->quadro_livre) {
            free(mf->quadro_livre);
        }
        free(mf);
        printf("Memoria fisica liberada com sucesso.\n");
    }
}

int alocar_quadro(MemoriaFisica *mf) {
    if (!mf || !mf->quadro_livre) {
        printf("Erro: Memoria fisica nao foi criada corretamente.\n");
        return -1;
    }
    
    // Procura por um quadro livre
    for (int i = 0; i < mf->numero_quadros; i++) {
        if (mf->quadro_livre[i] == 1) {
            mf->quadro_livre[i] = 0; // Marca como ocupado
            printf("Quadro %d alocado.\n", i);
            return i; // Retorna o índice do quadro alocado
        }
    }
    printf("Erro: Nao ha quadros livres na memoria fisica.\n");
    return -1; // Retorna -1 se não houver quadros livres
}

void liberar_quadro(MemoriaFisica *mf, int quadro_index) {
    if (!mf || !mf->quadro_livre) {
        printf("Erro: Memoria fisica nao foi criada corretamente.\n");
        return;
    }
    
    if (quadro_index >= 0 && quadro_index < mf->numero_quadros) {
        if (mf->quadro_livre[quadro_index] == 0) {
            mf->quadro_livre[quadro_index] = 1; // Marca como livre
            printf("Quadro %d liberado.\n", quadro_index);
        } else {
            printf("Atencao: Tentativa de liberar um quadro que ja estava livre (quadro %d).\n", quadro_index);
        }
    } else {
        printf("Erro: Indice de quadro invalido para liberacao (%d).\n", quadro_index);
    }
}

void escrever_na_memoria(MemoriaFisica *mf, int endereco_fisico, unsigned char valor) {
    if (!mf || !mf->memoria) {
        printf("Erro: Memoria fisica nao foi criada corretamente.\n");
        return;
    }
    
    if (endereco_fisico >= 0 && endereco_fisico < mf->tamanho_memoria) {
        mf->memoria[endereco_fisico] = valor;
    } else {
        printf("Erro: Tentativa de escrita em endereco fisico invalido (%d).\n", endereco_fisico);
    }
}

unsigned char ler_da_memoria(MemoriaFisica *mf, int endereco_fisico) {
    if (!mf || !mf->memoria) {
        printf("Erro: Memoria fisica nao foi criada corretamente.\n");
        return 0;
    }
    
    if (endereco_fisico >= 0 && endereco_fisico < mf->tamanho_memoria) {
        return mf->memoria[endereco_fisico];
    } else {
        printf("Erro: Tentativa de leitura de endereco fisico invalido (%d).\n", endereco_fisico);
        return 0; // Retorna 0 em caso de erro
    }
}

void exibir_status_memoria(MemoriaFisica *mf) {
    if (!mf || !mf->quadro_livre) {
        printf("Erro: Memoria fisica nao foi criada corretamente.\n");
        return;
    }
    
    int livres = 0, ocupados = 0;
    for (int i = 0; i < mf->numero_quadros; i++) {
        if (mf->quadro_livre[i]) livres++;
        else ocupados++;
    }
    
    printf("\n=== Status da Memoria Fisica ===\n");
    printf("Tamanho total: %d MB (%d bytes)\n", mf->tamanho_memoria / (1024 * 1024), mf->tamanho_memoria);
    printf("Tamanho da pagina: %d KB (%d bytes)\n", mf->tamanho_pagina / 1024, mf->tamanho_pagina);
    printf("Numero total de quadros: %d\n", mf->numero_quadros);
    printf("Quadros livres: %d\n", livres);
    printf("Quadros ocupados: %d\n", ocupados);
    printf("Taxa de utilizacao: %.2f%%\n", (float)ocupados / mf->numero_quadros * 100);
    printf("================================\n\n");
}

void visualizar_quadros_memoria(MemoriaFisica *mf, int quadros_por_linha) {
    if (!mf || !mf->quadro_livre) {
        printf("Erro: Memoria fisica nao foi criada corretamente.\n");
        return;
    }
    
    if (quadros_por_linha <= 0) {
        quadros_por_linha = 64; // Valor padrão para visualização em 0s e 1s
    }
    
    printf("\n=== VISUALIZACAO DOS QUADROS DE MEMORIA ===\n");
    printf("Legenda: 0 = Livre, 1 = Ocupado\n");
    printf("Quadros por linha: %d\n\n", quadros_por_linha);
    
    int num_linhas = (mf->numero_quadros + quadros_por_linha - 1) / quadros_por_linha;
    
    for (int linha = 0; linha < num_linhas; linha++) {
        // Imprime o número da linha
        printf("%4d: ", linha * quadros_por_linha);
        
        // Imprime os quadros desta linha
        for (int col = 0; col < quadros_por_linha; col++) {
            int quadro_index = linha * quadros_por_linha + col;
            
            if (quadro_index < mf->numero_quadros) {
                printf("%d", mf->quadro_livre[quadro_index] ? 0 : 1);
            } else {
                printf(" "); // Espaço vazio para completar a linha
            }
            
            // Adiciona um espaço a cada 8 quadros para melhor visualização
            if ((col + 1) % 8 == 0) {
                printf(" ");
            }
        }
        printf("\n");
    }
    
    // Estatísticas visuais
    int livres = 0, ocupados = 0;
    for (int i = 0; i < mf->numero_quadros; i++) {
        if (mf->quadro_livre[i]) livres++;
        else ocupados++;
    }
    
    printf("\nResumo:\n");
    printf("Quadros livres (0): %d\n", livres);
    printf("Quadros ocupados (1): %d\n", ocupados);
    printf("Taxa de utilizacao: %.1f%%\n", (float)ocupados / mf->numero_quadros * 100);
    printf("==========================================\n\n");
} 