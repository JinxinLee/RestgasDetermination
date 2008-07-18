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
#include "AliceHelixTrackRep.h"
#include "TpcRecoHit.h"
#include "Kalman.h"
#include "statTree.h"
#include "statTreeAnalyzer.h"
#include "AliceHelixTrackRep.h"

// Class Member definitions -----------


void buildTracks(int ntracks, std::vector<Track*>& tracklist, std::vector< TMatrixT<double> >& MCtruth){
  MCHitGenerator gen;
  for(int i=0; i<ntracks;++i){

    TMatrixT<double> aState(5,1);
	
    //phi
    aState[0][0] = gRandom->Uniform(5. /180. * TMath::Pi(),350. /180. * TMath::Pi());
    //z in meters

	aState[1][0] = gRandom->Uniform(1.e-2,2.e-2);
	//	std::cout << aState[1][0] << std::endl;
    //lambda
    aState[2][0] = gRandom->Uniform(20. /180. * TMath::Pi(),70. /180. * TMath::Pi());
    //C=1/(2*R) in meters^-1
    aState[3][0] = gRandom->Uniform(0.5 / 0.5,0.5 / 0.4);

    //D0 in meters
    aState[4][0] = gRandom->Uniform(1.e-5,2.e-4);
	

    //save the track params for later residual calculations


    MCtruth.push_back(aState);
    TMatrixT<double> cov(5,5);
    cov[0][0] = 50.*1. /180. * TMath::Pi();
    cov[0][0] *= cov[0][0];
    cov[1][1] = 50.*5.e-3;
    cov[1][1] *= cov[1][1];
    cov[2][2] = 50.*1. /180. * TMath::Pi();
    cov[2][2] *= cov[2][2];
    cov[3][3] = 50.*0.01*aState[3][0];
    cov[3][3] *= cov[3][3];
    cov[4][4] = 50.*1. /180. * TMath::Pi();
    cov[4][4] *= cov[4][4];
    
    // offdiagonals
    //cov[0][4]=-cov[0][0];
    //cov[1][3]=-cov[1][1];
    //cov[3][1]=-cov[3][3];
    //cov[4][0]=-cov[4][4];

    //cov.Print();

	
    double phi_off    = gRandom->Uniform(-20. /180. * TMath::Pi(),20. /180. * TMath::Pi());
    double z_off      = gRandom->Uniform(-2.e-2,2.e-2);
    double lambda_off = gRandom->Uniform(-10. /180. * TMath::Pi(),10. /180. * TMath::Pi());
    //Kruemmungskreis-Radius
    double R_off      = gRandom->Uniform(-0.05,0.05);
    double D0_off     = gRandom->Uniform(-1.e-4,1.e-4);
	

    
	
	TMatrixT<double> startState = aState;
	
	startState[0][0] += phi_off;
	startState[1][0] += z_off;
	startState[2][0] += lambda_off;
	startState[3][0]  = 0.5/(0.5/aState[3][0]+R_off);
	startState[4][0] += D0_off;
    
	



    // generate hits 
    double rSpacing= 0.02; // r-spacing
    double rOffset = 0.05;

    double dr = 1.e-10; 
    double dphi = 1./180. * TMath::Pi(); 
    double dz = 5.e-3; 

    AliceHelixTrackRep* rep=new AliceHelixTrackRep(rOffset-1.e-7,startState,cov);

    Track* trk=new Track(rep);

    gen.initHelix(rOffset-1.e-7,
				  aState,
				  cov);
    
    for(int ihit=0; ihit<20; ++ihit){
      double r=(ihit*rSpacing) + rOffset;;
      TpcRecoHit* hit=gen.generateTPC(r,dr,dphi,dz);
      trk->addHit(hit);
    }
    tracklist.push_back(trk);
  }
  return;
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
	AliceHelixTrackRep* rep = (AliceHelixTrackRep*)tracks[i]->getTrackRep(0);

    TPolyLine3D *stateTrack = new TPolyLine3D(nhits);
    TPolyLine3D *startStateTrack = new TPolyLine3D(nhits);

    gen.initHelix(rep->getS(),
				  rep->getState(),
				  rep->getCov()
				  );

    genStart.initHelix(rep->getStartS(),
				  rep->getStartState(),
				  rep->getStartCov()
				  );
	

    TPolyMarker3D *points = new TPolyMarker3D(nhits);
    points->SetMarkerStyle(24);
    for(int ihits=0;ihits<nhits;ihits++) {

      std::cout << "\thit #" << ihits << std::endl;
      AbsRecoHit* theHit = tracks[i]->getHit(ihits);
      r = theHit->getS();
      phi = (theHit->getHitCoord())[0][0]/r;
      z = (theHit->getHitCoord())[1][0];
      x = r*cos(phi);
      y = r*sin(phi);
      if(x<minx) minx = x;
      if(y<miny) miny = y;
      if(z<minz) minz = z;
      if(x>maxx) maxx = x;
      if(y>maxy) maxy = y;
      if(z>maxz) maxz = z;
      std::cout <<"(x,y,z) = ("<<x<<","<<y<<","<<z<<")"<<std::endl;
      std::cout <<"(r,phi,z) = ("<<r<<","<<phi<<","<<z<<")"<<std::endl;
      points->SetPoint(ihits,x,y,z);


      TpcRecoHit* hitTR=gen.generateTPC(r,1.e-10,1.e-10,1.e-10);
      double rTR = hitTR->getS();
      double phiTR = (hitTR->getHitCoord())[0][0]/rTR;
      double zTR = (hitTR->getHitCoord())[1][0];
      double xTR = rTR*cos(phiTR);
      double yTR = rTR*sin(phiTR);
      stateTrack->SetPoint(ihits,xTR,yTR,zTR);
	  delete hitTR;


      hitTR=genStart.generateTPC(r,1.e-10,1.e-10,1.e-10);
      rTR = hitTR->getS();
	  phiTR = (hitTR->getHitCoord())[0][0]/rTR;
	  zTR = (hitTR->getHitCoord())[1][0];
	  xTR = rTR*cos(phiTR);
	  yTR = rTR*sin(phiTR);
      startStateTrack->SetPoint(ihits,xTR,yTR,zTR);
	  delete hitTR;
    }
	double globMin = minx;
	double globMax = maxx;
	if(miny < globMin) globMin = miny;
	if(minz < globMin) globMin = minz;
	if(maxy > globMax) globMax = maxy;
	if(maxz > globMax) globMax = maxz;
    std::cout << "calling Draw() method" << std::endl;
	//    view->SetRange(minx,miny,minz,maxx,maxy,maxz);
    view->SetRange(globMin,globMin,globMin,globMax,globMax,globMax);
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
  buildTracks(20,tracks,MCtruth);
  std::cout<<tracks.size()<<" tracks built"<<std::endl;




  /*
  tracks[0]->getTrackRep(0)->Print();      
  tracks[1]->getTrackRep(0)->Print();      
  */


  std::cout << "fitting tracks" << std::endl;

  std::cout << "======================================================" << std::endl;
  std::cout << "======================================================" << std::endl;
  std::cout << "======================================================" << std::endl;

  std::for_each(tracks.begin(),tracks.end(),Kalman());





  std::cout << "======================================================" << std::endl;
  std::cout << "======================================================" << std::endl;
  std::cout << "======================================================" << std::endl;
  drawHitsAndTracks(tracks,argc,argv);
  std::cout << "making stat tree" << std::endl;
  makeStatTree(MCtruth,tracks,"statTree","statTree.root");
  std::cout << "analyzing stat tree" << std::endl;
  statTreeAnalyzer s("statTree","statTree.root","fitStats.root");
  s.Loop();




  return 0;
}
