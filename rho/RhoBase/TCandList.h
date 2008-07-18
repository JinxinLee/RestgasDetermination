#ifndef TCANDLIST_H
#define TCANDLIST_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TCandList								//
//                                                                      //
// Container class for TCandidates					//
//									//
// Author List:								//
// Marcel Kunze,  RUB, Feb. 99						//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////
#include <iostream>
#include "TNamed.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TVector.h"
#include "TVector3.h"

class TCandidate;
class VAbsPidSelector;
class VAbsVertexSelector;

class TCandList : public TNamed {
    
public:
    //Constructor
    TCandList(const char *name="TCandList",UInt_t capacity=512);
    TCandList(TCandList& b);
    //Destructor
    virtual ~TCandList();
    void Cleanup();
    
    void SetNumberOfTracks(Int_t);
    void SetNumberOfCandidates(Int_t n) { SetNumberOfTracks(n); }
    Int_t GetNumberOfTracks() const;
    Int_t GetNumberOfCandidates() const {return GetNumberOfTracks();}
    Int_t GetLength() const {return GetNumberOfTracks();}
    void Add(const TCandidate& c) { Put(c); }
    void Append(const TCandidate& c) { Put(c); }
    void Append(TCandList& l, VAbsPidSelector* pidmgr=0);
    void Put(const TCandidate&, Int_t i = -1 );
    void InsertAt(Int_t i, const TCandidate& c);
	
    void Combine(TCandList& l1, TCandList& l2,VAbsVertexSelector* s=0);
	void Combine(TCandList& l1, TCandList& l2, TCandList& l3);   //added 06/08 K.Goetzen
	void Combine(TCandList& l1, TCandList& l2, TCandList& l3, TCandList& l4);//added 06/08 K.Goetzen
	void Combine(TCandList& l1, TCandList& l2, TCandList& l3, TCandList& l4, TCandList& l5);//added 06/08 K.Goetzen
	
    void CombineAndAppend( TCandList& l1, TCandList& l2,VAbsVertexSelector* s=0);
	void CombineAndAppend(TCandList& l1, TCandList& l2, TCandList& l3);//added 06/08 K.Goetzen
	void CombineAndAppend(TCandList& l1, TCandList& l2, TCandList& l3, TCandList& l4);//added 06/08 K.Goetzen
	void CombineAndAppend(TCandList& l1, TCandList& l2, TCandList& l3, TCandList& l4, TCandList& l5);//added 06/08 K.Goetzen
	
    void Select(VAbsPidSelector* pidmgr);
    void Select(TCandList& l, VAbsPidSelector* pidmgr);
    void Select(TCandList& l, Bool_t (*selfunc)(TCandidate&) );
    Int_t OccurrencesOf(TCandidate& );
    Int_t Remove(TCandidate& ); // Returns #removed cands
    Int_t RemoveFamily(TCandidate& ); // Returns #removed cands
    Int_t RemoveClones(); // Returns #removed cands
    void Reset() { SetNumberOfTracks(0); }
    TCandidate* Get(Int_t);
    TCandidate* GetConst(Int_t) const;

    // Analysis functions

    void Boost(const TVector3& );
    Double_t GetTotalEnergy(Double_t emin=0.0);
    TVector3 GetTotalMomentum(Double_t pmin=0.0);
    
    //operations which work on existing lists
    
    void PrintOn( std::ostream& o=std::cout ) const;
    void Sort(int (*compfunc)(const TCandidate **, const TCandidate **) );
    
    void Remainder(TCandList&);
    
    void operator =(const TCandList& );
    TCandidate& operator[](Int_t);
    
    void SetFast(Bool_t yesno=kTRUE) { fFast = yesno; }

private:
    TObjArray*   fOwnList;	// This holds the candidates        
    Bool_t	 fFast;		// Fast mode = no error matrices

    ClassDef(TCandList,1) //Container for TCandidates

};      

// standalone print
std::ostream&  operator << (std::ostream& o, const TCandList&);

#endif
