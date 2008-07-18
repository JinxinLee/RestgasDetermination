//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      a test application for straightline fits with the kalman filter
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


// C/C++ Headers ----------------------
#include <vector>
#include <iostream>
#include <algorithm>

// Collaborating Class Headers --------
#include "TRandom.h"
#include "MCHitGenerator.h"
#include "Track.h"
#include "SLTrackRep.h"
#include "SL2DTrackRep.h"
#include "SltHit.h"
#include "Slt2DHit.h"
#include "Kalman.h"
#include "statTree.h"
#include "statTreeAnalyzer.h"

#include "TFile.h"
#include "TH1D.h"


// Class Member definitions -----------

void buildTracksPixelsAndStrips(int ntracks, std::vector<Track*>& tracklist, std::vector< TMatrixT<double> >& MCtruth){
  MCHitGenerator gen;
  for(int i=0; i<ntracks;++i){
    double ax=gRandom->Uniform(-3,3);
    double bx=gRandom->Uniform(-3,3);
    double ay=gRandom->Uniform(-3,3);
    double by=gRandom->Uniform(-3,3);

    //save the track params for later residual calculations
    TMatrixT<double> aVector(4,1);
    aVector[0][0] = ax;
    aVector[1][0] = bx;
    aVector[2][0] = ay;
    aVector[3][0] = by;

    MCtruth.push_back(aVector);

    double ax_off, bx_off;
    double ay_off, by_off;
    ax_off = gRandom->Uniform(-0.5,0.5);
    bx_off = gRandom->Uniform(-0.5,0.5);
    ay_off = gRandom->Uniform(-0.5,0.5);
    by_off = gRandom->Uniform(-0.5,0.5);
    SL2DTrackRep* rep=new SL2DTrackRep(ax+ax_off,bx+bx_off,ay+ay_off,by+by_off,0.2,0.2,0.2,0.2);
    Track* trk=new Track(rep);

    // generate hits in pixel detectors
    double dz=4.; // z-spacing
    double dx=0.2; // x resolution
    double dy=0.2; // y resolution
    for(int ihit=0; ihit<10; ++ihit){
      double z=ihit*dz;
      Slt2DHit* hit=gen.generateSL2D(z,ax,bx,ay,by,dx,dy);
      trk->addHit(hit);
    }

    // generate hits in x-strip detectors detectors
    double zoffset = 1.;
    dx = 0.02;
    for(int ihit=0; ihit<10; ++ihit){
      double z=ihit*dz+zoffset;
      SltHit* hit=gen.generateSL1D(z,ax,bx,dx);
      trk->addHit(hit);
    }


    tracklist.push_back(trk);
  }
  return;
}


void buildTracks2D(int ntracks, std::vector<Track*>& tracklist, std::vector< TMatrixT<double> >& MCtruth){
  MCHitGenerator gen;
  for(int i=0; i<ntracks;++i){
    double ax=gRandom->Uniform(-3,3);
    double bx=gRandom->Uniform(-3,3);
    double ay=gRandom->Uniform(-3,3);
    double by=gRandom->Uniform(-3,3);

    //save the track params for later residual calculations
    TMatrixT<double> aVector(4,1);
    aVector[0][0] = ax;
    aVector[1][0] = bx;
    aVector[2][0] = ay;
    aVector[3][0] = by;

    MCtruth.push_back(aVector);

    double ax_off, bx_off;
    double ay_off, by_off;
    ax_off = gRandom->Uniform(-0.2,0.2);
    bx_off = gRandom->Uniform(-0.5,0.5);
    ay_off = gRandom->Uniform(-0.5,0.5);
    by_off = gRandom->Uniform(-0.5,0.5);
    SL2DTrackRep* rep=new SL2DTrackRep(ax+ax_off,bx+bx_off,ay+ay_off,by+by_off,0.2,0.2,0.2,0.2);
    Track* trk=new Track(rep);
    // generate hits 
    double dz=4.; // z-spacing
    double dx=0.2; // x resolution
    double dy=0.2; // y resolution
    for(int ihit=0; ihit<10; ++ihit){
      double z=ihit*dz;
      Slt2DHit* hit=gen.generateSL2D(z,ax,bx,ay,by,dx,dy);
      trk->addHit(hit);
    }
    tracklist.push_back(trk);
  }
  return;
}


void buildTracks(int ntracks, std::vector<Track*>& tracklist, std::vector< TMatrixT<double> >& MCtruth){
  MCHitGenerator gen;
  for(int i=0; i<ntracks;++i){
    double a=gRandom->Uniform(-3,3);
    double b=gRandom->Uniform(-3,3);

    //save the track params for later residual calculations
    TMatrixT<double> aVector(2,1);
    aVector[0][0] = a;
    aVector[1][0] = b;
    MCtruth.push_back(aVector);

    double a_off, b_off;
    a_off = gRandom->Uniform(-0.5,0.5);
    b_off = gRandom->Uniform(-0.5,0.5);
    SLTrackRep* rep=new SLTrackRep(a+a_off,b+b_off,0.2,0.2);
    Track* trk=new Track(rep);
    // generate hits 
    double dz=4.; // z-spacing
    double dy=0.2; // y resolution
    for(int ihit=0; ihit<10; ++ihit){
      double z=ihit*dz;
      SltHit* hit=gen.generateSL1D(z,a,b,dy);
      trk->addHit(hit);
    }
    tracklist.push_back(trk);
  }
  return;
}


int main(){
  // create tracks
  std::vector<Track*> tracks;
  std::vector< TMatrixT<double> > MCtruth;
  buildTracks2D(10000,tracks,MCtruth);
  //buildTracksPixelsAndStrips(10000,tracks,MCtruth);
  std::cout<<tracks.size()<<" tracks built"<<std::endl;
  std::cout << "fitting tracks" << std::endl;
  std::for_each(tracks.begin(),tracks.end(),Kalman());


  std::cout << "making stat tree" << std::endl;
  makeStatTree(MCtruth,tracks,"statTree","statTree.root");
  std::cout << "analyzing stat tree" << std::endl;
  statTreeAnalyzer s("statTree","statTree.root","fitStats.root");
  s.Loop();

  return 0;
}
