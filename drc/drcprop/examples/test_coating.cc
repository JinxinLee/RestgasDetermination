


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
#include "PndDrcOptMatBK7.h"
#include "PndDrcOptMatMarcol7.h"
#include "PndDrcOptMatVacuum.h"
#include "PndDrcOptMatMgF2.h"
#include "PndDrcOptMatTiO2.h"
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
  

  PndDrcOptMatBK7        bk7;
  PndDrcOptMatMgF2       mgf2;
  PndDrcOptMatTiO2       tio2;
  
  cout<<" lambda loss --> debug.dat"<<endl;
  


  // ------> z
  //
  // +--------+ X+--------+
  // | brick2 | X| brick1 |
  // +--------+ X+--------+


  PndDrcOptBrick brick1(10,10,10);
  brick1.Surface("side1")->SetReflectivity(refl);
  brick1.SetOptMaterial(bk7);
  brick1.SetName("brick1");
  brick1.AddTransform(Transform3D(XYZVector(0,0,10)));

  PndDrcOptBrick brick2(10,10,10);
  brick2.Surface("side1")->SetReflectivity(refl);
  brick2.SetOptMaterial(PndDrcOptMatVacuum());
  brick2.SetName("brick2");
  brick2.AddTransform(Transform3D(XYZVector(0,0,-10)));

  PndDrcOptDevSys opt_system;
  opt_system.AddDevice(brick1);
  opt_system.AddDevice(brick2);



  PndDrcPhoton ph;
  
  fstream out;
  out.open("debug.dat",std::ios::out);
  
  
  for (double lambda=400; lambda<701; lambda+=1)
    {
      double theta=0; 
      ph.SetPosition(XYZPoint(0,0,-1));
      double z = cos(theta*kPi/180);
      double y = sin(theta*kPi/180);
      ph.SetDirection(XYZVector(0,y,z));
      ph.SetWavelength(lambda);
      ph.SetDevice(&brick2); 
      
      double refl_prob = refl.ReflProb(ph,XYZVector(0,0,-1),bk7.RefIndex(lambda),Drc::ReflIn);
      
      
      cout<<lambda<<" "<<refl_prob<<endl;
      out<< lambda<<" "<<refl_prob<<endl;
      
    }
  out.close();
  
    
  
  

  return EXIT_SUCCESS;

}
