//--------------------------------------------------------------------------
//----                     PndRpcHit                                    ----
//----                Created Everard Cordier 14/09/05                  ----
//----                Modified by A.Galoyan  4/12/07                   ----
//--------------------------------------------------------------------------

#include <iostream>
using namespace std;

#include "PndRpcHit.h"


// -----   Default constructor   -------------------------------------------

PndRpcHit::PndRpcHit()
{
    fTime = 0.;
//  fFlag = 1;
    fGap=0;
    fCell=0;
    fModule=0;
    fdetectorID=0;
    fIndex = 0;
 }


// -----   Standard constructor   ------------------------------------------

PndRpcHit::PndRpcHit(Int_t detectorID, Int_t Gap, Int_t Cell, Int_t Module, 
TVector3 pos, TVector3 dpos,  Double_t time, Int_t index)
: CbmHit(detectorID, pos, dpos, index)
{
    fTime = time;
//    fFlag = flag;
    fGap=Gap;
    fCell=Cell;
    fModule=Module;
    fdetectorID=detectorID;   
    fIndex = index;
    fpos=pos;
    fdpos=dpos;
}


// -----   Destructor   ----------------------------------------------------
PndRpcHit::~PndRpcHit() { }


// -----  Print  -----------------------------------------------------------
void PndRpcHit::Print(const Option_t* opt) const
{
  cout << "-I- RpcTofHit" << endl;
  cout << "    DetectorID: " <<  fdetectorID << endl;
  cout << "    Position: (" << fX
       << ", " << fY
       << ", " << fZ << ") cm"
       << endl;

  cout<< "Gap" << fGap << endl; 
  cout<< "Module" << fModule << endl; 
  cout<< "Cell" << fCell << endl; 
  cout << "Index of Hit " << fIndex << endl;
  cout << "    Position error: (" << fDx
       << ", " << fDy
       << ", " << fDz << ") cm"
       << endl;
  cout << "    Time: " << fTime << " ns" 
       << endl;
}
// -------------------------------------------------------------------------


ClassImp(PndRpcHit)
