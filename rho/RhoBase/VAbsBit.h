#ifndef VABSBIT_H
#define VABSBIT_H
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

#include <iostream>

#include "TObject.h"
#include "TTree.h"
#include "TList.h"
#include "RhoBase/VAbsTag.h"

class THashList;
class AbsEvent;

// Data structure used to store an integer value in a root branch:
struct TIntTagElem {
  Int_t value;
  Bool_t valid;
};


// Data structure used to store a float value in a root branch:
struct TFloatTagElem {
  Float_t value;
  Bool_t valid;
};

enum boolVals { boolFalse= 0, boolTrue= 1, boolInvalid= 2 };

class VAbsBit : public VAbsTag {
    
public:
    //Constructor
    VAbsBit();
    VAbsBit(TTree *t,const unsigned &bs = 32768); // Work in a specific tree
    
    //Destructor
    virtual ~VAbsBit();

    void SetTree(TTree *t) {fBitTree = t;}  // This sets the underlying tree
    TTree* GetTree() const { return fBitTree; }   // Get the underlying tree

    void SetEventNumber(UInt_t n) {fEventNumber = n;}  // This sets the actual position

    void Fill() { fBitTree->Fill(); }

    void ClearCache(void);		    //Reset the cache
    void FillCache(void) const;
    THashList *GetBoolCache() const { return fBoolBranchTable; }
    THashList *GetIntCache() const  { return fIntBranchTable; }
    THashList *GetFloatCache() const  { return fFloatBranchTable; }

    // Babarian access methods

    virtual char getBool(char &aBool,const char* key) const;
    virtual char getInt(Int_t &anInt,const char* key) const;
    virtual char getFloat(Float_t &aFloat,const char* key) const;
    virtual char putBool(char aBool,const char* key);
    virtual char putInt(Int_t anInt,const char* key);
    virtual char putFloat(Float_t aFloat,const char* key);

    // TagAccessor interface

    char declareBool(const char* name, char defaultValue = 0) { return kTRUE; }
    char declareInt(const char* name, int defaultValue = 0) { return kTRUE; }
    char declareFloat(const char* name, float defaultValue = 0.0) { return kTRUE; }
    virtual char declareTags(AbsEvent* ) {return kTRUE;}
    //char setup(AbsEvent* = 0);
    char setBool(const char* name, char value = kTRUE) { return putBool(value,name); }
    char setInt(const char* name, int value) { return putInt(value,name); }
    char setFloat(const char* name, float value)  { return putFloat(value,name); }

    virtual Bool_t GetTrigger(const char* key) const { char aBool; getBool(aBool,key); return aBool; }
    void   SetTrigger(const char* key,Bool_t aBool) { putBool(aBool,key); }

    virtual Int_t GetInt(const char* key) const { Int_t anInt; getInt(anInt,key); return anInt; }
    void   SetInt(const char* key,Int_t anInt) { putInt(anInt,key); }

    virtual Float_t GetFloat(const char* key) const { Float_t aFloat; getFloat(aFloat,key); return aFloat; }
    void   SetFloat(const char* key,Float_t aFloat) { putFloat(aFloat,key); }

    virtual Bool_t GetBool(const char* key) const { char aBool; getBool(aBool,key); return aBool; }
    void   SetBool(const char* key,Bool_t aBool) { putBool(aBool,key); }

    // VAbsTag Interface

    virtual Int_t	GetNumberOfTracks() const;
    virtual Int_t	GetNumberOfGoodTracksLoose() const;
    virtual Int_t	GetNumberOfGoodTracksTight() const;
    virtual TVector3    GetTotalMomentum() const;
    virtual Float_t	GetTotalEnergy() const;
    virtual Float_t	GetTotalMass() const;
    virtual TVector3    GetPrimaryTrackMomentum() const;
    virtual Float_t	GetPrimaryTrackShowerEnergy() const;
    virtual Int_t	GetPrimaryTrackHypothesis() const;
    virtual TVector3    GetSecondaryTrackMomentum() const;
    virtual Float_t	GetSecondaryTrackShowerEnergy() const;
    virtual Int_t	GetSecondaryTrackHypothesis() const;
    virtual Int_t	GetTotalChargeLoose() const;
    virtual Int_t	GetTotalChargeTight() const;
    virtual Int_t	GetPrimaryLeptonHypothesisLoose() const;
    virtual Int_t	GetSecondaryLeptonHypothesisLoose() const;
    virtual Int_t	GetPrimaryLeptonHypothesisTight() const;
    virtual Int_t	GetSecondaryLeptonHypothesisTight() const;
    virtual Int_t	GetPrimaryLeptonChargeLoose() const;
    virtual Int_t	GetSecondaryLeptonChargeLoose() const;
    virtual Int_t	GetPrimaryLeptonChargeTight() const;
    virtual Int_t	GetSecondaryLeptonChargeTight() const;
    virtual Float_t	GetPrimaryLeptonMomentumLoose() const;
    virtual Float_t	GetSecondaryLeptonMomentumLoose() const;
    virtual Float_t	GetPrimaryLeptonMomentumTight() const;
    virtual Float_t	GetSecondaryLeptonMomentumTight() const;
    virtual TVector3    GetTotalNeutralEnergy() const;
    virtual TVector3    GetHighestEnergyShower() const;
    virtual TVector3    GetSecondHighestEnergyShower() const;
    virtual TVector3	GetPrimaryVertex() const;
    virtual Float_t	GetPrimaryVertexProbability() const;
    virtual TVector3	GetThrust() const;
    virtual Float_t	GetR2() const;    
    virtual Int_t	GetLeptonsLoose() const;
    virtual Int_t	GetElectronsLoose() const;
    virtual Int_t	GetMuonsLoose() const;
    virtual Int_t	GetPionsLoose() const;
    virtual Int_t	GetKaonsLoose() const;
    virtual Int_t	GetProtonsLoose() const;
    virtual Int_t	GetGammasLoose() const;
    virtual Int_t	GetMergedPi0Loose() const;
    virtual Int_t	GetPi0Loose() const;
    virtual Int_t	GetKsLoose() const;
    virtual Int_t	GetLeptonsTight() const;
    virtual Int_t	GetElectronsTight() const;
    virtual Int_t	GetMuonsTight() const;
    virtual Int_t	GetPionsTight() const;
    virtual Int_t	GetKaonsTight() const;
    virtual Int_t	GetProtonsTight() const;
    virtual Int_t	GetGammasTight() const;
    virtual Int_t	GetMergedPi0Tight() const;
    virtual Int_t	GetPi0Tight() const;
    virtual Int_t	GetKsTight() const;
    // Trigger and Skim information
    virtual Float_t	GetBbqqDiscr() const ; 
    virtual Float_t	GetBGFR2() const ;
    virtual Float_t	GetBunchT0DeltaDircTrk() const ;
    virtual Float_t	GetBunchT0DeltaFF() const ; 
    virtual Float_t	GetBunchT0FinderErr() const ; 
    virtual Float_t	GetBunchT0FitterErr() const ;
    virtual Float_t	GetECharShow() const ; 
    virtual Float_t	GetElecLoose1cm() const ;
    virtual Float_t	GetElecLoose1l() const ;
    virtual Float_t	GetElecLoose2cm() const ; 
    virtual Float_t	GetElecLoose2l() const ; 
    virtual Float_t	GetElecTight1cm() const ;
    virtual Float_t	GetElecTight1l() const ; 
    virtual Float_t	GetElecTight2cm() const ; 
    virtual Float_t	GetElecTight2l() const ;
    virtual Float_t	GetENeutShow() const ;
    virtual Float_t	GetL3OutBhabhaFlatOprValue() const ; 
    virtual Float_t	GetL3OutBhabhaFlatValue() const ;
    virtual Float_t	GetMuonLoose1cm() const ; 
    virtual Float_t	GetMuonLoose1l() const ; 
    virtual Float_t	GetMuonLoose2cm() const ; 
    virtual Float_t	GetMuonLoose2l() const ; 
    virtual Float_t	GetMuonTight1cm() const ; 
    virtual Float_t	GetMuonTight1l() const ; 
    virtual Float_t	GetMuonTight2cm() const ; 
    virtual Float_t	GetMuonTight2l() const ;
    virtual Float_t	GetPTotalScalar() const ;
    virtual Float_t	GetR2All() const ;
    virtual Float_t	GetSphericityAll() const ; 
    virtual Float_t	GetTagLooseProton_maxCmsP() const ; 
    virtual Float_t	GetTagTightProton_maxCmsP() const ;
    virtual Float_t	GetThrustCosThAll() const ; 
    virtual Float_t	GetThrustMagAll() const ; 
    virtual Float_t	GetThrustPhiAll() const ; 
    virtual Int_t	GetD3PS() const ; 
    virtual Int_t	GetDmeson0() const ; 
    virtual Int_t	GetDsmeson0() const ; 
    virtual Int_t	GetDstar0() const ; 
    virtual Int_t	GetDstar1() const ; 
    virtual Int_t	GetLumiBhabhaWeight() const ; 
    virtual Int_t	GetNumberOfNeutralCands() const ; 
    virtual Int_t	GetNumberOfNeutralCndLoose() const ; 
    virtual Int_t	GetNumberOfNeutralCndTight() const ; 
    virtual Int_t	GetTagEtaPeppTwoBody_maxCmsPmaxCosT() const ; 
    virtual Int_t	GetTagEtaPrhogTwoBody_maxCmsPmaxCosT() const ; 
    virtual Int_t	GetTagEtaTwoBody_maxCmsPmaxCosT() const ; 
    virtual Int_t	GetTagKstpTwoBody_maxCmsPmaxCosT() const ; 
    virtual Int_t	GetTagKstTwoBody_maxCmsPmaxCosT() const ; 
    virtual Int_t	GetTagKTwoBody_maxCmsPmaxCosT() const ; 
    virtual Int_t	GetTagK_S0TwoBody_maxCmsPmaxCosT() const ; 
    virtual Int_t	GetTagOmegaTwoBody_maxCmsPmaxCosT() const ; 
    virtual Int_t	GetTagPhiTwoBody_maxCmsPmaxCosT() const ; 
    virtual Int_t	GetTagPi0TwoBody_maxCmsPmaxCosT() const ; 
    virtual Int_t	GetTagRhopTwoBody_maxCmsPmaxCosT() const ; 
    virtual Int_t	GetTagRhoTwoBody_maxCmsPmaxCosT() const ; 

    //###################################################################    
    
    void PrintOn( std::ostream& o=std::cout ) const;

private:
    void InvalidateBranches(void);	    //Reset the persistent buffer

    TTree *fBitTree;			    //!Tree to hold the bits
    ULong_t fEventNumber;		    //!Actual event
    ULong_t fBufferSize;		    //!Size of input buffer
    THashList *fBoolBranchTable;	    //!Bool cache
    THashList *fIntBranchTable;	     	    //!Int cache
    THashList *fFloatBranchTable;	    //!float cache

public:        
    ClassDef(VAbsBit,1) //Analysis bits (TAG)
	
};      

// standalone print
std::ostream&  operator <<(std::ostream& o, const VAbsBit&);

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

class VAbsBitBoolIterator : public TListIter {
    
public:
    
    VAbsBitBoolIterator(const VAbsBit* );
    
    virtual ~VAbsBitBoolIterator();
    
    // Operations
    
    // Return the next boolean element from the current tag. The
    // function value indicates whether a new element was located
    // or whether the iterator was exhausted.
    TObject* Next();
    
    // Return the name of the current boolean element within the tag.
    const char* Key();
    
    // Return the value of the current boolean element within the tag.
    Bool_t Value();
    
private:
    const VAbsBit* fTag; //!
    TBranch* fCurrentBranch; //!
public:
    ClassDef(VAbsBitBoolIterator,1) //Iterator for bool attributes
};


class VAbsBitIntIterator : public TListIter  {
    
public:
    
    VAbsBitIntIterator(const VAbsBit* );
    
    virtual ~VAbsBitIntIterator();
    
    // Operations
    
    // Return the next int element from the current tag. The
    // function value indicates whether a new element was located
    // or whether the iterator was exhausted.
    TObject* Next();
    
    // Return the name of the current int element within the tag.
    const char* Key();
    
    // Return the value of the current int element within the tag.
    Int_t Value();
    
private:
    
    // Data members:
    
    // List of int TAG branches:
    const VAbsBit* fTag; //!
    TBranch* fCurrentBranch; //!
public:
    ClassDef(VAbsBitIntIterator,1) //Iterator for integer attributes
};


class VAbsBitFloatIterator : public TListIter  {
    
public:
    
    VAbsBitFloatIterator(const VAbsBit* );
    
    virtual ~VAbsBitFloatIterator();
    
    // Operations
    
    // Return the next float element from the current tag. The
    // function value indicates whether a new element was located
    // or whether the iterator was exhausted.
    TObject* Next();
    
    // Return the name of the current float element within the tag.
    const char* Key();
    
    // Return the value of the current float element within the tag.
    Float_t Value();
    
private:
    
    // Data members:
    
    // List of float TAG branches:
    const VAbsBit* fTag; //!
    TBranch* fCurrentBranch; //!
public:
    ClassDef(VAbsBitFloatIterator,1) //Iterator for float attributes
};

#endif
