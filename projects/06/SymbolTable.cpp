#include "Assembler.hpp"


void SymbolTable::addEntry(string symbol, int address) {
  table[symbol] = address;
}

bool SymbolTable::contains(string symbol) {
  try {
    table.at(symbol);
  } catch (out_of_range&) {
    return false;
  }
  return true;
}

int SymbolTable::getAddress(string symbol) {
  return table[symbol];
}