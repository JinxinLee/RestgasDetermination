//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoSimpleVertexSelector                                              //
//                                                                      //
// Selector class to estimate the geometric intersection of two tracks  //
// Intended to provide the initial vertex position for V0 fits          //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoSelector/RhoSimpleVertexSelector.h"
#include "RhoBase/RhoCandidate.h"
#include "FairRun.h"
#include "FairRunSim.h"
#include "FairRunAna.h"
#include "FairField.h"

ClassImp ( RhoSimpleVertexSelector )

TBuffer& operator>> ( TBuffer& buf, RhoSimpleVertexSelector *&obj )
{
  obj = ( RhoSimpleVertexSelector* ) buf.ReadObject ( RhoSimpleVertexSelector::Class() );
  return buf;
}

#include <iostream>
using namespace std;

RhoSimpleVertexSelector::RhoSimpleVertexSelector ( const char* name, Double_t d, Double_t a, Double_t r1, Double_t r2 ) :
  RhoVertexSelectorBase ( name ) //, fQC(0)
{
  fDoca = d;
  fVtxip = a;
  fRmin = r1;
  fRmax = r2;
}

RhoSimpleVertexSelector::~RhoSimpleVertexSelector() {};// delete fQC; }

Bool_t RhoSimpleVertexSelector::Accept ( RhoCandidate& a, RhoCandidate& b )
{
  if ( &a==0 || &b==0 ) { return kFALSE; }

  // Position vectors
  TVector3 position1 = a.GetPosition();
  TVector3 position2 = b.GetPosition();

  double pnt[3], Bf[3];
  pnt[0]=0.5* ( position1.X() +position2.X() );
  pnt[1]=0.5* ( position1.Y() +position2.Y() );
  pnt[2]=0.5* ( position1.Z() +position2.Z() );
  if(FairRun::Instance()->IsAna()){
    FairRunAna::Instance()->GetField()->GetFieldValue ( pnt, Bf ); //[kGs]
  } else{
    FairRunSim::Instance()->GetField()->GetFieldValue ( pnt, Bf ); //[kGs]
  }
  Float_t bField = Bf[2]; // Retrieve the B-Field

  // Momentum vectors
  TVector3 ap3 = a.P3();
  Double_t pPerp1 = ap3.Perp();
  TVector3 d1 = ap3;
  d1.SetZ ( 0 );
  d1*=1.0/pPerp1;

  TVector3 bp3 = b.P3();
  Double_t pPerp2 = bp3.Perp();
  TVector3 d2 = bp3;
  d2.SetZ ( 0 );
  d2*=1.0/pPerp2;


  TVector3 dB ( 0,0,1.0 );
  // Radius and center
  Double_t rho1 = 100. * pPerp1/ ( 0.3*bField ); // Radius in cm //FIXME really?
  TVector3 r1=d1.Cross ( dB );
  r1 *= -a.Charge() *rho1;
  TVector3 center1 = position1 - r1;
  center1.SetZ ( 0 );

  Double_t rho2 = 100. * pPerp2/ ( 0.3*bField ); // Radius in cm //FIXME really?
  TVector3 r2=d2.Cross ( dB );
  r2 *= -b.Charge() *rho2;
  TVector3 center2 = position2 - r2;
  center2.SetZ ( 0 );

  // distance and angle of the axis between the two centers
  TVector3 ab = center2 - center1;
  Double_t dab = ab.Perp();
  Double_t cosTheAB = ab.X() /dab;
  Double_t sinTheAB = ab.Y() /dab;


  // x value of intersect at reduced system
  Double_t x = dab/2 + ( rho1*rho1 - rho2*rho2 ) / ( 2*dab );

  // y*y value of intersect at reduced system for helix A
  Double_t y2 = ( rho1+x ) * ( rho1-x );

  // both circles do not intersect (only one solution)
  Int_t nSolMax=1;
  Double_t y=0;

  if ( y2 > 0 ) {
    nSolMax=2;
    y = sqrt ( y2 );
  } else {
    //if( fabs(dab-rho1-rho2) > fActualDoca);
    //return kFALSE;
  }

  // now we compute the solution(s)

  TVector3 newap3[2];
  TVector3 newbp3[2];
  TVector3 newapos[2];
  TVector3 newbpos[2];
  Int_t best=0;
  fActualDoca=1.E8;
  for ( Int_t ns=0; ns<nSolMax; ns++ ) {   // loop on the solutions

    // radius vector of intersection point
    Double_t sign = ns ? 1.0 : -1.0;
    TVector3 rs1 ( cosTheAB*x - sinTheAB*y * sign, sinTheAB*x + cosTheAB*y * sign, 0 );
    TVector3 rs2 ( rs1-ab );

    // are we moving forward or backward?
    Double_t adir= ( rs1-r1 ).Dot ( ap3 ) >0 ? 1.0 : -1.0;
    Double_t aangle=adir * r1.Angle ( rs1 );
    // intersection point
    Double_t newaz=position1.Z() + rho1*aangle/pPerp1 * ap3.Z();
    newapos[ns].SetX ( center1.X() + rs1.X() );
    newapos[ns].SetY ( center1.Y() + rs1.Y() );
    newapos[ns].SetZ ( newaz );

    // adjust momentum
    newap3[ns]=rs1.Cross ( dB );
    newap3[ns]*=a.Charge() /rho1*pPerp1;
    newap3[ns].SetZ ( ap3.Z() );


    // same for b
    Double_t bdir= ( rs2-r2 ).Dot ( bp3 ) >0 ? 1.0 : -1.0;
    Double_t bangle=bdir * r2.Angle ( rs2 );
    Double_t newbz=position2.Z() + rho2*bangle/pPerp2 * bp3.Z();
    newbpos[ns].SetX ( center2.X() + rs2.X() ); // ==newapos[ns].X()
    newbpos[ns].SetY ( center2.Y() + rs2.Y() ); // ==newapos[ns].Y()
    newbpos[ns].SetZ ( newbz );
    newbp3[ns]=rs2.Cross ( dB );
    newbp3[ns]*=b.Charge() /rho2*pPerp2;
    newbp3[ns].SetZ ( bp3.Z() );

    Double_t delta = ( newapos[ns]-newbpos[ns] ).Mag();

    // take the solution of minimal deltaZ
    if ( delta < fActualDoca ) {
      best=ns;
      fActualDoca  = delta;
    }
  }

  fVertex=0.5* ( newapos[best]+newbpos[best] );
  fMomA=newap3[best];
  fMomB=newbp3[best];

  // Now reconstruct original flight path and
  // calculate the angle to vertex-ip

  TVector3 p=fMomA+fMomB;
  TVector3 vtxip = fVertex-fPrimaryVertex;
  fActualVtxip = vtxip.Angle ( p );
  fActualR = vtxip.Mag();

  //if (fQC!=0) fQC->Accumulate((Float_t)fActualDoca,(Float_t)fActualVtxip);

  if ( fActualDoca < fDoca &&
       fActualR >= fRmin && fActualR < fRmax &&
       fActualVtxip < fVtxip ) {

    return kTRUE;

  } else {
    fMomA=ap3;
    fMomB=bp3;
    fVertex = fPrimaryVertex;
    return kFALSE;
  }
  return kFALSE; //get rid of warnings
}

/*
void TSimpleVertexSelector::ActivateQualityControl()
{
    TPersistenceManager *persmgr = TRho::Instance()->GetPersistenceManager();
    if (persmgr == 0) {
  std::cerr << "TVertexSelector: Can not activate QC w/o PersistenceManager." << std::endl;
  return;
    }

    // Make a quality control directory, if needed and cd into it

    persmgr->SetDir("QC/TVertexSelector");
    fQC = persmgr->Histogram(GetName(),100,0.0,10.,100,0.0,3.14159265358979323846);
}
*/
