// The STT point is not defined by the (x, y, z) coordinates, but we ONLY know the
// (x, y) coordinates of the wire center (not the z!); moreover we know the drift
// radius. In order to find the MEASURED POINT to be used in the Kalman fit a 
// preliminary propagation must be performed to the point of closest approach to
// the firing wire.
// The plane is a virtual detector plane defined via:
// dj = wvi - vpf     (U coordinate in DetPlane)
// dk = wiredirection (V coordinate in DetPlane)
//
// input: 8 entries
// 0-1-2 ==> x,y,z of the 1st extremity of the firing wire;
// 3-4-5 ==> x,y,z of the 2nd extremity of the firing wire;
// 6     ==> drift radius;
// 7     ==> reconstructed z coordinate (0 for now!)

// This Class' Header ------------------
#include "PndSttRecoHit.h"

// C/C++ Headers ----------------------
#include <assert.h>
#include <cmath>

// Collaborating Class Headers --------
#include "GeaneTrackRep.h"
#include "DetPlane.h"
#include "PndSttHit.h"
#include "PndSttHelixHit.h"
#include "FairGeanePro.h"
#include "FairGeaneUtil.h"
#include "FitterExceptions.h"
  
// Class Member definitions -----------
using namespace std;

ClassImp(PndSttRecoHit)


PndSttRecoHit::~PndSttRecoHit()
{}

PndSttRecoHit::PndSttRecoHit()
  : WirepointRecoHit(NparHitRep)
{}


PndSttRecoHit::PndSttRecoHit(PndSttHit *currenthit) : WirepointRecoHit(NparHitRep){

  // wire1(3), wire2(3), rdrift, zreco
  TVector3 wiredirection = currenthit->GetWireDirection();
  TVector3 wiredirection2 = currenthit->GetTubeHalfLength() * wiredirection;                     // CHECK for short tubes
  TVector3 cenposition(currenthit->GetX(), currenthit->GetY(), currenthit->GetZ());  // CHECK! z = 35
  TVector3 wire1, wire2;
  wire1 = cenposition - wiredirection2;
  wire2 = cenposition + wiredirection2;
//   cout << "Wiredirection, wire1, wire2 " << endl;
//   wiredirection.Print();
//   wire1.Print();
//   wire2.Print();
  _hitCoord[0][0] = wire1.X();
  _hitCoord[1][0] = wire1.Y();
  _hitCoord[2][0] = wire1.Z();
  _hitCoord[3][0] = wire2.X();
  _hitCoord[4][0] = wire2.Y();
  _hitCoord[5][0] = wire2.Z();
  _hitCoord[6][0] = currenthit->GetIsochrone();
  _hitCoord[7][0] = 0.; // currenthit->GetZ(); // to be changed into Zreco! CHECK

  // errors on drift radius and z (by hand)
  for(int i = 0; i < NparHitRep; i++) for(int j = 0; j < NparHitRep; j++) _hitCov[i][j] = 0.;
  _hitCov[6][6] = 0.0150 * 0.0150; // currenthit->GetIsochroneError(); CHECK
  _hitCov[7][7] = 1.5 * 1.5;
 
}

PndSttRecoHit::PndSttRecoHit(PndSttHelixHit *currenthit) : WirepointRecoHit(NparHitRep){

  // wire1(3), wire2(3), rdrift, zreco
  TVector3 wiredirection = currenthit->GetWireDirection();
  TVector3 wiredirection2 = currenthit->GetTubeHalfLength() * wiredirection;                     // CHECK for short tubes
  TVector3 cenposition(currenthit->GetXcen(), currenthit->GetYcen(), currenthit->GetZcen());  // CHECK! z = 35
  TVector3 wire1, wire2;
  wire1 = cenposition - wiredirection2;
  wire2 = cenposition + wiredirection2;
//   cout << "Wiredirection, wire1, wire2 " << endl;
//   wiredirection.Print();
//   wire1.Print();
//   wire2.Print();
  _hitCoord[0][0] = wire1.X();
  _hitCoord[1][0] = wire1.Y();
  _hitCoord[2][0] = wire1.Z();
  _hitCoord[3][0] = wire2.X();
  _hitCoord[4][0] = wire2.Y();
  _hitCoord[5][0] = wire2.Z();
  _hitCoord[6][0] = currenthit->GetIsochrone();
  _hitCoord[7][0] = 0.; // currenthit->GetZ(); // to be changed into Zreco! CHECK

  // errors on drift radius and z (by hand)
  for(int i = 0; i < NparHitRep; i++) for(int j = 0; j < NparHitRep; j++) _hitCov[i][j] = 0.;
  _hitCov[6][6] = 0.0150 * 0.0150; // currenthit->GetIsochroneError(); CHECK
  _hitCov[7][7] = 1.5 * 1.5;

}

void 
PndSttRecoHit::setHMatrix(const AbsTrackRep* stateVector,
		       const TMatrixT<double>& state)
{
  if (dynamic_cast<const GeaneTrackRep*>(stateVector) != NULL) {
    //I know, since this is the same everytime, it could be done in the
    //the constructor, but I do it here anyway, to make clear that in the
    //case of several track-reps per hit, it would have to be done here
    _HMatrix.ResizeTo(2,5);

    _HMatrix[0][0] = 0.;
    _HMatrix[0][1] = 0.;
    _HMatrix[0][2] = 0.;
    _HMatrix[0][3] = 1.;
    _HMatrix[0][4] = 0.;

    _HMatrix[1][0] = 0.;
    _HMatrix[1][1] = 0.;
    _HMatrix[1][2] = 0.;
    _HMatrix[1][3] = 0.;
    _HMatrix[1][4] = 1.;
  }
  else {
    std::cerr << "PndSttRecoHit can only handle state"
              << " vectors of type GeaneTrackRep -> abort" << std::endl;
    throw;
  }
 
}

double 
PndSttRecoHit::residualScalar(AbsTrackRep* stateVector,
			   const TMatrixT<double>& state)
{
  throw;
}

const DetPlane& 
PndSttRecoHit::detPlane(AbsRecoHit* hit, AbsTrackRep* rep)
{
   
  if (dynamic_cast<const GeaneTrackRep*>(rep) != NULL) {
    
    TMatrixT<double> x=hit->getRawHitCoord();
    assert(x.GetNrows()==8);
    TVector3 wire1(x[0][0],x[1][0],x[2][0]);
    TVector3 wire2(x[3][0],x[4][0],x[5][0]);
  
   
    // propagation to closest approach
    // retrieve GEANE
    FairGeanePro *geane = ((GeaneTrackRep*)rep)->getPropagator(); 

    // call propagation to closest approach to a wire 
    Int_t pca = 2;
    TVector3 point;
    TVector3 start = rep->getPos();
    //  cout << "rep start point " << endl;
    //  start.Print();
    Double_t distance1, distance2;
    distance1 = (wire1 - start).Mag();
    distance2 = (wire2 - start).Mag();
    Double_t maxdistance;
    if(distance1 < distance2) maxdistance = distance2;
    else maxdistance = distance1;
    maxdistance *= 2.;  
    Double_t Rad = 0.;
    TVector3 vpf, vwi;
    Double_t Di = 0.;
    Float_t trklength = 0.;
    Int_t PDG = ((GeaneTrackRep*)rep)->getPDG();

    TMatrixT<double> status = rep->getState();
    DetPlane referenceplane = rep->getReferencePlane();

    TMatrixT<double> covar = rep->getCov();
    FairGeaneUtil util;
    Double_t cov55[5][5];
    for(int i = 0; i < 5; i++) for(int j = 0; j < 5; j++) cov55[i][j] = covar[i][j];
    Double_t CovMatrix[15];
    util.FromMat25ToVec15(cov55, CovMatrix);
    //  for(int i = 0; i < 15; i++) CovMatrix[i] = 0.;
  
    TVector3 posp = rep->getPos();
    TVector3 momp = rep->getMom(); // MARS
    Double_t q = rep->getCharge();
    TVector3 posErr(0.,0.,0.);
    TVector3 MomErr(0.,0.,0.);
    TVector3 o = referenceplane.getO();
    TVector3 dj = referenceplane.getU();
    TVector3 dk = referenceplane.getV();

    FairTrackParP *parrep = new FairTrackParP(posp, momp, posErr, MomErr, q,  o, dj, dk);

    //   posp.Print();
    //   momp.Print();
    //   o.Print();
    //   dj.Print();
    //   dk.Print();
  
    Int_t direction = ((GeaneTrackRep*)rep)->getPropDir();

    geane->ActualFindPCA(pca, parrep, direction);
    Int_t findpca = geane->FindPCA(pca, PDG, point, wire1, wire2, maxdistance, Rad, vpf, vwi, Di, trklength);
  
    if(findpca != 0) { 
      cout << "detector plane determination FAILED!" << endl;  //return kFALSE; 
      FitterException exc("findpca failure", __LINE__,__FILE__);	
      throw exc;    
    }
  
    Double_t distance;
    distance = TMath::Sqrt(fabs(((wire1-vpf).Mag2()*(wire2-wire1).Mag2()-pow((wire1-vpf).Dot(wire2-wire1),2))/(wire2-wire1).Mag2()));
    
    // check vpf inside tube 
    if(distance>0.5) {
      cout << "vpf outside the firing tube" << endl; 
      FitterException exc("distance vpf-wire > 0.5", __LINE__,__FILE__);	
      throw exc;    
    }

    // find plane
    // unitary vector along distance
    // vpf on track, vwi on wire
    TVector3 fromwiretoextr = vpf - vwi;     
    fromwiretoextr.SetMag(1.);
    // unitary vector along the wire
    TVector3 wiredirection = wire2 - wire1; 
    wiredirection.SetMag(1.);

    // check orthogonality
    if(fabs(fromwiretoextr * wiredirection) > 1e-3) {
      cout << "detector plane determination FAILED! not orthogonal" << endl; 
      FitterException exc("fromwiretoextr*wiredirection > 1e-3", __LINE__,__FILE__);	
      throw exc;    
    }
  
    TVector3 U;
    U = fromwiretoextr;
    TVector3 V;
    V = wiredirection;
    U.SetMag(1.);
    V.SetMag(1.);

    TVector3 O = vwi; // CHECK
  
    _detPlane = DetPlane(O, U, V);

    return _detPlane;
  }
  else {
    std::cerr << "PndSttRecoHit can only handle state"
              << " vectors of type GeaneTrackRep -> abort" << std::endl;
    throw;
  }
 
}

const DetPlane&
PndSttRecoHit::getDetPlane(AbsTrackRep* rep){

 // AbsRecoHit* thishit = clone();

  //   distance of one (the first) of the wire extremities from the plane
  Double_t distance =  _detPlane.dist(TVector3(_hitCoord[0][0], _hitCoord[1][0],_hitCoord[2][0])).Mag();
  if(distance < 1e-5) return _detPlane;

  _detPlane = detPlane((AbsRecoHit *)this, rep);
 return _detPlane;
}


