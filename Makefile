# Nom du fichier source
SRC = test.c

# Nom de l'exécutable
EXEC = test

# Compilateur
CC = gcc

# Options de compilation
CFLAGS = -Wall -Wextra -pedantic

# Règle par défaut
all: $(EXEC)

# Règle pour créer l'exécutable
$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC)

# Règle pour nettoyer les fichiers générés
clean:
	rm -f $(EXEC)

# Règle pour exécuter le programme
run: $(EXEC)
	./$(EXEC)

.PHONY: all clean run