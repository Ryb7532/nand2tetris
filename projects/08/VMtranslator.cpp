#include "VMtranslator.hpp"


int main(int argc, char* argv[]) {
  filesystem fs(argv[1]);
  CodeWrite cw(fs.directory+fs.name+".asm");
  cw.writeInit();
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
      } else if (cmdt == C_LABEL) {
        cw.writeLabel(parser.arg1());
      } else if (cmdt == C_GOTO) {
        cw.writeGoto(parser.arg1());
      } else if (cmdt == C_IF) {
        cw.writeIf(parser.arg1());
      } else if (cmdt == C_FUNCTION) {
        cw.writeFunction(parser.arg1(), parser.arg2());
      } else if (cmdt == C_CALL) {
        cw.writeCall(parser.arg1(), parser.arg2());
      } else if (cmdt == C_RETURN) {
        cw.writeReturn();
      }
      parser.advance();
    }
  }
  cw.close();
  return 0;
}