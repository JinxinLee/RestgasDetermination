// Macro to perform "live reocnstruction"

#include "tpc/PndTpcGem.h"
#include "tpc/PndTpcGas.h"
#include "tpc/PndTpcClusterFinder.h"
#include "tpc/PndTpcDigiMapper.h"
#include "tpc/PndTpcPadPlane.h"
#include "tpc/PndTpcPadShapePool.h"
#include "tpc/PndTpcDigi.h"
#include "tpc/PndTpcCluster.h"
#include "tpc/PndTpcClusterRadius.h"
#include "tpc/McIdCollection.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TH2D.h"
#include "TH2.h"
#include "TLine.h"
#include "TStyle.h"
#include "algorithm"
#include "sstream"
#include "tpc/PndTpcDigiAge.h"
#include "TSystem.h"
#include "TStopwatch.h"
#include "TMarker.h"
#include "TVector3.h"
#include "TGraph.h"
#include "TError.h"
#include "TSystem.h"
#include "TView3D.h"
#include "TMatrixD.h"
#include "tpc/TCovEllipse.h"
#include "tpc/tpcreco/PndTpcRiemannTrackFinder.h"
#include "tpc/tpcreco/PndTpcRiemannHTCorrelator.h"
#include "tpc/tpcreco/PndTpcClustershapeCorrelator.h"
#include "tpc/tpcreco/PndTpcProximityHTCorrelator.h"
#include "tpc/tpcreco/PndTpcRiemannTrack.h"
#include "tpc/tpcreco/PndTpcRiemannHit.h"
#include "tpc/DebugLogger.h"

void recoMovie(TTree* t, TString padplane, TString padshapes, bool movie=false){

  gStyle->SetFillColor(kWhite);


  TCanvas* c=new TCanvas("c1","Tpc Reconstruction",0,0,1200,800);
  c->Divide(4,3);

  TCanvas* c2=new TCanvas("c2","Clustering and Tracklets",100,100,1200,800);
  c2->Divide(2,2);

  c2->cd(2)->SetPad(0,0,0.35,1);
  c2->cd(1)->SetPad(0.35,0,0.7,1);
  c2->cd(4)->SetPad(0.7,0,1,0.5);
  c2->cd(3)->SetPad(0.7,0.5,1,1);
 
  PndTpcGas* _gas=new PndTpcGas("tpc/NEON-90_CO2-10_B2_PRES1013.asc",400);

  //TODO: Get these things from Database!!!
  PndTpcGem* _gem=new PndTpcGem(5000,           // Gain
                  0.02);          // Spread

  PndTpcPadShapePool* _padShapes = new PndTpcPadShapePool(padshapes.Data(),
				   *_gem,
				   0.5, // lookup range
				   0.02, // Lookup Step
				   0.01); // LookupIntegrationStep


  PndTpcPadPlane* _padPlane= new PndTpcPadPlane(padplane.Data(),_padShapes);

  

  //TFile* file = new TFile(datafile);
  //if(!file)return;
  //TGeoManager *geoMan = (TGeoManager*) file->Get("FAIRGeom");

  // setup reco algorithms ---------------------------
  PndTpcDigiMapper::getInstance(false)->init(_padPlane,_gem,_gas,-40,-60000,40);
 
  std::vector<PndTpcCluster*>*_cluster_buffer=new std::vector<PndTpcCluster*>;
  PndTpcClusterFinder* _finder=new PndTpcClusterFinder(PndTpcDigiMapper::getInstance()->getPadPlane(),
			       _cluster_buffer,
				  10,1); // timeslice and mode
  
  _finder->checkConsistency();

  // Riemann trackfinder -----------------------------
  std::vector<PndTpcRiemannTrack*> riemannlist;
 
  
  double planecut=0.0015;
  double szcut=0.5;
  double proxcut=0.01;
  unsigned int minhits=4;

  TLine* planeLine1=new TLine(-planecut,0,-planecut,1);
  planeLine1->SetLineColor(kRed);
  TLine* planeLine2=new TLine(planecut,0,planecut,1);
  planeLine1->SetLineColor(kRed);
  planeLine2->SetLineColor(kRed);
  TLine* szLine1=new TLine(-szcut,0,-szcut,1);
  TLine* szLine2=new TLine(szcut,0,szcut,1);
  szLine1->SetLineColor(kRed);
  szLine2->SetLineColor(kRed);
  TLine* proxLine=new TLine(proxcut,0,proxcut,1);
  proxLine->SetLineColor(kRed);
   
  PndTpcRiemannTrackFinder*_trackfinder= new PndTpcRiemannTrackFinder();
  _trackfinder->setMinHitsForFit(4);
   _trackfinder->addCorrelator(new PndTpcProximityHTCorrelator(2));
  _trackfinder->addCorrelator(new PndTpcRiemannHTCorrelator(planecut,
							    proxcut,
							    szcut));
  //_trackfinder->addCorrelator(new PndTpcClustershapeCorrelator(2));
  //----------------------------------------------
    
  // z-window for plotting and discarding tracks
  double zwindow=1.5;

  
  //TTree *t=(TTree*)file->Get("cbmsim") ;
  
  TClonesArray *sa=new TClonesArray("PndTpcDigi");
  t->SetBranchAddress("PndTpcDigi",&sa);

  TH2D* ma=new TH2D("map","Hit pads",840,-42,42,840,-42,42);
  ma->SetStats(false);
  TH1D* hPure=new TH1D("pure","Purity",1000,0.0 ,1.1);
  TH1D* hClPure=new TH1D("clpure","Cluster Purity",1000,0.8 ,1.1);
  TH1D* hNHits=new TH1D("nhits","N Hits per track",100,0,100);
  TH1I* hPieces=new TH1I("pieces","N Pieces per track",20,0,20);
  TGraph* gNTracks=new TGraph(10000);
  gNTracks->SetMarkerStyle(22);// active tracks
  TGraph* gclusters=new TGraph(10000); gclusters->SetTitle("Used Clusters");
  TGraph* gusedclusters=new TGraph(10000);
  TGraph* glostclusters=new TGraph(10000);
  gclusters->SetMarkerStyle(22);
  gusedclusters->SetMarkerStyle(22);
  glostclusters->SetMarkerStyle(22);

  TGraph* gclustereff=new TGraph(10000);
  TH1* hCuts=NULL;
  TH1* hRiem=NULL;
  TH1* hDRiem=NULL;

  Int_t nevts=t->GetEntries();
  std::cout<< nevts << " sub-events found in data stream" << endl;
//   for (Int_t j=0; j<nevts; j++)	{
//     t->GetEntry(j);
//     for (Int_t i=0; i<sa->GetEntriesFast(); i++)	{
//       PndTpcDigi *sig=(PndTpcDigi*)sa->At(i);
//       double x,y;
//       _padPlane->GetPadXY(sig->padId(),x,y);
//       ma->Fill(x,y,sig->amp());
//     }
//   }
  //ma->Draw();
  //c->SaveAs("allDigis.eps");

  // Produce movie
  ma->Reset();
  ma->SetDrawOption("COL");

  // get first event
  std::vector<PndTpcDigi*> vd;
  std::vector<PndTpcDigi*> vdslice;
  std::vector<PndTpcCluster*> clusters;
  std::vector<PndTpcCluster*> recoverclusters;


  std::cout << "Loading digis ..." << std::endl;
  ProcInfo_t info1, info2;
  gSystem->GetProcInfo(&info1);
  std::cout << "Memory before: " << info1.fMemResident << " + Virt" 
	    <<  info1.fMemVirtual <<  std::endl;
  

  Int_t ndig=0;
  Int_t evt=nevts;
  for (Int_t j=0; j< nevts; j++)	{
    t->GetEntry(j);
    ndig=sa->GetEntriesFast();
  
    vd.reserve(ndig);
    for (Int_t i=0; i<ndig; ++i){
      PndTpcDigi *sig=(PndTpcDigi*)sa->At(i);
      vd.push_back(new PndTpcDigi(*sig));
    }
    std::cout << vd.size() << " digis loaded ..." << std::endl;
  }
  std::cout << " .. finished loading digis." << std::endl;
  gSystem->GetProcInfo(&info2);
  std::cout << "Memory after: " << info2.fMemResident << " + Virt" 
	    <<  info2.fMemVirtual <<  std::endl;
  std::cout << "Memory per digit: "<< (info2.fMemResident+info2.fMemVirtual-info1.fMemResident-info1.fMemVirtual)/vd.size()<< std::endl;
  //sort in time;

  std::cout << "Starting sorting digis in time..." << std::endl;
  sort(vd.begin(),vd.end(),PndTpcDigiAge());
  std::cout << "Finished sorting." << std::endl;

  ndig= vd.size();
  std::cout << "Number of Digits: "<< ndig << std::endl;
  Int_t t1=vd[0]->t();
  std::cout << "starting at t=" << t1 << std::endl;

  // make frames
  
  Int_t thisstep=0;
  Double_t time=0;
  int counter=0;
  Int_t lastcl=0;
  Int_t tstep=10; // wait for tstep before doing tracklet building
  Int_t usedclusters=0;
  Int_t lostclusters=0;
  Int_t totalclusters=0;
  ////////////////////////////////////////
  /////////// Note: keep small tracklets longer and try to add later!!!
  ///////////
  ///////////////////
  
  TStopwatch watch1;
  watch1.Start();

  std::vector<PndTpcRiemannTrack*> finishedtracks;
  std::map<McId,int> trackpieces;

  for(Int_t i=0; i<ndig; ++i){ // loop over digis
    PndTpcDigi *sig=vd[i];
    thisstep=sig->t()-t1;
    if(thisstep>tstep || i==ndig-1){ // process timeslice
      ++counter;
      time+=thisstep*0.025;
      double z0=PndTpcDigiMapper::getInstance()->z_from_tick(t1);
      t1=sig->t();
      


      // run clusterfinder
      unsigned int nclb=_cluster_buffer->size();
      gErrorAbortLevel=kWarning;
      _finder->process(vdslice);
      vdslice.clear();
      totalclusters+=_cluster_buffer->size()-nclb;

      sort(_cluster_buffer->begin(),_cluster_buffer->end(),PndTpcClusterRadius());
      // run trackfinder
      _trackfinder->buildTracks(*_cluster_buffer,riemannlist);
      //std::cout<<riemannlist.size()<<std::endl;
      gErrorAbortLevel=kFatal;
      TH1* hCuts=DebugLogger::Instance()->GetHisto("riemanncuts");

      std::ostringstream plotfile;

      if(movie){
	c2->cd(1);
	// print frame
	
	plotfile<<"frames/digi";
	//if(counter<100)file<<"0";
	//if(counter<10)file<<"0";
	plotfile<<counter<<".gif";
	ma->Draw("BOX");
	c2->cd(2);
	ma->Draw("BOX");
	//plot clusters
	int ncl=_cluster_buffer->size();
	for(int icl=0;icl<ncl;++icl){
	  PndTpcCluster* cl=(*_cluster_buffer)[icl];
	  hClPure->Fill(cl->mcId().MaxRelWeight());
	  clusters.push_back(cl); // store clusters
	  if(cl==NULL)continue;
	  TMatrixD cov=cl->cov();
	  cov*=5;
	  
	  TCovEllipse* el=new TCovEllipse(cov,cl->pos().X(),cl->pos().Y());
	  el->SetLineColor(kRed);
	  c2->cd(1);el->Draw();
	  c2->cd(2);el->Draw();
	  
	  if(cov[0][0]<0.5 && cov[1][1]<0.5) {
	    TMarker* mk=new TMarker(cl->pos().X(),cl->pos().Y(),24);
	    mk->SetMarkerColor(kRed);
	    mk->SetMarkerSize(0.5);
	    c2->cd(1);mk->Draw();
	    c2->cd(2);mk->Draw();
	  }
      }// end loop over clusters

     
      // plot Tracks
     
      //c2->Clear();

      }// end plotting

      

      int ntrk=riemannlist.size();
      std::vector<PndTpcRiemannTrack*> activetracks;
     

      //std::cout<<"ntracks="<<ntrk<<std::endl;
      gNTracks->SetPoint(counter,time,ntrk);
      for(int itrk=0;itrk<ntrk;++itrk){
	//riemannlist[itrk]->Plot(false);
	// check if this track is still in active window...
	double z1=riemannlist[itrk]->getFirstHit()->cluster()->pos().Z();
	double z2=riemannlist[itrk]->getLastHit()->cluster()->pos().Z();
	//std::cout<<"z0z1z2 "<<z0<<"  "<<z1<<"  "<<z2<<std::endl;
	int nhits=riemannlist[itrk]->getNumHits();
	// only plot those in the window
	if((z1<z0-zwindow && z2<z0-zwindow) || (z1>z0+zwindow && z2>z0+zwindow)){
	  
	  finishedtracks.push_back(riemannlist[itrk]);
	  
	  if(nhits>minhits){
	      hPure->Fill(riemannlist[itrk]->mcid().MaxRelWeight());
	      int pieces=trackpieces[riemannlist[itrk]->mcid().DominantID()];
	      trackpieces[riemannlist[itrk]->mcid().DominantID()]=pieces+1;
	      hNHits->Fill(nhits);
	      usedclusters+=nhits;
	  }
	  else {lostclusters+=nhits;}
	  
	  continue;
	}
      
	
	// keep small tracklets for next round
	if(nhits<=minhits){
	  for(int ih=0;ih<nhits;++ih){
	    PndTpcCluster* cl=riemannlist[itrk]->getHit(ih)->cluster();
	    recoverclusters.push_back(cl);
	  }//end loop over hits
	  continue;
	}


	
	
	
	// check if track is in time window
        
	
	activetracks.push_back(riemannlist[itrk]);

	if(movie){
	TGraph* track=new TGraph(nhits);
	for(int ih=0;ih<nhits;++ih){
	  TVector3 pos=riemannlist[itrk]->getHit(ih)->cluster()->pos();
	  track->SetPoint(ih,pos.X(),pos.Y());
	}//end loop over hits
	track->SetMarkerStyle(23);
	track->SetMarkerSize(0.5);
	track->SetMarkerColor(kBlue);
	track->SetLineColor(kBlue);
	track->Draw("PL");
	}
      }// end loop over tracks
     

      if(movie){
	c->cd(1);
	gNTracks->SetTitle("Active Tracklets");
	gNTracks->Draw("AP");
	c2->cd(3);
	gNTracks->Draw("AP");
	
	
	c->cd(2);
	gclusters->SetPoint(counter,time,totalclusters);
	gusedclusters->SetPoint(counter,time,usedclusters);
	glostclusters->SetPoint(counter,time,lostclusters);
	gclusters->Draw("AP");
	gusedclusters->SetMarkerColor(kBlue);
	gusedclusters->Draw("P");
	
	glostclusters->SetMarkerColor(kRed);
	glostclusters->Draw("P");
	
	c2->cd(4);
	gclusters->Draw("AP");
	gusedclusters->Draw("P");
	glostclusters->Draw("P");
	
	if(usedclusters>0)gclustereff->SetPoint(counter,time,(double)usedclusters/(double)totalclusters);
	



	c->cd(3);
	gclustereff->Draw("AP");
	
	c->cd(4);
	//hClPure->Draw();
	hPure->SetLineColor(kRed);
	hPure->Draw();

	
	//c->cd(4);
	//if(hCuts!=NULL){
	//	hCuts->Draw();
	//	gPad->SetLogy();
	//}
	c->cd(6);
	hRiem=DebugLogger::Instance()->GetHisto("riem_d");
	if(hRiem!=NULL){
	  hRiem->Draw();
	  planeLine1->SetY2(hRiem->GetMaximum()*1.1);
	  planeLine2->SetY2(hRiem->GetMaximum()*1.1);
	  planeLine1->Draw();
	  planeLine2->Draw();
	}
	c->cd(7);
	TH1* hSZ=DebugLogger::Instance()->GetHisto("riem_l2");
	if(hSZ!=NULL){
	  hSZ->Draw();
	  szLine1->SetY2(hSZ->GetMaximum()*1.1);
	  szLine2->SetY2(hSZ->GetMaximum()*1.1);
	  szLine1->Draw();
	  szLine2->Draw();
	  szLine1->Draw();
	  szLine2->Draw();
	}
        c->cd(5);
	hDRiem=DebugLogger::Instance()->GetHisto("riem_l");
	if(hDRiem!=NULL){
	  hDRiem->Draw();
	  proxLine->SetY2(hDRiem->GetMaximum()*1.1);
	  proxLine->Draw();
	  proxLine->Draw();
	}
	
	c->cd(9);
	TH2* hCl=DebugLogger::Instance()->GetHisto2D("shape_dir_theta");
	if(hCl!=NULL)hCl->Draw();
	
	c->cd(8);
	hNHits->Draw();
	gPad->SetLogy();
	
	c->cd(9);
	std::map<McId,int>::iterator it=trackpieces.begin();
	while(it!=trackpieces.end()){
	  hPieces->Fill(it->second);
	  ++it;
	}
	hPieces->Draw();
	
	c2->Update();
	c2->SaveAs(plotfile.str().c_str());
      }// end plotting
      // clean up
      //ma->Reset();
   
      //hPure->Reset();
      //hNHits->Reset();
      //hPieces->Reset();
      //for(int icl=0;icl<ncl;++icl){
      //	PndTpcCluster* cl=(*_cluster_buffer)[icl];
      //	delete cl;
      //}
      _cluster_buffer->clear();
      *_cluster_buffer=recoverclusters;
      recoverclusters.clear();
      // reinsert small tracklets into cluster buffer and try to add 
      // again next time
      
      riemannlist.clear();
      cout << activetracks.size() << " active tracks" << endl;
      riemannlist=activetracks;
      activetracks.clear();
      cout << finishedtracks.size() << " finished tracks" << endl;
      cout << "lost clusters: " << lostclusters << endl;
      cout << "used clusters: " << usedclusters << endl;
      cout << "Processed "<<i<<" out of "<< ndig <<" digis." << endl;
    } // end process time slice
    //double x,y;
    //_padPlane->GetPadXY(sig->padId(),x,y);
    //ma->Fill(x,y,sig->amp());
    vdslice.push_back(sig);
  } // end loop over digis
 watch1.Print();
  //gSystem->Exec("gifsicle --delay=10 --loop=5 frames/digi*.gif > anim.gif");
  DebugLogger::Instance()->WriteFiles();
 
  
}

