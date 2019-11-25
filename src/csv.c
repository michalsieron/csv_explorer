#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "csv.h"
/*
 * Funkcja CSVreadFile wczytuje po jednym znaku z podanego pliku. Podczas 
 * jednej iteracji analizuje znak wczytany podczas poprzedniej. Dla każdego znaku
 * alokuje odpowiednią ilość pamięci 	
 */
CSV* CSVreadFile(CSV* csvp, FILE* fp)
{
	char** buffer = NULL;
	char readChar = '\0';
	char lastChar = '\0';
  
  unsigned long rows = 1;
	unsigned short cells = 1;
	unsigned short positionInCell = 0;

	bool inQuote = false;
	
	do
	{
    void* temp = NULL;
		readChar = fgetc(fp);
    // printf("i read char: %3hhd | %c\n", readChar, readChar);
		
		if (buffer == NULL)
		{
			buffer = (char**)calloc(1, sizeof(char*));
			(*buffer) = (char*)calloc(1, sizeof(char));
		}

		if (inQuote)
		{
      temp = (char*)realloc(buffer[cells - 1], (positionInCell + 2) * sizeof(char));
      if (temp != NULL)
      {
        buffer = temp;
      }
      else
      {
        // don't know what to do here yet
      }

      if (lastChar == '"' && readChar != '"')
      {
        inQuote = false;
      }
			buffer[cells - 1][positionInCell] = lastChar;
			positionInCell++;
		}
		else
		{
			switch (lastChar)
			{
				case '\0':
				case '\r':
					break;

				case ',':
					buffer[cells - 1][positionInCell] = '\0';
					positionInCell = 0;
					cells++;
					{
						temp = (char**)realloc(buffer, cells * sizeof(char*));
						if (temp != NULL)
						{
							buffer = temp;
							temp = (char*)calloc(1, sizeof(char));
							if (temp != NULL)
							{
								buffer[cells - 1] = temp;
							}
							else
							{
								// don't know what to do here yet
							}
						}
						else
						{
							// don't know what to do here yet
						}
					}
					break;

        case '"':
          temp = (char*)realloc(buffer[cells - 1], (positionInCell + 2) * sizeof(char));
          if (temp != NULL)
          {
            buffer = temp;
          }
          else
          {
            // don't know what to do here yet
          }
          buffer[cells - 1][positionInCell - 1] = lastChar;
			    positionInCell++;
          inQuote = true;
          break;

        case '\n':
					buffer[cells - 1][positionInCell] = '\0';
          positionInCell = 0;
					cells++;
          rows++;
					{
						temp = (char**)realloc(buffer, cells * sizeof(char*));
						if (temp != NULL)
						{
							buffer = temp;
							temp = (char*)calloc(1, sizeof(char));
							if (temp != NULL)
							{
								buffer[cells - 1] = temp;
							}
							else
							{
								// don't know what to do here yet
							}
						}
						else
						{
							// don't know what to do here yet
						}
					}
          break;

        default:
          temp = (char*)realloc(*(buffer + cells - 1), (positionInCell + 2) * sizeof(char));
          if (temp != NULL)
          {
            buffer[cells - 1] = temp;
          }
          else
          {
            // don't know what to do here yet
          }
          *(*(buffer + cells - 1) + positionInCell) = lastChar;
			    positionInCell++;
          break;
			}
      lastChar = readChar;
		}
	} while (readChar != EOF);
	buffer[cells - 1][positionInCell] = '\0';

	csvp->rows = rows;
	csvp->cols = cells / rows;
	csvp->table = buffer;

	return csvp;
}

char* CSVgetCell(CSV* csvp, unsigned long row, unsigned short col)
{
	return csvp->table[row * csvp->cols + col];
}

void CSVprintRow(CSV* csvp, unsigned long row)
{
	for (unsigned short col = 0; col < csvp->cols; col++)
	{
		printf("%s\n", CSVgetCell(csvp, row, col));
	}
}

void CSVprintInfo(CSV* csvp)
{
	printf("Dokument CSV zawiera %lu wierszy po %hu kolumn kazdy.\n", csvp->rows, csvp->cols);
}

void CSVclean(CSV* csvp)
{
	for (unsigned long i = 0; i < csvp->rows * csvp->cols; i++)
		free(csvp->table[i]);
	
	free(csvp->table);
}