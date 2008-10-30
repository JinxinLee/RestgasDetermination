// -------------------------------------------------------------------------
// -----                   PndDchRecoHit2 source file                  -----
// -----                  Created 29.09.2008  by A. Wronska            -----
// -------------------------------------------------------------------------

// Pnd Headers 
#include "PndDchRecoHit2.h"
#include "PndDchCylinderHit.h"
#include "GeaneTrackRep.h"
#include "DetPlane.h"
#include "CbmGeanePro.h"
#include "CbmGeaneUtil.h"
#include "FitterExceptions.h"

//ROOT includes
#include "TMath.h"

// C/C++ Headers 
#include <assert.h>
#include <exception>


ClassImp(PndDchRecoHit2)

//------------------------------------------------------------------------------
PndDchRecoHit2::PndDchRecoHit2(const PndDchCylinderHit* cylHit)
//  : WirepointRecoHit(NparHitRep)
{
  //  std::cout<<"I create PndDchRecoHit2 out of a CylinderHit"<<std::endl;
  _sangle = TMath::Sin(cylHit->GetWireAngle());
  _cangle = TMath::Cos(cylHit->GetWireAngle());
  TVector2 end1 = cylHit->GetWireEnd1();
  TVector2 end2 = cylHit->GetWireEnd2();
  Double_t z = cylHit->GetWireZcoordGlobal();
  
 _hitCoord[0][0] = end1.X();
 _hitCoord[1][0] = end1.Y();
 _hitCoord[2][0] = z;
 _hitCoord[3][0] = end2.X();
 _hitCoord[4][0] = end2.Y();
 _hitCoord[5][0] = z;
 _hitCoord[6][0] = cylHit->GetDistance();
 //_hitCoord[7][0] = z; 
 _hitCoord[7][0] = 0.0; // should be the coordinate along wire 

  for(int i = 0; i < NparHitRep; i++) 
    for(int j = 0; j < NparHitRep; j++) 
      _hitCov[i][j] = 0.;
  //  Double_t sigr = cylHit->GetDistanceError();
  Double_t sigr = 0.02;
  _hitCov[6][6] = sigr*sigr;
  Double_t sigz = 1.5;
  _hitCov[7][7] = sigz * sigz; 

  //cylHit->Print();
  //Print();
}

//------------------------------------------------------------------------------
// void PndDchRecoHit2::setHMatrix(const AbsTrackRep* stateVector,
// 			 const TMatrixT<double>& state)
// {
//   std::cout<<"I set up the H matrix now with NparHitRep = "<<NparHitRep<<std::endl;
//   assert(stateVector!=NULL);
//   if (dynamic_cast<const GeaneTrackRep*>(stateVector) != NULL) {
//     _HMatrix.ResizeTo(2,5);
   
//     _HMatrix[0][0] = 0.;
//     _HMatrix[0][1] = 0.;
//     _HMatrix[0][2] = 0.;
//     _HMatrix[0][3] = 1.;
//     _HMatrix[0][4] = 0.;

//     _HMatrix[1][0] = 0.;
//     _HMatrix[1][1] = 0.;
//     _HMatrix[1][2] = 0.;
//     _HMatrix[1][3] = 0.;
//     _HMatrix[1][4] = 1.;
//     //     _HMatrix[0][0] = _cangle;
//     //     _HMatrix[0][1] = _sangle;
//     //     _HMatrix[0][2] = 0.;
//     //     _HMatrix[0][3] = 0.;
//     //     _HMatrix[0][4] = 0.;
//     //     _HMatrix[1][0] = _sangle;
//     //     _HMatrix[1][1] = _cangle;
//     //     _HMatrix[1][2] = 0.;
//     //     _HMatrix[1][3] = 0.;
//     //     _HMatrix[1][4] = 0.;
//   }  else {
//     std::cerr << "PndDchRecoHit2 can only handle state"
//               << " vectors of type GeaneTrackRep  -> abort" 
// 	      << std::endl;
//     throw;
//   }
// }
//------------------------------------------------------------------------------
void  PndDchRecoHit2::Print(){
  std::cout<<"hitCoord:"; _hitCoord.Print();
  // std::cout<<"hit HMatrix:";getHMatrix().Print();
//   std::cout<<"hit DetPlane:";getDetPlane(0).Print();
//   std::cout<<"\n hitCov:";getHitCov(getDetPlane(0)).Print();
  std::cout<<"EOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOH"<<std::endl;
  
}
