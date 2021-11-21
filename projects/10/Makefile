CC = g++
LABEL = -Wall
MAIN_TAR = JackAnalyzer.cpp filesystem.o JackTokenizer.o CompilationEngine.o

all: JackAnalyzer

JackAnalyzer: $(MAIN_TAR)
	$(CC) $(LABEL) -o $@ $^

filesystem.o: filesystem.cpp
	$(CC) $(LABEL) -c $^

JackTokenizer.o: JackTokenizer.cpp
	$(CC) $(LABEL) -c $^

CompilationEngine.o: CompilationEngine.cpp
	$(CC) $(LABEL) -c $^

clean:
	rm *.o *.out