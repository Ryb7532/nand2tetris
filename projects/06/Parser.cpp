#include "Assembler.hpp"


Parser::Parser(char* path) {
  string p = path;
  string ext = p.substr(p.find_last_of("."));
  if (ext != ".asm") {
    cout << "extension (" << ext << ") is not .asm\n";
    exit(0);
  }
  ifstream fin(path);
  if (!fin) {
    cout << "can't open file: " << path << endl;
    exit(0);
  }
  string line;
  while(getline(fin, line)) {
    line.pop_back();
    size_t c;
    while ((c=line.find_first_of(' '))!=line.npos) {
      line.erase(c,1);
    }
    line = line.substr(0,line.find_first_of("/"));
    if (line.empty())
      continue;
    if (line[0]=='@') {
      cmds.push_back({A_COMMAND, line.substr(1)});
    } else if (line[0]=='(') {
      line.pop_back();
      cmds.push_back({L_COMMAND, line.substr(1)});
    } else {
      cmds.push_back({C_COMMAND, line});
    }
  }
  fin.close();
}

bool Parser::hasMoreCommands() { return current < (int)cmds.size(); }

void Parser::advance() { current++; }

command Parser::commandType() { return cmds[current].first; }

string Parser::symbol() { return cmds[current].second; }

string Parser::dest() {
  string cmd = cmds[current].second;
  auto eq = cmd.find("=");
  if (eq==cmd.npos)
    return "null";
  return cmd.substr(0,eq);
}

string Parser::comp() {
  string cmd = cmds[current].second;
  cmd = cmd.substr(0,cmd.find(";"));
  return cmd.substr(cmd.find("=")+1);
}

string Parser::jump() {
  string cmd = cmds[current].second;
  auto smcln = cmd.find(";");
  if (smcln==cmd.npos)
    return "null";
  return cmd.substr(smcln+1);
}

void Parser::reset() { current=0; }