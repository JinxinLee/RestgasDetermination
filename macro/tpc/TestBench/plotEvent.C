//Plotting macro for data. 
//Needs to be compiled!

//Author: Tiger

#include "TCanvas.h"
#include "TFile.h"
#include "TH2F.h"
#include "TH3D.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TPolyMarker.h"
#include "TPolyMarker3D.h"
#include "TPolyLine.h"
#include "TPolyLine3D.h"
#include "TVector3.h"
#include "TBox.h"
#include "TLine.h"
#include "TSystem.h"
#include "TROOT.h"

#include "TView3D.h"
#include "TApplication.h"
#include "TMath.h"
#include "TStyle.h"
#include "TColor.h"
#include "THStack.h"

#include "tpc/PndTpcCluster.h"
#include "tpc/PndTpcDigi.h"
#include "tpc/PndTpcSample.h"
#include "tpc/PndTpcPadPlane.h"
#include "tpc/PndTpcPadShapePool.h"
#include "tpc/PndTpcPad.h"

#include "genfit/GFTrackCand.h"
#include "genfit/GFTrack.h"

#include "/afs/e18/panda/SIM/vandenbm/tpcdecoding/GEM-TPCmonitor_repository/V2/Felix/mappingManager.h"
#include "/afs/e18/panda/SIM/vandenbm/tpcdecoding/GEM-TPCmonitor_repository/V2/Felix/configManager.h"
//#include "/afs/e18/panda/SIM/vandenbm/tpcdecoding/GEM-TPCmonitor_repository/V2/Felix/calibrationManager.h"
//#include "/afs/e18/panda/SIM/vandenbm/tpcdecoding/GEM-TPCmonitor_repository/V2/Felix/configManager.h"
#include "/afs/e18/panda/SIM/vandenbm/tpcdecoding/GEM-TPCmonitor_repository/V2/Felix/ExtendedKey.h"
//#include "/afs/e18/panda/SIM/vandenbm/tpcdecoding/GEM-TPCmonitor_repository/V2/Felix/logManager.h"
//#include "/afs/e18/panda/SIM/vandenbm/tpcdecoding/GEM-TPCmonitor_repository/V2/Felix/mappingStructures.h"
//#include "/afs/e18/panda/SIM/vandenbm/tpcdecoding/GEM-TPCmonitor_repository/V2/Felix/
//#include "/afs/e18/panda/SIM/vandenbm/tpcdecoding/GEM-TPCmonitor_repository/V2/Felix/PndTpcEvent.h"

#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <map>
#include <string>
#include <exception>

//3 MODES
//mode == -1: browsing mode to display tracks starting from evLo, cut on efficiency
//mode == -2: Display evLo with detailed information

void plotEvent(TString filename, int evLo, int mode)
{
  if (!(mode==-1 || mode==-2)){
    std::cout << "plotEvent(TString filename, int eventNumber, int mode) with mode == -1 for browsing and -2 for one event"<< std::endl; return;}
  //Cleaning & style
  while(gROOT->GetListOfCanvases()->GetSize())
    delete gROOT->GetListOfCanvases()->At(0);
  gStyle->SetOptStat(0);
  gStyle-> SetPalette(1);
  
  //Mapping manager init
  if(ConfigManager::Instance().good())
    cout << ConfigManager::Instance();
  
  MappingManager::Instance().initMapSystem();
  if(!MappingManager::Instance().good()){
    std::cout << MappingManager::Instance();}
  
  //opening file
  TFile* recofile = new TFile(filename);
  if(recofile->IsZombie()) {
    std::cerr<<"Reco file not existing! Aborting."<<std::endl;
  }
  std::string vmc = gSystem->Getenv("VMCWORKDIR");
  vmc.append("/tpc/TestBench/");
  std::cout << "wdir" << vmc.c_str() << std::endl;
  std::string copy = vmc;
  //loading padplane
  PndTpcPadShapePool* pool=new PndTpcPadShapePool(copy.append("TBhexa_pads.dat").c_str());
  copy=vmc;
  PndTpcPadPlane* plane = new PndTpcPadPlane(copy.append("newPadTC.dat").c_str(),pool);
  std::cout<<"Plane has "<<plane->GetNPads()<<" pads."<<std::endl;
  
  //arrays...
  TClonesArray* clArr = new TClonesArray("PndTpcCluster");
  TClonesArray* preTrArr = new TClonesArray("GFTrack");
  TClonesArray* postTrArr = new TClonesArray("GFTrack");
  
  TTree* recotree = (TTree*)recofile->Get("cbmsim");
  recotree->SetBranchAddress("PndTpcCluster", &clArr);
  recotree->SetBranchAddress("TrackPreFit", &preTrArr);
  recotree->SetBranchAddress("TrackPostFit", &postTrArr);

  std::cout << "Number of events: " << recotree->GetEntries() <<std::endl;
  
  TCanvas* canv3 =  new TCanvas("canv3","Pre/Post fit tracks",1000,1000);
  TCanvas* canv =  new TCanvas("canv","Samples and Clusters",1000,1000);
  canv->Divide(2,2);
  TCanvas* canv2 =  new TCanvas("canv2","Tracks",1000,1000);
  canv2->Divide(2,2);
  
  do//infinite loop for browsing
    {
      std::map<int,TPolyMarker*> clusters;
      std::map<int,TPolyMarker*> samples;
      std::map<int,TLine*> tracks;
      std::map<int,TPolyLine3D*> tracks3d;
      std::map<int,TPolyMarker3D*> clusters3d;
      std::map<int,TPolyMarker3D*> clusterstr3d;
      std::map<int,TPolyMarker*> clusterstr;
      std::map<int,TPolyMarker*>::iterator it;
      std::map<int,TPolyMarker3D*>::iterator it3d;
      std::map<int,TLine*>::iterator ittr;
      std::map<int,TPolyLine3D*>::iterator ittr3d;
      TLine* line=NULL;
      TPolyLine3D* line3d=NULL;
      TPolyMarker* marker=NULL;
      TPolyMarker3D* marker3d=NULL;
      std::vector<unsigned int> candIDs;
      //3D
      TH3D * real3d = new TH3D ("real3d","real3d",100,-5,5,100,-5,5,100,1,8.5);
      real3d->GetXaxis()->SetTitle("x (cm)");
      real3d->GetYaxis()->SetTitle("y (cm)");  
      real3d->GetZaxis()->SetTitle("z (cm)");
      canv->cd(3);real3d->Draw();
      TH3D * realtr3d = new TH3D ("realtr3d","realtr3d",100,-5,5,100,-5,5,100,1,8.5);
      realtr3d->GetXaxis()->SetTitle("x (cm)");
      realtr3d->GetYaxis()->SetTitle("y (cm)");  
      realtr3d->GetZaxis()->SetTitle("z (cm)");
      canv2->cd(3);realtr3d->Draw();
      TH3D * trackcomp3d = new TH3D ("trackcomp3d","trackcomp3d",100,-5,5,100,-5,5,100,1,8.5);
      trackcomp3d->GetXaxis()->SetTitle("x (cm)");
      trackcomp3d->GetYaxis()->SetTitle("y (cm)");  
      trackcomp3d->GetZaxis()->SetTitle("z (cm)");
      canv3->cd();trackcomp3d->Draw();

      //Draw padplane
      canv->cd(4);
      TH2F* real2;
      real2 = new TH2F("real2", "Real Space x-y",100,-5,5,100,-5,5);
      real2->GetXaxis()->SetTitle("x (cm)");
      real2->GetYaxis()->SetTitle("y (cm)");  
      real2->Draw();
      canv2->cd(4);
      TH2F* realtr2;
      realtr2 = new TH2F("realtr2", "Real Space x-y",100,-5,5,100,-5,5);
      realtr2->GetXaxis()->SetTitle("x (cm)");
      realtr2->GetYaxis()->SetTitle("y (cm)");  
      realtr2->Draw();
      int npads=plane->GetNPads();
      int adc_range = 2048;
      int nbchip = 25;
      for(int i=1;i<npads;++i){ 
	PndTpcPad* apad=0;
	try{apad=plane->GetPad(i);}
	catch (std::exception &e){
	  std::cout << "Padplane exeption :"<<e.what() << std::endl;
	  std::cout.flush();
	  continue;
	}
	canv->cd(4);	apad->Draw(kBlack);
	canv2->cd(4);	apad->Draw(kBlack);
      }
      canv->Update();
      canv2->Update();
      
      //EVENT DISPLAY
      TH1D * signal = new TH1D ("signals","Signal over time", 510,0,510);
      TH1D * signaltr = new TH1D ("signalstr","Cluster over time", 510,0,510);
      TH1D * hres = new TH1D ("hres","Residuals for X", 200,-1.5,1.5);
      npads++; //to avoid oneSig[npads] seg fault
      TH1F * oneSig[npads];
      for (int i=0;i<npads;i++) oneSig[i]=NULL;
      TH1F * occChip[nbchip];
      TH1F * ampChip[nbchip];
      for (int i=0;i<nbchip;i++) 
	{ 
	  char buf[60];	  sprintf(buf,"Timing for chip_%d, Ev. %d",i,evLo);
	  char buf0[10];  sprintf(buf0,"oct%d",i);
	  occChip[i] = new TH1F (buf0,buf,510,0,510);  
	  sprintf(buf,"amp for chip_%d, Ev. %d",i,evLo);
	  sprintf(buf0,"oca%d",i);
	  ampChip[i] = new TH1F (buf0,buf,510,0,510);  
	}
      for (int iQ=0;iQ<npads;iQ++) if(oneSig[iQ]!=NULL) {delete oneSig[iQ];oneSig[iQ]=NULL;} //oneSig zombie killer
      
      //END OF INIT - LOADING DATA
      recotree->GetEvent(evLo);
      
      //loop over clusters
      int nCl = clArr->GetEntries();
      int nTr = postTrArr->GetEntries();
      std::cout << "nb of clusters " << nCl << " for ev "<< evLo << ". Nb of postfit Tracks: "<< nTr << std::endl;
      
      //NOISE CUT
      if(((nCl<60||nCl>1900) || nTr==0) && mode==-1)
	{evLo++;std::cout << "No tracks or noise, skipping event..."<<std::endl;continue;}

      //Cluster LOOP
      int saNb=0;
      for(int c=0; c<nCl; c++) {
	PndTpcCluster* cl = (PndTpcCluster*)clArr->At(c);
	TVector3 pos = cl->pos();
	int size = cl->nDigi();
	if (!size) continue; //empty cluster ?! not good.
	
	float r, g, b;
	TColor::HLS2RGB((cl->getDigi(0).padId())/1500.0*256, (pos.Z()-0.5)/10.0, 1, r,g,b);
	
	marker3d = new TPolyMarker3D(1);
	marker3d->SetNextPoint(pos.X(),pos.Y(),pos.Z()); 
	marker3d->SetMarkerStyle(3);
	marker3d->SetMarkerSize((adc_range-cl->amp())/(adc_range*2));
	marker3d->SetMarkerColor(TColor::GetColor(r, g, b));
	clusters3d[c]=marker3d;
	
	marker =  new TPolyMarker(0);
	marker->SetMarkerStyle(4);
	double MkSize = 0.5+1.5*cl->amp()/1600.0;
	if (MkSize > 2) MkSize = 2;
	marker->SetMarkerSize(MkSize);
	marker->SetMarkerColor(TColor::GetColor(r, g, b));
	marker->SetPoint(0,pos.X(),pos.Y()); 
	clusters[c]=marker;
	
	//SAMPLE PART
	for (int d = 0; d < size; d++)
	  {
	    for (unsigned int s = 0; s < cl->getDigi(d).nSample(); s++)
	      {
		PndTpcSample psa  = cl->getDigi(d).getSample(s);
		int pad = psa.padId();
		PndTpcPad* sapad=plane->GetPad(pad);
		marker =  new TPolyMarker(0);
		marker->SetMarkerStyle(5);
		double MkSizeSa = 0.5+1.5*psa.amp()/1600.0; if (MkSizeSa > 2) MkSizeSa = 2;
		marker->SetMarkerSize(MkSizeSa);
		marker->SetMarkerColor(TColor::GetColor(r, g, b));
		marker->SetPoint(0,sapad->x(),sapad->y()); 
		samples[saNb]=marker;

		const ExtendedKey* key = MappingManager::Instance().getExtendedKey(pad);
		int NbChip = key->getGlobalChipID() +(key->getSourceID()==801)*16 ;
		//		      if (key->getSourceID()==801) continue;
		if (NbChip &&NbChip<nbchip)
		  {	  
		    occChip[NbChip]->Fill(psa.t());
		    ampChip[NbChip]->Fill(psa.t(),psa.amp());
		  }
		signal->Fill(psa.t() ,psa.amp());
		std::cout << "evLo "<<evLo << "pad " <<pad << "psa "<< psa.t() << " " << psa.amp() <<std::endl;
		if (oneSig[pad])
		  oneSig[pad]->Fill(psa.t() ,psa.amp());
		  
		else
		  {
		    char buf[20];char buf0[20];
		    sprintf(buf,"oneSig_%d, Ev. %d",pad,evLo);
		    sprintf(buf0,"oneSig%d",pad);
		    oneSig[pad] = new TH1F (buf0,buf,510,0,510);
		    oneSig[pad]->Fill(psa.t() ,psa.amp());
		    oneSig[pad]->SetFillColor(TColor::GetColor(r, g, b));
		    //TODO LINE FOR DIGI
		    // TLine * lidi = new TLine(cl->getDigi(d).t(),0,cl->getDigi(d).t(),200);//show digit time
		    // lidi->Draw("same");
		  }
	      saNb++;}//sample loop
	  }//digi loop

      }//cluster loop
      
      //TRACKS PART
      for(int t = 0;t<nTr;t++)
	{
	  GFTrack* tr = (GFTrack*) postTrArr->At(t);
	  GFTrackCand cand = tr->getCand();
	  candIDs.clear(); 
	  candIDs = cand.GetHitIDs(2);
	  TVector3 mom = tr->getMom();     //momentum after fit
	  TVector3 sl_dir = mom;
	  sl_dir*=(1/mom.Mag());
	  TVector3 sl_pos = tr->getPos();  //SL starting position after fit
	  //TODO scale line to histograms
	  line = new TLine(sl_pos.X()-4.0*sl_dir.X(),sl_pos.Y()-4.0*sl_dir.Y(),
			   sl_pos.X()+4.0*sl_dir.X(),sl_pos.Y()+4.0*sl_dir.Y());
	  line3d = new TPolyLine3D(100);
	  line3d->SetPoint(0,sl_pos.X()-4.0*sl_dir.X(),sl_pos.Y()-4.0*sl_dir.Y(),sl_pos.Z()-4.0*sl_dir.Z());
	  line3d->SetPoint(1,sl_pos.X()+4.0*sl_dir.X(),sl_pos.Y()+4.0*sl_dir.Y(),sl_pos.Z()+4.0*sl_dir.Z());
	  PndTpcCluster* fcl = (PndTpcCluster*)clArr->At(candIDs[0]);//COLOR OF THE FIRST SAMPLE
	  float r, g, b;
	  TColor::HLS2RGB((fcl->getDigi(0).padId())/1500.0*256, (fcl->pos().Z()-0.5)/10.0, 1, r,g,b);
	  line->SetLineColor(TColor::GetColor(r, g, b));
	  line->SetLineWidth(3);
	  tracks[t]=line;
	  line3d->SetLineColor(TColor::GetColor(r, g, b));
	  line3d->SetLineWidth(3);
	  tracks3d[t]=line3d;

	  //CLUSTER OF FITTED tracks
	  for(unsigned int k=0; k<candIDs.size(); k++){
	    PndTpcCluster* cl = (PndTpcCluster*)clArr->At(candIDs[k]);
	    TVector3 cl_pos = cl->pos();
	    
	    //markers
	    marker =  new TPolyMarker(0);
	    marker->SetMarkerStyle(20);
	    double MkSize = 0.5+1.5*cl->amp()/1600.0;
	    if (MkSize > 2) MkSize = 2;
	    marker->SetMarkerSize(MkSize);
	    marker->SetMarkerColor(TColor::GetColor(r, g, b));
	    marker->SetPoint(0,cl_pos.X(),cl_pos.Y()); 
	    clusterstr[k]=marker;

	    marker3d = new TPolyMarker3D(1);
	    marker3d->SetNextPoint(cl_pos.X(),cl_pos.Y(),cl_pos.Z()); 
	    marker3d->SetMarkerStyle(3);
	    marker3d->SetMarkerSize((adc_range-cl->amp())/(adc_range*2));
	    marker3d->SetMarkerColor(TColor::GetColor(r, g, b));
	    clusterstr3d[k]=marker3d;
	    
	    //calculate residual
	    TVector3 cl_rel = cl_pos-sl_pos;
	    double l = sl_dir*cl_rel; //length along SL to projection point
	    TVector3 res = cl_rel - l*sl_dir;
	    hres->Fill(res.X());
	  }//end of cluster tracks
  	}//tracks loop
      //2d clusters
      char bufc[20];
      sprintf(bufc,"PadPlane Ev. %d",evLo);  
      real2->SetTitle(bufc);
      canv->cd(4);
      for(it=samples.begin(); it!=samples.end(); it++) 
	it->second->Draw("same");
      for(it=clusters.begin(); it!=clusters.end(); it++) 
	it->second->Draw("same");
      //3d cluster
      sprintf(bufc,"X Y Z Ev. %d   ",evLo);
      canv->cd(3);
      real3d->SetTitle(bufc);
      for(it3d=clusters3d.begin(); it3d!=clusters3d.end(); it3d++) 
	it3d->second->Draw("same");
      //2d tracks
      realtr2->SetTitle(bufc);
      canv2->cd(4);
      for(it=clusterstr.begin(); it!=clusterstr.end(); it++) 
	it->second->Draw("same");
      for(ittr=tracks.begin(); ittr!=tracks.end(); ittr++) 
	ittr->second->Draw("same");
      //3d tracks
      canv2->cd(3);
      realtr3d->SetTitle(bufc);
      for(it3d=clusterstr3d.begin(); it3d!=clusterstr3d.end(); it3d++) 
	it3d->second->Draw("same");
      for(ittr3d=tracks3d.begin(); ittr3d!=tracks3d.end(); ittr3d++) 
	ittr3d->second->Draw("same");
            
      //oneSig->Draw();
      double max =1;int maxi=-1;
      for (int i=0;i<1500;i++)
	if (oneSig[i])
	  {
	    if(oneSig[i]->Integral()/oneSig[i]->GetMaximum()>max) 
	      {maxi=i;max=oneSig[i]->Integral()/oneSig[i]->GetMaximum();}
	  }
      if (maxi!=-1)
	{
	  canv->cd(1);
	  oneSig[maxi]->Draw();
	  canv->cd(2);
	  signal->Draw();
	  canv->Update();
	}
      
      if(mode == -2)
	for (int chip = 0;chip<nbchip;chip++)
	  {
	    TCanvas * TChip= new TCanvas;
	    char ralala[60];
	    sprintf(ralala,"Chip Nb. %d",chip);
	    TChip->SetTitle(ralala);
	    TChip->Divide(1,2);
	    TChip->cd(1);
	    occChip[chip]->Draw();
	    TChip->cd(2);
	    ampChip[chip]->Draw();
	  }
      canv->Update();
      canv2->cd(1);
      hres->Draw();
      canv2->cd(2);
      signaltr->Draw();
      canv2->Update();
      
      if (mode==-1)
	{
	  gApplication->SetReturnFromRun(kTRUE);
	  gSystem->Run();
	}
      
      evLo++;
      std::cout << evLo << std::endl;
    }
  while(mode==-1);
}//END OF VOID PLOTEVENT

/*
//3Drotation
 if(mode==-2)
   {
     TString qd='y';
  //   std::cout << "3d rotation? y/n" <<  std::endl;
  //     std::cin >> qd ; 
     if (qd=='y')
       {
	 canv->cd(3);
	 TView3D * vw = new TView3D(1,0,0);
	 for (int irot = 0;irot<36;irot++) {
	   vw->RotateView(irot*10,70,0);
	   canv->Print("test.gif+20");
	 }
       }
   }
*/
