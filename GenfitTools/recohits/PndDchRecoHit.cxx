// -------------------------------------------------------------------------
// -----                   PndDchRecoHit source file                   -----
// -----             Created 5.03.2008  by S.Neubert, A. Wronska       -----
// -------------------------------------------------------------------------

// Pnd Headers 
#include "PndDchRecoHit.h"
#include "PndDchCylinderHit.h"
#include "LSLTrackRep.h"
//#include "GeaneTrackRep.h"
#include "GFDetPlane.h"

// C/C++ Headers 
#include <assert.h>
#include "math.h"

//ROOT includes
#include "TMath.h"

ClassImp(PndDchRecoHit)


PndDchRecoHit::~PndDchRecoHit()
{}

PndDchRecoHit::PndDchRecoHit()
  : PlanarRecoHit(NparHitRep)
{}

PndDchRecoHit::PndDchRecoHit(double r, double wireposx, 
			     double angle, double z,
			     double sigr)
  : PlanarRecoHit(NparHitRep), _wirepos(wireposx)
{
  _sangle=TMath::Sin(angle);
  _cangle=TMath::Cos(angle);

  fHitCoord[0][0] = r;
  fPolicy.setDetPlane(GFDetPlane(TVector3(0,0,z),// position of wi in glo coord. at y=0
		       TVector3(_cangle,_sangle,0),
		       TVector3(-_sangle,_cangle,0)));
  fHitCov[0][0] = sigr*sigr;
}



PndDchRecoHit::PndDchRecoHit(const PndDchCylinderHit* cylHit)
  : PlanarRecoHit(NparHitRep)
{
  //  std::cout<<"I create PndDchRecoHit out of a CylinderHit"<<std::endl;
  _sangle = TMath::Sin(cylHit->GetWireAngle());
  _cangle = TMath::Cos(cylHit->GetWireAngle());
  fHitCoord[0][0] = cylHit->GetDistance();
  Double_t z = cylHit->GetWireZcoordGlobal();
  _wirepos = cylHit->GetWireXcoordLocal();
  fPolicy.setDetPlane(GFDetPlane(TVector3(0,0,z),// position of wi in glo coord. at y=0
		       TVector3(_cangle,_sangle,0),
		       TVector3(-_sangle,_cangle,0)));
  Double_t sigr = cylHit->GetDistanceError();
  fHitCov[0][0] = sigr*sigr;

//   cylHit->Print();
//   this->Print();
}


GFAbsRecoHit* PndDchRecoHit::clone(){
  return new PndDchRecoHit(*this);
}


TMatrixT<double> PndDchRecoHit::getHMatrix(const GFAbsTrackRep* stateVector)
{
  std::cout<<"I set up the H matrix now with NparHitRep = "<<NparHitRep<<std::endl;
  assert(stateVector!=NULL);
  if (dynamic_cast<const LSLTrackRep*>(stateVector) != NULL) {
    // LSLTrackRep (x,y,x',y',q/p)
    TMatrixT<double> HMatrix(NparHitRep,5);
   
    HMatrix[0][0] = _cangle;
    HMatrix[0][1] = _sangle;
    HMatrix[0][2] = 0.;
    HMatrix[0][3] = 0.;
    HMatrix[0][4] = 0.;

    //     fHMatrix[1][0] = _sangle;
    //     fHMatrix[1][1] = _cangle;
    //     fHMatrix[1][2] = 0.;
    //     fHMatrix[1][3] = 0.;
    //     fHMatrix[1][4] = 0.;
    return HMatrix;
  }
  else {
    std::cerr << "PndDchRecoHit can only handle state"
              << " vectors of type LSLTrackRep  -> abort" 
	      << std::endl;
    throw;
  }
 
}

TMatrixT<double> PndDchRecoHit::residualVector(const GFAbsTrackRep* stateVector,
					       const TMatrixT<double>& state, 
					       const GFDetPlane& d){
  TMatrixT<double> H=getHMatrix(stateVector);
  TMatrixT<double> trackpos=(H*state);
  Double_t res1 = _wirepos+fHitCoord[0][0]-trackpos[0][0];
  Double_t res2 = _wirepos-fHitCoord[0][0]-trackpos[0][0];
  TMatrixT<double> res(1,1);
  if(fabs(res1)>fabs(res2)){
    res[0][0]=res2;
  }
  else res[0][0]=res1;
  
  return res;
}

void  PndDchRecoHit::Print(){
  std::cout<<"hitCoord:"; fHitCoord.Print();
  //  std::cout<<"hit HMatrix:";getHMatrix().Print();
  std::cout<<"hit GFDetPlane:";getDetPlane(0).Print();
  std::cout<<"\n hitCov:";getHitCov(getDetPlane(0)).Print();
  
}
