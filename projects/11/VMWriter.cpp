#include "JackAnalyzer.hpp"


string seg2str[8] = {"constant", "argument", "local", "static", "this", "that", "pointer", "temp"};
string cmd2str[9] = {"add", "sub", "neg", "eq", "gt", "lt", "and", "or", "not"};

VMWriter::VMWriter(string directory, string name) {
  file_name = name;
  fout.open(directory+name+".vm");
}

void VMWriter::setClassName(string name) {
  class_name = name;
  if (class_name != file_name) {
    cout << "class name doesn't match file name: class " << class_name << ", file " << file_name << endl;
    this->close();
    exit(1);
  }
}

void VMWriter::setIsMethod(bool method) {
  isMethod = method;
}

string VMWriter::getClassName() {
  return class_name;
}

void VMWriter::writePush(Segment seg, int index) {
  if (isMethod && seg == SEG_ARG)
    index++;
  fout << "push " << seg2str[seg] << " " << index << endl;
}

void VMWriter::writePop(Segment seg, int index) {
  if (isMethod && seg == SEG_ARG)
    index++;
  fout << "pop " << seg2str[seg] << " " << index << endl;
}

void VMWriter::writeArithmetic(ArithCommand cmd) {
  fout << cmd2str[cmd] << endl;
}

void VMWriter::writeLabel(string label) {
  fout << "label " << label << endl;
}

void VMWriter::writeGoto(string label) {
  fout << "goto " << label << endl;
}

void VMWriter::writeIf(string label) {
  fout << "if-goto " << label << endl;
}

void VMWriter::writeCall(string name, int nArgs) {
  fout << "call " << name << " " << nArgs << endl;
}

void VMWriter::writeFunction(string name, int nLocals) {
  fout << "function " << class_name << "." << name << " " << nLocals << endl;
}

void VMWriter::writeReturn() {
  fout << "return" << endl;
}

void VMWriter::close() {
  fout.close();
}
