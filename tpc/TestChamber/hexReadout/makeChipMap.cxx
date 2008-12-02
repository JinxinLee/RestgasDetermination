#include<iostream>


int main(){
  for(int i=0;i<128;++i){
    if(i%2==0)    std::cout << i+45 << " " << 1 << std::endl;
    else    std::cout << i+45 << " " << 3 << std::endl;
  }
  for(int i=128;i<256;++i){
    if(i%2==0)    std::cout << i+45 << " " << 2 << std::endl;
    else    std::cout << i+45 << " " << 4 << std::endl;
  }
}
