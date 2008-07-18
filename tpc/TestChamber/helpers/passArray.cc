#include<iostream>




void func(double *par){
  std::cout << par[0] << std::endl;
  std::cout << par[1] << std::endl;
  std::cout << par[2] << std::endl;
  std::cout << par[3] << std::endl;
  std::cout << "this should go wrong" << std::endl;
  std::cout << par[4] << std::endl;
}


int main(){
  double x[4];
  x[0]=1.;  x[1]=2.;  x[2]=3.;  x[3]=4.;
  func(x);
}
