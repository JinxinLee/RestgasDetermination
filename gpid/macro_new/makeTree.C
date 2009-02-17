#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <iterator>
#include <memory>

#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TH2F.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "PndEmcDigi.h"
#include "PndTofPoint.h"
#include "PndLhePidTrack.h"
#include "PndMvdMCPoint.h"
#include "PndTpcPoint.h"
#include "PndTofHit.h"
#include "PndEmcCluster.h"
#include "PndEmcMapper.h"
#include "PndEmcTwoCoordIndex.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TSystem.h"
#include "CbmMCTrack.h"
#include "CbmMCPoint.h"
#include "CbmTrackParH.h"
#include "TNtuple.h"
#include "TFile.h"
#include "PndPidCand.h"
#include "TGraph.h"
#include "TH3F.h"
#include "TColor.h"


void tubeFill(TString const inf1, TNtuple &tube )
//void histFill(TString const inf1 )
{
  TFile *freco1 = new TFile(inf1);
  TTree *treco1 = ( TTree* )freco1->Get("cbmsim");


 TClonesArray *ArrPndPidCand1 = new TClonesArray("PndPidCand");
 treco1->SetBranchAddress("PndPidCand",&ArrPndPidCand1);




 for (Int_t i =0; i < treco1->GetEntries();i++)     
     {
     treco1->GetEntry(i);
    // PndEmcCluster *cluster = (PndEmcCluster *) ArrCluster->At(0);
     for (Int_t j=0; j < ArrPndPidCand1->GetEntriesFast(); j++) {
     PndPidCand *track = (PndPidCand *) ArrPndPidCand1->At(j);
     if (track == NULL) continue;

     tube.Fill(track->Get("p"),track->Get("stt"),track->Get("mvd"),track->Get("tof"),track->Get("thetaC"),track->Get("emc"),
       track->Get("pion"),track->Get("electron"));

	}

	}
}


void makeTree()
{

PndEmcMapper *emcMap=PndEmcMapper::Instance(2,"el_sttcombi.root");

TFile *out = new TFile("pid_tube_P_EMC_STT_500T_10000S.root","RECREATE");
//TNtuple *ntuple = new TNtuple("pion","pion","p:stt:mvd:tof:thetaC:emc:pion:electron");
TNtuple *ntuple1 = new TNtuple("electron","electron","p:stt:mvd:tof:thetaC:emc:pion:electron");

//tubeFill("pi_pid.root",*ntuple);
tubeFill("ele_pid.root",*ntuple1);

//out->WriteObject(ntuple,"pion");
out->WriteObject(ntuple1,"electron");
out->Close(); 
}
