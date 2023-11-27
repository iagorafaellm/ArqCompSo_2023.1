#include <stdio.h>

void troca(int vet[], int i, int j) {
	int temp;
	temp = vet[i];
	vet[i] = vet[j];
	vet[j] = temp;
}

void selection(int vet[], int tam) {
	int i, j, idMenor;

	for (i = 0; i <= tam - 1; i++) {
		idMenor = i;

		for (j = i + 1; j < tam; j++) {
			if (vet[j] < vet[idMenor]) {
				idMenor = j;
			}
		}

		if (i != idMenor) {
			troca(vet, i, idMenor);
		}
	}
}

int main() {
	int vet[] = {5, 4, 3, 2, 1};

	selection(vet, 5);

	for (int i = 0; i < 5; i++) {
		printf("%d\n", vet[i]);
	}
}
