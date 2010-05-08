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
#include "TVector3.h"
#include "TBox.h"
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
//evHi == -1: browsing mode to display tracks starting from evLo, cut on efficiency
//evHi == -2: Display evLo with detailed information
//evHi > 0  : display general run info for event evLo<e<evLo+evHi

void plotEvent(TString filename, int evLo, int evHi)
{
   while(gROOT->GetListOfCanvases()->GetSize())
    delete gROOT->GetListOfCanvases()->At(0);
  gStyle->SetOptStat(0);
  gStyle-> SetPalette(1);
	  
  //MAPPING MANAGER

  if(ConfigManager::Instance().good())
    cout << ConfigManager::Instance();
  
  MappingManager::Instance().initMapSystem();
  if(!MappingManager::Instance().good()){
    std::cout << MappingManager::Instance();}

  // std::cout << key->getPin();
  // const ExtendedKey* key = MappingManager::Instance().getExtendedKey(747);
  TFile* recofile = new TFile(filename);
  if(recofile->IsZombie()) {
    std::cerr<<"Reco file not existing! Aborting."<<std::endl;
  }
  std::string vmc = gSystem->Getenv("VMCWORKDIR");
  vmc.append("/tpc/TestBench/");
  std::cout << "wdir" << vmc.c_str() << std::endl;
  std::string copy = vmc;
  PndTpcPadShapePool* pool=new PndTpcPadShapePool(copy.append("TBhexa_pads.dat").c_str());
  copy=vmc;
  PndTpcPadPlane* plane = new PndTpcPadPlane(copy.append("newPadTC.dat").c_str(),pool);
  std::cout<<"Plane has "<<plane->GetNPads()<<" pads."<<std::endl;

  TClonesArray* clArr = new TClonesArray("PndTpcCluster");
  TClonesArray* preTrArr = new TClonesArray("GFTrack");
  TClonesArray* postTrArr = new TClonesArray("GFTrack");

  TObjArray* digis = new TObjArray();
  TTree* recotree = (TTree*)recofile->Get("cbmsim");
  recotree->SetBranchAddress("PndTpcCluster", &clArr);
  recotree->SetBranchAddress("GFTrack", &preTrArr);
  recotree->SetBranchAddress("GFTrack", &postTrArr);

  std::cout << "Number of events: " << recotree->GetEntries() <<std::endl;
  TCanvas* canv =  new TCanvas("canv","canv",1000,1000);
  canv->Divide(2,2);
 
  int  u= 1;
do
    {
      unsigned int nEv = evHi-evLo;
      
      std::map<int,TPolyMarker*> clusters;
      //  std::vector<std::map<int,TPolyMarker*>* > clusters2;
      std::map<int,TPolyMarker3D*> clusters3d;
      
      std::map<int,TPolyMarker*>::iterator it;
      std::map<int,TPolyMarker3D*>::iterator it3d;
      
      std::vector<PndTpcCluster*> totClusters;
      
      TPolyMarker* digimarker = new TPolyMarker(0);
      digimarker->SetMarkerStyle(5);
      TPolyMarker* marker=NULL;
      TPolyMarker3D* marker3d=NULL;

      //3D
      TH3D * real3d = new TH3D ("bla3d","xzy",100,-5,5,100,-5,5,100,1,8.5);
      real3d->GetXaxis()->SetTitle("x (cm)");
      real3d->GetYaxis()->SetTitle("y (cm)");  
      real3d->GetZaxis()->SetTitle("z (cm)");

      canv->cd(3);real3d->Draw();
      //Draw padplane
      canv->cd(4);
      TH2F* real2;
      real2 = new TH2F("bla2", "Real Space x-y",100,-5,5,100,-5,5);
      real2->GetXaxis()->SetTitle("x (cm)");
      real2->GetYaxis()->SetTitle("y (cm)");  
      real2->Draw();
      int npads=plane->GetNPads();
      
      for(int i=1;i<npads;++i){ 
	PndTpcPad* apad=0;
	try{
	  apad=plane->GetPad(i);
	}
	catch (std::exception &e){
	  std::cout << "HERE !"<<e.what() << std::endl;
	  std::cout.flush();
	  continue;
	}
	apad->Draw(kBlack);
      }
      canv->Update();
      
      //G HIST
      std::vector <TH1*> GHIST;
      TH1F * smtit = new TH1F ("smtit","Amp of smaller pads",1024,0,2048);
      GHIST.push_back(smtit);
      TH1F * smt = new TH1F ("smt","Timing of smaller pads",510,0,510);
      smtit->SetFillColor(kBlue);
      smt->SetFillColor(kBlue);
      GHIST.push_back(smt);
      TH1F * bigtit = new TH1F ("bigtit","Amp of bigger pads",1024,0,2048);
      bigtit->SetFillColor(kRed);
      GHIST.push_back(bigtit);
      TH1F * bigt = new TH1F ("bigt","Timing of bigger pads",510,0,510);
      bigt->SetFillColor(kRed);
      GHIST.push_back(bigt);

      
      //  TH1F * tits = new TH1F ("tits","Diff od timing bet. big and small pads",1024,0,2048);
      TH1F * charge = new TH1F ("charge","Charge per event",1000,0,20000);  GHIST.push_back(charge);
  
      TH2D * ampMap = new TH2D ("ampMap","ampMap", 100,-5,5,100,-5,5);  GHIST.push_back(ampMap);
      TH2D * occMap = new TH2D ("occMap","occMap", 100,-5,5,100,-5,5);  GHIST.push_back(occMap);
      
      TH2D * tdiVsZ = new TH2D ("tdiVsZ"," tdiVsZ",510,0,510,20,0,20);  GHIST.push_back(tdiVsZ);
      
      TH1F * sampleamp = new TH1F ("sampleamp","Amplitude of samples",1024,0,2048);  GHIST.push_back(sampleamp);
      TH1F * occ = new TH1F ("Occupancy","Occupancy",100,0,2000);  GHIST.push_back(occ);
      
      TH1F * amptime = new TH1F ("amptime","Amplitude vs time",510,0,510);  GHIST.push_back(amptime);
      TH1F * occt = new TH1F ("Occupancyt","Occupancyt",510,0,510);  GHIST.push_back(occt);
      //EVENT DISPLAY
      TH1D * signal = new TH1D ("Signal over time","signals", 510,0,510);
      TH1F * oneSig[1501]; 
      for (int i=0;i<1501;i++) oneSig[i]=NULL;

      //TH1F * occChip;// = new TH1F ("buf0","buf",510,0,510);
      // GHIST.push_back(occChip);
	
      
      TH1F * occChip[25];
      TH1F * ampChip[25];
      for (int i=0;i<25;i++) 
	{ 
	  char buf[60];
	  sprintf(buf,"Timing for chip_%d, Ev. %d",i,evLo);
	  char buf0[10];
	  sprintf(buf0,"oct%d",i);
	  std::cout << buf << buf0 << std::endl;
	  occChip[i] = new TH1F (buf0,buf,510,0,510);  
	  sprintf(buf,"amp for chip_%d, Ev. %d",i,evLo);
	  sprintf(buf0,"oca%d",i);
	  ampChip[i] = new TH1F (buf0,buf,510,0,510);  
	  //	  GHIST.push_back(ampChip[i]);
	}
      

      
      int MAX = 0;
      if (evHi == -2) MAX=evLo;
      else if (evHi == -1) MAX=evLo;
      else MAX=evLo+evHi;

      
      for(unsigned int k=evLo; k<=MAX; k++) 
	{

	  recotree->GetEvent(k);

	  
	  //loop over clusters
	  int nCl = clArr->GetEntries();
	  occ->Fill(nCl);
	  std::cout << "nb of clusters " << nCl << " ev "<< k <<std::endl;

	  
	  //NOISE CUT
	  if((nCl<60||nCl>1900) && evHi==-1)
	    {evLo++;MAX++;
	    std::cout << "NOISE !"<<std::endl;continue;}
	  for (int iQ=0;iQ<1501;iQ++) if(oneSig[iQ]!=NULL) {delete oneSig[iQ];oneSig[iQ]=NULL;}
	  //	  for (int iQ=0;iQ<25;iQ++) if(occChip[iQ]!=NULL) {delete occChip[iQ];occChip[iQ]=NULL;}


	  double ch =0.0;
	  for(int c=0; c<nCl; c++) {
	    PndTpcCluster* cl = (PndTpcCluster*)clArr->At(c);
	    totClusters.push_back(new PndTpcCluster(*cl));
	    TVector3 pos = cl->pos();
	    
	    int size = cl->nDigi();
	    if (!size) continue;
	    if (size>1) std::cout << "!";
	    int size2d = cl->get2DSize();
	    //plot cluster in the right size     
   // std::cout << pos.X()<<" , " << pos.Y()<<" , " << pos.Z()<<" - " << cl->getDigi(0).padId() <<std::endl;      
	    // std::cout << "size " << size <<std::endl;
	        
	    float r, g, b;
	    TColor::HLS2RGB((cl->getDigi(0).padId())/1500.0*256, (pos.Z()-0.5)/10.0, 1, r,g,b);
	    	    
	    //G_HIST
	    if(evHi!=-1)
	      {
		ampMap->Fill(pos.X(),pos.Y(),cl->amp());
		occMap->Fill(pos.X(),pos.Y());
	      }
	  //EV_DISPLAY
	    if(evHi<0) 
	      { 
		
		marker3d = new TPolyMarker3D(1);
		marker3d->SetNextPoint(pos.X(),pos.Y(),pos.Z()); 
		marker3d->SetMarkerStyle(3);
		int adc_range;
		adc_range = 2048;
		marker3d->SetMarkerSize((adc_range-cl->amp())/(adc_range*2));
		marker3d->SetMarkerColor(TColor::GetColor(r, g, b));
		clusters3d[c]=marker3d;

		marker =  new TPolyMarker(0);
		clusters[c]=marker;
		marker->SetMarkerStyle(20);
		double MkSize = 0.5+1.5*cl->amp()/1600.0;
		if (MkSize > 2) MkSize = 2;
		marker->SetMarkerSize(MkSize);
		marker->SetMarkerColor(TColor::GetColor(r, g, b));//TColor::GetColor(COL*0xFF0000+COL*0x00FF00+COL));
		marker->SetPoint(0,pos.X(),pos.Y()); 
	      }    
	    ch+=cl->amp();
	    
	    //SIGNAL PART
	    if (size)
	      for (int d = 0; d < size; d++)
		{
		  tdiVsZ->Fill(cl->getDigi(d).t(),cl->getDigi(d).nSample());
		  for (int s = 0; s < cl->getDigi(d).nSample(); s++)
		    //	  if (cl->getDigi(size).getSample(s))
		    {
		      PndTpcSample psa  = cl->getDigi(d).getSample(s);
		      int pad = cl->getDigi(d).padId();
		      const ExtendedKey* key = MappingManager::Instance().getExtendedKey(pad);
		      int NbChip = key->getGlobalChipID() +(key->getSourceID()==801)*16 ;
		      //		      if (key->getSourceID()==801) continue;
		      
		      if (pos.Y()>0.6)
			{
			  bigtit->Fill(psa.amp());
			  bigt->Fill(psa.t());
			  continue;
			}
		      else 
			{
			  smtit->Fill(psa.amp());
			  smt->Fill(psa.t());
			}
		      sampleamp->Fill(psa.amp());
		      occt->Fill(psa.t());
		      amptime->Fill(psa.t(),psa.amp());
		      
		      if (key->getGlobalChipID()>0 &&key->getGlobalChipID()<25 )
			{
			  occChip[NbChip]->Fill(psa.t());
			  ampChip[NbChip]->Fill(psa.t(),psa.amp());
			}
		      
		      if (evHi<0)//EVENT HIST
			{
			  signal->Fill(psa.t() ,psa.amp());
			  //	
			  std::cout << "evLo "<<evLo << "pad " <<pad << "psa "<< psa.t() << " " << psa.amp() <<std::endl;
			  
			  
			  
			  if (oneSig[pad])
			    {oneSig[pad]->Fill(psa.t() ,psa.amp());
			    }
			  else
			    {
			      char buf[10];
			      sprintf(buf,"oneSig_%d, Ev. %d",pad,evLo);
			      oneSig[pad] = new TH1F (buf,buf,510,0,510);
			      oneSig[pad]->Fill(psa.t() ,psa.amp());
			      oneSig[pad]->SetFillColor(TColor::GetColor(r, g, b));
			    }
			}
		    }//sampple loop
		}//digi loop
	  }//cluster loop
	  charge->Fill(ch);
	}//event loop
      //    REAL2->Draw();
      if (evHi<0)//Event Display
	{
	  std::cout << clusters.size() << std::endl; 
	  char bufc[20];
	  sprintf(bufc,"PadPlane Ev. %d",evLo);
	  real2->SetTitle(bufc);
	  for(it=clusters.begin(); it!=clusters.end(); it++) 
	    it->second->Draw("same");


	  sprintf(bufc,"X Y Z Ev. %d   ",evLo);
	  canv->cd(3);
	  real3d->SetTitle(bufc);
	  for(it3d=clusters3d.begin(); it3d!=clusters3d.end(); it3d++) 
	    it3d->second->Draw("same");
	  	  
	  //oneSig->Draw();
	  double max =1;int maxi=-1;
	  for (int i=0;i<1500;i++)
	    if (oneSig[i])
	      {//std::cout << "PAD " << i << " int " << oneSig[i]->Integral() << " max " << oneSig[i]->GetMaximum()<<std::endl;
	      if(oneSig[i]->Integral()/oneSig[i]->GetMaximum()>max) 
		{maxi=i;max=oneSig[i]->Integral()/oneSig[i]->GetMaximum();}
	      }
	  std::cout << "MAXI " << maxi<< std::endl;
	  if (maxi!=-1)
	    {
	      canv->cd(1);
	      oneSig[maxi]->Draw();
	      canv->cd(2);
	      signal->Draw();
	      canv->Update();
	    }
	  else return ;
	  
	}
      if(evHi!=-1) //general display
	{
	  
	  TH1F * tamp = (TH1F*) amptime->Clone("tamp");
	  tamp->SetTitle("Mean amp. over time");
	  tamp->Divide(occt);
	  tamp->Draw();
	  GHIST.push_back(tamp);
	  
	  TH2D * tMap = (TH2D*) ampMap->Clone("tMap");
	  tMap->SetTitle("2D view Mean amp.");
	  tMap->Draw("COLZ");
	  tMap->Divide(occMap);
	  GHIST.push_back(tMap);

	  for (int u=0;u<GHIST.size();++u)
	    {
	      TCanvas * G_Hc = new TCanvas;
	      G_Hc->cd();
	      G_Hc->SetTitle(GHIST.at(u)->GetTitle());
	      TString drawOpt("");
	      if ( dynamic_cast<TH2*>(GHIST.at(u)) ) drawOpt="COLZ";
	      GHIST.at(u)->Draw(drawOpt);
	      G_Hc->Update();
	    }

	  for (int chip = 0;chip<25;chip++)
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
	      
	  TCanvas * ampVsPadSize = new TCanvas;
	  smtit->Draw();
	  bigtit->SetTitle("Amp. of big (blue) and small(red) pads");
	  bigtit->Draw("same");
	  

	  TCanvas * timeVsPadSize = new TCanvas;
	  smt->Draw();
	  bigt->SetTitle("Time of big (blue) and small(red) pads");
	  bigt->Draw("same");


	}
      
      
      //std::cout << "u?";
      canv->Update();

      if (evHi==-1)
	{
	  gApplication->SetReturnFromRun(kTRUE);
	  gSystem->Run();
	  for (int u=0;u<GHIST.size();++u)
	    GHIST.at(u)->Delete();
	}

      
      if(u){MAX++;evLo++;}
      std::cout << evLo << std::endl;
    }  while(evHi==-1 && u);

/*//3Drotation
 if(evHi==-2)
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

}
