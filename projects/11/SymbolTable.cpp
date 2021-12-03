#include "JackAnalyzer.hpp"

SymbolTable::SymbolTable() {}

void SymbolTable::startSubroutine() {
  SubRoutine.clear();
  id[2] = 0;
  id[3] = 0;
}

void SymbolTable::define(string name, string type, Kind kind) {
  if (kind < 2) {
    try {
      Class.at(name);
      cout << "variable " << name << " is already defined.\n";
    } catch (out_of_range&) {
      Class[name] = {type, kind, id[kind]++};
    }
  } else if (kind < 4) {
    try {
      Class.at(name);
      cout << "variable " << name << " is already defined in this class.\n";
    } catch (out_of_range&){
      try {
        SubRoutine.at(name);
        cout << "variable " << name << " is already defined.\n";
      } catch (out_of_range&) {
        SubRoutine[name] = {type, kind, id[kind]++};
      }
    }
  }
}

int SymbolTable::varCount(Kind kind) {
  return id[kind];
}

Kind SymbolTable::kindOf(string name) {
  try {
    return get<1>(Class.at(name));
  } catch (out_of_range&) {
    try {
      return get<1>(SubRoutine.at(name));
    } catch (out_of_range&) {
      return NONE;
    }
  }
}

string SymbolTable::typeOf(string name) {
  try {
    return get<0>(Class.at(name));
  } catch (out_of_range&) {
    try {
      return get<0>(SubRoutine.at(name));
    } catch (out_of_range&) {
      cout << name << ": undefined variable in this scope\n";
      exit(1);
    }
  }
}

int SymbolTable::indexOf(string name) {
  try {
    return get<2>(Class.at(name));
  } catch (out_of_range&) {
    try {
      return get<2>(SubRoutine.at(name));
    } catch (out_of_range&) {
      cout << name << ": undefined variable in this scope\n";
      exit(1);
    }
  }
}
