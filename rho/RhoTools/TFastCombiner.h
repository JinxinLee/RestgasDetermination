//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TFastCombiner.h,v 1.1.1.1 2001-05-04 16:19:53 marcel Exp $
//
// Description:
//      Combiner class for TCandidates
//      Makes internal use of the CompCombination class
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Gautier Hamel de Monchenault
//      Fernando Martinez-Vidal
//
// Copyright Information:
//	Copyright (C) 1999		Saclay & Paris VI
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#ifndef TFASTCOMBINER_H
#define TFASTCOMBINER_H

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandListIterator.h"
#include "RhoBase/TCandidate.h"
class TDecayMode;
class TLorentzVector;
class VAbsPidSelector;

//		---------------------
// 		-- Class Interface --
//		---------------------

class TFastCombiner : public TObject 
{
    
public:
    
    // constructors
    TFastCombiner(TDecayMode&, VAbsPidSelector* theSelector=0 );
    
    TFastCombiner(TDecayMode&,
	TList&, VAbsPidSelector* theSelector=0 );
    
    TFastCombiner(TDecayMode&,
	TCandList* list0,
	TCandList* list1,
	TCandList* list2=0,
	TCandList* list3=0,
	TCandList* list4=0,
	TCandList* list5=0,
	VAbsPidSelector* theSelector=0 );
    
    // Destructor
    virtual ~TFastCombiner();
    
    // Virtual functions implementations
    TCandList* GetList() { return _theList; }
    TCandidate* Next();
    void Rewind();
    void Reset();
    
    // same lists in input?
    Bool_t SameLists() {return _sameLists;}
    
    // doWrongChargeOnly?
    void DoWrongChargeOnly() {_doWrongChargeOnly=kTRUE;}
    void DoNotWrongChargeOnly() {_doWrongChargeOnly=kFALSE;}
 
    void DoCheckType(Bool_t yesNo=kTRUE) { _checkType = yesNo; }

private:
    
    // the combine function
    void Combine( TList& );
    
    // fill the internal list
    void Combination_( TCandidate* cand0, 
	TCandidate* cand1=0,
	TCandidate* cand2=0,
	TCandidate* cand3=0,
	TCandidate* cand4=0,
	TCandidate* cand5=0   );
    
    
    Bool_t CheckChargeAndMass(const TLorentzVector &m, double charge,
	double lowMass, double hiMass);
    
    // the list of owned candidates
    TCandList *_theList; //!Do not stream
    
    // the iterator 
    TCandListIterator* _theIterator; //!Do not stream
    
    // the decay mode
    TDecayMode* _theDecayMode; //!Do not stream
    
    // bool variable for same lists in input
    Bool_t _sameLists; //!Do not stream
    
    // doWrongChargeOnly?
    Bool_t _doWrongChargeOnly; //!Do not stream
    
    // Check daughter types
    Bool_t _checkType; //!Do not stream
    
    // the composite selector
    VAbsPidSelector* _theSelector; //!Do not stream

public:
    ClassDef(TFastCombiner,1) // Combiner class for TCandidates
};

#endif

