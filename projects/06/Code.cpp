#include "Assembler.hpp"


map<string, array<bool, 3>> jump_binary = {
    {"null", {0, 0, 0}}, {"JGT", {0, 0, 1}},
    {"JEQ", {0, 1, 0}}, {"JGE", {0, 1, 1}},
    {"JLT", {1, 0, 0}}, {"JNE", {1, 0, 1}},
    {"JLE", {1, 1, 0}}, {"JMP", {1, 1, 1}}};

map<string, array<bool, 7>> comp_binary = {
    {"0",   {0, 1, 0, 1, 0, 1, 0}},
    {"1",   {0, 1, 1, 1, 1, 1, 1}},
    {"-1",  {0, 1, 1, 1, 0, 1, 0}},
    {"D",   {0, 0, 0, 1, 1, 0, 0}},
    {"A",   {0, 1, 1, 0, 0, 0, 0}}, {"M",   {1, 1, 1, 0, 0, 0, 0}},
    {"!D",  {0, 0, 0, 1, 1, 0, 1}},
    {"!A",  {0, 1, 1, 0, 0, 0, 1}}, {"!M",  {1, 1, 1, 0, 0, 0, 1}},
    {"-D",  {0, 0, 0, 1, 1, 1, 1}},
    {"-A",  {0, 1, 1, 0, 0, 1, 1}}, {"-M",  {1, 1, 1, 0, 0, 1, 1}},
    {"D+1", {0, 0, 1, 1, 1, 1, 1}},
    {"A+1", {0, 1, 1, 0, 1, 1, 1}}, {"M+1", {1, 1, 1, 0, 1, 1, 1}},
    {"D-1", {0, 0, 0, 1, 1, 1, 0}},
    {"A-1", {0, 1, 1, 0, 0, 1, 0}}, {"M-1", {1, 1, 1, 0, 0, 1, 0}},
    {"D+A", {0, 0, 0, 0, 0, 1, 0}}, {"D+M", {1, 0, 0, 0, 0, 1, 0}},
    {"D-A", {0, 0, 1, 0, 0, 1, 1}}, {"D-M", {1, 0, 1, 0, 0, 1, 1}},
    {"A-D", {0, 0, 0, 0, 1, 1, 1}}, {"M-D", {1, 0, 0, 0, 1, 1, 1}},
    {"D&A", {0, 0, 0, 0, 0, 0, 0}}, {"D&M", {1, 0, 0, 0, 0, 0, 0}},
    {"D|A", {0, 0, 1, 0, 1, 0, 1}}, {"D|M", {1, 0, 1, 0, 1, 0, 1}}};

template <size_t n>
bits<n>::bits() { fill(bit, bit+n, 0); }

template <size_t n>
bool &bits<n>::operator[](int i) { return bit[i]; }

template <size_t n>
bits<n> &bits<n>::operator=(array<bool,n> ar) {
  for (size_t i=0; i<n; i++)
    bit[i] = ar[i];
  return *this;
}


bits<3> Code::dest(string mnemonic) {
  bits<3> binary;
  if (mnemonic.find('A')!=mnemonic.npos)
    binary[0]=true;
  if (mnemonic.find('D') != mnemonic.npos)
    binary[1] = true;
  if (mnemonic.find('M') != mnemonic.npos)
    binary[2] = true;
  return binary;
}

bits<7> Code::comp(string mnemonic) {
  bits<7> binary;
  binary = comp_binary[mnemonic];
  return binary;
}

bits<3> Code::jump(string mnemonic) {
  bits<3> binary;
  binary = jump_binary[mnemonic];
  return binary;
}
