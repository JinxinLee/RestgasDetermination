//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsBit								//
//                                                                      //
// Definition of the Tag database					//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 1999					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/VAbsBit.h"
#include <assert.h>
#include "TTree.h"
#include "TBranch.h"
#include "TString.h"
#include "THashList.h"

ClassImp(VAbsBit)

#include <iostream>
#include <iomanip>
using namespace std;

VAbsBit::VAbsBit() : 
fBitTree(0), fEventNumber(0), fBufferSize(32768),
fBoolBranchTable(0), fIntBranchTable(0), fFloatBranchTable(0) 
{
    fBoolBranchTable  = new THashList();
    fIntBranchTable   = new THashList();
    fFloatBranchTable = new THashList();
}

VAbsBit::VAbsBit(TTree *t,const unsigned & bufferSize ) : 
fBitTree(t), fEventNumber(0), fBufferSize(bufferSize),
fBoolBranchTable(0), fIntBranchTable(0), fFloatBranchTable(0)
{
    fBoolBranchTable  = new THashList();
    fIntBranchTable   = new THashList();
    fFloatBranchTable = new THashList();
}


VAbsBit::~VAbsBit() 
{
    //Do not delete as ROOT tries to delete the branches
    //delete fBoolBranchTable;
    //delete fIntBranchTable;
    //delete fFloatBranchTable;
}

// Reset the cache
void VAbsBit::ClearCache()
{
    fBoolBranchTable->Clear("nodelete");
    fIntBranchTable->Clear("nodelete");
    fFloatBranchTable->Clear("nodelete");
}

// Preset the persistent buffer
void VAbsBit::InvalidateBranches()
{
    TObjArray* branches= fBitTree->GetListOfBranches();
    TIter iter( branches );
    TBranch* branch= 0;
    while( branch= (TBranch*) iter.Next() ) {
	if( strstr( branch->GetName(), "TAG_b_" ) != 0 ) {
	    void* address= branch->GetAddress();
	    assert(address);
	    *((UChar_t*)(address))= boolInvalid;
	} else 
	    if( strstr( branch->GetName(), "TAG_i_" ) != 0 ) {
		void* address= branch->GetAddress();
		assert(address);
		TIntTagElem* i= (TIntTagElem*)(address);
		i->value= 0;
		i->valid= kFALSE;
	    } else 
		if( strstr( branch->GetName(), "TAG_f_" ) != 0 ) {
		    void* address= branch->GetAddress();
		    assert(address);
		    TFloatTagElem* f= (TFloatTagElem*)(address);
		    f->value= 0.;
		    f->valid= kFALSE;
		}
    }
}

// Read a Bool_t value from the TAG:
char VAbsBit::getBool( char & value, const char* key ) const 
{    
    // Has setRootTree been called?:
    assert( fBitTree != 0 );
    
    // Return value:
    char result= kFALSE;
    
    // Check if a branch for the tag variable exists:
    // first look in hash directory:
    
    TString theKey= "TAG_b_";
    theKey += key;
    
    TBranch* branch = (TBranch*) fBoolBranchTable->FindObject(theKey);
    
    if (branch == 0) {
	// then try to find it in the root file (linear search):
	branch= ((TTree*)fBitTree)->GetBranch( theKey );
	if( branch == 0 ) { 
	    value= kFALSE; 
	    cerr << " error, couldn' get branch " << theKey.Data() << endl;
	}
	else {
	    fBoolBranchTable->Add(branch); // Add the branch to the table
	}
    }
    
    if( branch != 0 ) { 
	
	// Get the address of the memory buffer for this branch, create one
	// if neccessary:
	void* address= branch->GetAddress();
	if( address == 0 ) {
	    address= new UChar_t( 0 );
	    branch->SetAddress( address );
	}
	
	// Read the branch:
	Int_t nbytes= branch->GetEntry( fEventNumber );
	if( nbytes == 0 ) {
	    value= kFALSE; 
	    cerr << " error, no data for branch " << key 
		<< " in event " << fEventNumber << endl;
	}
	else {
	    
	    // Put retrieved value Int_to input argument and return status:
	    UChar_t val= *((UChar_t*)address);
	    if( val == boolFalse ) {
		value= kFALSE;
		result= kTRUE;
	    }
	    else if( val == boolTrue ) {
		value= kTRUE;
		result= kTRUE;
	    }
	    else if( val == boolInvalid ) {
		value= kFALSE;
	    }
	    else {
		cerr << " error, bool value " << val << " nonsense" 
		    << " in event " << fEventNumber << endl;
	    }
	    
	}
    }
    
    // The End:
    return result;
    
}


// Read an Int_t value from the TAG:
char VAbsBit::getInt( Int_t & value, const char* key ) const 
{    
    // Has setRootTree been called?:
    assert( fBitTree != 0 );
    
    // Check if a branch for the tag variable exists:
    // first look in hash directory:
    
    TString theKey= "TAG_i_";
    theKey += key;
    
    TBranch* branch = (TBranch*) fIntBranchTable->FindObject(theKey);
    
    if (branch == 0) {
	// Check if tag variable exists:
	branch= ((TTree*)fBitTree)->GetBranch( theKey );
	if( branch == 0 ) { 
	    value= 0;
	    cerr << " error, couldn' get branch " << theKey.Data() << endl;
	    return kFALSE; 
	}
	else {
	    fIntBranchTable->Add(branch); // Add the branch to the table
	}
    }
    
    if( branch != 0 ) { 
	
	// Get the address of the memory buffer for this branch, create one
	// if neccessary:
	void* address= branch->GetAddress();
	if( address == 0 ) {
	    address= new TIntTagElem;
	    branch->SetAddress( address );
	}
	
	// Read the branch:
	Int_t nbytes= branch->GetEntry( fEventNumber );
	if( nbytes == 0 ) {
	    value= kFALSE; 
	    cerr << " error, no data for branch " << key 
		<< " in event " << fEventNumber << endl;
	    return kFALSE;
	}
	
	// Put retrieved value into input argument and return status:
	TIntTagElem val= *((TIntTagElem*)address);
	if( val.valid ) {
	    value= val.value;
	    return kTRUE;
	}
	else {
	    value= 0;
	    return kFALSE;
	}
    }
    return kTRUE;
}


// Read a Float_t value from the TAG:
char VAbsBit::getFloat( Float_t & value, const char* key ) const 
{    
    // Has setRootTree been called?:
    assert( fBitTree != 0 );
    
    // Check if tag variable exists:
    // first look in hash directory:
    
    TString theKey= "TAG_f_";
    theKey += key;
    
    TBranch* branch = (TBranch*) fFloatBranchTable->FindObject(theKey);
    
    if (branch == 0) {
	// Check if tag variable exists:
	branch= ((TTree*)fBitTree)->GetBranch( theKey );
	if( branch == 0 ) { 
	    value= 0.0; 
	    cerr << " error, couldn' get branch " << theKey.Data() << endl;
	    return kFALSE; 
	}
	else {
	    fFloatBranchTable->Add(branch); // Add the branch to the table
	}
    }
    
    if( branch != 0 ) { 
	
	// Get the address of the memory buffer for this branch, create one
	// if neccessary:
	void* address= branch->GetAddress();
	if( address == 0 ) {
	    address= new TFloatTagElem;
	    branch->SetAddress( address );
	}
	
	// Read the branch:
	Int_t nbytes= branch->GetEntry( fEventNumber );
	if( nbytes == 0 ) {
	    value= kFALSE; 
	    cerr << " error, no data for branch " << theKey.Data() 
		<< " in event " << fEventNumber << endl;
	    return kFALSE;
	}
	
	// Put retrieved value Int_to input argument and return status:
	TFloatTagElem val= *((TFloatTagElem*)address);
	if( val.valid ) {
	    value= val.value;
	    return kTRUE;
	}
	else {
	    value= 0.0;
	    return kFALSE;
	}
    }
    return kTRUE;
}


// Put a char value Int_to a branch in the ROOT tree:
char VAbsBit::putBool( char value, const char* key ) 
{   
    // Has setRootTree been called?
    assert( fBitTree != 0 );
    
    // Try to get branch:
    TString theKey= "TAG_b_";
    theKey+= key;
    TBranch* branch= fBitTree->GetBranch( theKey );
    
    if( branch != 0 ) {
	
	// The variable already exists, just fill the branch buffer:
	*((UChar_t*)(branch->GetAddress()))= value;
	
    }
    else {
	
	// It's a new variable, create a new branch:
	//    TBranch* tagBranch= fBitTree->GetBranch( _branchName );
	//    assert( tagBranch != 0 );
	UChar_t* address= new UChar_t( 0 );
	//    TBranch* newBranch= new TBranch( theKey, address, "value/b", 
	//				     fBufferSize );
	//    tagBranch->GetListOfBranches()->Add( newBranch );
	//    _Bool_tBranchTable.Add( newBranch );
	TBranch* newBranch= fBitTree->Branch( theKey, address, 
	    "value/b", fBufferSize );
	
	// For all events except this one set value of the new branch to "invalid":
	*address= boolInvalid;
	for( unsigned i= 0; i < fEventNumber; i++ ) newBranch->Fill();
	
	// Put the value for this event in the buffer:
	if( value ) *address= boolTrue;
	else *address= boolFalse;
	
    }
    
    // The End;
    return kTRUE;
    
}


// Put an Int_t value Int_to a branch in the ROOT tree:
char VAbsBit::putInt( Int_t value, const char* key ) 
{  
    assert( fBitTree != 0 );
    TString theKey= "TAG_i_";
    theKey+= key;
    TBranch* branch= fBitTree->GetBranch( theKey );
    
    if( branch != 0 ) {
	
	// The variable already exists, just fill the branch buffer:
	TIntTagElem* i= (TIntTagElem*)(branch->GetAddress());
	i->value= value;
	i->valid= kTRUE;
	
    }
    else {
	
	// It's a new variable, create a new branch:
	//    TBranch* tagBranch= fBitTree->GetBranch( _branchName );
	//    assert( tagBranch != 0 );
	TIntTagElem* address= new TIntTagElem;
	//    TBranch* newBranch= new TBranch( theKey, address, "value/I:valid/b",
	//				     fBufferSize );
	//    tagBranch->GetListOfBranches()->Add( newBranch );
	//    _Int_tBranchTable.Add( newBranch );
	TBranch* newBranch= fBitTree->Branch( theKey, address, 
	    "value/I:valid/b", fBufferSize );
	
	// For all events except this mark the branch content as "invalid":    
	address->value= 0;
	address->valid= kFALSE;
	for( unsigned i= 0; i < fEventNumber; i++ ) newBranch->Fill();
	
	// Put the value for this event in the buffer:
	address->value= value;
	address->valid= kTRUE;
	
    }
    
    // The End:
    return kTRUE;
    
}


// Put a Float_t value Int_to a branch in the ROOT tree:
char VAbsBit::putFloat( Float_t value, const char* key ) 
{  
    assert( fBitTree != 0 );
    TString theKey= "TAG_f_";
    theKey+= key;
    TBranch* branch= fBitTree->GetBranch( theKey );
    
    if( branch != 0 ) {
	
	// The variable already exists, just fill the branch buffer:
	TFloatTagElem* f= (TFloatTagElem*)(branch->GetAddress());
	f->value= value;
	f->valid= kTRUE;
	
    }
    else {
	
	// It's a new variable, create a new branch:
	//    TBranch* tagBranch= fBitTree->GetBranch( _branchName );
	//    assert( tagBranch != 0 );
	TFloatTagElem* address= new TFloatTagElem;
	//    TBranch* newBranch= new TBranch( theKey, address, "value/F:valid/b",
	//				     fBufferSize );
	//    tagBranch->GetListOfBranches()->Add( newBranch );
	//    _Float_tBranchTable.Add( newBranch );
	TBranch* newBranch= fBitTree->Branch( theKey, address, 
	    "value/F:valid/b", fBufferSize );
	
	// For all events except this mark the branch content as "invalid":
	address->value= 0.0;
	address->valid= kFALSE;
	for( unsigned i= 0; i < fEventNumber; i++ ) newBranch->Fill();
	
	// Put the value for this event in the buffer:
	address->value= value;
	address->valid= kTRUE;
	
    }
    
    // The End:
    return kTRUE;
    
}

void VAbsBit::FillCache(void) const
{
    // Get the list of branches in the tree, strip out those
    // containing TAG Bool_t values:
    if (fBitTree!=0 && fBitTree->GetTree()!=0) {
	TObjArray* branches= fBitTree->GetTree()->GetListOfBranches();
	if (branches!=0) {
	    TIter iter( branches );
	    TBranch* branch= 0;
	    while( branch= (TBranch*) iter.Next() ) {
		TString theKey = branch->GetName();
		if(theKey.Index("TAG_b_")>=0)
		    if (fBoolBranchTable->FindObject(theKey)==0) fBoolBranchTable->Add(branch);
		    else if (theKey.Index("TAG_i_")>=0)
			if (fIntBranchTable->FindObject(theKey)==0) fIntBranchTable->Add(branch);
			else if (theKey.Index("TAG_f_")>=0)
			    if (fFloatBranchTable->FindObject(theKey)==0) fFloatBranchTable->Add(branch);
	    }
	}
    }
}


void VAbsBit::PrintOn(std::ostream& o) const 
{
    o<<endl;
    o <<"############################ Tag values and bits #################################" << endl;
    o.setf(ios::right);
    
    o << endl;
    o << "Integer:" << fIntBranchTable->GetSize() << endl;
    VAbsBitIntIterator intIter(this);
    while (intIter.Next()) o << setw(20) << intIter.Key() << setw(20) << intIter.Value() << endl;
    
    o << endl;
    o << "Float:" << fFloatBranchTable->GetSize() << endl;
    VAbsBitFloatIterator floatIter(this);
    while (floatIter.Next()) o << setw(20) << floatIter.Key() << setw(20) << floatIter.Value() << endl;
    
    o << endl;
    o << "Boolean:" << fBoolBranchTable->GetSize() << endl;
    
    VAbsBitBoolIterator boolIter(this);
    while (boolIter.Next()) {
	o << setw(20) << boolIter.Key() << setw(20) ;
        if (boolIter.Value())
	    o << 'T' << endl;
	else
	    o << 'F' << endl;
    }
    
    o <<"##################################################################################" << endl;
}        


std::ostream&  operator << (std::ostream& o, const VAbsBit& a) { a.PrintOn(o); return o; }


// Implementation of the VAbsTag interface

Int_t	VAbsBit::GetNumberOfTracks() const { return GetInt("nTracks"); }
Int_t	VAbsBit::GetNumberOfGoodTracksLoose() const { return GetInt("nGoodTrkLoose"); }
Int_t	VAbsBit::GetNumberOfGoodTracksTight() const { return GetInt("nGoodTrkTight"); }
TVector3 VAbsBit::GetTotalMomentum() const { return TVector3(GetFloat("pTotalCosTh"),GetFloat("pTotalPhi"),GetFloat("pTotalMag")); }
Float_t	VAbsBit::GetTotalEnergy() const { return GetFloat("eTotal"); }
Float_t	VAbsBit::GetTotalMass() const { return GetFloat("pTotalMass"); }
TVector3 VAbsBit::GetPrimaryTrackMomentum() const { return TVector3(GetFloat("p1CosTheta"),GetFloat("p1Phi"),GetFloat("p1Mag")); }
Float_t	VAbsBit::GetPrimaryTrackShowerEnergy() const { return GetFloat("p1EmcCandEnergy"); }
Int_t	VAbsBit::GetPrimaryTrackHypothesis() const { return GetInt("p1PidHypo"); }
TVector3 VAbsBit::GetSecondaryTrackMomentum() const { return TVector3(GetFloat("p2CosTheta"),GetFloat("p2Phi"),GetFloat("p2Mag")); }
Float_t	VAbsBit::GetSecondaryTrackShowerEnergy() const { return GetFloat("p2EmcCandEnergy"); }
Int_t	VAbsBit::GetSecondaryTrackHypothesis() const { return GetInt("p2PidHypo"); }
Int_t	VAbsBit::GetTotalChargeLoose() const { return GetInt("totChargeLoose"); }
Int_t	VAbsBit::GetTotalChargeTight() const { return GetInt("totChargeTight"); }
Int_t	VAbsBit::GetPrimaryLeptonHypothesisLoose() const { return GetInt("leptLoose1hypo"); }
Int_t	VAbsBit::GetSecondaryLeptonHypothesisLoose() const { return GetInt("leptLoose2hypo"); }
Int_t	VAbsBit::GetPrimaryLeptonHypothesisTight() const { return GetInt("leptTight1hypo"); }
Int_t	VAbsBit::GetSecondaryLeptonHypothesisTight() const { return GetInt("leptTight2hypo"); }
Int_t	VAbsBit::GetPrimaryLeptonChargeLoose() const { return GetInt("leptLoose1charge"); }
Int_t	VAbsBit::GetSecondaryLeptonChargeLoose() const { return GetInt("leptLoose2charge"); }
Int_t	VAbsBit::GetPrimaryLeptonChargeTight() const { return GetInt("leptTight1charge"); }
Int_t	VAbsBit::GetSecondaryLeptonChargeTight() const { return GetInt("leptTight2charge"); }
Float_t	VAbsBit::GetPrimaryLeptonMomentumLoose() const { return GetFloat("leptLoose1p"); }
Float_t	VAbsBit::GetSecondaryLeptonMomentumLoose() const { return GetFloat("leptLoose2p"); }
Float_t	VAbsBit::GetPrimaryLeptonMomentumTight() const { return GetFloat("leptTight1p"); }
Float_t	VAbsBit::GetSecondaryLeptonMomentumTight() const { return GetFloat("leptTight2p"); }
TVector3 VAbsBit::GetTotalNeutralEnergy() const { return TVector3(GetFloat("eNeutralCosTh"),GetFloat("eNeutralPhi"),GetFloat("eNeutralMag")); }
TVector3 VAbsBit::GetHighestEnergyShower() const { return TVector3(GetFloat("e1CosTheta"),GetFloat("e1Phi"),GetFloat("e1Mag")); }
TVector3 VAbsBit::GetSecondHighestEnergyShower() const { return TVector3(GetFloat("e2CosTheta"),GetFloat("e2Phi"),GetFloat("e2Mag")); }
TVector3 VAbsBit::GetPrimaryVertex() const { return TVector3(GetFloat("xPrimaryVtx"),GetFloat("yPrimaryVtx"),GetFloat("zPrimaryVtx")); }
Float_t	VAbsBit::GetPrimaryVertexProbability() const { return GetFloat("probPrimaryVtx"); }
TVector3 VAbsBit::GetThrust() const { return TVector3(GetFloat("thrustCosTh"),GetFloat("thrustPhi"),GetFloat("thrustMag")); }
Float_t	VAbsBit::GetR2() const { return GetFloat("R2"); }    
Int_t	VAbsBit::GetLeptonsLoose() const { return GetInt("nLeptonsLoose"); }
Int_t	VAbsBit::GetElectronsLoose() const { return GetInt("neLoose"); }
Int_t	VAbsBit::GetMuonsLoose() const { return GetInt("nmuLoose"); }
Int_t	VAbsBit::GetPionsLoose() const { return GetInt("npiLoose"); }
Int_t	VAbsBit::GetKaonsLoose() const { return GetInt("nKLoose"); }
Int_t	VAbsBit::GetProtonsLoose() const { return GetInt("npLoose"); }
Int_t	VAbsBit::GetGammasLoose() const { return GetInt("ngammaLoose"); }
Int_t	VAbsBit::GetMergedPi0Loose() const { return GetInt("npi0MergedLoose"); }
Int_t	VAbsBit::GetPi0Loose() const { return GetInt("npi0Loose"); }
Int_t	VAbsBit::GetKsLoose() const { return GetInt("nKsLoose"); }
Int_t	VAbsBit::GetLeptonsTight() const { return GetInt("nLeptonsTight"); }
Int_t	VAbsBit::GetElectronsTight() const { return GetInt("neTight"); }
Int_t	VAbsBit::GetMuonsTight() const { return GetInt("nmuTight"); }
Int_t	VAbsBit::GetPionsTight() const { return GetInt("npiTight"); }
Int_t	VAbsBit::GetKaonsTight() const { return GetInt("nKTight"); }
Int_t	VAbsBit::GetProtonsTight() const { return GetInt("npTight"); }
Int_t	VAbsBit::GetGammasTight() const { return GetInt("ngammaTight"); }
Int_t	VAbsBit::GetMergedPi0Tight() const { return GetInt("npi0MergedTight"); }
Int_t	VAbsBit::GetPi0Tight() const { return GetInt("npi0Tight"); }
Int_t	VAbsBit::GetKsTight() const { return GetInt("nKsTight"); }
Float_t VAbsBit::GetBbqqDiscr() const { return GetFloat("bbqqDiscr"); }
Float_t VAbsBit::GetBGFR2() const { return GetFloat("BGFR2"); }
Float_t VAbsBit::GetBunchT0DeltaDircTrk() const { return GetFloat("BunchT0DeltaDircTrk"); }
Float_t VAbsBit::GetBunchT0DeltaFF() const { return GetFloat("BunchT0DeltaFF"); } 
Float_t VAbsBit::GetBunchT0FinderErr() const { return GetFloat("BunchT0FinderErr"); } 
Float_t VAbsBit::GetBunchT0FitterErr() const { return GetFloat("BunchT0FitterErr"); }
Float_t VAbsBit::GetECharShow() const { return GetFloat("eCharShow"); } 
Float_t VAbsBit::GetElecLoose1cm() const { return GetFloat("elecLoose1cm"); }
Float_t VAbsBit::GetElecLoose1l() const { return GetFloat("elecLoose1l"); }
Float_t VAbsBit::GetElecLoose2cm() const { return GetFloat("elecLoose2cm"); } 
Float_t VAbsBit::GetElecLoose2l() const { return GetFloat("elecLoose2l"); } 
Float_t VAbsBit::GetElecTight1cm() const { return GetFloat("elecTight1cm"); }
Float_t VAbsBit::GetElecTight1l() const { return GetFloat("elecTight1l"); } 
Float_t VAbsBit::GetElecTight2cm() const { return GetFloat("elecTight2cm"); } 
Float_t VAbsBit::GetElecTight2l() const { return GetFloat("elecTight2l"); }
Float_t VAbsBit::GetENeutShow() const { return GetFloat("eNeutShow"); }
Float_t VAbsBit::GetL3OutBhabhaFlatOprValue() const { return GetFloat("L3OutBhabhaFlatOprValue"); } 
Float_t VAbsBit::GetL3OutBhabhaFlatValue() const { return GetFloat("L3OutBhabhaFlatValue"); }
Float_t VAbsBit::GetMuonLoose1cm() const { return GetFloat("muonLoose1cm"); } 
Float_t VAbsBit::GetMuonLoose1l() const { return GetFloat("muonLoose1l"); } 
Float_t VAbsBit::GetMuonLoose2cm() const { return GetFloat("muonLoose2cm"); } 
Float_t VAbsBit::GetMuonLoose2l() const { return GetFloat("muonLoose2l"); } 
Float_t VAbsBit::GetMuonTight1cm() const { return GetFloat("muonTight1cm"); } 
Float_t VAbsBit::GetMuonTight1l() const { return GetFloat("muonTight1l"); } 
Float_t VAbsBit::GetMuonTight2cm() const { return GetFloat("muonTight2cm"); } 
Float_t VAbsBit::GetMuonTight2l() const { return GetFloat("muonTight2l"); }
Float_t VAbsBit::GetPTotalScalar() const { return GetFloat("pTotalScalar"); }
Float_t VAbsBit::GetR2All() const { return GetFloat("R2All"); }
Float_t VAbsBit::GetSphericityAll() const { return GetFloat("sphericityAll"); } 
Float_t VAbsBit::GetTagLooseProton_maxCmsP() const { return GetFloat("TagLooseProton_maxCmsP"); } 
Float_t VAbsBit::GetTagTightProton_maxCmsP() const { return GetFloat("TagTightProton_maxCmsP"); }
Float_t VAbsBit::GetThrustCosThAll() const { return GetFloat("thrustCosThAll"); } 
Float_t VAbsBit::GetThrustMagAll() const { return GetFloat("thrustMagAll"); } 
Float_t VAbsBit::GetThrustPhiAll() const { return GetFloat("thrustPhiAll"); } 
Int_t	VAbsBit::GetD3PS() const { return GetInt("D3PS"); } 
Int_t	VAbsBit::GetDmeson0() const { return GetInt("Dmeson0"); } 
Int_t	VAbsBit::GetDsmeson0() const { return GetInt("Dsmeson0"); } 
Int_t	VAbsBit::GetDstar0() const { return GetInt("Dstar0"); } 
Int_t	VAbsBit::GetDstar1() const { return GetInt("Dstar1"); } 
Int_t	VAbsBit::GetLumiBhabhaWeight() const { return GetInt("LumiBhabhaWeight"); } 
Int_t	VAbsBit::GetNumberOfNeutralCands() const { return GetInt("nNeutralCands"); } 
Int_t	VAbsBit::GetNumberOfNeutralCndLoose() const { return GetInt("nNeutralCndLoose"); } 
Int_t	VAbsBit::GetNumberOfNeutralCndTight() const { return GetInt("nNeutralCndTight"); } 
Int_t	VAbsBit::GetTagEtaPeppTwoBody_maxCmsPmaxCosT() const { return GetInt("TagEtaPeppTwoBody_maxCmsPmaxCosT"); } 
Int_t	VAbsBit::GetTagEtaPrhogTwoBody_maxCmsPmaxCosT() const { return GetInt("TagEtaPrhogTwoBody_maxCmsPmaxCosT"); } 
Int_t	VAbsBit::GetTagEtaTwoBody_maxCmsPmaxCosT() const { return GetInt("TagEtaTwoBody_maxCmsPmaxCosT"); } 
Int_t	VAbsBit::GetTagKstpTwoBody_maxCmsPmaxCosT() const { return GetInt("TagKstpTwoBody_maxCmsPmaxCosT"); } 
Int_t	VAbsBit::GetTagKstTwoBody_maxCmsPmaxCosT() const { return GetInt("TagKstTwoBody_maxCmsPmaxCosT"); } 
Int_t	VAbsBit::GetTagKTwoBody_maxCmsPmaxCosT() const { return GetInt("TagKTwoBody_maxCmsPmaxCosT"); } 
Int_t	VAbsBit::GetTagK_S0TwoBody_maxCmsPmaxCosT() const { return GetInt("TagK_S0TwoBody_maxCmsPmaxCosT"); } 
Int_t	VAbsBit::GetTagOmegaTwoBody_maxCmsPmaxCosT() const { return GetInt("TagOmegaTwoBody_maxCmsPmaxCosT"); } 
Int_t	VAbsBit::GetTagPhiTwoBody_maxCmsPmaxCosT() const { return GetInt("TagPhiTwoBody_maxCmsPmaxCosT"); } 
Int_t	VAbsBit::GetTagPi0TwoBody_maxCmsPmaxCosT() const { return GetInt("TagPi0TwoBody_maxCmsPmaxCosT"); } 
Int_t	VAbsBit::GetTagRhopTwoBody_maxCmsPmaxCosT() const { return GetInt("TagRhopTwoBody_maxCmsPmaxCosT"); } 
Int_t	VAbsBit::GetTagRhoTwoBody_maxCmsPmaxCosT() const { return GetInt("TagRhoTwoBody_maxCmsPmaxCosT"); } 

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsBitBoolIterator	                                                //
// VAbsBitIntIterator	                                                //
// VAbsBitFloatIterator	                                                //
//                                                                      //
// Iterators for VAbsBitBase                                            //
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 1999					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

ClassImp(VAbsBitBoolIterator)
ClassImp(VAbsBitIntIterator)
ClassImp(VAbsBitFloatIterator)

TBuffer &operator>>(TBuffer &buf, VAbsBitBoolIterator *&obj)
{
   obj = (VAbsBitBoolIterator *) buf.ReadObject(VAbsBitBoolIterator::Class());
   return buf;
}

TBuffer &operator>>(TBuffer &buf, VAbsBitIntIterator *&obj)
{
   obj = (VAbsBitIntIterator *) buf.ReadObject(VAbsBitIntIterator::Class());
   return buf;
}

TBuffer &operator>>(TBuffer &buf, VAbsBitFloatIterator *&obj)
{
   obj = (VAbsBitFloatIterator *) buf.ReadObject(VAbsBitFloatIterator::Class());
   return buf;
}

VAbsBitBoolIterator::VAbsBitBoolIterator(const VAbsBit* theTag) :
TListIter((TList*)theTag->GetBoolCache()), fTag(theTag), fCurrentBranch(0)
{
    static Bool_t cached = kFALSE;
    if (!cached) {
	fTag->FillCache();
	cached = kTRUE;
    }
}

// Destructor:
VAbsBitBoolIterator::~VAbsBitBoolIterator() 
{}

TObject* VAbsBitBoolIterator::Next() 
{
    TObject *next = TListIter::Next();
    fCurrentBranch = (TBranch *) next;
    return next; 
}

// Return the TAG key:
const char* VAbsBitBoolIterator::Key() 
{
    const char* result= 0;
    if( fCurrentBranch != 0 ) {
	result= fCurrentBranch->GetName()+6;
    }
    return result;
}

// Return the TAG Bool_t value:
Bool_t VAbsBitBoolIterator::Value() 
{
    Bool_t result= kFALSE;
    if(fCurrentBranch != 0) {
	result = fTag->GetBool(Key());
    }
    return result;
}


VAbsBitIntIterator::VAbsBitIntIterator(const VAbsBit* theTag) :
TListIter((TList*)theTag->GetIntCache()), fTag(theTag), fCurrentBranch(0)
{
    static Bool_t cached = kFALSE;
    if (!cached) {
	fTag->FillCache();
	cached = kTRUE;
    }
}

VAbsBitIntIterator::~VAbsBitIntIterator() 
{}

TObject* VAbsBitIntIterator::Next() 
{
    TObject *next = TListIter::Next();
    fCurrentBranch = (TBranch *) next;
    return next; 
}

// Return the TAG key:
const char* VAbsBitIntIterator::Key() 
{
    const char* result= 0;
    if( fCurrentBranch != 0 ) {
	result= fCurrentBranch->GetName()+6;
    }
    return result;
}

// Return the TAG int value:
Int_t VAbsBitIntIterator::Value() 
{
    Int_t result= 0;
    if(fCurrentBranch != 0) {
	result = fTag->GetInt(Key());
    }
    return result;
}


VAbsBitFloatIterator::VAbsBitFloatIterator(const VAbsBit* theTag) :
TListIter((TList*)theTag->GetFloatCache()), fTag(theTag), fCurrentBranch(0)
{
    static Bool_t cached = kFALSE;
    if (!cached) {
	fTag->FillCache();
	cached = kTRUE;
    }
}

VAbsBitFloatIterator::~VAbsBitFloatIterator() 
{}

TObject* VAbsBitFloatIterator::Next() 
{
    TObject *next = TListIter::Next();
    fCurrentBranch = (TBranch *) next;
    return next; 
}

// Return the TAG key:
const char* VAbsBitFloatIterator::Key() 
{
    const char* result= 0;
    if( fCurrentBranch != 0 ) {
	result= fCurrentBranch->GetName()+6;
    }
    return result;
}

// Return the TAG int value:
Float_t VAbsBitFloatIterator::Value() 
{
    Float_t result= 0;
    if(fCurrentBranch != 0) {
	result = fTag->GetFloat(Key());
    }
    return result;
}
