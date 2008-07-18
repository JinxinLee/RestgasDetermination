#include <vector>
#include <iostream>
#include <algorithm>

// Collaborating Class Headers --------
#include "TRandom.h"
#include "TMath.h"
#include "TPolyLine3D.h"
#include "TPolyMarker3D.h"
#include "TCanvas.h"
#include "TView.h"
#include "TApplication.h"


#include "MCHitGenerator.h"
#include "Track.h"
#include "LSLTrackRep.h"
#include "TpcRecoHit.h"
#include "Slt2DHit.h"
#include "Kalman.h"
#include "statTree.h"
#include "statTreeAnalyzer.h"


// Class Member definitions -----------


void buildTracks(int ntracks, std::vector<Track*>& tracklist, std::vector< TMatrixT<double> >& MCtruth){
  MCHitGenerator gen;
  for(int i=0; i<ntracks;++i){
    
    LSLTrackRep* rep=new LSLTrackRep(0,10,10,
		    .1,0.,1./0.3,
		    1.,1.,1.,1.,1.);
    LSLTrackRep repstart(*rep);

    Track* trk=new Track(rep);
    double dz=50.;
    double z=0;
    for(int ihit=1; ihit<12; ++ihit){
      
      //if(ihit<150)z+=dz;
      //else z-=dz;
      z+=dz;
      TMatrixT<double> pred(5,1);
      rep->extrapolate(z,pred);
      rep->setState(pred);
      rep->setS(z);
      double x=pred[0][0]; double y=pred[1][0];
      //std::cout<< "x="<<x<<" y="<<y<<std::endl;
      // smear
      x+=gRandom->Gaus(0.,0.1);
      y+=gRandom->Gaus(0.,0.1);
      //double r=TMath::Sqrt(x*x+y*y);
      //double phi=TMath::ATan2(y,x);
      Slt2DHit* hit=new Slt2DHit(x,y,z,0.1,0.1);
      trk->addHit(hit);
    }
    // set back to starting point:
    *rep=repstart;
    rep->setS(0);

    // smear starting values
    
    TMatrixT<double> start=rep->getState();
    double dx=start[2][0];
    double dy=start[3][0];
    dx+=0.2*dx;
    dy+=0.2*dy;
    start[2][0]=dx;
    start[3][0]=dy;
    start[4][0]*=1.1;
    rep->setState(start);

    

    rep->setStartState(rep->getState());
    rep->setStartCov(rep->getCov());
    rep->setStartS(rep->getS());

    tracklist.push_back(trk);
  }
  return;
}


void drawTracks(std::vector<Track*> tracks,int argc, char **argv) {
  TApplication theApp("App",&argc,argv);
  TCanvas *c1 = new TCanvas("c1");
  TView *view = new TView(1);

  view->ShowAxis();
  c1->Draw();
  double x,y,r,phi,z,minx,maxx,miny,maxy,minz,maxz;
  minx=100000.;
  miny=100000.;
  minz=100000.;
  maxx=-100000.;
  maxy=-100000.;
  maxz=-100000.;


  for(int i=0;i<tracks.size();i++) {
    std::cout << "Drawing track #" << i << std::endl;

    int nhits = tracks[i]->getNumHits();
    TPolyLine3D *l = new TPolyLine3D(nhits);
    TPolyMarker3D *points = new TPolyMarker3D((int)nhits/2);
    TPolyMarker3D *points2 = new TPolyMarker3D((int)nhits/2);
    points->SetMarkerStyle(24);
    points2->SetMarkerStyle(24);
    points2->SetMarkerColor(kRed);
    for(int ihits=0;ihits<nhits;ihits++) {
      //std::cout << "\thit #" << ihits << std::endl;
      AbsRecoHit* theHit = tracks[i]->getHit(ihits);
      z = theHit->getS();
      //phi = (theHit->getHitCoord())[0][0]/r;
      //z = (theHit->getHitCoord())[1][0];
      x = (theHit->getHitCoord())[0][0];
      y = (theHit->getHitCoord())[1][0];
      if(x<minx) minx = x;
      if(y<miny) miny = y;
      if(z<minz) minz = z;
      if(x>maxx) maxx = x;
      if(y>maxy) maxy = y;
      if(z>maxz) maxz = z;
      std::cout <<"(x,y,z) = ("<<x<<","<<y<<","<<z<<")"<<std::endl;
      //std::cout <<"(r,phi,z) = ("<<r<<","<<phi<<","<<z<<")"<<std::endl;
      l->SetPoint(ihits,x,y,z);
      if(ihits<nhits/2)points->SetPoint(ihits,x,y,z);
      else points2->SetPoint(ihits,x,y,z);
      
    }
    std::cout << "calling Draw() method" << std::endl;
    view->SetRange(minx,miny,minz,maxx,maxy,maxz);
    l->Draw();
    points->Draw("same");
    points2->Draw("same");
    theApp.Run(kTRUE);    
    std::string dummy;
    //    cin >> dummy;
    delete l;
    std::cout << "++++++++++++++++++++++" << std::endl;
  }
}



void drawHitsAndTracks(std::vector<Track*> tracks,int argc, char **argv) {
  TApplication theApp("App",&argc,argv);
  double x,y,r,phi,z,minx,maxx,miny,maxy,minz,maxz;
  minx=100000.;
  miny=100000.;
  minz=100000.;
  maxx=-100000.;
  maxy=-100000.;
  maxz=-100000.;


  MCHitGenerator gen;
  MCHitGenerator genStart;

  for(int i=0;i<tracks.size();i++) {
    std::cout << "Drawing track #" << i << std::endl;


	TCanvas *c1 = new TCanvas("c1");
	TView *view = new TView(1);
	
	view->ShowAxis();
	c1->Draw();


    int nhits = tracks[i]->getNumHits();
	LSLTrackRep* rep = (LSLTrackRep*) tracks[i]->getTrackRep(0);

    TPolyLine3D *stateTrack = new TPolyLine3D(nhits);
    TPolyLine3D *startStateTrack = new TPolyLine3D(nhits);


    TMatrixT<double> state(5,1);
    LSLTrackRep fittedTrack(*rep);
    fittedTrack.setState(rep->getState());
    fittedTrack.setS(rep->getS());
    fittedTrack.setCov(rep->getCov());
    //extrapolate to start
    fittedTrack.extrapolate(rep->getStartS(),state);
    fittedTrack.setState(state);
    fittedTrack.setS(rep->getStartS());
    //std::cout<<"extrapolating fitted to start"<<std::endl;
    state.Print();
    
    LSLTrackRep startTrack(*rep);
    startTrack.setState(rep->getStartState());
    startTrack.setS(rep->getStartS());
    startTrack.setCov(rep->getStartCov());
	

    TPolyMarker3D *points = new TPolyMarker3D(nhits);
    points->SetMarkerStyle(24);
    for(int ihits=0;ihits<nhits;ihits++) {

      //std::cout << "\thit #" << ihits << std::endl;
      AbsRecoHit* theHit = tracks[i]->getHit(ihits);
      z = theHit->getS();
      x = (theHit->getHitCoord())[0][0];
      y = (theHit->getHitCoord())[1][0];
      if(x<minx) minx = x;
      if(y<miny) miny = y;
      if(z<minz) minz = z;
      if(x>maxx) maxx = x;
      if(y>maxy) maxy = y;
      if(z>maxz) maxz = z;
      //std::cout <<"(x,y,z) = ("<<x<<","<<y<<","<<z<<")"<<std::endl;
      points->SetPoint(ihits,x,y,z);


      // create fitted track
     
      fittedTrack.extrapolate(z,state);
      //std::cout<<state[0][0]<<"   "<<state[1][0]<<std::endl;
      stateTrack->SetPoint(ihits,state[0][0],state[1][0],z);
      fittedTrack.setState(state);
      fittedTrack.setS(z);
	  


      // create start value track
      startTrack.extrapolate(z,state);
      startTrack.setState(state);
      startTrack.setS(z);
      startStateTrack->SetPoint(ihits,state[0][0],state[1][0],z);
	  
    }
    std::cout << "calling Draw() method" << std::endl;
    view->SetRange(minx,miny,minz,maxx,maxy,maxz);
	view->Centered();
	view->Top();
	stateTrack->SetLineWidth(5);
	stateTrack->SetLineColor(kGreen);
	startStateTrack->SetLineWidth(2);
	startStateTrack->SetLineColor(kRed);
	startStateTrack->SetLineStyle(2);
	stateTrack->Draw();
	startStateTrack->Draw();
    points->Draw();
    points->Print();
    stateTrack->Print();
    startStateTrack->Print();
    theApp.Run(kTRUE);    
    
    delete stateTrack;
    delete startStateTrack;
	delete points;
	delete view;
	delete c1;
    std::cout << "++++++++++++++++++++++" << std::endl;
  }
}










int main(int argc, char **argv){

  // create tracks
  std::vector<Track*> tracks;
  std::vector< TMatrixT<double> > MCtruth;
  buildTracks(1,tracks,MCtruth);
  std::cout<<tracks.size()<<" tracks built"<<std::endl;


  //drawTracks(tracks,argc,argv);  


  std::cout << "fitting tracks" << std::endl;
  std::for_each(tracks.begin(),tracks.end(),Kalman());

  drawHitsAndTracks(tracks,argc,argv);

  std::cout << "making stat tree" << std::endl;

/*
  makeStatTree(MCtruth,tracks,"statTree","statTree.root");
  std::cout << "analyzing stat tree" << std::endl;
  statTreeAnalyzer s("statTree","statTree.root","fitStats.root");
  s.Loop();
  */


  return 0;
}
