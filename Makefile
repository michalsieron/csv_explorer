# Makefile

SOURCES = $(wildcard src/*.c)

debug: $(SOURCES)
	gcc -g $(SOURCES) -o bin/debug/csv_explorer

release: $(SOURCES)
	gcc $(SOURCES) -o bin/release/csv_explorer
