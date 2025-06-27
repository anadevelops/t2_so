#include "memoria.h"
#include "processo.h"

void exibir_menu() {
    printf("\n=== MENU PRINCIPAL ===\n");
    printf("1. Visualizar memoria\n");
    printf("2. Criar processo\n");
    printf("3. Visualizar tabela de paginas\n");
    printf("0. Sair\n");
    printf("Escolha uma opcao: ");
}

void visualizar_memoria_completa(MemoriaFisica *mf) {
    if (!mf) {
        printf("Erro: Memoria fisica nao foi criada.\n");
        return;
    }
    
    // Calcula o percentual de memória livre
    int quadros_livres = 0;
    for (int i = 0; i < mf->numero_quadros; i++) {
        if (mf->quadro_livre[i]) {
            quadros_livres++;
        }
    }
    
    float percentual_livre = (float)quadros_livres / mf->numero_quadros * 100;
    
    printf("\n=== VISUALIZACAO DA MEMORIA FISICA ===\n");
    printf("Percentual de memoria livre: %.2f%%\n", percentual_livre);
    printf("Quadros livres: %d / %d\n", quadros_livres, mf->numero_quadros);
    printf("Tamanho total: %d MB (%d bytes)\n", mf->tamanho_memoria / (1024 * 1024), mf->tamanho_memoria);
    printf("Tamanho da pagina: %d KB (%d bytes)\n", mf->tamanho_pagina / 1024, mf->tamanho_pagina);
    printf("\nEstado dos quadros (0=Livre, 1=Ocupado):\n");
    
    // Visualização dos quadros
    int quadros_por_linha = 64;
    int num_linhas = (mf->numero_quadros + quadros_por_linha - 1) / quadros_por_linha;
    
    for (int linha = 0; linha < num_linhas; linha++) {
        printf("%4d: ", linha * quadros_por_linha);
        
        for (int col = 0; col < quadros_por_linha; col++) {
            int quadro_index = linha * quadros_por_linha + col;
            
            if (quadro_index < mf->numero_quadros) {
                printf("%d", mf->quadro_livre[quadro_index] ? 0 : 1);
            } else {
                printf(" ");
            }
            
            if ((col + 1) % 8 == 0) {
                printf(" ");
            }
        }
        printf("\n");
    }
    
    printf("==========================================\n\n");
}

void criar_processo_interativo(GerenciadorProcessos *gp, MemoriaFisica *mf) {
    int id_processo, tamanho_processo;
    
    printf("\n=== CRIAR PROCESSO ===\n");
    
    // Solicita ID do processo
    printf("Digite o ID do processo: ");
    scanf("%d", &id_processo);
    
    // Solicita tamanho do processo
    do {
        printf("Digite o tamanho do processo em bytes: ");
        scanf("%d", &tamanho_processo);
        
        if (tamanho_processo > TAMANHO_MAX_PROCESSO) {
            printf("Erro: Tamanho maior que o maximo permitido (%d bytes).\n", TAMANHO_MAX_PROCESSO);
            printf("Digite um valor menor ou igual a %d bytes.\n", TAMANHO_MAX_PROCESSO);
        }
    } while (tamanho_processo > TAMANHO_MAX_PROCESSO);
    
    if (tamanho_processo <= 0) {
        printf("Erro: Tamanho deve ser maior que zero.\n");
        return;
    }
    
    // Cria o processo
    int resultado = criar_processo(gp, mf, id_processo, tamanho_processo, mf->tamanho_pagina);
    
    if (resultado == -1) {
        printf("Erro: Nao foi possivel criar o processo. Memoria insuficiente ou erro interno.\n");
    } else {
        printf("Processo %d criado com sucesso!\n", id_processo);
    }
}

void visualizar_tabela_paginas_interativo(GerenciadorProcessos *gp) {
    int id_processo;
    
    printf("\n=== VISUALIZAR TABELA DE PAGINAS ===\n");
    printf("Digite o ID do processo: ");
    scanf("%d", &id_processo);
    
    Processo *processo = encontrar_processo(gp, id_processo);
    if (!processo) {
        printf("Erro: Processo com ID %d nao encontrado.\n", id_processo);
        return;
    }
    
    visualizar_tabela_paginas(gp, id_processo);
}

int main() {
    int tamanho_mb, tamanho_pagina_kb;
    
    printf("=== Simulador de Gerenciamento de Memoria ===\n");
    printf("Digite o tamanho da memoria fisica em MB: ");
    scanf("%d", &tamanho_mb);
    
    printf("Digite o tamanho da pagina/quadro em KB: ");
    scanf("%d", &tamanho_pagina_kb);
    
    // Validação básica dos parâmetros
    if (tamanho_mb <= 0) {
        printf("Erro: Tamanho da memoria deve ser maior que zero.\n");
        return 1;
    }
    
    if (tamanho_pagina_kb <= 0) {
        printf("Erro: Tamanho da pagina deve ser maior que zero.\n");
        return 1;
    }
    
    // Cria a memória física com o tamanho especificado
    MemoriaFisica *mf = criar_memoria_fisica(tamanho_mb, tamanho_pagina_kb);
    if (!mf) {
        printf("Erro: Falha ao criar memoria fisica.\n");
        return 1;
    }
    
    // Inicializa a memória
    inicializar_memoria_fisica(mf);
    
    // Inicializa o gerenciador de processos
    GerenciadorProcessos gp;
    inicializar_gerenciador_processos(&gp);
    
    // Menu interativo
    int opcao;
    do {
        exibir_menu();
        scanf("%d", &opcao);
        
        switch (opcao) {
            case 1:
                visualizar_memoria_completa(mf);
                break;
                
            case 2:
                criar_processo_interativo(&gp, mf);
                break;
                
            case 3:
                visualizar_tabela_paginas_interativo(&gp);
                break;
                
            case 0:
                printf("Saindo do programa...\n");
                break;
                
            default:
                printf("Opcao invalida! Tente novamente.\n");
                break;
        }
        
    } while (opcao != 0);
    
    // Libera a memória
    destruir_memoria_fisica(mf);
    
    return 0;
}
