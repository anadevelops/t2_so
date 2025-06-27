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
        
        // Inicializa a tabela de páginas
        for (int j = 0; j < MAX_PAGINAS_PROCESSO; j++) {
            gp->processos[i].tabela_paginas[j].quadro_fisico = -1;
            gp->processos[i].tabela_paginas[j].presente = 0;
            gp->processos[i].tabela_paginas[j].modificada = 0;
        }
    }
    
    printf("Gerenciador de processos inicializado.\n");
}

int calcular_num_paginas(int tamanho) {
    int num_paginas = tamanho / TAMANHO_PAGINA;
    if (tamanho % TAMANHO_PAGINA != 0) {
        num_paginas++; // Arredonda para cima
    }
    return num_paginas;
}

void gerar_dados_aleatorios_processo(Processo *processo) {
    srand(time(NULL) + processo->id); // Seed baseado no ID do processo
    
    for (int i = 0; i < processo->tamanho; i++) {
        processo->memoria_logica[i] = rand() % 256; // Valores de 0 a 255
    }
    
    printf("Dados aleatórios gerados para o processo %d (%d bytes).\n", 
           processo->id, processo->tamanho);
}

int criar_processo(GerenciadorProcessos *gp, MemoriaFisica *mf, int tamanho) {
    // Verifica se há espaço para mais processos
    if (gp->num_processos >= MAX_PROCESSOS) {
        printf("Erro: Número máximo de processos atingido (%d).\n", MAX_PROCESSOS);
        return -1;
    }
    
    // Verifica se o tamanho é válido
    if (tamanho <= 0 || tamanho > TAMANHO_MAX_PROCESSO) {
        printf("Erro: Tamanho de processo inválido (%d). Deve estar entre 1 e %d bytes.\n", 
               tamanho, TAMANHO_MAX_PROCESSO);
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
        printf("Erro: Não foi possível encontrar um slot livre para o processo.\n");
        return -1;
    }
    
    // Cria o processo
    Processo *processo = &gp->processos[slot_livre];
    processo->id = gp->proximo_id++;
    processo->tamanho = tamanho;
    processo->num_paginas = calcular_num_paginas(tamanho);
    processo->ativo = 1;
    
    // Inicializa a tabela de páginas
    for (int i = 0; i < MAX_PAGINAS_PROCESSO; i++) {
        processo->tabela_paginas[i].quadro_fisico = -1;
        processo->tabela_paginas[i].presente = 0;
        processo->tabela_paginas[i].modificada = 0;
    }
    
    // Gera dados aleatórios para a memória lógica
    gerar_dados_aleatorios_processo(processo);
    
    // Aloca quadros físicos para o processo
    if (!alocar_quadros_processo(gp, mf, processo->id)) {
        printf("Erro: Falha ao alocar quadros para o processo %d.\n", processo->id);
        processo->ativo = 0; // Marca como inativo
        return -1;
    }
    
    // Copia dados da memória lógica para a física
    copiar_memoria_logica_para_fisica(gp, mf, processo->id);
    
    gp->num_processos++;
    
    printf("Processo %d criado com sucesso!\n", processo->id);
    printf("  - Tamanho: %d bytes\n", processo->tamanho);
    printf("  - Páginas: %d\n", processo->num_paginas);
    
    return processo->id;
}

int alocar_quadros_processo(GerenciadorProcessos *gp, MemoriaFisica *mf, int id_processo) {
    Processo *processo = encontrar_processo(gp, id_processo);
    if (!processo) {
        printf("Erro: Processo %d não encontrado.\n", id_processo);
        return 0;
    }
    
    printf("Alocando %d quadros para o processo %d...\n", processo->num_paginas, id_processo);
    
    for (int i = 0; i < processo->num_paginas; i++) {
        int quadro = alocar_quadro(mf);
        if (quadro == -1) {
            printf("Erro: Não há quadros suficientes para alocar a página %d do processo %d.\n", 
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
        
        printf("  Página %d -> Quadro %d\n", i, quadro);
    }
    
    return 1;
}

void copiar_memoria_logica_para_fisica(GerenciadorProcessos *gp, MemoriaFisica *mf, int id_processo) {
    Processo *processo = encontrar_processo(gp, id_processo);
    if (!processo) {
        printf("Erro: Processo %d não encontrado.\n", id_processo);
        return;
    }
    
    printf("Copiando dados da memória lógica para a física do processo %d...\n", id_processo);
    
    for (int pagina = 0; pagina < processo->num_paginas; pagina++) {
        if (processo->tabela_paginas[pagina].presente) {
            int quadro = processo->tabela_paginas[pagina].quadro_fisico;
            int endereco_fisico_base = quadro * TAMANHO_PAGINA;
            int endereco_logico_base = pagina * TAMANHO_PAGINA;
            
            // Calcula quantos bytes copiar para esta página
            int bytes_para_copiar = TAMANHO_PAGINA;
            if (endereco_logico_base + TAMANHO_PAGINA > processo->tamanho) {
                bytes_para_copiar = processo->tamanho - endereco_logico_base;
            }
            
            // Copia os dados
            for (int i = 0; i < bytes_para_copiar; i++) {
                unsigned char valor = processo->memoria_logica[endereco_logico_base + i];
                escrever_na_memoria(mf, endereco_fisico_base + i, valor);
            }
            
            printf("  Página %d: %d bytes copiados para o quadro %d\n", 
                   pagina, bytes_para_copiar, quadro);
        }
    }
}

void visualizar_tabela_paginas(GerenciadorProcessos *gp, int id_processo) {
    Processo *processo = encontrar_processo(gp, id_processo);
    if (!processo) {
        printf("Erro: Processo %d não encontrado.\n", id_processo);
        return;
    }
    
    printf("\n=== TABELA DE PÁGINAS - PROCESSO %d ===\n", id_processo);
    printf("Tamanho do processo: %d bytes\n", processo->tamanho);
    printf("Número de páginas: %d\n", processo->num_paginas);
    printf("\n");
    printf("Página | Quadro Físico | Presente | Modificada\n");
    printf("-------|---------------|----------|-----------\n");
    
    for (int i = 0; i < processo->num_paginas; i++) {
        EntradaTabelaPagina *entrada = &processo->tabela_paginas[i];
        printf("%6d | %13d | %8s | %10s\n", 
               i,
               entrada->quadro_fisico,
               entrada->presente ? "Sim" : "Não",
               entrada->modificada ? "Sim" : "Não");
    }
    
    printf("\n");
    
    // Mostra estatísticas
    int paginas_presentes = 0;
    int paginas_modificadas = 0;
    
    for (int i = 0; i < processo->num_paginas; i++) {
        if (processo->tabela_paginas[i].presente) {
            paginas_presentes++;
        }
        if (processo->tabela_paginas[i].modificada) {
            paginas_modificadas++;
        }
    }
    
    printf("Estatísticas:\n");
    printf("  - Páginas presentes na memória: %d/%d (%.1f%%)\n", 
           paginas_presentes, processo->num_paginas, 
           (float)paginas_presentes / processo->num_paginas * 100);
    printf("  - Páginas modificadas: %d/%d (%.1f%%)\n", 
           paginas_modificadas, processo->num_paginas,
           (float)paginas_modificadas / processo->num_paginas * 100);
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
        printf("Erro: Processo %d não encontrado.\n", id_processo);
        return 0;
    }
    
    printf("Removendo processo %d...\n", id_processo);
    
    // Libera todos os quadros alocados pelo processo
    for (int i = 0; i < processo->num_paginas; i++) {
        if (processo->tabela_paginas[i].presente) {
            liberar_quadro(mf, processo->tabela_paginas[i].quadro_fisico);
            printf("  Quadro %d liberado (página %d)\n", 
                   processo->tabela_paginas[i].quadro_fisico, i);
        }
    }
    
    // Marca o processo como inativo
    processo->ativo = 0;
    processo->id = 0;
    processo->tamanho = 0;
    processo->num_paginas = 0;
    
    // Limpa a tabela de páginas
    for (int i = 0; i < MAX_PAGINAS_PROCESSO; i++) {
        processo->tabela_paginas[i].quadro_fisico = -1;
        processo->tabela_paginas[i].presente = 0;
        processo->tabela_paginas[i].modificada = 0;
    }
    
    gp->num_processos--;
    
    printf("Processo %d removido com sucesso!\n", id_processo);
    return 1;
}

void listar_processos(GerenciadorProcessos *gp) {
    printf("\n=== LISTA DE PROCESSOS ===\n");
    
    if (gp->num_processos == 0) {
        printf("Nenhum processo ativo.\n");
        return;
    }
    
    printf("ID | Tamanho (bytes) | Páginas | Status\n");
    printf("---|-----------------|---------|--------\n");
    
    for (int i = 0; i < MAX_PROCESSOS; i++) {
        if (gp->processos[i].ativo) {
            Processo *p = &gp->processos[i];
            printf("%2d | %15d | %7d | Ativo\n", 
                   p->id, p->tamanho, p->num_paginas);
        }
    }
    
    printf("\nTotal de processos ativos: %d\n", gp->num_processos);
    printf("\n");
} 