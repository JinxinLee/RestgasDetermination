// The STT point is not defined by the (x, y, z) coordinates, but we ONLY know the
// (x, y) coordinates of the wire center (not the z!); moreover we know the drift
// radius. In order to find the MEASURED POINT to be used in the Kalman fit a 
// preliminary propagation must be performed to the point of closest approach to
// the firing wire.
// The plane is a virtual detector plane defined via:
// dj = wvi - vpf     (U coordinate in DetPlane)
// dk = wiredirection (V coordinate in DetPlane)
// (see WirepointHitPolicy.cxx)
//
// input: 8 entries
// 0-1-2 ==> x,y,z of the 1st extremity of the firing wire;
// 3-4-5 ==> x,y,z of the 2nd extremity of the firing wire;
// 6     ==> drift radius;
// 7     ==> reconstructed coordinate along the wire

// This Class' Header ------------------
#include "PndSttRecoHit.h"

// C/C++ Headers ----------------------

// Collaborating Class Headers --------
#include "GeaneTrackRep.h"
#include "PndSttHit.h"
#include "PndSttHelixHit.h"

#include "TMath.h"

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
  _hitCov[6][6] = 0.0100 * 0.0100; // currenthit->GetIsochroneError(); CHECK
  _hitCov[7][7] = 1. * 1.;

  // has coordinate along the wire
  _wireCoordFlag = true;
  // cut on distance
  _maxdistance = 0.5;
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
  if(wiredirection == TVector3(0.,0.,1.)) {
    _hitCoord[7][0] = currenthit->GetZ() - currenthit->GetZcen(); 
  }
  else  {
    //   cout << "SKEWED " << endl;
    _hitCoord[7][0] = (currenthit->GetZ() - currenthit->GetZcen())/(TVector3(0.,0.,1.).Dot(wiredirection)/wiredirection.Mag());
  }

  // errors on drift radius and z (by hand)
  for(int i = 0; i < NparHitRep; i++) for(int j = 0; j < NparHitRep; j++) _hitCov[i][j] = 0.;
  _hitCov[6][6] = 0.0100 * 0.0100; 
  //  _hitCov[6][6] = pow(currenthit->GetIsochroneError(), 2); // CHECK

  _hitCov[7][7] = 1. * 1.;

  // has coordinate along the wire
  _wireCoordFlag = true;
  // cut on distance
  _maxdistance = 0.5;

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


