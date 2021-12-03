CC = g++
LABEL = -Wall
MAIN_TAR = JackCompiler.cpp filesystem.o JackTokenizer.o CompilationEngine.o SymbolTable.o VMWriter.o

all: JackAnalyzer

JackAnalyzer: $(MAIN_TAR)
	$(CC) $(LABEL) -o $@ $^

filesystem.o: filesystem.cpp
	$(CC) $(LABEL) -c $^

JackTokenizer.o: JackTokenizer.cpp
	$(CC) $(LABEL) -c $^

CompilationEngine.o: CompilationEngine.cpp
	$(CC) $(LABEL) -c $^

SymbolTable.o: SymbolTable.cpp
	$(CC) $(LABEL) -c $^

VMWriter.o: VMWriter.cpp
	$(CC) $(LABEL) -c $^

clean:
	rm *.o *.out