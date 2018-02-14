//
// C++ Interface: PndMvdMCEventAna
//
// Description: 
//
//
// Author: t.stockmanns <stockman@ikp455>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PNDMVDDIGIEVENTANA_H
#define PNDMVDDIGIEVENTANA_H

#include "PndMvdEventAna.h"
#include "PndSdsMCPoint.h"
#include "PndSdsDigiPixel.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TClonesArray.h"

/**
@author t.stockmanns
*/
class PndMvdDigiEventAna : public PndMvdEventAna{
public:
  PndMvdDigiEventAna(){};
	PndMvdDigiEventAna(TString fileName);
  virtual ~PndMvdDigiEventAna(){};
	
	virtual void InitBranch();
	virtual void InitHistos();
	virtual void AnaHitNr(Int_t hitNr);
	void EndOfEventAction();
	
	void CreateMaxHitsHisto();
	void DrawSingleModuleFEHisto(TString path);
	void DrawAllModulesFEHisto();

protected:
	std::map<TString, std::vector<Int_t>* > fFeHits;
	std::map<TString, std::vector<PndSdsDigiPixel> > fDigiPixelArray;

ClassDef(PndMvdDigiEventAna,2);

};

#endif
