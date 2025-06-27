#include "memoria.h"

int main() {
    MemoriaFisica mf;
    inicializar_memoria_fisica(&mf);

    // Aloca 3 quadros
    int q1 = alocar_quadro(&mf);
    int q2 = alocar_quadro(&mf);
    int q3 = alocar_quadro(&mf);

    printf("Quadros alocados: %d, %d, %d\n", q1, q2, q3);

    // Libera o segundo quadro
    liberar_quadro(&mf, q2);
    printf("Quadro %d liberado.\n", q2);

    // Escreve e lê em um endereço físico do primeiro quadro
    int endereco = q1 * TAMANHO_PAGINA;
    escrever_na_memoria(&mf, endereco, 42);
    unsigned char valor = ler_da_memoria(&mf, endereco);
    printf("Valor escrito e lido no quadro %d (endereço %d): %u\n", q1, endereco, valor);

    // Conta quadros livres
    int livres = 0, ocupados = 0;
    for (int i = 0; i < NUMERO_QUADROS; i++) {
        if (mf.quadro_livre[i]) livres++;
        else ocupados++;
    }
    printf("Quadros livres: %d, ocupados: %d\n", livres, ocupados);

    return 0;
}
