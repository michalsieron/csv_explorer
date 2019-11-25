#pragma once

#include <stdio.h>

typedef struct
{
    unsigned short cols;
    unsigned long rows;
    char** table;
} CSV;

/* 
 * Read given file byte by byte.
 */
CSV* CSVreadFile(CSV* csvp, FILE* fp);
char* CSVgetCell(CSV* csvp, unsigned long row, unsigned short col);
void CSVprintRow(CSV* csvp, unsigned long row);
void CSVprintInfo(CSV* csvp);
void CSVclean(CSV* csvp);
