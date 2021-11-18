#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <bitset>
#include <vector>
#include <string>
#include <array>
#include <map>

using namespace std;

enum command {
  A_COMMAND,
  C_COMMAND,
  L_COMMAND
};


class Parser {
  private:
    vector<pair<command, string>> cmds;
    int current = 0;
  public:
    Parser(char *path);
    bool hasMoreCommands();
    void advance();
    command commandType();
    string symbol();
    string dest();
    string comp();
    string jump();
    void reset();
};


template <size_t n>
struct bits {
  bool bit[n];
  bits();
  bool &operator[](int i);
  bits<n> &operator=(array<bool,n> ar);
  friend ostream &operator<<(ostream &os, const bits<n> &b) {
    for (size_t i=0; i<n; i++)
      os << b.bit[i];
    return os;
  }
};


namespace Code {
  bits<3> dest(string mnemonic);
  bits<7> comp(string mnemonic);
  bits<3> jump(string mnemonic);
};


class AssemblerL {
  Parser parser;
  string name;
  public:
    AssemblerL(char* path);
    void asm2hack();
};


class SymbolTable {
  private:
    map<string,int> table;
  public:
    SymbolTable() {}
    void addEntry(string symbol, int address);
    bool contains(string symbol);
    int getAddress(string symbol);
};


class Assembler {
  private:
    Parser parser;
    string name;
    int ROMaddress=0, RAMaddress=16;
  public:
    Assembler(char* path);
    void asm2hack();
};
