#include <stdio.h>

#include "csv.h"

// 4K		-> printf
// 4K		-> scanf
// 552B	-> fopen (FILE *)
// 4K		-> fgetc
// 8B		-> buffer (String **)
// 8B		-> buffer[0] (String *)
// 16B	-> buffer[0][0] (String {char *, unsigned short})
// 1B		-> buffer[0][0].c_str (char)

// duża liczba zaalokowanych bajtów zwracana przez valgrind'a
// nie liczy zwolnień pamięci

int main()
{
	char filepath[200] = {0};
	printf("Podaj sciezke do pliku .csv: ");
	scanf("%s", filepath);
	CSV csv;
	FILE *fp = fopen(filepath, "r");
	if (fp)
	{
		printf("\nZostal otworzony plik %s\n", filepath);
		if (!CSVreadFile(&csv, fp))
		{
			printf("Wystapil nieoczekiwany blad. Sprawdz poprawnosc pliku.\n");
			fclose(fp);
			return 1;
		}
		CSVprintInfo(&csv);

		for (unsigned long row = 0; row < csv.rows; row++)
		{
			CSVprintRow(&csv, row);
			printf("\n");
		}

		CSVclean(&csv);
	}

	fclose(fp);
	return 0;
}
