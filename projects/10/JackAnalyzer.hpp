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

using namespace std;


enum Token {
  KEYWORD,
  SYMBOL,
  IDENTIFIER,
  INT_CONST,
  STRING_CONST
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

class CompilationEngine {
  private:
    ofstream fout;
    JackTokenizer tokenizer;
    void compileClass();
    void compileClassVarDec();
    void compileSubroutine();
    void compileParameterList();
    void compileVarDec();
    void compileStatements();
    void compileDo();
    void compileLet();
    void compileWhile();
    void compileReturn();
    void compileIf();
    void compileExpression();
    void compileTerm();
    void compileExpressionList();
    void compileKeyword();
    void compileSymbol();
    void compileInt();
    void compileString();
    void compileIdentifier();
    void compileType();
    void tokenCheck(Token expect);
    void keywordCheck(string expect);
    void symbolCheck(string expect);
    void raiseError(string type, string expect, string actual);
  public:
    CompilationEngine(string input, string output);
};