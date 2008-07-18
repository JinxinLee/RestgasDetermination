//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TPidTruthElectronSelector                                            //
// TPidTruthMuonSelector                                                //
// TPidTruthPionSelector                                                //
// TPidTruthKaonSelector                                                //
// TPidTruthProtonSelector                                              //
//                                                                      //
// Selector classes for particle selection                              //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TParticlePDG.h"
#include "RhoSelector/TPidTruthSelector.h"
#include "RhoSelector/TPidQuality.h"

#include "RhoBase/TRho.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/VAbsMicroCandidate.h"
#include "RhoManager/TAssociator.h"

//----------------------------------------------------------------

ClassImp(TPidTruthElectronSelector)

TBuffer &operator>>(TBuffer &buf, TPidTruthElectronSelector *&obj)
{
   obj = (TPidTruthElectronSelector *) buf.ReadObject(TPidTruthElectronSelector::Class());
   return buf;
}

#include <iostream>
using namespace std;

TPidTruthElectronSelector::TPidTruthElectronSelector(const char *name, Bool_t qc) :
VAbsPidSelector(name,"e-")  
{
    if (qc) fQC = new TPidQuality(name);
}

TPidTruthElectronSelector::~TPidTruthElectronSelector() { delete fQC; }    

Bool_t TPidTruthElectronSelector::Accept(TCandidate& b) 
{
    Bool_t decision = kFALSE;
    if (&b == 0) return kFALSE;

    // Get the associated MC-Candidate

    if (GetAssociator() == 0) {

	TEventManager *evtmgr = TRho::Instance()->GetEventManager();
	if (evtmgr == 0) {
	    cerr << "TPidTruthElectronSelector: No handle to event manager." << endl;
	    return kFALSE;
	}
	else
	    SetAssociator(new TMapAssociator(*evtmgr));
    }

    TCandidate *mc = GetAssociator()->McFromReco(&b);
    if (mc == 0) return kFALSE; // No info available
    if (mc->PdtEntry() == 0) return kFALSE; // No info available

    Int_t pdgCode = mc->PdtEntry()->PdgCode();

    if (pdgCode==11 || pdgCode==-11) { // e- or e+
	SetTypeAndMass(b);
	decision = kTRUE;
    }

    if (fQC) fQC->Control(b.GetMicroCandidate(),decision);	    // Delegate quality control
    
    return decision;
}

Bool_t TPidTruthElectronSelector::Accept(VAbsMicroCandidate& b) 
{
    Bool_t decision = kFALSE;
    if (&b == 0) return kFALSE;
    if (b.GetMcPid()==0) decision = kTRUE;
    if (fQC) fQC->Control(b,decision);	    // Delegate quality control
    return decision;
}

//----------------------------------------------------------------

ClassImp(TPidTruthMuonSelector)

TBuffer &operator>>(TBuffer &buf, TPidTruthMuonSelector *&obj)
{
   obj = (TPidTruthMuonSelector *) buf.ReadObject(TPidTruthMuonSelector::Class());
   return buf;
}

TPidTruthMuonSelector::TPidTruthMuonSelector(const char *name, Bool_t qc) :
VAbsPidSelector(name,"mu-")
{
    if (qc) fQC = new TPidQuality(name);
}

TPidTruthMuonSelector::~TPidTruthMuonSelector() { delete fQC; }    

Bool_t TPidTruthMuonSelector::Accept(TCandidate& b)
{
    Bool_t decision = kFALSE;
    if (&b == 0) return kFALSE;

    // Get the associated MC-Candidate

    if (GetAssociator() == 0) {

	TEventManager *evtmgr = TRho::Instance()->GetEventManager();
	if (evtmgr == 0) {
	    cerr << "TPidTruthMuonSelector: No handle to event manager." << endl;
	    return kFALSE;
	}
	else
	    SetAssociator(new TMapAssociator(*evtmgr));
    }

    TCandidate *mc = GetAssociator()->McFromReco(&b);
    if (mc == 0) return kFALSE; // No info available
    if (mc->PdtEntry() == 0) return kFALSE; // No info available

    Int_t pdgCode = mc->PdtEntry()->PdgCode();

    if (pdgCode==13 || pdgCode==-13) { // mu- or mu+
	SetTypeAndMass(b);
	decision = kTRUE;
    }

    if (fQC) fQC->Control(b.GetMicroCandidate(),decision);	    // Delegate quality control
    
    return decision;
}

Bool_t TPidTruthMuonSelector::Accept(VAbsMicroCandidate& b) 
{
    Bool_t decision = kFALSE;
    if (&b == 0) return kFALSE;
    if (b.GetMcPid()==1) decision = kTRUE;
    if (fQC) fQC->Control(b,decision);	    // Delegate quality control
    return decision;
}

//----------------------------------------------------------------

ClassImp(TPidTruthPionSelector)

TBuffer &operator>>(TBuffer &buf, TPidTruthPionSelector *&obj)
{
   obj = (TPidTruthPionSelector *) buf.ReadObject(TPidTruthPionSelector::Class());
   return buf;
}

TPidTruthPionSelector::TPidTruthPionSelector(const char *name, Bool_t qc) :
VAbsPidSelector(name,"pi+")
{
    if (qc) fQC = new TPidQuality(name);
}

TPidTruthPionSelector::~TPidTruthPionSelector() { delete fQC; }    

Bool_t TPidTruthPionSelector::Accept(TCandidate& b)
{
    Bool_t decision = kFALSE;
    if (&b == 0) return kFALSE;

    // Get the associated MC-Candidate

    if (GetAssociator() == 0) {

	TEventManager *evtmgr = TRho::Instance()->GetEventManager();
	if (evtmgr == 0) {
	    cerr << "TPidTruthPionSelector: No handle to event manager." << endl;
	    return kFALSE;
	}
	else
	    SetAssociator(new TMapAssociator(*evtmgr));
    }

    TCandidate *mc = GetAssociator()->McFromReco(&b);
    if (mc == 0) return kFALSE; // No info available
    if (mc->PdtEntry() == 0) return kFALSE; // No info available

    Int_t pdgCode = mc->PdtEntry()->PdgCode();

    if (pdgCode==211 || pdgCode==-211) { // pi- or pi+
	SetTypeAndMass(b);
	decision = kTRUE;
    }

    if (fQC) fQC->Control(b.GetMicroCandidate(),decision);	    // Delegate quality control
    
    return decision;
}

Bool_t TPidTruthPionSelector::Accept(VAbsMicroCandidate& b)
{
    Bool_t decision = kFALSE;
    if (&b == 0) return kFALSE;
    if (b.GetMcPid()==2) decision = kTRUE;
    if (fQC) fQC->Control(b,decision);	    // Delegate quality control
    return decision;
}

//----------------------------------------------------------------

ClassImp(TPidTruthKaonSelector)

TBuffer &operator>>(TBuffer &buf, TPidTruthKaonSelector *&obj)
{
   obj = (TPidTruthKaonSelector *) buf.ReadObject(TPidTruthKaonSelector::Class());
   return buf;
}

TPidTruthKaonSelector::TPidTruthKaonSelector(const char *name, Bool_t qc) :
VAbsPidSelector(name,"K+")
{
    if (qc) fQC = new TPidQuality(name);
}

TPidTruthKaonSelector::~TPidTruthKaonSelector() { delete fQC; }    

Bool_t TPidTruthKaonSelector::Accept(TCandidate& b)
{
    Bool_t decision = kFALSE;
    if (&b == 0) return kFALSE;

    // Get the associated MC-Candidate

    if (GetAssociator() == 0) {

	TEventManager *evtmgr = TRho::Instance()->GetEventManager();
	if (evtmgr == 0) {
	    cerr << "TPidTruthKaonSelector: No handle to event manager." << endl;
	    return kFALSE;
	}
	else
	    SetAssociator(new TMapAssociator(*evtmgr));
    }

    TCandidate *mc = GetAssociator()->McFromReco(&b);
    if (mc == 0) return kFALSE; // No info available
    if (mc->PdtEntry() == 0) return kFALSE; // No info available

    Int_t pdgCode = mc->PdtEntry()->PdgCode();

    if (pdgCode==321 || pdgCode==-321) { // K- or K+
	SetTypeAndMass(b);
	decision = kTRUE;
    }

    if (fQC) fQC->Control(b.GetMicroCandidate(),decision);	    // Delegate quality control
    
    return decision;
}

Bool_t TPidTruthKaonSelector::Accept(VAbsMicroCandidate& b)
{
    Bool_t decision = kFALSE;
    if (&b == 0) return kFALSE;
    if (b.GetMcPid()==3) decision = kTRUE;
    if (fQC) fQC->Control(b,decision);	    // Delegate quality control
    return decision;
}

//----------------------------------------------------------------

ClassImp(TPidTruthProtonSelector)

TBuffer &operator>>(TBuffer &buf, TPidTruthProtonSelector *&obj)
{
   obj = (TPidTruthProtonSelector *) buf.ReadObject(TPidTruthProtonSelector::Class());
   return buf;
}

TPidTruthProtonSelector::TPidTruthProtonSelector(const char *name, Bool_t qc) :
VAbsPidSelector(name,"p+")
{
    if (qc) fQC = new TPidQuality(name);
}

TPidTruthProtonSelector::~TPidTruthProtonSelector() { delete fQC; }    

Bool_t TPidTruthProtonSelector::Accept(TCandidate& b)
{
    Bool_t decision = kFALSE;
    if (&b == 0) return kFALSE;

    // Get the associated MC-Candidate

    if (GetAssociator() == 0) {

	TEventManager *evtmgr = TRho::Instance()->GetEventManager();
	if (evtmgr == 0) {
	    cerr << "TPidTruthProtonSelector: No handle to event manager." << endl;
	    return kFALSE;
	}
	else
	    SetAssociator(new TMapAssociator(*evtmgr));
    }

    TCandidate *mc = GetAssociator()->McFromReco(&b);
    if (mc == 0) return kFALSE; // No info available
    if (mc->PdtEntry() == 0) return kFALSE; // No info available

    Int_t pdgCode = mc->PdtEntry()->PdgCode();

    if (pdgCode==2212 || pdgCode==-2212) { // p- or p+
	SetTypeAndMass(b);
	decision = kTRUE;
    }

    if (fQC) fQC->Control(b.GetMicroCandidate(),decision);	    // Delegate quality control
    
    return decision;
}

Bool_t TPidTruthProtonSelector::Accept(VAbsMicroCandidate& b)
{
    Bool_t decision = kFALSE;
    if (&b == 0) return kFALSE;
    if (b.GetMcPid()==4) decision = kTRUE;
    if (fQC) fQC->Control(b,decision);	    // Delegate quality control
    return decision;
}

