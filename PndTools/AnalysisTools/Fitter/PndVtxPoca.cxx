
#include <iostream>
#include "PndVtxPoca.h"
#include "PndAnalysisCalcTools.h"

using namespace std;

ClassImp(PndVtxPoca)

TBuffer& operator>>(TBuffer& buf, PndVtxPoca *&obj)
{
  obj = (PndVtxPoca*) buf.ReadObject(PndVtxPoca::Class());
  return buf;
}

//Include only those constraint which need vertex Info....
PndVtxPoca::PndVtxPoca()
{
}

PndVtxPoca::~PndVtxPoca()
{
}


Double_t PndVtxPoca::GetPocaVtx(TVector3& vertex, RhoCandidate& composite)
{
  vertex.SetXYZ(0.,0.,0.);
  if ( composite.NDaughters() <  2 ) { return 0.; }
  if ( composite.NDaughters() == 2 ) { return GetPoca(vertex,composite.Daughter(0),composite.Daughter(1)); }

  std::vector<Double_t> distances;
  std::vector<TVector3> results;
  // loop over daughters, take the mean value of all "best" positions
  // TODO do this smarter by using already found vertices ?
  TVector3 theVertex(0.,0.,0.);
  Double_t actualDoca=0.;
  for(Int_t daug1 =0; daug1<composite.NDaughters(); daug1++) {
    RhoCandidate* a=composite.Daughter(daug1);
    for(Int_t daug2=daug1+1; daug2<composite.NDaughters(); daug2++) {
      RhoCandidate* b=composite.Daughter(daug2);
      actualDoca = GetPoca(theVertex,a,b);
      distances.push_back(actualDoca);
      results.push_back(theVertex);
    }//daug2
  }//daug1
  // Averaging vertex results from each track pair, weighted with 1/distance
  std::vector<Double_t>::iterator iterDoca;
  std::vector<TVector3>::iterator iterVtx;
  Double_t docaweight=0,sumdocaweigts=0;
  TVector3 vertexK;
  for(iterVtx=results.begin(), iterDoca=distances.begin(); iterVtx!=results.end()&&iterDoca!=distances.end(); ++iterVtx,++iterDoca) {
    docaweight=1/(*iterDoca);
    //docaweight *= docaweight;
    vertexK=*iterVtx;
    if (docaweight == 0) { docaweight = 1; } // right so?
    vertexK *= docaweight;
    vertex+=vertexK;
    sumdocaweigts+=docaweight;
  }
  if (sumdocaweigts == 0) { sumdocaweigts=1; }
  vertex*=1./sumdocaweigts;
  //sumdocaweigts = sqrt(sumdocaweigts);
  return composite.NDaughters()/sumdocaweigts;
}


Double_t PndVtxPoca::GetPoca(TVector3& vertex,RhoCandidate* a, RhoCandidate* b)
{
  vertex.SetXYZ(0.,0.,0.);
  Double_t bField = 0.1*PndAnalysisCalcTools::GetBz(vertex); // T, assume field in z only
  Double_t bc = 0.0029979246*bField;
  TVector3 dB(0,0,1.0);
  TVector3 position1 = a->GetPosition();
  // Momentum vectors
  TVector3 ap3 = a->P3();
  Double_t pPerp1 = ap3.Perp();
  TVector3 d1 = ap3;
  d1.SetZ(0);
  d1*=1.0/pPerp1;

  // Radius and center
  Double_t rho1 = pPerp1/bc; // Radius in cm
  TVector3 r1=d1.Cross(dB);
  r1 *= -a->Charge()*rho1;
  TVector3 center1 = position1 - r1;
  center1.SetZ(0);

  TVector3 position2 = b->GetPosition();

  // Momentum vectors
  TVector3 bp3 = b->P3();
  Double_t pPerp2 = bp3.Perp();
  TVector3 d2 = bp3;
  d2.SetZ(0);
  d2*=1.0/pPerp2;

  // Radius and center
  Double_t rho2 =  pPerp2/bc; // Radius in cm
  TVector3 r2=d2.Cross(dB);
  r2 *= -b->Charge()*rho2;
  TVector3 center2 = position2 - r2;
  center2.SetZ(0);

  // distance and angle of the axis between the two centers
  TVector3 ab = center2 - center1;
  Double_t dab = ab.Perp();
  Double_t cosTheAB = ab.X()/dab;
  Double_t sinTheAB = ab.Y()/dab;

  Double_t darr = dab;
  darr -= rho1;
  darr -= rho2;

  // both circles do not intersect (only one solution)
  Int_t nSolMax=1;
  Double_t x=0;
  Double_t y=0;
  if (darr < 0) {
    // sum of radii is smaller than the two centers distance, circles intersect at two points
    nSolMax=2;
    // x value of intersect at reduced system
    x = 0.5*dab + ( rho1*rho1 - rho2*rho2 )/(2*dab);
    // y*y value of intersect at reduced system for helix A
    Double_t y2 = (rho1+x)*(rho1-x);
    if (y2 > 0) { y = sqrt(y2); }
  } else {
    // no intersecting circles, take the mid point between both circles
    x = 0.5*(dab + rho1 - rho2);
  }
  // now we compute the solution(s)
  TVector3 newapos[2];
  TVector3 newbpos[2];
  Int_t best=0;
  Double_t actualDoca=1.E8;
  for (Int_t ns=0; ns<nSolMax; ns++) {     // loop on the solutions
    // radius vector of intersection point
    Double_t sign = ns ? 1.0 : -1.0;
    TVector3 rs1( cosTheAB*x - sinTheAB*y * sign, sinTheAB*x + cosTheAB*y * sign, 0);
    TVector3 rs2( rs1-ab );

    // are we moving forward or backward?
    Double_t adir=(rs1-r1).Dot(ap3)>0 ? 1.0 : -1.0;
    Double_t aangle=adir * r1.Angle(rs1);
    // intersection point
    Double_t newaz=position1.Z() + rho1*aangle/pPerp1 * ap3.Z();
    newapos[ns].SetX( center1.X() + rs1.X() );
    newapos[ns].SetY( center1.Y() + rs1.Y() );
    newapos[ns].SetZ( newaz );

    // same for b
    Double_t bdir=(rs2-r2).Dot(bp3)>0 ? 1.0 : -1.0;
    Double_t bangle=bdir * r2.Angle(rs2);
    Double_t newbz=position2.Z() + rho2*bangle/pPerp2 * bp3.Z();
    newbpos[ns].SetX( center2.X() + rs2.X());   // ==newapos[ns].X()
    newbpos[ns].SetY( center2.Y() + rs2.Y());   // ==newapos[ns].Y()
    newbpos[ns].SetZ( newbz );

    Double_t delta = (newapos[ns]-newbpos[ns]).Mag();

    // take the solution of minimal deltaZ
    if ( delta < actualDoca ) {
      best=ns;
      actualDoca  = delta;
    }
  }
  vertex=0.5*(newapos[best]+newbpos[best]);
  return actualDoca;
}

