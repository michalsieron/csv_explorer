# csv_explorer

1. biblioteka csv.c + csv.h która pozwala na operacje na plikach csv
2. po otworzeniu pliku wskaźnik jest przekazywany do funkcji, która przepisuje go do pamięci
3. każda komórka jest tablicą charów zakończona '\0'
4. każdy wiersz jest tablicą wskaźników na wskaźniki na komórki
5. liczba kolumn i ich nazwy są definiowane przez pierwszy wiersz będący wierszem nagłówkowym
6. operacje na pliku są wykonywane przez specjalne funkcje
7. istnieje funkcja do parsowania jednej komórki
8. może istnieć funkcja minimalizująca użycie '"' w komórce
9. biblioteka nic nie wypisuje jedynie zwraca wartości
10. biblioteka jest przenośna
11. front jest minimalny wypisujemy po jednym wierszu: komórka '\n' komórka '\n' itd.