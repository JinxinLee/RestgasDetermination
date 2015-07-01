//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class PndEmcBump. This call provides a standard interface for
//          Emc bump classes.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//
// Author List:
//	Stephen J. Gowdy	Originator
// Copyright Information:
//	Copyright (C) 1997	University of Edinburgh
//
// Dima Melnychuk, adaption for PANDA
//------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCBUMP_H
#define PNDEMCBUMP_H

//----------------------
// Base Class Headers --
//----------------------
#include "PndEmcCluster.h"
#include "TObject.h"

class PndEmcBump: public PndEmcCluster
{

public:

	// Constructors
	PndEmcBump();
	
	// Destructor
	virtual ~PndEmcBump();
	
	/** Copy **/
	PndEmcBump(const PndEmcBump& copy);

	// Modifiers
	virtual void MadeFrom( Int_t clusterIndex );
// 	virtual Int_t NumberOfDigis() const;
// 	virtual Double_t RnumberOfDigis() const;
	virtual void Print(Int_t iBump) const;

	Int_t GetClusterIndex()  { return fClusterIndex; };

	//time information
	const Double_t GetTimeStamp() const { return fTimeStamp; }
	void SetTimeStamp(Double_t time) { fTimeStamp = time; }
	const Int_t GetEventNo() const { return fEvtNo; }
	void SetEventNo(Int_t evtNo) { fEvtNo = evtNo; }
	//temp usage
	//PndEmcDigi* GetSeedDigi() const { return fSeedDigi; }
	//void SetSeedDigi(PndEmcDigi* digi){ fSeedDigi = digi; }

	//const Double_t GetTimeStamp1() const { return fTimeStamp1; }
	//const Double_t GetTimeStamp2() const { return fTimeStamp2; }
	//const Double_t GetTimeStamp3() const { return fTimeStamp3; }
	//void SetTimeStamp1(Double_t time) { fTimeStamp1 = time; }
	//void SetTimeStamp2(Double_t time) { fTimeStamp2 = time; }
	//void SetTimeStamp3(Double_t time) { fTimeStamp3 = time; }

	//TVector3  fSeedPosition;
protected:
  // Data members
  Int_t  fClusterIndex; // Index of cluster the bump is made in TClonesArray
	Int_t  fEvtNo;//event number of seed digi

	Double_t fTimeStamp;//three different weighted time

	ClassDef(PndEmcBump,1)
private:
		PndEmcBump& operator=(const PndEmcBump& rv);
};
#endif // PNDEMCBUMP_HH
