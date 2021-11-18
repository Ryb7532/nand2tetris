CC = g++
LABEL = -Wall
MAIN_TAR = VMtranslator.cpp filesystem.o Parser.o CodeWrite.o

all: VMtranslator

VMtranslator: $(MAIN_TAR)
	$(CC) $(LABEL) -o $@ $^

filesystem.o: filesystem.cpp
	$(CC) $(LABEL) -c $^

Parser.o: Parser.cpp
	$(CC) $(LABEL) -c $^

CodeWrite.o: CodeWrite.cpp
	$(CC) $(LABEL) -c $^

clean:
	rm *.o *.out