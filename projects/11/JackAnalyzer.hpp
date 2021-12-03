#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <bitset>
#include <vector>
#include <string>
#include <array>
#include <map>
#include <set>
#include <algorithm>
#include <dirent.h>
#include <stdlib.h>

using namespace std;


enum Token {
  KEYWORD,
  SYMBOL,
  IDENTIFIER,
  INT_CONST,
  STRING_CONST
};

enum Kind {
  STATIC,
  FIELD,
  ARG,
  VAR,
  NONE
};

enum Segment {
  CONST,
  SEG_ARG,
  LOCAL,
  SEG_STATIC,
  THIS,
  THAT,
  POINTER,
  TEMP
};

enum ArithCommand {
  ADD,
  SUB,
  NEG,
  EQ,
  GT,
  LT,
  AND,
  OR,
  NOT
};


extern set<string> keywords;
extern set<string> symbols;

class filesystem {
  public:
    string name;
    string directory;
    vector<string> files;
    filesystem(char* path);
};

class JackTokenizer {
  private:
    vector<pair<Token, string>> tokens;
    int current = 0;
  public:
    JackTokenizer(string path);
    bool hasMoreTokens();
    void advance();
    void regress();
    Token tokenType();
    string keyword();
    string symbol();
    string identifier();
    int intVal();
    string stringVal();
    bool checkClassVarDec();
    bool checkSubroutine();
    bool checkOp();
    bool checkUnaryOp();
    bool checkKeywordConstant();
};

class SymbolTable {
  private:
    map<string, tuple<string,Kind,int>> Class, SubRoutine;
    int id[4] = {0,0,0,0};
  public:
    SymbolTable();
    void startSubroutine();
    void define(string name, string type, Kind kind);
    int varCount(Kind kind);
    Kind kindOf(string name);
    string typeOf(string name);
    int indexOf(string name);
};

class VMWriter {
  private:
    ofstream fout;
    string class_name, file_name;
    bool isMethod = false;
  public:
    VMWriter(string directory, string name);
    void setClassName(string name);
    string getClassName();
    void setIsMethod(bool method);
    void writePush(Segment seg, int index);
    void writePop(Segment seg, int index);
    void writeArithmetic(ArithCommand cmd);
    void writeLabel(string label);
    void writeGoto(string label);
    void writeIf(string label);
    void writeCall(string name, int nArgs);
    void writeFunction(string name, int nLocals);
    void writeReturn();
    void close();
};

class CompilationEngine {
  private:
    // ofstream fout;
    JackTokenizer tokenizer;
    SymbolTable table;
    VMWriter writer;
    int if_label=0, while_label=0;
    void compileClass();
    void compileClassVarDec();
    void compileSubroutine();
    void compileParameterList();
    int compileVarDec();
    void compileStatements();
    void compileDo();
    void compileLet();
    void compileWhile();
    void compileReturn();
    void compileIf();
    void compileExpression();
    void compileTerm();
    int compileExpressionList();
    void compileSubroutineCall();
    string compileKeyword();
    string compileSymbol();
    void compileInt();
    void compileString();
    string compileIdentifier(string category, string state, string type);
    string compileType();
    void tokenCheck(Token expect);
    void keywordCheck(string expect);
    void symbolCheck(string expect);
    void raiseError(string type, string expect, string actual);
  public:
    CompilationEngine(string input, string directory, string name);
};
