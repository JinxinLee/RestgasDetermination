//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TEventInfo								//
//                                                                      //
// Contains and provides access to summary information in the event	//
// useful for analysis							//
//									//
// Author List:								//
// Marcel Kunze,  RUB, Dec. 2k						//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/TEventInfo.h"
#include <assert.h>

ClassImp(TEventInfo)

#include <iostream>
using namespace std;

TEventInfo::TEventInfo():
fBeamSpot(TVector3(0,0,0)),fBeamSpotBFlight(TVector3(0,0,0)),fPrimaryVertex(0), fEventShape(0),fMCIP(0)
{}

//--------------
// Destructor --
//--------------

TEventInfo::~TEventInfo( )
{
    delete fPrimaryVertex;
    delete fMCIP;
}

void 
TEventInfo::SetPrimaryVertex(VAbsVertex* inVtx) {
    delete fPrimaryVertex;
    fPrimaryVertex = (inVtx == 0) ? (VAbsVertex*) 0 : (VAbsVertex*) inVtx->Clone();
}

const TPointErr
TEventInfo::PrimaryVertex() const {
    return (fPrimaryVertex == 0) ?  TPointErr(TVector3(0,0,0)):
    TPointErr(fPrimaryVertex->Point(),fPrimaryVertex->XXCov());
}
void 
TEventInfo::SetIPTruth(const TLorentzVector& inVtx) {
    fMCIP=new TVector3(inVtx.X(),inVtx.Y(),inVtx.Z());
}

void 
TEventInfo::SetIPTruth(const TVector3& inVtx) {
    fMCIP=new TVector3(inVtx);
}

void 
TEventInfo::SetEventShape(TEventShape* eventShape)
{
    fEventShape = eventShape;
}
//--------------
// Operations --
//--------------

void TEventInfo::PrintOn(std::ostream& o) const {
    o << "e+, e- beams: "<<PositronBeam()<<ElectronBeam();
    o << " spots: "<<BeamSpot()<<BeamSpotBFlight()<<PrimaryVertex();
}

std::ostream&  operator << (std::ostream& o, const TEventInfo& a) {a.PrintOn(o); return o;}


