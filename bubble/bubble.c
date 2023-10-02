#include <stdio.h>
#define TRUE 1
#define FALSE 0

void troca(int vet[], int k) {
	int temp;
	temp = vet[k];
	vet[k] = vet[k+1];
	vet[k+1] = temp;
}

void bubble(int vet[], int size) {
	int i, trocado;
	trocado = FALSE;

	do {
		trocado = FALSE;
		for (i = 0; i < size - 1; i++) {
			if (vet[i + 1] < vet[i]) {
				troca(vet, i);
				trocado = TRUE;
			}
		}
	} while (trocado);
}

int main() {
	int vet[] = { 5, 4, 3, 2, 1 };

	bubble(vet, 5);

	for (int i = 0; i < 5; i++) {
		printf("%d\n", vet[i]);
	}
}

