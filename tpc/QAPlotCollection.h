//! Collection of QA plots
/*
* User requests histograms from this collection, which he fills 
* It can draw itselve
* TODO: add reference plot support
*/

#ifndef QAPLOTCOLLECTION_H
#define QAPLOTCOLLECTION_H

#include "TNamed.h"
#include "TString.h"

class TH1D;
class TH2D;
class TClonesArray;
class TCanvas;

class QAPlotCollection : public TNamed
{
public:
	QAPlotCollection();
	QAPlotCollection(TString name);
	virtual ~QAPlotCollection();
	
	//! Request a TH1D. QAPlotCollection retains ownership
	TH1D* getTH1D(TString name, TString title, Int_t nbins, 
				  Double_t xmin, Double_t xmax);
	
	//! Request a TH2D. QAPlotCollection retains ownership
	TH2D* getTH2D(TString name, TString title, 
				  Int_t nbinsx,Double_t xmin, Double_t xmax,
				  Int_t nbinsy,Double_t ymin, Double_t ymax);
	
	//! Draw histograms
	TCanvas* Draw();
	
private:
	//! List of 1D histograms
	TClonesArray* fH1Dlist;
	//! List of 2D histograms
	TClonesArray* fH2Dlist;
	
	
	ClassDef(QAPlotCollection,1)
};

#endif
