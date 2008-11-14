#include "Mapping.h"
#include<string>
#include<iostream>

int main(int argc, char* argv[]) {

  if(argc != 3) {
    std::cerr << "NumArgs is not 2. Args: infile outfile" << std::endl;
	throw;
  }
  std::string infile(argv[1]);
  std::string outfile(argv[2]);


  Mapping m;
  m.read(infile.c_str());
  m.write(outfile.c_str());
  
  double x,y;
  m.map(4,48,x,y);
  std::cout << x << " " << y << std::endl;

}
