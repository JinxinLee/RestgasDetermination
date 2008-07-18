#ifndef VABSREADER_H
#define VABSREADER_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsReader								//
//                                                                      //
// Base class of the event reader classes				//
//                                                                      //
// Author: Marcel Kunze, RUB, May 2000					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include "TNamed.h"

class VAbsBit;
class VAbsTag;
class VAbsMicroCandidate;
class PAFListBase;
class TCandList;
class VAbsTruth;
class PAFEventBase;
class PAFMcBase;
class PAFMcAssocBase;
class EidInfoR;
class BtaCandAssocMapR;
class BtaCandListR;
class StdHepR;
class StdHepTrkR;
class TCandidate;
class TPrintTree;

class VAbsReader : public TNamed {
    
public:
    VAbsReader(const char *, const char *);
    virtual ~VAbsReader( );
    
    // Operations

    void SetVerbose(Bool_t yesNo = kTRUE) { fVerbose = yesNo; }
    virtual TPrintTree *GetPrintTree() const = 0;
    virtual void SetPrintTree(TPrintTree* tree) = 0;

    virtual void InitializeStreams(Bool_t multi) = 0;
    virtual Bool_t SetStreamStatus(const char *stream,Bool_t status=kTRUE) = 0;

    virtual UInt_t  GetCurrentIndex() = 0;
    virtual Int_t GetEvent(UInt_t number) = 0;
    virtual UInt_t GetRunNumber() const = 0;    
    virtual UInt_t GetEventNumber() const = 0;    
    virtual UInt_t GetNumberOfTracks() const = 0;   
    virtual UInt_t GetNumberOfChargedTracks() const = 0;   
    virtual UInt_t GetNumberOfNeutralTracks() const = 0;   
    UInt_t GetNumberOfCandidates() const { return GetNumberOfTracks(); }   
    virtual UInt_t GetNumberOfEventsProcessed() const = 0;
    virtual UInt_t GetNumberOfMcTruthTracks() const = 0;
    virtual UInt_t GetNumberOfCandAssocs() const = 0;
    virtual UInt_t GetNumberOfLists() const = 0;
    virtual const char* GetListName(Int_t i) const = 0;
    virtual UInt_t GetNumberOfComposites() const = 0;
    virtual TCandidate& GetCompositeCandidate(Int_t i) const = 0;
    virtual VAbsMicroCandidate& GetMicroCandidate(Int_t i) const = 0;

    virtual UInt_t GetMajorID() const = 0;
    virtual UInt_t GetMinorID() const = 0;
    
    virtual PAFEventBase&	    GetHeader() const = 0;    
    virtual VAbsTag&	    GetTag() const = 0;    
    virtual VAbsBit&	    GetBit() const = 0;    
    virtual PAFListBase&    GetListBase() const = 0;
    virtual EidInfoR&	    GetEidInfo() const = 0;
    
    virtual VAbsTruth& GetMcTruth(Int_t n) = 0;
    virtual PAFMcBase& GetMcBase(Int_t n) const = 0;
    virtual PAFMcAssocBase& GetMcAssocBase(Int_t i) = 0;
    virtual StdHepTrkR& GetStdHepTrk(Int_t n) const = 0;
    virtual BtaCandAssocMapR& GetCandAssocMap(Int_t i) const = 0;
    virtual BtaCandListR& GetCandList(Int_t i) const = 0;

    // Fill lists from micro
    virtual Int_t Fill( TCandList* ) = 0;
    virtual Int_t FillCharged( TCandList* ) = 0;
    virtual Int_t FillNeutral( TCandList* ) = 0;
    virtual Int_t FillFromList(TCandList *l,const char* listname) = 0;
    virtual Int_t FillCompositeList(TCandList *l,const char* listname) = 0;
    virtual Int_t FillAllCompositeList(TCandList *l) = 0;

	// MC handling
    virtual Int_t FillCanonicalMcTruthList(TCandList *) = 0;
    virtual Int_t FillMcTruthList(TCandList *) = 0;

    // Evaluation of tag based cuts
    virtual void SetTagFormula(const char* expr) = 0;
    virtual void SetBitFormula(const char* expr) = 0;
    virtual Double_t EvalTagFormula() = 0;
    virtual Double_t EvalBitFormula() = 0;

private:
    Bool_t  fVerbose;		//!Debug
public:
    ClassDef(VAbsReader,1) // Base class to read ROOT data files
};


#endif





