


#include <iostream>
using std::cout;
using std::cerr;
using std::cin;
using std::endl;

#include <valarray>
using std::valarray;

#include <vector>
using std::vector;

#include <string>
using std::string;

#include <list>
using std::list;

#include <limits>

#include <fstream>
using std::fstream;

#include <utility>
using std::pair;

#include <map>
using std::map;

//#include <cmath>

#include "TROOT.h"
#include "TRint.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TRotation.h"
#include "Math/Vector3D.h"
using ROOT::Math::XYZVector;

#include "Math/Point3D.h"
using ROOT::Math::XYZPoint;

#include "Math/Transform3D.h"
using ROOT::Math::Transform3D;

#include "Math/RotationX.h"
using ROOT::Math::RotationX;
#include "Math/RotationY.h"
using ROOT::Math::RotationY;
#include "Math/RotationZ.h"
using ROOT::Math::RotationZ;
#include "Math/Rotation3D.h"
using ROOT::Math::Rotation3D;


#include "PndDrcPhoton.h"
#include "PndDrcSurfPolyFlat.h"
#include "PndDrcOptReflSilver.h"
#include "PndDrcOptReflPerfect.h"
#include "PndDrcOptReflGeffcken.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptMatMarcol7.h"
#include "PndDrcOptMatVacuum.h"
#include "PndDrcOptMatMgF2.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"
#include "PndDrcOptBrick.h"
#include "PndDrcOptCylLens.h"
#include "PndDrcOptLens.h"

int main(int argc, char *argv[])
{

  static const double kPi=3.1415926535;

  

  PndDrcOptReflGeffcken refl;
  refl.SetVerbosity(5);
  

  PndDrcOptMatLithotecQ0 lithotec;
  PndDrcOptMatMgF2       mgf2;
  
  cout<<" lambda loss --> debug.dat"<<endl;
  
  PndDrcOptBrick brick1(10,10,10);
  brick1.Surface("side1")->SetReflectivity(refl);
  brick1.SetOptMaterial(lithotec);
  brick1.SetName("brik1");

  PndDrcOptBrick brick2(10,10,10);
  //brick2.Surface("side1")->SetReflectivity(PndDrcOptReflGeffcken());
  brick2.SetOptMaterial(PndDrcOptMatVacuum());
  brick2.SetName("brik2");




  PndDrcPhoton ph;
  
  fstream out;
  out.open("debug.dat",std::ios::out);
  
  
  for (double lambda=200; lambda<600; lambda+=1)
    {
      double theta=0; 
      ph.SetPosition(XYZPoint(0,0,5));
      double z = -cos(theta*kPi/180);
      double y = sin(theta*kPi/180);
      ph.SetDirection(XYZVector(0,y,z));
      ph.SetWavelength(lambda);
      ph.SetDevice(&brick2); 
      
      double refl_prob = refl.ReflProb(ph,XYZVector(0,0,-1),lithotec.RefIndex(lambda),Drc::ReflIn);
      
      
      cout<<lambda<<" "<<refl_prob<<" "<<mgf2.RefIndex(lambda)<<endl;
      out<< lambda<<" "<<refl_prob<<endl;
      
    }
  out.close();
  
    
  
  

  return EXIT_SUCCESS;

}
