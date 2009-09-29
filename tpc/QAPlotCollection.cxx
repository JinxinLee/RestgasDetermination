#include "QAPlotCollection.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TClonesArray.h"
#include "TCanvas.h"
#include "TMath.h"

ClassImp(QAPlotCollection);

QAPlotCollection::QAPlotCollection()
 : TNamed("QAPlotCollection","QAPlots"), fH1Dlist(NULL), fH2Dlist(NULL)
{}

QAPlotCollection::QAPlotCollection(TString name)
 : TNamed(name,name), fH1Dlist(NULL), fH2Dlist(NULL)
{}


QAPlotCollection::~QAPlotCollection()
{
}


TH1D* 
QAPlotCollection::getTH1D(TString name, TString title, Int_t nbins, 
			  Double_t xmin, Double_t xmax) {
	if(fH1Dlist==NULL)fH1Dlist=new TClonesArray("TH1D");
	return new((*fH1Dlist)[fH1Dlist->GetEntriesFast()]) TH1D(name, title,
														  nbins, xmin, xmax); 
}

TH2D* 
QAPlotCollection::getTH2D(TString name, TString title, 
			  Int_t nbinsx, Double_t xmin, Double_t xmax,
			  Int_t nbinsy, Double_t ymin, Double_t ymax) {
	if(fH2Dlist==NULL)fH2Dlist=new TClonesArray("TH2D");
	return new((*fH2Dlist)[fH2Dlist->GetEntriesFast()]) TH2D(name, title,
														  nbinsx, xmin, xmax,
														  nbinsy, ymin, ymax); 
}


TCanvas*
QAPlotCollection::Draw(){
	// How many pads do we need?
	Int_t npads=0;
	Int_t npads1=0; Int_t npads2=0;
	if(fH1Dlist!=NULL)npads1+=fH1Dlist->GetEntriesFast();
	if(fH2Dlist!=NULL)npads2+=fH2Dlist->GetEntriesFast();
	npads=npads1+npads2;
	if(npads==0)return NULL;
	
	TCanvas* c=new TCanvas("QAPlots",GetName(),10,10,1000,1000);
	// How should we divide the pad?
	int n1=(int)TMath::Floor(TMath::Sqrt(npads));
	int n2=(int)TMath::Ceil(TMath::Sqrt(npads));
	c->Divide(n2,n1);
	
	int ipad =1;
	// Draw 1D histos
	for(int i1=0;i1<npads1;++i1){
		c->cd(ipad++);
		TH1D* h=(TH1D*)fH1Dlist->At(i1);
		h->Draw();
	}
	
	// Draw 2D histos
	for(int i2=0;i2<npads2;++i2){
		c->cd(ipad++);
		TH2D* h=(TH2D*)fH2Dlist->At(i2);
		h->Draw();		
	}
	
	return c;
}
