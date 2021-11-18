#include "VMtranslator.hpp"


int main(int argc, char* argv[]) {
  filesystem fs(argv[1]);
  CodeWrite cw(fs.directory+fs.name+".asm");
  for (string f: fs.files) {
    Parser parser(fs.directory+f);
    f = f.substr(0,f.find_last_of('.'));
    cw.setFileName(f);
    while(parser.hasMoreCommands()) {
      VMcommand cmdt = parser.commandType();
      if (cmdt == C_ARITHMETIC) {
        cw.writeArithmetic(parser.arg1());
      } else if (cmdt == C_PUSH || cmdt == C_POP) {
        cw.writePushPop(cmdt, parser.arg1(), parser.arg2());
      }
      parser.advance();
    }
  }
  cw.close();
  return 0;
}