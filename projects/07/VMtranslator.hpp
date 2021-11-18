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
    int alabel=0;
  public:
    CodeWrite(string path);
    void setFileName(string fileName);
    void writeArithmetic(string cmd);
    void writePushPop(VMcommand cmd, string seg, int index);
    void close();
};