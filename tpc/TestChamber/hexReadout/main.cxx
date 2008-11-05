#include "Mapping.h"


int main(){
  
  Mapping m;
  m.read("mapping.txt");
  m.write("mapping2.txt");
  
  double x,y;
  m.map(4,48,x,y);
  std::cout << x << " " << y << std::endl;

}
