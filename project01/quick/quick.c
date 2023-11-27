#include <stdio.h>

void troca(int vet[], int i, int j) {
	int temp;
	temp = vet[i];
	vet[i] = vet[j];
	vet[j] = temp;
}

void quick(int vet[], int inicio, int fim) {
	int i, j, pivo;

	i = inicio;
	j = fim;
	pivo = vet[(i + j) / 2];

	while (i <= j) {
		while (vet[i] < pivo) {
			i = i + 1;
		}

		while (vet[j] > pivo) {
			j = j - 1;
		}

		if (i <= j) {
			troca(vet, i, j);
			i = i + 1;
			j = j - 1;
		}
	}

	if (inicio < j) {
		quick(vet, inicio, j);
	}

	if (i < fim) {
		quick(vet, i, fim);
	}
}

int main() {
	int vet[] = {5, 4, 3, 2, 1};

	quick(vet, 0, 4);

	for (int i = 0; i < 5; i++) {
		printf("%d\n", vet[i]);
	}
}