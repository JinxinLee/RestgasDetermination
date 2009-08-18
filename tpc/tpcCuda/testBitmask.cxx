#include <iostream>
#include <cstdlib>

typedef unsigned int uint;

bool getBit(char* c, int n) {
  return (c[n>>3] & (1 << (n & 7)))!=0;
}

void setBit(char* c, int n) {
  c[n>>3] = c[n>>3] | (1 << (n & 7 ));
}

void clearBit(char* c, int n) {
  c[n>>3] = c[n>>3] & ~(1 << (n & 7 ));
}


int main() {

  int LENGTH = 16;

  char* bitstring = (char*) malloc(10*sizeof(char));
  bitstring[0] = 'a';
  bitstring[1] = 'b';  
  bitstring[2] = 'c';
  
  //setBit(bitstring,10);
  clearBit(bitstring,9);
  
  std::cout<<"\nSize of uint: "<<sizeof(uint)<<std::endl;
  std::cout<<"Size of bool: "<<sizeof(bool)<<"\n\n"<<std::endl;

  std::cout<<"FROM LSB to MSB: \n"<<std::endl;

  for(int n=0; n<LENGTH; n++) {
    if(n%8==0 && n>0)
      std::cout<<" ";
    std::cout<<getBit(bitstring,n);
  } 
  std::cout<<std::endl;


}



