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

#include <stdlib.h>

#include "RhoBase/TFactory.h"
#include "RhoBase/TFitParams.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/TSimpleVertex.h"

ClassImp(TFactory)

using namespace std;

TFactory* TFactory::fgInstance = 0;
TClonesArray* TFactory::fgCandBuffer = 0;
Int_t TFactory::fgCandPointer=0;
Int_t TFactory::fgCandWatermark=0;
TClonesArray* TFactory::fgVertexBuffer = 0;
Int_t TFactory::fgVertexPointer=0;
Int_t TFactory::fgVertexWatermark=0;

void TFactory::Reset() 
{
    fgCandPointer = 0;
    fgVertexPointer = 0;
}

TFactory* TFactory::Instance()
{
    if (!fgInstance) fgInstance= new TFactory();
    return fgInstance;
}

TFactory::~TFactory()
{
    delete fgInstance;
    fgInstance = 0;
}

//---------------------------------------

TCandidate* TFactory::NewCandidate()
{
    TCandidate c;
    return NewCandidate(c);
}

TCandidate* TFactory::NewCandidate(const TCandidate &c)
{
    if (fgCandBuffer==0) fgCandBuffer = new TClonesArray("TCandidate");
    int current = fgCandPointer++;
    if (current>fgCandWatermark) fgCandWatermark = current;
    //if (current<fgCandWatermark) (const_cast<TCandidate*>(&c))->RemoveAssociations();
    new ((*fgCandBuffer)[current]) TCandidate(c);
    return GetCandidate(current);
}

TCandidate* TFactory::NewCandidate(const TFitParams &f, Int_t uid)
{
    if (fgCandBuffer==0) fgCandBuffer = new TClonesArray("TCandidate");
    int current = fgCandPointer++;
    if (current>fgCandWatermark) fgCandWatermark = current;
    if (current<fgCandWatermark) {
	TCandidate *c = GetCandidate(current);
	c->RemoveAssociations();
    }
    new ((*fgCandBuffer)[current]) TCandidate(f.P4(),f.GetCharge());
    TCandidate *c = GetCandidate(current);
    c->SetPosition(f.GetPosition());
    c->GetFitParams().SetCov7(f.Cov7());
    c->SetUid(uid);
    return c;
}

TCandidate* TFactory::NewCandidate( TLorentzVector p4, 
			   TError &p4Err,
			   TCandListIterator& iterDau,
			   VAbsVertex& theVertex,
			   const TParticlePDG* hypo )
{
    if (fgCandBuffer==0) fgCandBuffer = new TClonesArray("TCandidate");
    int current = fgCandPointer++;
    if (current>fgCandWatermark) fgCandWatermark = current;
    if (current<fgCandWatermark) {
	TCandidate *c = GetCandidate(current);
	c->RemoveAssociations();
    }
    new ((*fgCandBuffer)[current]) TCandidate(p4,p4Err,iterDau,theVertex,hypo);
    return GetCandidate(current);
}

TCandidate* TFactory::GetCandidate(Int_t i)
{
    if (fgCandBuffer==0 || i>fgCandWatermark) return 0;
    return (TCandidate *) (*fgCandBuffer)[i];
}

Int_t TFactory::GetCandidateWatermark() { return fgCandWatermark; }

//---------------------------------------

TSimpleVertex* TFactory::NewVertex()
{
    TSimpleVertex c;
    return NewVertex(c);
}

TSimpleVertex* TFactory::NewVertex(const TSimpleVertex &c)
{
    if (fgVertexBuffer==0) fgVertexBuffer = new TClonesArray("TSimpleVertex");
    int current = fgVertexPointer++;
    if (current>fgVertexWatermark) fgVertexWatermark = current;
    new ((*fgVertexBuffer)[current]) TSimpleVertex(c);
    return GetVertex(current);
}

TSimpleVertex* TFactory::GetVertex(Int_t i)
{
    if (fgVertexBuffer==0 || i>fgVertexWatermark) return 0;
    TSimpleVertex *vertex = (TSimpleVertex *) (*fgVertexBuffer)[i];
    return vertex;
}

Int_t TFactory::GetVertexWatermark() { return fgVertexWatermark; }

