#include "Assembler.hpp"



AssemblerL::AssemblerL(char* path) : parser(path) {
  string p = path;
  string base_name = p.substr(p.find_last_of("/") + 1);
  name = base_name.substr(0, base_name.find_last_of("."));
  name += ".hack";
}

void AssemblerL::asm2hack() {
  ofstream fout;
  name = "asm/"+name;
  fout.open(name);
  if (!fout) {
    cout << "can't open file: " << name << endl;
    exit(0);
  }
  while(parser.hasMoreCommands()) {
    if (parser.commandType() != C_COMMAND) {
      int symbol = stoi(parser.symbol());
      fout << bitset<16>(symbol) << endl;
    } else {
      fout << "111" << Code::comp(parser.comp()) << Code::dest(parser.dest()) << Code::jump(parser.jump()) << endl;
    }
    parser.advance();
  }
  fout.close();
}


Assembler::Assembler(char* path) : parser(path) {
  string p = path;
  if (p.find_last_of('/')!=p.npos)
    p = p.substr(p.find_last_of("/") + 1);
  name = p.substr(0, p.find_last_of("."));
  name += ".hack";
}

void Assembler::asm2hack() {
  ofstream fout;
  name = "binary/"+name;
  fout.open(name);
  if (!fout) {
    cout << "can't open file: " << name << endl;
    exit(0);
  }
  SymbolTable table;
  table.addEntry("SP", 0);
  table.addEntry("LCL", 1);
  table.addEntry("ARG", 2);
  table.addEntry("THIS", 3);
  table.addEntry("THAT", 4);
  for (int i=0; i<16; i++) {
    stringstream ss;
    ss << "R" << i;
    table.addEntry(ss.str(), i);
  }
  table.addEntry("SCREEN", 16384);
  table.addEntry("KBD", 24576);
  while(parser.hasMoreCommands()) {
    if (parser.commandType() == L_COMMAND) {
      table.addEntry(parser.symbol(), ROMaddress);
    } else {
      ROMaddress++;
    }
    parser.advance();
  }
  parser.reset();
  while(parser.hasMoreCommands()) {
    if (parser.commandType() == A_COMMAND) {
      string symbol = parser.symbol();
      int address;
      if (table.contains(symbol)) {
        address = table.getAddress(symbol);
      } else if (symbol[0]-'0' >= 0 && symbol[0]-'0' < 10) {
        address = stoi(symbol);
      } else {
        address = RAMaddress;
        table.addEntry(symbol, address);
        RAMaddress++;
      }
      fout << bitset<16>(address) << endl;
    } else if (parser.commandType() == C_COMMAND){
      fout << "111" << Code::comp(parser.comp()) << Code::dest(parser.dest()) << Code::jump(parser.jump()) << endl;
    }
    parser.advance();
  }
  fout.close();
}


int main(int argc, char* argv[]) {
  Assembler assembler(argv[1]);
  assembler.asm2hack();
  return 0;
}