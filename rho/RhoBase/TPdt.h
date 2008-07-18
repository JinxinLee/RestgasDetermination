#ifndef TPDT_H
#define TPDT_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TPdt									//
//                                                                      //
// Read the BABAR style PDT.						//
//                                                                      //
// Author: Marcel Kunze, RUB, 1999-2000					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TDatabasePDG.h"

class TPdt : public TDatabasePDG
{
public:
    TPdt() {}
    TPdt(const char *file) { ReadPDGTable(file); }
    virtual ~TPdt() {}
    virtual void ReadPDGTable(const char *filename); //Read the BABAR style pdt.table
    virtual TParticlePDG*   AddParticle(const char*  Name, 
	const char*  Title,
	Double_t     Mass, 
	Bool_t       Stable,
	Double_t     DecayWidth, 
	Double_t     Charge, 
	const char*  ParticleClass,
	Int_t        PdgCode,
	Int_t        Anti=-1,
	Int_t        TrackingCode=0);
    
    ClassDef(TPdt, 1)   // BABAR Particle Data Table
};

#endif
