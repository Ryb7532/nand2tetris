#include "VMtranslator.hpp"


set<string> arith = {"add", "sub", "neg", "eq", "gt", "lt", "and", "or", "not"};

Parser::Parser(string path) {
  string ext = path.substr(path.find_last_of("."));
  if (ext != ".vm") {
    cout << "extension (" << ext << ") is not .vm\n";
    return;
  }
  ifstream fin(path);
  if (!fin) {
    cout << "can't open file: " << path << endl;
    return;
  }
  string line;
  while(getline(fin, line)) {
    line.pop_back();
    line = line.substr(0,line.find_first_of("/"));
    if (line.empty())
      continue;
    istringstream iss(line);
    string cmd;
    iss >> cmd;
    if (arith.find(cmd) != arith.end()) {
      cmds.push_back({C_ARITHMETIC, {cmd}});
    } else if (cmd == "push") {
      string seg, index;
      iss >> seg >> index;
      cmds.push_back({C_PUSH, {cmd, seg, index}});
    } else if (cmd == "pop") {
      string seg, index;
      iss >> seg >> index;
      cmds.push_back({C_POP, {cmd, seg, index}});
    } else if (cmd == "label") {
      string symbol;
      iss >> symbol;
      cmds.push_back({C_LABEL, {cmd, symbol}});
    } else if (cmd == "goto") {
      string symbol;
      iss >> symbol;
      cmds.push_back({C_GOTO, {cmd, symbol}});
    } else if (cmd == "if-goto") {
      string symbol;
      iss >> symbol;
      cmds.push_back({C_IF, {cmd, symbol}});
    } else if (cmd == "function") {
      string funcName, nLocals;
      iss >> funcName >> nLocals;
      cmds.push_back({C_POP, {cmd, funcName, nLocals}});
    } else if (cmd == "return") {
      cmds.push_back({C_RETURN, {cmd}});
    } else {
      string funcName, nArgs;
      iss >> funcName >> nArgs;
      cmds.push_back({C_CALL, {cmd, funcName, nArgs}});
    }
  }
  fin.close();
}

bool Parser::hasMoreCommands() { return current < (int)cmds.size(); }

void Parser::advance() { current++; }

VMcommand Parser::commandType() { return cmds[current].first; }

string Parser::arg1() {
  if (cmds[current].first == C_ARITHMETIC)
    return cmds[current].second[0];
  return cmds[current].second[1];
}

int Parser::arg2() { return stoi(cmds[current].second[2]); }
