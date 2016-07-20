/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

/** PndTimeStructureAnaTask.h
 **
 **/

#ifndef PndTimeStructureAnaTask_H
#define PndTimeStructureAnaTask_H

#include "FairTask.h"                   // for FairTask, InitStatus
#include "FairTSBufferFunctional.h"

#include "TH1.h"
#include "TGraph.h"

#include "Rtypes.h"                     // for Bool_t, Int_t, kTRUE, etc
#include "TString.h"                    // for TString

class FairTimeStamp;
class TClonesArray;

struct DataObject
{
	DataObject():fBranchName(""), fBranch(0), fTimeHisto(0), fEventHisto(0), fOldEventNr(-1), fOldTimeStamp(0), fEventMixture(kFALSE){};

	DataObject(TString branchName): fBranchName(branchName), fBranch(0), fOldEventNr(-1), fOldTimeStamp(0), fEventMixture(kFALSE)
	{
		TString histoName("hBetweenEvents");
		histoName += fBranchName;
		TString eventHistoName("hInsideEvent");
		eventHistoName += fBranchName;
		TString eventDiffHistoName("hEventLength");
		eventDiffHistoName += fBranchName;
		fTimeHisto = new TH1D(histoName, histoName, 10000, 0, 10000);
		fEventHisto = new TH1D(eventHistoName, eventHistoName, 100, 0, 100);
		fEventDiffHisto = new TH1D(eventDiffHistoName, eventDiffHistoName, 1000, 0, 1000);
	}
	std::vector<Double_t> CalcIntegral(TH1* h1){
		std::vector<Double_t> result;
		Double_t sum = 0;
		for (int i = 0; i < h1->GetNbinsX(); i++){
			sum += h1->GetBinContent(i);
			result.push_back(sum);
		}
		return result;
	}

	std::vector<Double_t> GetBinCenters(TH1* h1){
		std::vector<Double_t> result;
		for (int i = 0; i < h1->GetNbinsX(); i++){
			result.push_back(h1->GetBinCenter(i));
		}
		return result;
	}
	void FillGraphs(){

		TString eventGapName = fBranchName + "EventGap";
		fEventGap = new TH1D(eventGapName.Data(), eventGapName.Data(), fTimeHisto->GetNbinsX(), fTimeHisto->GetXaxis()->GetXmin(), fTimeHisto->GetXaxis()->GetXmax());
		FillGraph(fEventGap, fTimeHisto);

		TString eventGapPercName = fBranchName + "EventGapPerc";
		fEventGapPerc = new TH1D(eventGapPercName.Data(),eventGapPercName.Data(), fTimeHisto->GetNbinsX(), fTimeHisto->GetXaxis()->GetXmin(), fTimeHisto->GetXaxis()->GetXmax());
		FillGraphPercent(fEventGapPerc, fTimeHisto);

		TString overlapName = fBranchName + "Overlap";
		fOverlap = new TH1D(overlapName.Data(), overlapName.Data(),fEventHisto->GetNbinsX(), fEventHisto->GetXaxis()->GetXmin(), fEventHisto->GetXaxis()->GetXmax());
		FillGraph(fOverlap, fEventHisto);

		TString overlapPercName = fBranchName + "OverlapPerc";
		fOverlapPerc = new TH1D(overlapPercName.Data(), overlapPercName.Data(), fEventHisto->GetNbinsX(), fEventHisto->GetXaxis()->GetXmin(), fEventHisto->GetXaxis()->GetXmax());
		FillGraphPercent(fOverlapPerc, fEventHisto);
	}

	void FillGraph(TH1D* theGraph, TH1* h1){
		std::vector<Double_t> values = CalcIntegral(h1);
		std::vector<Double_t> binCenters = GetBinCenters(h1);
		for (int i = 0; i < values.size(); i++)
			theGraph->Fill(binCenters[i], values[i]);
	}

	void FillGraphPercent(TH1D* theGraph, TH1* h1){
		std::vector<Double_t> values = CalcIntegral(h1);
		std::vector<Double_t> binCenters = GetBinCenters(h1);
		for (int i = 0; i < values.size(); i++)
			theGraph->Fill(binCenters[i], values[i]/values.back() * 100.0);
	}

	TString fBranchName;
	TClonesArray* fBranch;
	TH1D* fTimeHisto;
	TH1D* fEventHisto;
	TH1D* fEventDiffHisto;
	TH1D* fEventGap;
	TH1D* fEventGapPerc;
	TH1D* fOverlap;
	TH1D* fOverlapPerc;
	Int_t fOldEventNr;
	Double_t fOldTimeStamp;
	Bool_t fEventMixture;
	std::map<Int_t, std::pair<Double_t, Double_t> > fEventStartStopMap;
};

class PndTimeStructureAnaTask : public FairTask
{
  public:

    /** Default constructor **/
    PndTimeStructureAnaTask():
      FairTask("TimeStructureAnaTask"), fEntryNr(0)
	{
    	SetVerbose(0);
    }

    /** Named constructor **/
    PndTimeStructureAnaTask(const char* name):
      FairTask(name), fEntryNr(0)
    {
      SetVerbose(0);
    }

    /** Destructor **/
    virtual ~PndTimeStructureAnaTask() {
    }

    /** Virtual method Init **/
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    virtual void FinishEvent();
    virtual void FinishTask();

    virtual void SetParContainers() {};

    void AddBranchName(TString name){
    	fData.push_back(DataObject(name));
    	name += "Prim";
    	fDataPrim.push_back(DataObject(name));
    }


  protected:

    std::vector<DataObject> fData;		//data for all type of particles
    std::vector<DataObject> fDataPrim;	//data for primary particles
    TH1D* fHistoMixedEvents;
    TH1D* fHistoMixedEventsPrim;

    Int_t fEntryNr;

    ClassDef(PndTimeStructureAnaTask,1);

};

#endif
