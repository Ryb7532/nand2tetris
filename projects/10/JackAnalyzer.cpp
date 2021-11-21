#include "JackAnalyzer.hpp"


int main(int argc, char* argv[]) {
  filesystem fs(argv[1]);
  for (string f: fs.files){
    string ext = f.substr(f.find_last_of("."));
    if (ext != ".jack") {
      cout << "extension (" << ext << ") is not .jack\n";
      continue;
    }
    string file_name = f.substr(0, f.find_last_of("."));
    CompilationEngine compile(fs.directory+f, fs.directory+file_name+".xml");
  }
  return 0;
}