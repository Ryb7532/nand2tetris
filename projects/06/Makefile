CC = g++
LABEL = -Wall
MAIN_TAR = Assembler.cpp Parser.o Code.o SymbolTable.o

all: Assembler

Assembler: $(MAIN_TAR)
	$(CC) $(LABEL) -o $@ $^

Parser.o: Parser.cpp
	$(CC) $(LABEL) -c $^

Code.o: Code.cpp
	$(CC) $(LABEL) -c $^

SymbolTable.o: SymbolTable.cpp
	$(CC) $(LABEL) -c $^

clean:
	rm *.o *.out