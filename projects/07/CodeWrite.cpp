#include "VMtranslator.hpp"


CodeWrite::CodeWrite(string path) { // This implementation differs from API in that argument is output file.
  fout.open(path);
}

void CodeWrite::setFileName(string fileName_) { fileName = fileName_; }

set<string> binaryFunc = {"add", "sub", "and", "or"};
map<string, string> arith2asm = {{"neg", "M=-M"}, {"not", "M=!M"}, {"add", "M=D+M"}, {"sub", "M=M-D"},
        {"and", "M=D&M"}, {"or", "M=D|M"}, {"eq", "JEQ"}, {"gt", "JGT"}, {"lt", "JLT"}};

void CodeWrite::writeArithmetic(string cmd) {
  fout << "\t@SP\n";
  if (cmd == "neg" || cmd == "not") {
    fout  << "\tA=M-1\n"
          << "\t" << arith2asm[cmd]  << "\n";
    return ;
  }
  fout  << "\tAM=M-1\n"
        << "\tD=M\n"
        << "\t@SP\n"
        << "\tA=M-1\n";
  if (binaryFunc.find(cmd) != binaryFunc.end()) {
    fout << "\t" << arith2asm[cmd] << "\n";
    return ;
  }
  fout  << "\tD=M-D\n"
        << "\t@THEN" << alabel << "\n"
        << "\tD;" << arith2asm[cmd] << "\n"
        << "\t@SP\n"
        << "\tA=M-1\n"
        << "\tM=0\n"
        << "\t@ENDIF" << alabel << "\n"
        << "\t0;JMP\n"
        << "(THEN" << alabel << ")\n"
        << "\t@SP\n"
        << "\tA=M-1\n"
        << "\tM=-1\n"
        << "(ENDIF" << alabel << ")\n";
  alabel++;
}

set<string> segHasSymbol = {"local", "argument", "this", "that"};
map<string, string> seg2symbol = {{"local", "@LCL"}, {"argument", "@ARG"}, {"this", "@THIS"}, {"that", "@THAT"},};
map<string, int> seg2register = {{"pointer", 3}, {"temp", 5}};

void CodeWrite::writePushPop(VMcommand cmd, string seg, int index) {
  if (cmd == C_PUSH) {
    if (seg == "constant") {
      fout  << "\t@" << index << "\n"
            << "\tD=A\n";
    }
    if (segHasSymbol.find(seg) != segHasSymbol.end()) {
      fout  << "\t@" << index << "\n"
            << "\tD=A\n"
            << "\t" << seg2symbol[seg] << "\n"
            << "\tA=D+M\n"
            << "\tD=M\n";
    }
    if (seg == "pointer" || seg == "temp") {
      fout  << "\t@R" << seg2register[seg]+index << "\n"
            << "\tD=M\n";
    }
    if (seg == "static") {
      fout  << "\t@" << fileName+"." << index << "\n"
            << "\tD=M\n";
    }
    fout  << "\t@SP\n"
          << "\tAM=M+1\n"
          << "\tA=A-1\n"
          << "\tM=D\n";
    return ;
  } else {
    if (segHasSymbol.find(seg) != segHasSymbol.end()) {
      fout  << "\t@" << index << "\n"
            << "\tD=A\n"
            << "\t" << seg2symbol[seg] << "\n"
            << "\tM=D+M\n"
            << "\t@SP\n"
            << "\tAM=M-1\n"
            << "\tD=M\n"
            << "\t" << seg2symbol[seg] << "\n"
            << "\tA=M\n"
            << "\tM=D\n"
            << "\t@" << index << "\n"
            << "\tD=A\n"
            << "\t" << seg2symbol[seg] << "\n"
            << "\tM=M-D\n";
      return ;
    }
    fout  << "\t@SP\n"
          << "\tAM=M-1\n"
          << "\tD=M\n";
    if (seg == "pointer" || seg == "temp") {
      fout  << "\t@R" << seg2register[seg]+index << "\n";
    }
    if (seg == "static") {
      fout  << "\t@" << fileName+"." << index << "\n";
    }
    fout << "\tM=D\n";
    return;
  }
}

void CodeWrite::close() {
  fout << "(END)\n\t@END\n\t0;JMP\n";
  fout.close();
}