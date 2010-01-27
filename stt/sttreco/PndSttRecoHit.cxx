// The STT point is not defined by the (x, y, z) coordinates, but we ONLY know the
// (x, y) coordinates of the wire center (not the z!); moreover we know the drift
// radius. In order to find the MEASURED POINT to be used in the GFKalman fit a 
// preliminary propagation must be performed to the point of closest approach to
// the firing wire.
// The plane is a virtual detector plane defined via:
// dj = wvi - vpf     (U coordinate in GFDetPlane)
// dk = wiredirection (V coordinate in GFDetPlane)
// (see GFWirepointHitPolicy.cxx)
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
  fHitCoord[0][0] = wire1.X();
  fHitCoord[1][0] = wire1.Y();
  fHitCoord[2][0] = wire1.Z();
  fHitCoord[3][0] = wire2.X();
  fHitCoord[4][0] = wire2.Y();
  fHitCoord[5][0] = wire2.Z();
  fHitCoord[6][0] = currenthit->GetIsochrone();
  fHitCoord[7][0] = 0.; // currenthit->GetZ(); // to be changed into Zreco! CHECK

  // errors on drift radius and z (by hand)
  for(int i = 0; i < NparHitRep; i++) for(int j = 0; j < NparHitRep; j++) fHitCov[i][j] = 0.;
  fHitCov[6][6] = 0.0100 * 0.0100; // currenthit->GetIsochroneError(); CHECK
//  fHitCov[7][7] = 1. * 1.;
  fHitCov[7][7] = 1. * 1000.;   // just a big number ; this is sigma_z

  // cut on distance
  fPolicy.setMaxDistance(0.5);
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
  fHitCoord[0][0] = wire1.X();
  fHitCoord[1][0] = wire1.Y();
  fHitCoord[2][0] = wire1.Z();
  fHitCoord[3][0] = wire2.X();
  fHitCoord[4][0] = wire2.Y();
  fHitCoord[5][0] = wire2.Z();
  fHitCoord[6][0] = currenthit->GetIsochrone();
  if(wiredirection == TVector3(0.,0.,1.)) {
    fHitCoord[7][0] = currenthit->GetZ() - currenthit->GetZcen(); 
  }
  else  {
    //   cout << "SKEWED " << endl;
    fHitCoord[7][0] = (currenthit->GetZ() - currenthit->GetZcen())/(TVector3(0.,0.,1.).Dot(wiredirection)/wiredirection.Mag());
  }

  // errors on drift radius and z (by hand)
  for(int i = 0; i < NparHitRep; i++) for(int j = 0; j < NparHitRep; j++) fHitCov[i][j] = 0.;
  fHitCov[6][6] = 0.0100 * 0.0100; 
  //  fHitCov[6][6] = pow(currenthit->GetIsochroneError(), 2); // CHECK

  fHitCov[7][7] = 1. * 1.;

  // cut on distance
  fPolicy.setMaxDistance(0.5);

}

TMatrixT<double>
PndSttRecoHit::getHMatrix(const GFAbsTrackRep* stateVector)
{
  if (dynamic_cast<const GeaneTrackRep*>(stateVector) != NULL) {
    TMatrixT<double> HMatrix(2,5);

    HMatrix[0][0] = 0.;
    HMatrix[0][1] = 0.;
    HMatrix[0][2] = 0.;
    HMatrix[0][3] = 1.;
    HMatrix[0][4] = 0.;

    HMatrix[1][0] = 0.;
    HMatrix[1][1] = 0.;
    HMatrix[1][2] = 0.;
    HMatrix[1][3] = 0.;
    HMatrix[1][4] = 1.;
    return HMatrix;
  }
  else {
    std::cerr << "PndSttRecoHit can only handle state"
              << " vectors of type GeaneTrackRep -> abort" << std::endl;
    throw;
  }
 
}


