//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TPidQuality								//
//                                                                      //
// Mix-in class for particle identification quality control		//
//                                                                      //
//                                                                      //
// Author: Marcel Kunze, Bochum University, Aug. 99			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoSelector/TPidQuality.h"
#include "RhoHistogram/TTuple.h"
#include "RhoBase/TRho.h"
#include "RhoBase/VAbsMicroCandidate.h"
#include "RhoManager/TPersistenceManager.h"
#include "RhoBase/TCandidate.h"

ClassImp(TPidQuality)

TBuffer &operator>>(TBuffer &buf, TPidQuality *&obj)
{
   obj = (TPidQuality *) buf.ReadObject(TPidQuality::Class());
   return buf;
}

#include <iostream>
using namespace std;

TPidQuality::TPidQuality(const char* n)
{
    TString name(n);

    TPersistenceManager *persmgr = TRho::Instance()->GetPersistenceManager();
    if (persmgr == 0) {
	cerr << "TPidQuality: Can not activate QC w/o PersistenceManager for " << name << endl;
    }
    else {
	// Make a quality control directory, if needed and cd into it

	TString dir = TString("QC/") + name;
	persmgr->SetDir(dir);

	// Add nTuples for the selector (Owner: PersistenceManager)

	fAccepted = new TTuple("Accepted",name+" accepted tracks");
	persmgr->Add(fAccepted);
	fRejected = new TTuple("Rejected",name+" rejected tracks");
	persmgr->Add(fRejected);
    }
}

TPidQuality::~TPidQuality() 
{
}

void TPidQuality::Control(TCandidate &c, Bool_t decision)
{
    if (&c == 0) return;
    Control(c.GetMicroCandidate(),decision);
}

void TPidQuality::Control(VAbsMicroCandidate &cand, Bool_t decision)
{
    if (&cand == 0) return;

    Float_t mom  = cand.GetMomentum().Mag();   // Get the momentum
    if (mom <= 0.0) return;
    Float_t cth  = cand.GetMomentum().Z()/mom; // Get cos(theta)
    Float_t svt  = cand.GetSvtMeanDEdx();      // Get the energy loss
    Float_t dch  = cand.GetDchMeanDEdx();      // Get the energy loss
    Float_t drc  = cand.GetDrcThetaC();        // Get the Cerenkov angle
    Float_t emc  = cand.GetEmcRawEnergy();     // Get the energy deposit in EMC
    Float_t ifr  = cand.GetIfrMeasuredInteractionLengths();     // Get the ifr info
    Float_t hits  = cand.GetDchHits();      	// Get the hits

    TTuple *tuple;
    if (decision)
	tuple = fAccepted;
    else
	tuple = fRejected;

    tuple->Column("mom",mom,0.0f,"DOUBLE");
    tuple->Column("cth",cth,0.0f,"DOUBLE");
    tuple->Column("svt",svt,0.0f,"DOUBLE");
    tuple->Column("dch",dch,0.0f,"DOUBLE");
    tuple->Column("drc",drc,0.0f,"DOUBLE");
    tuple->Column("emc",emc,0.0f,"DOUBLE");
    tuple->Column("ifr",ifr,0.0f,"DOUBLE");
    tuple->Column("hits",hits,0.0f,"DOUBLE");
    tuple->DumpData();

}

