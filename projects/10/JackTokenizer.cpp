#include "JackAnalyzer.hpp"


set<string> keywords = {"class", "constructor", "function", "method",
        "field", "static", "var", "int", "char", "boolean", "void",
        "true", "false", "null", "this", "let", "do", "if", "else",
        "while", "return"};

set<string> symbols = {"{", "}", "(", ")", "[", "]", ".", ",", ";",
        "+", "-", "*", "/", "&", "|", "<", ">", "=", "~"};

set<string> op = {"+", "-", "*", "/", "&", "|", "<", ">", "="};

JackTokenizer::JackTokenizer(string path) {
  ifstream fin(path);
  if (!fin) {
    cout << "can't open file: " << path << endl;
    return;
  }
  bool int_ = false;
  char c;
  string s;
  while (fin.get(c)) {
    if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
      if (keywords.find(s) != keywords.end()) {
        tokens.push_back({KEYWORD, s});
      } else if (symbols.find(s) != symbols.end()) {
        tokens.push_back({SYMBOL, s});
      } else if (int_) {
        tokens.push_back({INT_CONST, s});
        int_ = false;
      } else if (!s.empty()) {
        tokens.push_back({IDENTIFIER, s});
      }
      s.clear();
      continue;
    }
    if (c == '\"') {
      if (keywords.find(s) != keywords.end()) {
        tokens.push_back({KEYWORD, s});
      } else if (symbols.find(s) != symbols.end()) {
        tokens.push_back({SYMBOL, s});
      } else if (int_) {
        tokens.push_back({INT_CONST, s});
        int_ = false;
      } else if (!s.empty()) {
        tokens.push_back({IDENTIFIER, s});
      }
      s.clear();      while (fin.get(c)) {
        if (c == '\n' || c == '\r') {
          cout << "string has newline char.\n";
          exit(1);
        }
        if (c == '\"')
          break;
        s.push_back(c);
      }
      tokens.push_back({STRING_CONST, s});
      s.clear();
      continue;
    }
    if (c == '/' && s[0] == '/') {
      while (fin.get(c)) {
        if (c == '\n' || c == '\r')
          break;
      }
      s.clear();
      continue;
    }
    if (c == '*' && s[0] == '/') {
      s.clear();
      while (fin.get(c)) {
        if (c == '/' && s[0] == '*')
          break;
        s[0] = c;
      }
      s.clear();
      continue;
    }
    if (symbols.find({c}) != symbols.end()) {
      if (keywords.find(s) != keywords.end()) {
        tokens.push_back({KEYWORD, s});
      } else if (symbols.find(s) != symbols.end()) {
        tokens.push_back({SYMBOL, s});
      } else if (int_) {
        tokens.push_back({INT_CONST, s});
        int_ = false;
      } else if (!s.empty()) {
        tokens.push_back({IDENTIFIER, s});
      }
      s.clear();
      s.push_back(c);
      continue;
    }
    if (symbols.find(s) != symbols.end()) {
      tokens.push_back({SYMBOL, s});
      s.clear();
    }
    if (s.empty() && (c-'0'>=0 && c-'0'<=9))
      int_ = true;
    s.push_back(c);
  }
  fin.close();
}

bool JackTokenizer::hasMoreTokens() { return current < (int)tokens.size(); }

void JackTokenizer::advance() { current++; }

Token JackTokenizer::tokenType() { return tokens[current].first; }

string JackTokenizer::keyword() { return tokens[current].second; }

string JackTokenizer::symbol() { return tokens[current].second; }

string JackTokenizer::identifier() { return tokens[current].second; }

int JackTokenizer::intVal() {
  int val = stoi(tokens[current].second);
  return val;
}

string JackTokenizer::stringVal() { return tokens[current].second; }

bool JackTokenizer::checkClassVarDec() {
  if (tokens[current].first != KEYWORD)
    return false;
  string key = keyword();
  if (key == "static" || key == "field")
    return true;
  return false;
}

bool JackTokenizer::checkSubroutine() {
  if (tokens[current].first != KEYWORD)
    return false;
  string key = keyword();
  if (key == "constructor" || key == "function" || key == "method")
    return true;
  return false;
}

bool JackTokenizer::checkOp() {
  if (tokens[current].first != SYMBOL)
    return false;
  string sym = symbol();
  if (op.find(sym) != op.end())
    return true;
  return false;
}

bool JackTokenizer::checkUnaryOp() {
  if (tokens[current].first != SYMBOL)
    return false;
  string sym = symbol();
  if (sym == "-" || sym == "~")
    return true;
  return false;
}

bool JackTokenizer::checkKeywordConstant() {
  if (tokens[current].first != KEYWORD)
    return false;
  string key = keyword();
  if (key == "true" || key == "false" || key == "null" || key == "this")
    return true;
  return false;
}