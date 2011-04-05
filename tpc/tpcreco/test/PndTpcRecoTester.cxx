//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcRecoTester
//      see PndTpcRecoTester.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcRecoTester.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <map>
#include <algorithm>
#include <cmath>

// Collaborating Class Headers --------
#include "TApplication.h"
#include "PndTpcCluster.h"
#include "GFKalman.h"
#include "PndTpcClusterRadius.h"
#include "PndTpcClusterZ.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "PndTpcConfMapRecoHit.h"
#include "PndTpcConfMapFit.h"
#include "GFDetPlane.h"
#include "PndTpcConfTrackFinder.h"
#include "GFTrackCand.h"
#include "GFTrack.h"
#include "TCovEllipse.h"
#include "TGraph.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
#include "TH1I.h"
#include "TF2.h"
#include "TH2D.h"
#include "TLorentzVector.h"
#include "TPolyMarker3D.h"
#include "TPolyLine3D.h"
#include "DebugLogger.h"
#include "dbgstream.h"
#include "TMatrixT.h"
#include "TVectorD.h"
#include "StdDiscriminantFcn.h"
#include "PndTpcRiemannHit.h"
#include "PndTpcRiemannTrack.h"
#include "PndTpcRiemannTrackFinder.h"
#include "PndTpcRiemannHTCorrelator.h"
#include "PndTpcRiProxHTCorrelator.h"
#include "PndTpcSzHTCorrelator.h"
#include "PndTpcProximityHTCorrelator.h"
#include "TMath.h"
// Class Member definitions -----------

ClassImp(PndTpcRecoTester)

void
PndTpcRecoTester::testConfMap(){
  /*
  std::vector<PndTpcCluster*> cll; // clusterlist
  maketoytracks(2,cll);
  
  TGraph* confhits=new TGraph(cll.size());
  TGraph* hits=new TGraph(cll.size());
  
  for(int i=0;i<cll.size();++i){
    hits->SetPoint(i,cll[i]->pos().X(),cll[i]->pos().Y());
    PndTpcConfMapRecoHit hit(cll[i]);
    confhits->SetPoint(i,hit.getXcf(),
		       hit.getHitCoord(GFDetPlane())[0][0]);
  }

  cll.clear();
  
  maketoytracks(2,cll,true);
  TGraph* confhits2=new TGraph(cll.size());
  TGraph* hits2=new TGraph(cll.size());
  
  for(int i=0;i<cll.size();++i){
    hits2->SetPoint(i,cll[i]->pos().X(),cll[i]->pos().Y());
    PndTpcConfMapRecoHit hit(cll[i]);
    confhits2->SetPoint(i,hit.getXcf(),
		       hit.getHitCoord(GFDetPlane())[0][0]);
  }


  TFile* outfile=new TFile("confhits.root","RECREATE");
  confhits->SetTitle("Conformal Hits");
  confhits->Write("confhits");
  hits->SetTitle("Hits");
  hits->Write("hits");

  confhits2->SetTitle("Conformal Hits from IP");
  confhits2->Write("confhitsIP");
  hits2->SetTitle("Hits from IP");
  hits2->Write("hitsIP");

  outfile->Close();
  */
  return;
}

void
PndTpcRecoTester::testConfMapFit(){
  /*
  PndTpcConfMapFit myfit;
  TMatrixT<double> cov(2,2);
  //cov[0][0]=10;
  //cov[1][1]=10;
  //cov[0][1]=-1;
  //cov[1][0]=-1;
  //myfit.setCov(cov);
  myfit.Print();
  PndTpcCluster mycl(TVector3(5,5,3),1,0);
  PndTpcConfMapRecoHit hit(&mycl);
  
  
 
  std::cout<<"Residual:"<<std::endl;
  hit.residualVector(&myfit,myfit.getState()).Print();

  GFKalman myfitter;
  double chi2=myfitter.getChi2Hit(&hit,&myfit);
  std::cout<<"Chi2="<<chi2<<std::endl;
  */
}

void
PndTpcRecoTester::testBasicConfFit(){
  /*
  PndTpcConfMapRecoHit hit1(0.5,0.51);
  PndTpcConfMapRecoHit hit2(0.7,0.68);
  PndTpcConfMapRecoHit hit3(1.2,1.22);
  PndTpcConfMapRecoHit hit4(1.2,1.24);
  PndTpcConfMapRecoHit hit5(1.2,1.26);
  PndTpcConfMapRecoHit hit6(1.2,1.28);

  Track* trk=new Track(new PndTpcConfMapFit());
  TMatrixT<double> par(2,1);
  par[0][0]=1;
  par[1][0]=0;
  trk->getTrackRep(0)->setState(par);
  trk->getTrackRep(0)->Print();
  
   GFKalman myfitter;
  double chi2=myfitter.getChi2Hit(&hit1,trk->getTrackRep(0));
  std::cout<<"Chi2 for hit1="<<chi2<<std::endl;

  trk->addHit(&hit1,2,0);
  myfitter.continueTrack(trk,1);
  trk->getTrackRep(0)->Print();

  
  chi2=myfitter.getChi2Hit(&hit2,trk->getTrackRep(0));
  std::cout<<"Chi2 for hit2="<<chi2<<std::endl;

  trk->addHit(&hit2,2,1);
  myfitter.continueTrack(trk,1);
  trk->getTrackRep(0)->Print();
  
  chi2=myfitter.getChi2Hit(&hit3,trk->getTrackRep(0));
  std::cout<<"Chi2 for hit3="<<chi2<<std::endl;
  chi2=myfitter.getChi2Hit(&hit4,trk->getTrackRep(0));
 std::cout<<"Chi2 for hit4="<<chi2<<std::endl;
  chi2=myfitter.getChi2Hit(&hit5,trk->getTrackRep(0));
 std::cout<<"Chi2 for hit5="<<chi2<<std::endl;
  chi2=myfitter.getChi2Hit(&hit6,trk->getTrackRep(0));
 std::cout<<"Chi2 for hit6="<<chi2<<std::endl;
  */
}

int
PndTpcRecoTester::testConfFinder(){
  /*
  std::vector<PndTpcCluster*> cll; // clusterlist
  maketoytracks(2,cll);

  //std::cout<<cll.size()<<" clusters produced."<<std::endl;

  PndTpcConfTrackFinder _finder;
  _finder.configure(1,1,2, // proximity cuts
		    3,     // chi2cut
		    4,     // numhitsforfit 
		    false,
		    0.05,0.05, // merge cuts
		    4.);   // proximity merge cut
  
  std::vector<TrackCand*> cands;
  _finder.buildTracks(cll,cands);

  //std::cout<<cands.size()<<" track candidates found."<<std::endl;
  for(unsigned int i=0; i<cands.size(); ++i){
    std::cout<<" candidate"<<i<<" with "<<cands[i]->getNHits()<<" hits and radius "<<1./cands[i]->getCurv()<<std::endl;
  }

  // draw graphs
  std::cout<< _finder.getTrkListRef().size()<<" tracks used in finder"<<std::endl;
  TGraph* confhits=new TGraph();
  TGraph* zshits=new TGraph();
  unsigned int hitcounter=0;
  std::vector<Track*>& trks=_finder.getTrkListRef(); // beware using reference!
  for(unsigned int i=0;i<trks.size();++i){// loop over tracks
    if(trks[i]==NULL)continue;
    if(dynamic_cast<PndTpcConfMapFit*>(trks[i]->getTrackRep(0))->isRotated())
      std::cout<<"Track "<<i<<" is rotated"<<std::endl;
    unsigned int nh=trks[i]->getNumHits();
    for(unsigned int ih=0;ih<nh;++ih){//loop over hits in track
      PndTpcConfMapRecoHit* conf=dynamic_cast<PndTpcConfMapRecoHit*>(trks[i]->getHit(ih));
      confhits->SetPoint(hitcounter,conf->getXcf(),
			 conf->getHitCoord(GFDetPlane())[0][0]);
      zshits->SetPoint(hitcounter++,conf->s(),
			 conf->z());
    }// end loop over hits in track

  }// end loop over tracks
  TFile* outfile=new TFile("confhits.root","RECREATE");
  confhits->Write("confhits");
  zshits->Write("zshits");
  outfile->Close();
  return cands.size();
  */
}

void
PndTpcRecoTester::testRotated(){
  PndTpcConfMapRecoHit hit1(new PndTpcCluster(TVector3(5,5,3),1,0));
  std::cout<<hit1.getHitCoord(GFDetPlane())[0][0]<<" "
	   <<hit1.getXcf()<<std::endl;
  hit1.setRotated();
  std::cout<<hit1.getHitCoord(GFDetPlane())[0][0]<<" "
	   <<hit1.getXcf()<<std::endl;
  hit1.setRotated(false);
  std::cout<<hit1.getHitCoord(GFDetPlane())[0][0]<<" "
	   <<hit1.getXcf()<<std::endl;
}


void
PndTpcRecoTester::testConfFinderStat(){
  /*
  TH1I* histo=new TH1I("nfound","nfound",30,0,30);
  for(int ev=0;ev<1000;++ev){
    histo->Fill(testConfFinder());
  }
  TFile* outfile=new TFile("toystat.root","RECREATE");
  histo->Write();
  outfile->Close();
  */
}


void
PndTpcRecoTester::testConfTrackMerge(int nevents, bool split, bool merge){
  /*
  
  PndTpcConfTrackFinder _finder;
  _finder.configure(1,1,2, // proximity cuts
		    3,     // chi2cut
		    3,     // numhitsforfit 
		    merge, // do merge
		    0.05,0.05, // merge cuts
		    4.);   // proximity merge cut
  if(split)_finder.setMaxHitsInTrack(20);
  std::vector<TMatrixT<double> > confresults;
  std::vector<TMatrixT<double> > confcov;
  _finder.setConfFitResults(&confresults,&confcov);

  
  

  // make event
  std::vector<double> rv;
  std::vector<TVector2> dirv;
  std::vector<double> dipv;
  std::vector<TVector2> startv;
  //maketoyevent(2,rv,dirv,dipv);
  rv.push_back(50);
  rv.push_back(51);
  TVector2 dir(1,0.3);dir=dir.Unit();
  dirv.push_back(dir);
  TVector2 dir2(1.1,0.6);dir2=dir2.Unit();
  dirv.push_back(dir2);
  dipv.push_back(1);
  dipv.push_back(1);
  TVector2 start1(0,0);startv.push_back(start1);
  TVector2 start2(0,0);startv.push_back(start2);


  for(int evts=0; evts<nevents; ++evts){
    std::vector<PndTpcCluster*> cll; // clusterlist
    maketoyhits(2,rv,dirv,dipv,startv,cll);
    std::vector<TrackCand*> cands;
    std::vector<TrackCand*> cands2;
    _finder.buildTracks(cll,cands);
  }

  TFile* outfile=new TFile("toystat.root","RECREATE");
  TTree* tr=new TTree("stattree","stattree");
  double p1;
  double p2;
  tr->Branch("p1",&p1,"p1/D"); 
  tr->Branch("p2",&p2,"p2/D"); 
  std::cout<<confcov.size()<<std::endl;
  for(int i=0;i<confresults.size();++i){
    p1=confresults[i][0][0];
    p2=confresults[i][1][0];
    tr->Fill();
    TCovEllipse* el=new TCovEllipse(confcov[i],p1,p2);
    confcov[i].Print();
    std::cout<<"p1="<<p1<<"  p2="<<p2<<std::endl;
    char name[20];sprintf(name,"el%i",i);
    el->Write(name);
  }

  tr->Write();
  outfile->Close();
  */
}

//************************************************************************
//****************** discrimant analysis *********************************

// param[0]=m[0], param[1]=m[1], param[2]=cov[0][0], param[3]=cov[1][1],
// param[4]=cov[1][0]=cov[0][1]
double ftf2(Double_t *x, Double_t *params)
{
  TVectorT<double> m(2);
  m[0]=params[0];
  m[1]=params[1];
  TMatrixT<double> cov(2,2);
  cov[0][0]=params[2];
  cov[1][1]=params[3];
  cov[0][1]=params[4];
  cov[1][0]=cov[0][1];
  StdDiscriminantFcn dis(m,cov);
  TVectorT<double> xv(2,x);
  return dis(xv);
}

double dis2(Double_t *x, Double_t *params)
{
  // class1 
  TVectorT<double> m1(2);
  m1[0]=params[0];
  m1[1]=params[1];
  TMatrixT<double> cov1(2,2);
  cov1[0][0]=params[2];
  cov1[1][1]=params[3];
  cov1[0][1]=params[4];
  cov1[1][0]=cov1[0][1];
  StdDiscriminantFcn dis1(m1,cov1);

  // class2
  TVectorT<double> m2(2);
  m2[0]=params[5];
  m2[1]=params[6];
  TMatrixT<double> cov2(2,2);
  cov2[0][0]=params[7];
  cov2[1][1]=params[8];
  cov2[0][1]=params[9];
  cov2[1][0]=cov2[0][1];
  StdDiscriminantFcn dis2(m2,cov2);

  TVectorT<double> xv(2,x);
  return dis1(xv)-dis2(xv);
}

double dis3(Double_t *x, Double_t *params)
{
  double d1=ftf2(x, params);
  double d2=ftf2(x, &(params[5]));
  double d3=ftf2(x, &(params[10]));
  double m1= d1>d2 ? d1 : d2;
  return m1>d3 ? m1 : d3;
}

double dis3_cut(Double_t *x, Double_t *params)
{
  double d=dis3(x,params);
  return params[15]>d ? params[15] : d;
}

void
PndTpcRecoTester::testStdDis(double* par)
{
  TF2* f=new TF2("dis",&ftf2,0,4,0,4,5);
  double pars[16]={2,2,0.4,0.8,0.5,
		   5,1,0.4,0.8,0.5,
		   3,4,0.1,0.2,0.1,-2};
  if(par!=NULL)for(int i=0;i<16;++i)pars[i]=par[i];
  f->SetParameters(pars);
  TMatrixT<double> cov(2,2);
  cov[0][0]=pars[2];
  cov[1][1]=pars[3];
  cov[0][1]=pars[4];
  cov[1][0]=cov[0][1];
  TCovEllipse* el=new TCovEllipse(cov,pars[0],pars[1]);


  TF2* f2=new TF2("dis2",&dis2,-5,10,-5,10,10);
  f2->SetParameters(pars);
  TMatrixT<double> cov2(2,2);
  cov2[0][0]=pars[7];
  cov2[1][1]=pars[8];
  cov2[0][1]=pars[9];
  cov2[1][0]=cov2[0][1];
  TCovEllipse* el2=new TCovEllipse(cov2,pars[5],pars[6]);

  
  TF2* f3=new TF2("dis3",&dis3,-5,10,-5,10,15);
  f3->SetParameters(pars);
  TMatrixT<double> cov3(2,2);
  cov3[0][0]=pars[12];
  cov3[1][1]=pars[13];
  cov3[0][1]=pars[14];
  cov3[1][0]=cov2[0][1];
  TCovEllipse* el3=new TCovEllipse(cov3,pars[10],pars[11]);

  TF2* f4=new TF2("dis3_cut",&dis3_cut,-5,10,-5,10,16);
  f4->SetParameters(pars);

  TFile* file=new TFile("StdDis.root","RECREATE");
  el->Write("covel");
  el2->Write("covel2");
  el3->Write("covel3");
  f->Write();
  f2->Write();
  f3->Write();
  f4->Write();
  file->Close();
}

// ***********************************************************************
// *************** Riemann tracker ***************************************
void
PndTpcRecoTester::testRiemannMap(){

  std::vector<PndTpcCluster*> cll; // clusterlist
  maketoytracks(1,cll);

  std::vector<PndTpcRiemannHit*> rhits;
  int nhits=cll.size();
  for(int i=0;i<nhits;++i){
    rhits.push_back(new PndTpcRiemannHit(cll[i]));
  }
  
  TPolyMarker3D* line=new TPolyMarker3D(nhits);

  PndTpcRiemannTrack track;

  for(int j=0;j<nhits;++j){
    TVector3 x=rhits[j]->x();
    //x.Print();
    
    track.addHit(rhits[j]);

    TVector3 r=x-TVector3(0,0,0.5);
    //std::cout<<"R="<<r.Mag()<<std::endl;
    line->SetPoint(j,x.X(),x.Y(),x.Z());
  }

  track.av().Print();
  track.refit();
  track.szFit();
  track.n().Print();
  std::cout<<"abs(n)="<<TMath::Sqrt(track.n().Norm2Sqr())<<std::endl;
  std::cout<<"r="<<track.r()<<std::endl;
  std::cout<<"m="<<track.m()<<std::endl;
  std::cout<<"dip="<<track.dip()<<std::endl;
  std::cout<<"sign="<<track.sign()<<std::endl;
  std::cout<<"Center:";
  track.orig().Print();
  
  double c=track.c();
  TVectorD o(3);
  o[0]=rhits[0]->x().X();
  o[1]=rhits[0]->x().Y();
  o[2]=rhits[0]->x().Z();
  TVectorD e=o+track.n();

  TGraph* sz=new TGraph(nhits);
  for(int j=0;j<nhits;++j){
    if(j>0)rhits[j]->calcPosOnTrk(rhits[j-1]);
    sz->SetPoint(j,rhits[j]->s(),rhits[j]->z());
  }

  TPolyLine3D* arr=new TPolyLine3D(2);
  arr->SetPoint(0,o[0],o[1],o[2]);
  arr->SetPoint(1,e[0],e[1],e[2]);

  TFile* file=new TFile("Riemann.root","RECREATE");
  line->Write("rieTrack");
  arr->Write("arrow");
  sz->Write("sz");
  file->Close();
    

}

void
PndTpcRecoTester::testRiemannSZFit(double sigma){
  TH1D* resSZ=new TH1D("resSZ","resSZ",100,-1,1);
  for(int i=0; i<10; ++i){
  std::vector<PndTpcCluster*> cll;
  maketoytracks(1,cll,false,sigma);
  
  std::sort(cll.begin(),cll.end(),PndTpcClusterZ());
  
  
  PndTpcRiemannTrack* trk=new PndTpcRiemannTrack();
  std::vector<PndTpcRiemannHit*> rhits;
  
  int nhits=cll.size();
  for(int ii=0;ii<nhits;++ii){
    PndTpcRiemannHit* hit=new PndTpcRiemannHit(cll[ii]);
    trk->addHit(hit);
    rhits.push_back(hit);
  }
   
  TCanvas* c=new TCanvas();
  trk->szFit();
  TGraph* sz=new TGraph(nhits);
  for(int j=0;j<nhits;++j){
    sz->SetPoint(j,rhits[j]->s(),rhits[j]->z());
    std::cout<<"szDistRaw="<<trk->szDist(rhits[j],false)
	     <<"   szDistRe"<<trk->szDist(rhits[j],true)<<std::endl;
    resSZ->Fill(trk->szDist(rhits[j],false));
  }

  std::cout<<trk->m()<<std::endl;
  TF1* f1=new TF1("sz","[0]*x+[1]",0,60);
  f1->SetParameter(0,trk->m());
  f1->SetParameter(1,trk->t());
  sz->SetMarkerStyle(23);
  sz->Draw("AP");
  f1->SetLineColor(kRed);
  f1->Draw("same");
  gApplication->SetReturnFromRun(kTRUE);
  gSystem->Run();
  delete sz;
  delete c;
}
  resSZ->Draw();
gApplication->SetReturnFromRun(kTRUE);
  gSystem->Run();
}


void
PndTpcRecoTester::testRiemannFit(unsigned int ntrk, double r){
  
  TH1D* rh=new TH1D("rh","rh",100,r-20,r+20);
  TH1D* diph=new TH1D("diph","diph",40,-0.1,0.1);

  double mean=0;

  PndTpcRiemannTrackFinder finder;
  finder.setMinHitsForFit(4);
  finder.addCorrelator(new PndTpcRiemannHTCorrelator(1E-3));
  finder.addCorrelator(new PndTpcSzHTCorrelator(1.0));
  finder.addCorrelator(new PndTpcRiProxHTCorrelator(0.01));
  finder.addCorrelator(new PndTpcProximityHTCorrelator(2.));
  

  for(int i=0;i<ntrk;++i){
    std::vector<PndTpcCluster*> cll; // clusterlist
    std::vector<double> rv;
    std::vector<TVector2> dirv;
    std::vector<double> dipv;
    std::vector<TVector2> startv;
    
    rv.push_back(r);
    dipv.push_back(10);
    
    maketoyevent(1,rv,dirv,dipv,startv);
    maketoyhits(1,rv,dirv,dipv,startv,cll);
    
    std::vector<PndTpcRiemannTrack*> cands;
        
    finder.buildTracks(cll,cands);
    
    int ncands=cands.size();
    for(int ii=0;ii<ncands;++ii){
      PndTpcRiemannTrack* trk=cands.at(ii);
      if(trk->r()>10000)continue;
      mean+=trk->r()*100;
      rh->Fill(trk->r()*100);
      diph->Fill(fabs(trk->m())-0.2);
    }
  }
  mean/=(double)ntrk;  
  std::cout<<"mean="<<mean<<std::endl;

  TFile* outf=new TFile("riemannfit.root","RECREATE");
  if(!outf->IsOpen()){std::cout<<"No file"<<std::endl;return;}
  rh->Write();
  diph->Write();
  outf->Close();
}


void
PndTpcRecoTester::testRiemannFinder(unsigned int ntrk){

  std::vector<PndTpcCluster*> cll; // clusterlist
  maketoytracks(ntrk,cll,0,0.2);
  std::vector<PndTpcRiemannTrack*> cands;

  PndTpcRiemannTrackFinder finder;
  finder.setMinHitsForFit(4);
  finder.addCorrelator(new PndTpcRiemannHTCorrelator(1E-2));
  finder.addCorrelator(new PndTpcSzHTCorrelator(1.0));
  finder.addCorrelator(new PndTpcRiProxHTCorrelator(0.5));
  finder.addCorrelator(new PndTpcProximityHTCorrelator(2.));
  finder.buildTracks(cll,cands);
  
  
  unsigned int ntrks=cands.size();
  //std::vector<TPolyMarker3D*> marker;
  TFile* file=new TFile("RiemannFinder.root","RECREATE");
  TH1D* resRiem=new TH1D("resRiem","residual riemann",100,-0.005,0.005);
  TH1D* resZ=new TH1D("resZ","residual sz",100,-0.5,0.5);

  for(int i=0;i<ntrks;++i){
    PndTpcRiemannTrack* trk=cands.at(i);
    
    if(trk->getNumHits()<4)continue;
    std::cout<<"Origin";trk->orig().Print();
    std::cout<<"Normal Vector";trk->n().Print();

    //abuse TLorentzVector:
    TLorentzVector* plane=new TLorentzVector(trk->n()[0],
					     trk->n()[1],
					     trk->n()[2],
					     trk->c());

    

    trk->szFit();
    unsigned int nhits=trk->getNumHits();
    TPolyMarker3D* maker=new TPolyMarker3D(nhits);
    TPolyMarker3D* clust=new TPolyMarker3D(nhits);
    TGraph* sz=new TGraph(nhits);
    

    for(int h=0;h<nhits;++h){
      PndTpcRiemannHit* hit=trk->getHit(h);
      maker->SetPoint(h,hit->x().X(),hit->x().Y(),hit->x().Z());
      sz->SetPoint(h,hit->s(),hit->z());
      clust->SetPoint(h,hit->cluster()->pos().X(),hit->cluster()->pos().Y(),hit->cluster()->pos().Z());
      resRiem->Fill(trk->dist(hit));
      resZ->Fill(trk->szDist(hit));

    }
    char name[200];
    sprintf(name,"track%i",i);
    maker->Write(name);
    sprintf(name,"sztrack%i",i);
    sz->Write(name);
    sprintf(name,"clusters%i",i);
    clust->Write(name);
    sprintf(name,"plane%i",i);
    plane->Write(name);

  }
  resRiem->Write();
  resZ->Write();
  //DebugLogger::Instance()->WriteFiles();
  file->Close(); 
  
}

void
PndTpcRecoTester::testRiemannInit(){
  std::vector<double> rv; 
  std::vector<TVector2> dirv;
  std::vector<double> dipv;
  std::vector<TVector2> startv;
  maketoyevent(1,rv,dirv,dipv,startv);
  std::vector<PndTpcCluster*> cll; // clusterlist
  maketoyhits(1,rv,dirv,dipv,startv,cll,0);


  double r=rv[0];
  TVector2 start=startv[0];
  TVector2 dir=dirv[0];
  TVector2 radial=dir.Unit().Rotate(TMath::Pi()/2)*r;
  TVector2 origin=start+radial;
  double x0=origin.X();
  double y0=origin.Y();

  PndTpcRiemannTrack* trk=new PndTpcRiemannTrack();
  trk->init(x0,y0,r,0,0);
  trk->orig().Print();
 
  
  TFile* file=new TFile("RiemannInit.root","RECREATE");
  //abuse TLorentzVector:
  TLorentzVector* plane=new TLorentzVector(trk->n()[0],
					   trk->n()[1],
					   trk->n()[2],
					   trk->c());

    
  int nhits=cll.size();
  TPolyMarker3D* maker=new TPolyMarker3D(nhits);
  TPolyMarker3D* clust=new TPolyMarker3D(nhits);
  for(int h=0;h<nhits;++h){
    PndTpcRiemannHit* hit=new PndTpcRiemannHit(cll[h]);
    maker->SetPoint(h,hit->x().X(),hit->x().Y(),hit->x().Z());
    clust->SetPoint(h,hit->cluster()->pos().X()/30,hit->cluster()->pos().Y()/30,0);
    trk->addHit(hit);
    if(h>5){
      trk->refit();
      std::cout<<h<<" -- ";
      trk->orig().Print();
    }
  }
   trk->refit();
   trk->orig().Print();
  char name[200];
  sprintf(name,"track1");
  maker->Write(name);
  sprintf(name,"clusters1");
  clust->Write(name);
  sprintf(name,"plane1");
  plane->Write(name);
  file->Close(); 

}

#include "TROOT.h"

void 
PndTpcRecoTester::testRiemannSort(double dip, double sigma, int draw){
  std::vector<PndTpcCluster*> cll;
  std::vector<double> rv;rv.push_back(30);
  std::vector<TVector2> dirv;dirv.push_back(TVector2(1,1)); 
  std::vector<double> dipv;dipv.push_back(dip);
  std::vector<TVector2> startv;startv.push_back(TVector2(1,1));
  maketoyhits(1,rv,dirv,dipv,startv,cll,sigma);

  //maketoytracks(1,cll,false);
  
  // shuffel clusters
  //std::random_shuffle(cll.begin(),cll.end());
  std::sort(cll.begin(),cll.end(),PndTpcClusterZ());

  
  PndTpcRiemannTrack* trk=new PndTpcRiemannTrack();
  std::list<PndTpcRiemannHit*> rhits;
  int nhits=cll.size();
  for(int i=0;i<nhits;++i){
    PndTpcRiemannHit* hit=new PndTpcRiemannHit(cll[i]);
    TVector3 posX=cll[i]->pos();
    trk->resetNit();
    std::list<PndTpcRiemannHit*>::iterator it;
    if(rhits.size()<2)rhits.push_back(hit);
    else {
      // check if we start at end or at beginning:
      std::list<PndTpcRiemannHit*>::iterator it2=rhits.end();
      --it2;
      TVector3 posend=(*it2)->cluster()->pos();
      std::list<PndTpcRiemannHit*>::iterator it1=rhits.begin();
      TVector3 posstart=(*it1)->cluster()->pos();
      double s1x=(posX-posstart).Perp();
      double s2x=(posX-posend).Perp();
      if(s1x>s2x){// hit closer to s2
	trk->insertHit(rhits,hit,trk->sortHit(hit,rhits,-1));
      }
      else {
	trk->insertHit(rhits,hit,trk->sortHit(hit,rhits,1));
      }
    }
    if(i<draw){
      TCanvas* c=new TCanvas("c");
      TPolyMarker3D* maker=new TPolyMarker3D(rhits.size());
      TPolyLine3D* line=new TPolyLine3D(rhits.size());
      TPolyMarker3D* point=new TPolyMarker3D(1);
      point->SetPoint(0,posX.X(),posX.Y(),posX.Z());
      point->SetMarkerColor(kRed);
      point->SetMarkerStyle(23);
      std::list<PndTpcRiemannHit*>::iterator It=rhits.begin();
      int count=0;
      while(It!=rhits.end()){
	TVector3 pos=(*It)->cluster()->pos();
	//pos.Print();
	maker->SetPoint(count,pos.X(),pos.Y(),pos.Z());
	line->SetPoint(count,pos.X(),pos.Y(),pos.Z());
	++It;
	++count;
      }
      maker->SetMarkerStyle(23);
      maker->Draw();
      line->Draw();
      point->Draw();
      gApplication->SetReturnFromRun(kTRUE);
      gSystem->Run();
      delete maker;
      delete line;
      delete point;
      delete c;
      
    }
  }
  
  
  
  TPolyMarker3D* maker=new TPolyMarker3D(rhits.size());
  TPolyLine3D* line=new TPolyLine3D(rhits.size());
  std::list<PndTpcRiemannHit*>::iterator it=rhits.begin();
  int count=0;
  while(it!=rhits.end()){
    TVector3 pos=(*it)->cluster()->pos();
    //pos.Print();
    maker->SetPoint(count,pos.X(),pos.Y(),pos.Z());
    line->SetPoint(count,pos.X(),pos.Y(),pos.Z());
    ++it;
    ++count;
  }
  
  TCanvas* c=NULL;
  if(draw>=0){
    c=new TCanvas("c");
    maker->SetMarkerStyle(23);
    maker->Draw();
    line->Draw();
    gApplication->SetReturnFromRun(kTRUE);
    gSystem->Run();
  }
  
  
  TPolyLine3D* rawmaker=new TPolyLine3D(cll.size());
  std::vector<PndTpcCluster*>::iterator it2=cll.begin();
  count=0;
  
  std::cout<<" ****************** clusters: ***************** "<<std::endl;

  while(it2!=cll.end()){
    TVector3 pos=(*it2)->pos();
    //pos.Print();
    rawmaker->SetPoint(count++,pos.X(),pos.Y(),pos.Z());
    ++it2;
  }


  TFile* file=new TFile("RiemannSort.root","RECREATE");
  //maker->SetName("clusters");
  maker->Write("sorted");
  rawmaker->Write("clusters");
  file->Close();
 if(c!=NULL)delete c;
}
  
void 
PndTpcRecoTester::testRiemannSortBatch(int ntrk,double sigma){
  TH1I* histo=new TH1I("histo","distance from original index",20,-10,10);

  for(int i=0; i<ntrk; ++i){ // loop over tracks
    std::vector<PndTpcCluster*> cll;
    maketoytracks(1,cll,false,sigma);
    
    // make a copy:
    std::vector<PndTpcCluster*> cllorig(cll);
    
    // shuffel clusters
    //std::random_shuffle(cll.begin(),cll.end());
    std::sort(cll.begin(),cll.end(),PndTpcClusterZ());
    
    
    PndTpcRiemannTrack* trk=new PndTpcRiemannTrack();
    std::list<PndTpcRiemannHit*> rhits;
    int dir=0;
    int nhits=cll.size();
    for(int ii=0;ii<nhits;++ii){
      PndTpcRiemannHit* hit=new PndTpcRiemannHit(cll[ii]);
      TVector3 posX=cll[ii]->pos();
      trk->resetNit();
      std::list<PndTpcRiemannHit*>::iterator it;
      if(rhits.size()<2)rhits.push_back(hit);
      else {
	// check if we start at end or at beginning:
	std::list<PndTpcRiemannHit*>::iterator it2=rhits.end();
	--it2;
	TVector3 posend=(*it2)->cluster()->pos();
	std::list<PndTpcRiemannHit*>::iterator it1=rhits.begin();
	TVector3 posstart=(*it1)->cluster()->pos();
	double s1x=(posX-posstart).Perp();
	double s2x=(posX-posend).Perp();
	if(s1x>s2x){// hit closer to s2
	  trk->insertHit(rhits,hit,trk->sortHit(hit,rhits,-1));
	  dir=-1;
	}
	else {
	  dir=1;
	  trk->insertHit(rhits,hit,trk->sortHit(hit,rhits,1));
	}
      }
    }
    
    
    // now we compute the distance to the original position in the track
    
    std::list<PndTpcRiemannHit*>::iterator it=rhits.begin();
    int count=0;
    while(it!=rhits.end()){
      const PndTpcCluster* clus=(*it)->cluster();
      int index1=std::distance(cllorig.begin(),std::find(cllorig.begin(),cllorig.end(),clus));
      int index2=std::distance(std::find(cllorig.begin(),cllorig.end(),clus),cllorig.end());
      int d= dir>0 ? index1-count : index2-count;
      histo->Fill(d-1);
      ++it;
      ++count;
    }
    //PlotRiemannHits(rhits);
    rhits.clear();
    cll.clear();
    cllorig.clear();
    delete trk;
  } //end loop over tracks;
  TCanvas* c=new TCanvas("c");
  histo->Draw();
  gApplication->SetReturnFromRun(kTRUE);
  gSystem->Run();
  
}

// ***********************************************************************
// *************** Private methods ***************************************


void
PndTpcRecoTester::PlotRiemannHits(std::list<PndTpcRiemannHit*>& rhits){
  TCanvas* c=new TCanvas("c");
  TPolyMarker3D* maker=new TPolyMarker3D(rhits.size());
  TPolyLine3D* line=new TPolyLine3D(rhits.size());
  std::list<PndTpcRiemannHit*>::iterator it=rhits.begin();
  int count=0;
  while(it!=rhits.end()){
    TVector3 pos=(*it)->cluster()->pos();
    //pos.Print();
    maker->SetPoint(count,pos.X(),pos.Y(),pos.Z());
    line->SetPoint(count,pos.X(),pos.Y(),pos.Z());
    ++it;
    ++count;
  }
  maker->SetMarkerStyle(23);
  maker->Draw();
  line->Draw();
  gApplication->SetReturnFromRun(kTRUE);
  gSystem->Run();
  delete maker;
  delete line;
  delete c;
}



void 
PndTpcRecoTester::maketoyevent(int n,std::vector<double>& rv,std::vector<TVector2>& dirv, std::vector<double>& dipv, std::vector<TVector2>& startv){
  bool rflag=false;
  bool dipflag=false;
  if(rv.size()!=0)rflag=true;
  if(dipv.size()!=0)dipflag=true;
  for(int i=0;i<n;++i){
    double r=gRandom->Uniform(30,30);
    double dirstartx;
    double dirstarty;
    gRandom->Circle(dirstartx,dirstarty,15);
    TVector2 dir(dirstartx,dirstarty);
    double dz_dphi=gRandom->Uniform(0.2,10.);

    double startr=gRandom->Uniform(0,40);
    double startx; double starty;
    gRandom->Circle(startx,starty,startr);
    TVector2 start(startx,starty);
    std::cout<<"Track: r="<<r<<"  dz/dphi="<<dz_dphi
	     <<"   dz/ds="<<dz_dphi/r<<" dir=("
	     <<dir.X()<<","<<dir.Y()<<")  start=("
	     <<start.X()<<","<<start.Y()<<")"<<std::endl;
    if(!rflag)rv.push_back(r);
    dirv.push_back(dir);
    if(!dipflag)dipv.push_back(dz_dphi);
    startv.push_back(start);
  }
}


void
PndTpcRecoTester::maketoyhits(int ntrk,std::vector<double>& rv,
			      std::vector<TVector2>& dirv, 
			      std::vector<double>& dipv,
			      std::vector<TVector2>& startv,
			      std::vector<PndTpcCluster*>& cll,
			      double sigma){
  //produce hits
  TGraph* hits=new TGraph();
  TGraph* confhits=new TGraph();
  TGraph* hitsxz=new TGraph();
  TGraph* confhits_shift=new TGraph();

  unsigned int hitcounter=0;
  double sig=sigma; // sigma in space

  for(int t=0;t<ntrk;++t){// track loop
    
    unsigned int hitsintrk=0;

    double r=rv[t];
    std::cout<<"Radius r="<<r<<std::endl;
    TVector2 start=startv[t];
    TVector2 dir=dirv[t];
    TVector2 radial=dir.Unit().Rotate(TMath::Pi()/2)*r;
    TVector2 origin=start+radial;
    std::cout<<"Origin("<<origin.X()<<","<<origin.Y()<<")"<<std::endl;
    
    double dz_dphi=dipv[t];
    double dz_ds=dz_dphi/r;
    double ds=0.5;
    double dz=dz_ds*ds;
    double dphir2=ds*ds-dz*dz;
    double dphi=TMath::Sqrt(dphir2)/r;
    double rot=gRandom->Uniform(-1,1)>0 ? 1. : -1.;
    std::cout<<"rot="<<rot<<std::endl;
    std::cout<<"sigma="<<sigma<<std::endl;
    double z0=gRandom->Uniform(-10,10);

    for(int i=1; i<70; ++i){// hit loop 
      double phi=i*dphi;
      TVector2 point=origin-radial.Rotate(phi*rot);
      double xhit=point.X()+gRandom->Gaus(0,sig);
      double yhit=point.Y()+gRandom->Gaus(0,sig);
      double zhit=phi*dz_dphi+gRandom->Gaus(0,sig)+z0;
      double rr=TMath::Sqrt(xhit*xhit+yhit*yhit);
      //if(rr<5) continue;
      hits->SetPoint(hitcounter,xhit,yhit);
      hitsxz->SetPoint(hitcounter,xhit,zhit);
      PndTpcCluster* cl=new PndTpcCluster(TVector3(xhit,yhit,zhit),1,hitcounter);
      //PndTpcConfMapRecoHit conf(cl);
      //conf.setRotated();
      cll.push_back(cl);
      //confhits->SetPoint(hitcounter,conf.getXcf(),conf.getHitCoord(GFDetPlane())[0][0]);
      //conf.setReferencePoint(start.X(),start.Y());
      //confhits_shift->SetPoint(hitcounter,conf.getXcf(),conf.getHitCoord(GFDetPlane())[0][0]);
      ++hitcounter;
      ++hitsintrk;
    }
    //std::cout<<hitsintrk<<" hits in track"<<t<<std::endl;
  }
  TFile* outfile=new TFile("toyhits.root","RECREATE");
  hits->Write("hits");
  hitsxz->Write("hitsxz");
  confhits->Write("confhits");
  confhits_shift->Write("confhits2");
  outfile->Close();

}

void
PndTpcRecoTester::maketoytracks(int n,std::vector<PndTpcCluster*>& cll, bool vertex, double sigma)
{
  // make events
  std::vector<double> rv;
  std::vector<TVector2> dirv;
  std::vector<double> dipv;
  std::vector<TVector2> startv;
  maketoyevent(n,rv,dirv,dipv,startv);
  if(vertex){
    for(int i=0;i<startv.size();++i)(startv[i]).Set(0.0,0.0);
  }
  maketoyhits(n,rv,dirv,dipv,startv,cll,sigma);
}

//-------------------------------------------------------------
