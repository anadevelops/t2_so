#include "processo.h"
#include <string.h>

void inicializar_gerenciador_processos(GerenciadorProcessos *gp) {
    gp->num_processos = 0;
    gp->proximo_id = 1;
    
    // Inicializa todos os processos como inativos
    for (int i = 0; i < MAX_PROCESSOS; i++) {
        gp->processos[i].ativo = 0;
        gp->processos[i].id = 0;
        gp->processos[i].tamanho = 0;
        gp->processos[i].num_paginas = 0;
        gp->processos[i].tabela_paginas = NULL;
    }
    
    printf("Gerenciador de processos inicializado.\n");
}

int calcular_num_paginas(int tamanho, int tamanho_pagina) {
    int num_paginas = tamanho / tamanho_pagina;
    if (tamanho % tamanho_pagina != 0) {
        num_paginas++; // Arredonda para cima
    }
    return num_paginas;
}

void gerar_dados_aleatorios_processo(Processo *processo) {
    srand(time(NULL) + processo->id); // Seed baseado no ID do processo
    
    for (int i = 0; i < processo->tamanho; i++) {
        processo->memoria_logica[i] = rand() % 256; // Valores de 0 a 255
    }
    
    printf("Dados aleatorios gerados para o processo %d (%d bytes).\n", 
           processo->id, processo->tamanho);
}

int criar_processo(GerenciadorProcessos *gp, MemoriaFisica *mf, int id_processo, int tamanho, int tamanho_pagina, int tamanho_max_processo) {
    // Verifica se há espaço para mais processos
    if (gp->num_processos >= MAX_PROCESSOS) {
        printf("Erro: Numero maximo de processos atingido (%d).\n", MAX_PROCESSOS);
        return -1;
    }
    
    // Verifica se o ID já existe
    if (encontrar_processo(gp, id_processo)) {
        printf("Erro: Processo com ID %d ja existe.\n", id_processo);
        return -1;
    }
    
    // Verifica se o tamanho é válido
    if (tamanho <= 0 || tamanho > tamanho_max_processo) {
        printf("Erro: Tamanho de processo invalido (%d). Deve estar entre 1 e %d bytes.\n", 
               tamanho, tamanho_max_processo);
        return -1;
    }
    
    // Encontra um slot livre para o processo
    int slot_livre = -1;
    for (int i = 0; i < MAX_PROCESSOS; i++) {
        if (!gp->processos[i].ativo) {
            slot_livre = i;
            break;
        }
    }
    
    if (slot_livre == -1) {
        printf("Erro: Nao foi possivel encontrar um slot livre para o processo.\n");
        return -1;
    }
    
    // Cria o processo
    Processo *processo = &gp->processos[slot_livre];
    processo->id = id_processo;
    processo->tamanho = tamanho;
    processo->num_paginas = calcular_num_paginas(tamanho, tamanho_pagina);
    processo->ativo = 1;
    
    // Aloca a tabela de páginas dinamicamente
    processo->tabela_paginas = (EntradaTabelaPagina*)malloc(processo->num_paginas * sizeof(EntradaTabelaPagina));
    if (!processo->tabela_paginas) {
        printf("Erro: Falha ao alocar tabela de paginas para o processo %d.\n", processo->id);
        processo->ativo = 0;
        return -1;
    }
    
    // Inicializa a tabela de páginas
    for (int i = 0; i < processo->num_paginas; i++) {
        processo->tabela_paginas[i].quadro_fisico = -1;
        processo->tabela_paginas[i].presente = 0;
        processo->tabela_paginas[i].modificada = 0;
    }
    
    // Aloca a memória lógica dinamicamente
    processo->memoria_logica = (unsigned char*)malloc(tamanho);
    if (!processo->memoria_logica) {
        printf("Erro: Falha ao alocar memoria logica para o processo %d.\n", processo->id);
        processo->ativo = 0;
        free(processo->tabela_paginas);
        processo->tabela_paginas = NULL;
        return -1;
    }
    
    // Gera dados aleatórios para a memória lógica
    gerar_dados_aleatorios_processo(processo);
    
    // Aloca quadros físicos para o processo
    if (!alocar_quadros_processo(gp, mf, processo->id)) {
        printf("Erro: Falha ao alocar quadros para o processo %d.\n", processo->id);
        liberar_processo(processo);
        return -1;
    }
    
    // Copia dados da memória lógica para a física
    copiar_memoria_logica_para_fisica(gp, mf, processo->id);
    
    gp->num_processos++;
    
    printf("Processo %d criado com sucesso!\n", processo->id);
    printf("  - Tamanho: %d bytes\n", processo->tamanho);
    printf("  - Paginas: %d\n", processo->num_paginas);
    
    return processo->id;
}

int alocar_quadros_processo(GerenciadorProcessos *gp, MemoriaFisica *mf, int id_processo) {
    Processo *processo = encontrar_processo(gp, id_processo);
    if (!processo) {
        printf("Erro: Processo %d nao encontrado.\n", id_processo);
        return 0;
    }
    
    printf("Alocando %d quadros para o processo %d...\n", processo->num_paginas, id_processo);
    
    for (int i = 0; i < processo->num_paginas; i++) {
        int quadro = alocar_quadro(mf);
        if (quadro == -1) {
            printf("Erro: Nao ha quadros suficientes para alocar a pagina %d do processo %d.\n", 
                   i, id_processo);
            
            // Libera os quadros já alocados
            for (int j = 0; j < i; j++) {
                liberar_quadro(mf, processo->tabela_paginas[j].quadro_fisico);
                processo->tabela_paginas[j].quadro_fisico = -1;
                processo->tabela_paginas[j].presente = 0;
            }
            return 0;
        }
        
        processo->tabela_paginas[i].quadro_fisico = quadro;
        processo->tabela_paginas[i].presente = 1;
        processo->tabela_paginas[i].modificada = 0;
        
        printf("  Pagina %d -> Quadro %d\n", i, quadro);
    }
    
    return 1;
}

void copiar_memoria_logica_para_fisica(GerenciadorProcessos *gp, MemoriaFisica *mf, int id_processo) {
    Processo *processo = encontrar_processo(gp, id_processo);
    if (!processo) {
        printf("Erro: Processo %d nao encontrado.\n", id_processo);
        return;
    }
    
    printf("Copiando dados da memoria logica para a fisica do processo %d...\n", id_processo);
    
    for (int pagina = 0; pagina < processo->num_paginas; pagina++) {
        if (processo->tabela_paginas[pagina].presente) {
            int quadro = processo->tabela_paginas[pagina].quadro_fisico;
            int endereco_fisico_base = quadro * mf->tamanho_pagina;
            int endereco_logico_base = pagina * mf->tamanho_pagina;
            
            // Se o endereço lógico base já está fora do tamanho do processo, não copia nada
            if (endereco_logico_base >= processo->tamanho) {
                continue;
            }
            // Calcula quantos bytes copiar para esta página
            int bytes_para_copiar = mf->tamanho_pagina;
            if (endereco_logico_base + mf->tamanho_pagina > processo->tamanho) {
                bytes_para_copiar = processo->tamanho - endereco_logico_base;
            }
            if (bytes_para_copiar <= 0) {
                continue;
            }
            // Copia os dados
            for (int i = 0; i < bytes_para_copiar; i++) {
                unsigned char valor = processo->memoria_logica[endereco_logico_base + i];
                escrever_na_memoria(mf, endereco_fisico_base + i, valor);
            }
            printf("  Pagina %d: %d bytes copiados para o quadro %d\n", 
                   pagina, bytes_para_copiar, quadro);
        }
    }
}

void visualizar_tabela_paginas(GerenciadorProcessos *gp, int id_processo) {
    Processo *processo = encontrar_processo(gp, id_processo);
    if (!processo) {
        printf("Erro: Processo %d nao encontrado.\n", id_processo);
        return;
    }
    
    printf("\n=== TABELA DE PAGINAS - PROCESSO %d ===\n", id_processo);
    printf("Tamanho do processo: %d bytes\n", processo->tamanho);
    printf("Numero de paginas: %d\n", processo->num_paginas);
    printf("\n");
    printf("Pagina | Quadro Fisico | Presente | Modificada\n");
    printf("-------|---------------|----------|-----------\n");
    
    for (int i = 0; i < processo->num_paginas; i++) {
        EntradaTabelaPagina *entrada = &processo->tabela_paginas[i];
        printf("%6d | %13d | %8s | %10s\n", 
               i,
               entrada->quadro_fisico,
               entrada->presente ? "Sim" : "Nao",
               entrada->modificada ? "Sim" : "Nao");
    }
    
    printf("\n");
}

Processo* encontrar_processo(GerenciadorProcessos *gp, int id_processo) {
    for (int i = 0; i < MAX_PROCESSOS; i++) {
        if (gp->processos[i].ativo && gp->processos[i].id == id_processo) {
            return &gp->processos[i];
        }
    }
    return NULL;
}

int remover_processo(GerenciadorProcessos *gp, MemoriaFisica *mf, int id_processo) {
    Processo *processo = encontrar_processo(gp, id_processo);
    if (!processo) {
        printf("Erro: Processo %d nao encontrado.\n", id_processo);
        return 0;
    }
    
    printf("Removendo processo %d...\n", id_processo);
    
    // Libera os quadros físicos alocados
    for (int i = 0; i < processo->num_paginas; i++) {
        if (processo->tabela_paginas[i].presente) {
            liberar_quadro(mf, processo->tabela_paginas[i].quadro_fisico);
            printf("  Quadro %d liberado\n", processo->tabela_paginas[i].quadro_fisico);
        }
    }
    
    // Libera a memória do processo
    liberar_processo(processo);
    
    // Marca o slot como livre
    processo->ativo = 0;
    processo->id = 0;
    processo->tamanho = 0;
    processo->num_paginas = 0;
    
    gp->num_processos--;
    
    printf("Processo %d removido com sucesso.\n", id_processo);
    return 1;
}

void listar_processos(GerenciadorProcessos *gp) {
    printf("\n=== LISTA DE PROCESSOS ===\n");
    printf("ID | Tamanho | Paginas | Status\n");
    printf("---|---------|---------|--------\n");
    
    for (int i = 0; i < MAX_PROCESSOS; i++) {
        if (gp->processos[i].ativo) {
            printf("%2d | %7d | %7d | Ativo\n", 
                   gp->processos[i].id,
                   gp->processos[i].tamanho,
                   gp->processos[i].num_paginas);
        }
    }
    
    printf("Total de processos ativos: %d\n", gp->num_processos);
    printf("==========================\n\n");
}

void liberar_processo(Processo *processo) {
    if (processo) {
        if (processo->tabela_paginas) {
            free(processo->tabela_paginas);
            processo->tabela_paginas = NULL;
        }
        if (processo->memoria_logica) {
            free(processo->memoria_logica);
            processo->memoria_logica = NULL;
        }
    }
} 