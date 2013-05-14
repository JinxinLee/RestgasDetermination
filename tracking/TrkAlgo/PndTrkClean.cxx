//
// PndTrkClean.cxx
// 
// 
// 
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkClean.h"

#include "PndTrkTools.h"
#include "TArc.h"
#include <iostream>


using namespace std;

PndTrkClean::PndTrkClean() {
  fGeoH = PndGeoHandling::Instance();
}

PndTrkClean::~PndTrkClean() {}

// /**
// LAY NAME

// barrel, layers from innermost to outermost:
// 1   PixeloBlo1
// 6   PixeloBlo2
// 15  StripoBl3o(Silicon)
// 17  StripoBl4o(Silicon)

// forward wheels, inner, increasing z:
// left
// 2   PixeloSdkoco(Silicon)_1
// 3   PixeloSdkoco(Silicon)_2
// 7   PixeloLdkoco(Silicon)_1
// 8   PixeloLdkoco(Silicon)_2
// 9   PixeloLdkoco(Silicon)_3
// 10  PixeloLdkoco(Silicon)_4

// right
// 4   PixeloSdkoco(Silicon)_3
// 5   PixeloSdkoco(Silicon)_4
// 11  PixeloLdkoco(Silicon)_5
// 12  PixeloLdkoco(Silicon)_6
// 13  PixeloLdkoco(Silicon)_7
// 14  PixeloLdkoco(Silicon)_8

// forward wheels, last two, outer:
// left
// 16  Fwdo(Silicon)_1 
// right
// 20  StripoLdko5-6oTrapSo(Silicon)_1


// **/
// int PndTrkClean::FindMvdLayer(int sensorID) {
//   TString geoPath;
//   int Layer = 0;

//   bool flag = true;

//   geoPath=fGeoH->GetPath(sensorID);
//   cout << "path " << geoPath << endl;
//   TVector3 o, u, v;
//   fGeoH->GetOUVPath(geoPath, o, u, v);
//   o.Print();
//   u.Print();
//   v.Print();


//   if (flag && geoPath.Contains("PixeloBlo1")){Layer=1;flag=false;}
//   if (flag && geoPath.Contains("PixeloSdko(Silicon)_1")){Layer=2;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloSdkoco(Silicon)_1")){Layer=2;flag=false;}
//   if (flag && geoPath.Contains("PixeloSdko(Silicon)_2")){Layer=3;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloSdkoco(Silicon)_2")){Layer=3;flag=false;}
//   if (flag && geoPath.Contains("PixeloSdko(Silicon)_3")){Layer=4;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloSdkoco(Silicon)_3")){Layer=4;flag=false;}
//   if (flag && geoPath.Contains("PixeloSdko(Silicon)_4")){Layer=5;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloSdkoco(Silicon)_4")){Layer=5;flag=false;}
//   if (flag && geoPath.Contains("PixeloBlo2")){Layer=6;flag=false;}
//   if (flag && geoPath.Contains("PixeloLdkoio(Silicon)_1")){Layer=7;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_1")){Layer=7;flag=false;}
//   if (flag && geoPath.Contains("PixeloLdkoiio(Silicon)_1")){Layer=8;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_2")){Layer=8;flag=false;}
//   if (flag && geoPath.Contains("PixeloLdkoiiio(Silicon)_1")){Layer=9;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_3")){Layer=9;flag=false;}
//   if (flag && geoPath.Contains("PixeloLdkoiiio(Silicon)_2")){Layer=10;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_4")){Layer=10;flag=false;}
//   if (flag && geoPath.Contains("PixeloLdkoiio(Silicon)_2")){Layer=11;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_5")){Layer=11;flag=false;}
//   if (flag && geoPath.Contains("PixeloLdkoio(Silicon)_2")){Layer=12;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_6")){Layer=12;flag=false;}
//   if (flag && geoPath.Contains("PixeloLdkoiiio(Silicon)_4")){Layer=13;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_7")){Layer=13;flag=false;}
//   if (flag && geoPath.Contains("PixeloLdkoiiio(Silicon)_3")){Layer=14;flag=false;}  //naming after MVD2.2
//   if (flag && geoPath.Contains("PixeloLdkoco(Silicon)_8")){Layer=14;flag=false;}
    
//   if (flag && geoPath.Contains("StripoBl3o(Silicon)")){Layer=15;flag=false;}
//   if (flag && geoPath.Contains("Fwdo(Silicon)_1")){Layer=16;flag=false;}
//   if (flag && geoPath.Contains("StripoBl4o(Silicon)")){Layer=17;flag=false;}
//   //if (flag && geoPath.Contains("Fwdo(Silicon)_2")){Layer=10;flag=false;}
    
//   if (flag && geoPath.Contains("StripoLdkoTrapSoRingAoSilicon_1")){Layer=18;flag=false;}
//   if (flag && geoPath.Contains("StripoLdkoTrapSoRingAoSilicon_2")){Layer=19;flag=false;}
//   if (flag && geoPath.Contains("StripoLdkoTrapSoRingBoSilicon_1")){Layer=18;flag=false;}
//   if (flag && geoPath.Contains("StripoLdkoTrapSoRingBoSilicon_2")){Layer=19;flag=false;}
//   if (flag && geoPath.Contains("StripoLdko5-6oTrapSo(Silicon)_1")){Layer=20;flag=false;}
//   return Layer;
// }

/** MY ONE
LAY NAME

left side
barrel, layers from innermost to outermost:
1   PixeloBlo1*
3   PixeloBlo2*
5   StripoBl3o(Silicon)*
7   StripoBl4o(Silicon)*

forward wheels, inner, increasing z:
9   PixeloSdkoco(Silicon)_1
11  PixeloSdkoco(Silicon)_2
13  PixeloLdkoco(Silicon)_1
15  PixeloLdkoco(Silicon)_2
17  PixeloLdkoco(Silicon)_3
19  PixeloLdkoco(Silicon)_4

forward wheels, last two, outer:
21  Fwdo(Silicon)_1 
23  Fwdo(Silicon)_1 

right
barrel, layers from innermost to outermost:
2   PixeloBlo1*
4   PixeloBlo2*
6   StripoBl3o(Silicon)*
8   StripoBl4o(Silicon)*

forward wheels, inner, increasing z:
10  PixeloSdkoco(Silicon)_3
12  PixeloSdkoco(Silicon)_4
14  PixeloLdkoco(Silicon)_5
16  PixeloLdkoco(Silicon)_6
18  PixeloLdkoco(Silicon)_7
20  PixeloLdkoco(Silicon)_8

forward wheels, last two, outer:
22  Fwdo(Silicon)_2
24  Fwdo(Silicon)_2


**/
int PndTrkClean::FindMvdLayer(int sensorID) {
  TString geoPath;
  int Layer = 0;

  geoPath=fGeoH->GetPath(sensorID);
  //  cout << "path " << geoPath << endl;
  TVector3 o, u, v;
  fGeoH->GetOUVPath(geoPath, o, u, v);
//   o.Print();
//   u.Print();
//   v.Print();

  // barrel
  // left
  if (geoPath.Contains("PixeloBlo1")) {
    Layer=1;
    // right
    if(o.X() < 0) Layer += 1;
  }
  else if (geoPath.Contains("PixeloBlo2")) {
    Layer=3;
    // right
    if(o.X() < 0) Layer += 1;
  }
  else if (geoPath.Contains("StripoBl3o(Silicon)")) {
    Layer=5;
    // right
    if(o.X() < 0) Layer += 1;
  }
  else if (geoPath.Contains("StripoBl4o(Silicon)")) {
    Layer=7;
    // right
    if(o.X() < 0) Layer += 1;
  }
  // inner wheels
  // left
  else if (geoPath.Contains("PixeloSdkoco(Silicon)_1")){Layer=9;}
  else if (geoPath.Contains("PixeloSdkoco(Silicon)_2")){Layer=11;}
  // right0
  else if (geoPath.Contains("PixeloSdkoco(Silicon)_3")){Layer=10;}
  else if (geoPath.Contains("PixeloSdkoco(Silicon)_4")){Layer=12;}
  //
  // left
  else if (geoPath.Contains("PixeloLdkoco(Silicon)_1")){Layer=13;}
  else if (geoPath.Contains("PixeloLdkoco(Silicon)_2")){Layer=15;}
  else if (geoPath.Contains("PixeloLdkoco(Silicon)_3")){Layer=17;}
  else if (geoPath.Contains("PixeloLdkoco(Silicon)_4")){Layer=19;}
  // right
  else if (geoPath.Contains("PixeloLdkoco(Silicon)_5")){Layer=14;}
  else if (geoPath.Contains("PixeloLdkoco(Silicon)_6")){Layer=16;}
  else if (geoPath.Contains("PixeloLdkoco(Silicon)_7")){Layer=18;}
  else if (geoPath.Contains("PixeloLdkoco(Silicon)_8")){Layer=20;}
  
  // last two wheels
  else if (geoPath.Contains("Fwdo(Silicon)_1")){  // left
    // 1st 
    Layer=21;
    // 2nd
    if(o.Z() > 20) Layer += 2;
  }
  else if (geoPath.Contains("Fwdo(Silicon)_2")){ // right
    // 1st 
    Layer=22;
    // 2nd
  if(o.Z() > 20) Layer += 2;
  }
  else cout << "I have no idea" << endl;


  return Layer;
}
ClassImp(PndTrkClean)
 
