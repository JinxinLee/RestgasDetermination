//g++ particleGun.cxx -o particleGun `root-config --glibs --cflags`
#include<iostream>
#include<sstream>
#include<fstream>

#include "TRandom.h"
#include "TVector3.h"

int main(int argc,char **argv){
  std::string args[argc-1];
  for(int i=0;i<argc-1;++i){
    args[i]=argv[i+1];
  }
  std::istringstream istr(args[0]);
  int nev = -1;
  istr >> nev;
  
  std::ofstream f(args[1].c_str());

  TRandom ran(0);

  for(int iev=0;iev<nev;++iev){
    TVector3 pos,mom;
    pos.SetX(ran.Uniform(-10.,20.));
    pos.SetY(ran.Uniform(-1.,2.));
    pos.SetZ(-5.);

    mom.SetZ(1.);
    mom.SetX(ran.Gaus(0.,.7));
    mom.SetY(ran.Gaus(0.,.7));

    f << "1 " << iev << " " << pos.X() << " "
      << pos.Y() << " " << pos.Z() << std::endl;
    f << "13 " << mom.X() << " "
      << mom.Y() << " " << mom.Z() << std::endl;

  }
  f.close();

}
