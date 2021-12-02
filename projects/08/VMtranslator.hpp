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


enum VMcommand {
  C_ARITHMETIC,
  C_PUSH,
  C_POP,
  C_LABEL,
  C_GOTO,
  C_IF,
  C_FUNCTION,
  C_RETURN,
  C_CALL
};


class filesystem {
  public:
    string name;
    string directory;
    vector<string> files;
    filesystem(char* path);
};


class Parser {
  private:
    vector<pair<VMcommand, vector<string>>> cmds;
    int current = 0;
  public:
    Parser(string path);
    bool hasMoreCommands();
    void advance();
    VMcommand commandType();
    string arg1();
    int arg2();
};


class CodeWrite {
  private:
    ofstream fout;
    string fileName;
    string funcName;
    int alabel=0;
    int rlabel=0;
  public:
    CodeWrite(string path);
    void setFileName(string fileName);
    void writeInit();
    void writeArithmetic(string cmd);
    void writePushPop(VMcommand cmd, string seg, int index);
    void writeLabel(string label);
    void writeGoto(string label);
    void writeIf(string label);
    void writeCall(string funcName, int nArgs);
    void writeReturn();
    void writeFunction(string funcName, int nLocals);
    void callerSave();
    void close();
};