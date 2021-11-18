#include "VMtranslator.hpp"


filesystem::filesystem(char* path) {
  string p = path;
  while(p.back() == '/')
    p.pop_back();
  name = p;
  directory = p;
  if (name.find_last_of('/')!=p.npos)
    name = name.substr(name.find_last_of("/") + 1);
  name = name.substr(0, name.find_last_of("."));
  if (directory.find('.') != directory.npos) {
    directory = directory.substr(0,directory.find_last_of("/"));
    directory.push_back('/');
    files.push_back(name+".vm");
    return ;
  }
  directory.push_back('/');
  DIR *dir;
  struct dirent *diread;
  if ((dir = opendir(path)) != nullptr) {
    while ((diread = readdir(dir)) != nullptr) {
      files.push_back(diread->d_name);
    }
    closedir(dir);
  } else {
    cout << "can't open directory: " << path << endl;
    exit(0);
  }
}