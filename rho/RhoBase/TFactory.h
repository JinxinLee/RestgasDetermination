#ifndef TFACTORY_H
#define TFACTORY_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TFactory								//
//                                                                      //
// Object factory							//
//									//
// Author List:								//
// Marcel Kunze,  RUB, Feb. 01						//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "TClonesArray.h"

class TCandidate;
class TFitParams;
class TSimpleVertex;
class TLorentzVector;
class TError;
class TCandListIterator;
class VAbsVertex;
class TParticlePDG;

class TFactory : public TObject {
    
public:
    static TFactory* Instance();
    virtual ~TFactory();
    static void Reset();
    static TCandidate* NewCandidate();
    static TCandidate* NewCandidate(const TCandidate &c);
    static TCandidate* NewCandidate(const TFitParams &f, Int_t uid=0);
    static TCandidate* NewCandidate( TLorentzVector p4, 
	TError& p4Err,
	TCandListIterator& iterDau,
	VAbsVertex& theVertex,
	const TParticlePDG* hypo = 0);

    static TCandidate* GetCandidate(Int_t);
    static Int_t GetCandidateWatermark();
    static TSimpleVertex* NewVertex();
    static TSimpleVertex* NewVertex(const TSimpleVertex &c);
    static TSimpleVertex* GetVertex(Int_t);
    static Int_t GetVertexWatermark();

private:
    TFactory() {}
    static TFactory*	fgInstance;  //! Do not stream
    static TClonesArray*   fgCandBuffer; //! Candidate buffer
    static Int_t	   fgCandPointer; //! Actual position
    static Int_t	   fgCandWatermark; //! Max. position
    static TClonesArray*   fgVertexBuffer; //! Candidate buffer
    static Int_t	   fgVertexPointer; //! Actual position
    static Int_t	   fgVertexWatermark; //! Max. position
    
    ClassDef(TFactory,1) //Factory for candidates and vertices

};      
#endif
