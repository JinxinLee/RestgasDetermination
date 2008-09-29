// -------------------------------------------------------------------------
// -----                   PndDchRecoHit source file                   -----
// -----             Created 5.03.2008  by S.Neubert, A. Wronska       -----
// -------------------------------------------------------------------------

// Pnd Headers 
#include "PndDchRecoHit.h"
#include "PndDchCylinderHit.h"
#include "LSLTrackRep.h"
//#include "GeaneTrackRep.h"
#include "DetPlane.h"

// C/C++ Headers 
#include <assert.h>

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

  _hitCoord[0][0] = r;
  setDetPlane(DetPlane(TVector3(0,0,z),// position of wi in glo coord. at y=0
		       TVector3(_cangle,_sangle,0),
		       TVector3(-_sangle,_cangle,0)));
  _hitCov[0][0] = sigr*sigr;
}



PndDchRecoHit::PndDchRecoHit(const PndDchCylinderHit* cylHit)
  : PlanarRecoHit(NparHitRep)
{
  std::cout<<"I create PndDchRecoHit out of a CylinderHit"<<std::endl;
  _sangle = TMath::Sin(cylHit->GetWireAngle());
  _cangle = TMath::Cos(cylHit->GetWireAngle());
  _hitCoord[0][0] = cylHit->GetDistance();
  Double_t z = cylHit->GetWireZcoordGlobal();
  _wirepos = cylHit->GetWireXcoordLocal();
  setDetPlane(DetPlane(TVector3(0,0,z),// position of wi in glo coord. at y=0
		       TVector3(_cangle,_sangle,0),
		       TVector3(-_sangle,_cangle,0)));
  Double_t sigr = cylHit->GetDistanceError();
  _hitCov[0][0] = sigr*sigr;

  cylHit->Print();
  this->Print();
}


AbsRecoHit* PndDchRecoHit::clone(){
  return new PndDchRecoHit(*this);
}


void PndDchRecoHit::setHMatrix(const AbsTrackRep* stateVector,
			 const TMatrixT<double>& state)
{
  std::cout<<"I set up the H matrix now with NparHitRep = "<<NparHitRep<<std::endl;
  assert(stateVector!=NULL);
  if (dynamic_cast<const LSLTrackRep*>(stateVector) != NULL) {
    // LSLTrackRep (x,y,x',y',q/p)
    _HMatrix.ResizeTo(NparHitRep,5);
   
    _HMatrix[0][0] = _cangle;
    _HMatrix[0][1] = _sangle;
    _HMatrix[0][2] = 0.;
    _HMatrix[0][3] = 0.;
    _HMatrix[0][4] = 0.;

    //     _HMatrix[1][0] = _sangle;
    //     _HMatrix[1][1] = _cangle;
    //     _HMatrix[1][2] = 0.;
    //     _HMatrix[1][3] = 0.;
    //     _HMatrix[1][4] = 0.;
  }
  else {
    std::cerr << "PndDchRecoHit can only handle state"
              << " vectors of type LSLTrackRep  -> abort" 
	      << std::endl;
    throw;
  }
 
}

TMatrixT<double> PndDchRecoHit::residualVector(AbsTrackRep* stateVector,
			      const TMatrixT<double>& state)
{  // calculate two residuals (ambiguity) return smaller one
  std::cout<<"........Inside of  PndDchRecoHit::residualVector(...)\n rep: (stateVector):"<<std::endl;
  stateVector->Print();
  std::cout<<" state (matrix):"<<std::endl;
  TMatrixT<double> trackpos=(_HMatrix*state);
  Double_t trackr=trackpos[0][0];
  std::cout<<"_wirepos="<<_wirepos<<"  hitCoord[0][0]="<<_hitCoord[0][0]<<"  trackpos[0][0]="<<trackpos[0][0]<<std::endl;
  Double_t res1 = _wirepos+_hitCoord[0][0]-trackpos[0][0];
  Double_t res2 = _wirepos-_hitCoord[0][0]-trackpos[0][0];
  TMatrixT<double> res(1,1);
  if(fabs(res1)>fabs(res2)){
    res[0][0]=res2;
  }
  else res[0][0]=res1;

  std::cout<<".......res1 = \t"<<res1<<"\t res2 = "<<res2<<"\t res = "<<res[0][0]<<std::endl;

  return res;
}

void  PndDchRecoHit::Print(){
  std::cout<<"hitCoord:"; _hitCoord.Print();
  std::cout<<"hit HMatrix:";getHMatrix().Print();
  std::cout<<"hit DetPlane:";getDetPlane(0).Print();
  std::cout<<"\n hitCov:";getHitCov(getDetPlane(0)).Print();
  std::cout<<"EOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOH"<<std::endl;
  
}
