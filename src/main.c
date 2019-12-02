#include <stdio.h>

#include "csv.h"

int main()
{
    char filepath[200] = {0};
    printf("Podaj sciezke do pliku .csv: ");
    scanf("%s", filepath);
    CSV csv;
    FILE* fp = fopen(filepath, "r");
    if (fp)
    {
        printf("\nZostal otworzony plik %s\n", filepath);
        CSVreadFile(&csv, fp);
        CSVprintInfo(&csv);

        for (unsigned long row = 1; row < csv.rows; row++)
        {
            CSVprintRow(&csv, row);
            printf("\n");
        }
        CSVsetCell(&csv, 1, 1, "new \ncontent boiz");

        for (unsigned long row = 1; row < csv.rows; row++)
        {
            CSVprintRow(&csv, row);
            printf("\n");
        }
        CSVclean(&csv);
    }

    fclose(fp);
    return 0;
}
