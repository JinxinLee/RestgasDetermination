#include "tpc/PndTpcGem.h"
#include "tpc/PndTpcPadPlane.h"
#include "tpc/PndTpcPadShapePool.h"
#include "tpc/PndTpcSignal.h"
#include "tpc/PndTpcDigi.h"
#include "tpc/PndTpcDriftedElectron.h"
#include "tpc/PndTpcAvalanche.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TH1I.h"
#include "TH2I.h"
#include "TStyle.h"
#include <iostream>

void plotDigiOutput(char* filename){
   gStyle->SetFillColor(kWhite);
   TFile* file = new TFile(filename);
   TTree *t=(TTree*)file->Get("cbmsim") ;
   if(t==NULL){
     std::cout<<"Data tree not found. Aborting." <<std::endl;
     return;
   }

   // create histos
   TH1D* de_dist_H=new TH1D("de_dist","de_dist",100,0,1);
   TH1D* de_dt_H=new TH1D("de_dt","de_dt",100,0,100);
   TH1D* av_dist_H=new TH1D("av_dist","av_dist",100,0,1);
   TH1D* av_dt_H=new TH1D("av_dt","av_dt",100,0,100);
   TH1I* nsig_per_pad_H=new TH1I("nsig_per_pad","nsig_per_pad",100,0,100);
   TH1I* digi_dt_H=new TH1I("digi_dt_H","digi_dt",100,0,100);
   TH2I* digi_tdt_H=new TH2I("digi_tdt_H","digi_tdt",100,0,100,100,3000,5000);

   //Get Drifted Electrons
   TClonesArray *da=new TClonesArray("PndTpcDriftedElectron");
   t->SetBranchAddress("PndTpcDriftedElectron",&da);
   
   //Get Avalanches
   TClonesArray *aa=new TClonesArray("PndTpcAvalanche");
   t->SetBranchAddress("PndTpcAvalanche",&aa);

   //Get Signals
   TClonesArray *sa=new TClonesArray("PndTpcSignal");
   t->SetBranchAddress("PndTpcSignal",&sa);
   
   //Get Signals
   TClonesArray *diga=new TClonesArray("PndTpcDigi");
   t->SetBranchAddress("PndTpcDigi",&diga);

   // Event loop
   Int_t nevents=t->GetEntriesFast();
   for (Int_t j=0; j<nevents ; j++)	{
    t->GetEntry(j);

    // look at drifted electrons ------------------------------------------
    Int_t nde=da->GetEntriesFast();
    for (Int_t i=0; i<nde; ++i)	{
      PndTpcDriftedElectron *de=(PndTpcDriftedElectron*)da->At(i);
      double x,y,t;
      x=de->x();
      y=de->y();
      t=de->t();
      // look through all other electrons
      for(Int_t i2=i+1; i2<nde; ++i2){
	if(i2==i) continue; // skip self
	PndTpcDriftedElectron *de2=(PndTpcDriftedElectron*)da->At(i2);
	double dx=x-de2->x();
	double dy=y-de2->y();
	double dt=t-de2->t();
	double dr=sqrt(dx*dx+dy*dy);
	de_dist_H->Fill(dr);
	if(dr<0.3)de_dt_H->Fill(fabs(dt));
      }
    }
    
    // look at avalanches --------------------------------------
    Int_t nav=aa->GetEntriesFast();
    for (Int_t i=0; i<nav; ++i)	{
      PndTpcAvalanche *a=(PndTpcAvalanche*)aa->At(i);
      double x,y,t;
      x=a->x();
      y=a->y();
      t=a->t();
      // look through all other electrons
      for(Int_t i2=i+1; i2<nav; ++i2){
	if(i2==i) continue; // skip self
	PndTpcAvalanche *a2=(PndTpcAvalanche*)aa->At(i2);
	double dx=x-a2->x();
	double dy=y-a2->y();
	double dt=t-a2->t();
	double dr=sqrt(dx*dx+dy*dy);
	av_dist_H->Fill(dr);
	if(dr<0.3)av_dt_H->Fill(fabs(dt));
      }
    }

    //look at signals ---------------------------------------------------
    Int_t nsig=sa->GetEntriesFast();
    // map of hit pads -> how often has one pad been hit?
    std::map<unsigned int,unsigned int> hitpads;

    for (Int_t i=0; i<nsig; ++i)	{
      PndTpcSignal *sig=(PndTpcSignal*)sa->At(i);
      unsigned int id=sig->padId();
      (hitpads[id])++;
    }
    std::map<unsigned int,unsigned int>::iterator padit=hitpads.begin();
    while(padit!=hitpads.end()){
      nsig_per_pad_H->Fill((*padit).second);
      ++padit;
    }
      
    //look at Digis
    Int_t ndigi=diga->GetEntriesFast();
    // map of hit pads -> what are the times between digis?
    std::map<unsigned int,unsigned int> digitimes;
    for (Int_t i=0; i<ndigi; ++i)	{
      PndTpcDigi *digi=(PndTpcDigi*)diga->At(i);
      unsigned int id=digi->padId();
      unsigned int t=digi->t();
      if(t==0)std::cout<<"warning t==0!"<<std::endl;
      if(digitimes[id]!=0){
	unsigned int dt= digitimes[id]<t ? t-digitimes[id] : digitimes[id]-t;
	digi_dt_H->Fill(dt);
	digi_tdt_H->Fill(dt,t);
      }
      digitimes[id]=t;

    }

   } // end event loop

   
 
}
