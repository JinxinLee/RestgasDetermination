
#include "TROOT.h"
#include "TRint.h"
#include "TFile.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TRotation.h"
#include "TH1.h"
#include "TH2D.h"
#include "TAxis.h"

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


#include "PndDrcSurfPolyFlat.h"
#include "PndDrcOptReflPerfect.h"
#include "PndDrcOptReflNone.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"
#include "PndDrcOptBrick.h"


void setup_geometry(PndDrcOptDevManager* manager)
{
  double bar_height = 17.5;
  double bar_width  = 35.0/2;
  double bar_length = 1250;


  PndDrcOptBrick bar(bar_width/2,bar_height/2,bar_length/2);
  bar.SetOptMaterial(PndDrcOptMatLithotecQ0());
  bar.SetName("bar");
  bar.Surface("side6")->SetReflectivity(PndDrcOptReflPerfect());
  // shift bar in z to have bars end at 0,0,0
  bar.AddTransform(Transform3D(XYZVector(0,0,bar_length/2)));

  bar.Surface("side1")->SetName("out");
  


  PndDrcOptBrick box(300/2,300/2,300/2); // expansion box  600x600x300mm
  box.SetOptMaterial(PndDrcOptMatLithotecQ0());
  box.SetName("box");
  // eliminate unwanted ambiguities
  // shift bar in z to have bars end at 0,-250,0
  box.AddTransform(Transform3D(XYZVector(0,270/2,-300/2)));
  box.Surface("side1")->SetPixel();
  box.Surface("side1")->SetName("pixel");
  

  PndDrcOptDevSys opt_system;
  opt_system.AddDevice(bar);
  opt_system.AddDevice(box);
  opt_system.CoupleDevice("bar","box","out","side6");


  manager->AddDeviceSystem(opt_system);

 
  
}
