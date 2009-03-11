//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class GenfitTester
//      see GenfitTester.hh for details
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
#include "GenfitTester.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <assert.h>

#include "TVector3.h"
#include "TPolyMarker3D.h"
#include "TFile.h"
#include "TRandom.h"

// Collaborating Class Headers --------
#include "LSLTrackRep.h"
#include "TestRecoHit.h"
#include "Track.h"
#include "DetPlane.h"
#include "trackProximity.h"
#include "VirtSpacePointRecoHit.h"
// Class Member definitions -----------

GenfitTester::GenfitTester()
{}

GenfitTester::~GenfitTester()
{}


void
GenfitTester::test_RecoHit(){
  TestRecoHit hit(2,2,10);
}

void
GenfitTester::test_locTrkRep(){
  std::cout<<"*** test local track rep"<<std::endl;
  
  LSLTrackRep trkRep(0,1,1,0.1,0,2,0,0,0,0,0,NULL);
  trkRep.Print();
  TMatrixT<double> bla(5,1);
  DetPlane p(TVector3(2,3,150),TVector3(1,0,0),TVector3(0,1,0));
  trkRep.extrapolate(p,bla);
  bla.Print();
  
  TestRecoHit hit(2,2,10);

  std::cout<<"hit in coordinate system of track:"<<std::endl;
  
  hit.getHitCoord(p).Print();

  // Calculate Residual
  TMatrixT<double> res=hit.residualVector(&trkRep,bla);
  res.Print();
    
}

//-------------------------------------------------
void
GenfitTester::test_DetPlane(){
 std::cout<<"*** test DetPlane"<<std::endl;
 TVector3 o(0,0,1);
 TVector3 u(1,0,0);
 TVector3 v(1,1,0);
 DetPlane myplane(o,u,v);
 myplane.Print();
 
 TVector3 bla(2.3,8,3);
 std::cout<<"bla ...  and its projection"<<std::endl;
 bla.Print();
 myplane.project(bla).Print();

 myplane.dist(bla).Print();
 bla*=-1.;

 myplane.dist(bla).Print();

 o.SetZ(-1);
 myplane.setO(o);
 myplane.Print();
 
 TVector3 d=myplane.dist(bla);
 d.Print();
 
 TVector2 blaPlane=myplane.LabToPlane(bla);
 TVector3 blaprim=myplane.toLab(blaPlane);
 TVector3 blaback=blaprim-d;

 std::cout<<"bla ... bla in plane ... bla in lab again"<<std::endl;
 bla.Print();
 blaPlane.Print();
 blaback.Print();

 TVector3 k=o-bla;
 TVector3 s=blaprim;
 std::cout<<"d ... k ... s ... k+s"<<std::endl;
 d.Print();
 k.Print();
 s.Print();
 (k+s).Print();


 // just create some planes and let sane check them
 double x,y,z;
 for(int i=1; i<100; ++i){
   gRandom->Sphere(x,y,z,gRandom->Uniform(1,10));
   TVector3 ao(x,y,z);
   gRandom->Sphere(x,y,z,1);
   TVector3 au(x,y,z);
   gRandom->Sphere(x,y,z,1);
   TVector3 av(x,y,z);
   DetPlane aplane(ao,au,av);
   aplane.Print();

   gRandom->Sphere(x,y,z,gRandom->Uniform(1,20));
   TVector3 p(x,y,z);
   
   

   // check dist
   TVector3 d=aplane.dist(p);
   TVector3 pinplane=aplane.toLab(aplane.LabToPlane(p));
   TVector3 pback=pinplane-d;
   std::cout<<"Check dist: "<<std::endl;
   (p-pback).Print();
   assert((p-pback).Mag()<1E-4);
  
 }



}


//------------------------------------------------

void
GenfitTester::test_LSLTrkRep(double accuracy, bool adaptive){
  std::cout<<"*** test LSL track rep"<<std::endl;
  double z0=200;
  LSLTrackRep* rep=new LSLTrackRep(z0,1,1,0.001,1,5,0,0,0,0,0,NULL);
  rep->setRungeKuttaAcc(accuracy);
  rep->setRungeKuttaAdaptive(adaptive);
  Track* trk=new Track(rep);
  std::cout<<"Pos before extrapolation:"<<std::endl;
  trk->getPos().Print();
  TMatrixT<double> state1=trk->getTrackRep(0)->getState();
  state1.Print();
  
  TPolyMarker3D* mark=new TPolyMarker3D();

  std::vector<bool> success;

  for(double z=10; z<300; z+=20){
    trk->getTrackRep(0)->setState(state1);
    std::cout<<"Extrapolate to z="<<z<<std::endl;
    // create virtual space point hit
    VirtSpacePointRecoHit virtHit(TVector3(20,20,z));
    DetPlane pl=virtHit.getDetPlane(rep);
    double l=trk->getTrackRep(0)->extrapolate(pl);
    std::cout<<"length="<<l<<std::endl;
 
    //trk->gotoPoint();
  
    std::cout<<"Pos after extrapolation:"<<std::endl;
    TVector3 p=trk->getPos();
    p.Print();
    mark->SetNextPoint(p.X(),p.Y(),p.Z());
    
    trk->gotoPoint(TVector3(20,20,z0));
    TMatrixT<double> state2=trk->getTrackRep(0)->getState();
    TVector3 pos2=trk->getPos();

    TMatrixT<double> dif=state2-state1;
    bool out=false;
    for(int i=0;i<2;++i){
      double d=dif[i][0];
      if(d>accuracy)out=true;
    }
    if(out){
      std::cout<<"Out of bounds!"<<std::endl;
      state2.Print();
      pos2.Print();
      trk->getTrackRep(0)->getReferencePlane().Print();
      //break;
    }
    success.push_back(!out);
    
  }

  
  TVector3 pos3=trk->getPos();
  trk->stepalong(10);
  TVector3 pos4=trk->getPos();
  std::cout<<"doing a step of 10 cm. before/after"<<std::endl;
  pos3.Print();
  pos4.Print();

  
  std::cout<<"done "<<success.size()<<" extrapolations"<<std::endl;

  for(int i=0;i<success.size();++i){
    std::cout<<"Extrapolation#"<<i<<" was "<<success[i]<<std::endl;
  }


  TFile* f=new TFile("lsltrkrep.root","RECREATE");
  mark->Write("trk");
  f->Close();

}

void
GenfitTester::test_PCA(){
  std::cout<<"*** test Extrapolate to PCA ***"<<std::endl;
  double z0=20;
  Track* trk1=new Track(new LSLTrackRep(z0,1,1,1,0.001,2,0,0,0,0,0,NULL));
  Track* trk2=new Track(new LSLTrackRep(z0,1,2,-1,0.001,2,0,0,0,0,0,NULL));
  // go away from vertex
  trk1->stepalong(70);
  trk2->stepalong(80);
  std::cout<<"Start positions"<<std::endl;
  trk1->getPos().Print();
  trk2->getPos().Print();
  TVector3 dif1=trk1->getPos()-trk2->getPos();
  std::cout<<"d="<<dif1.Mag()<<std::endl;;
  
  trackProximity(trk1,trk2);
  
  std::cout<<"Positions after Extrapolation"<<std::endl;
  trk1->getPos().Print();
  trk2->getPos().Print();
  TVector3 dif2=trk1->getPos()-trk2->getPos();
  std::cout<<"d="<<dif2.Mag()<<std::endl;
}


void 
GenfitTester::test_LSLtoGLOB(){
   LSLTrackRep* rep=new LSLTrackRep(10,1,1,0.001,1,5,0.1,0.1,0.1,0.1,0.1,NULL);
   rep->Print();
   TVectorT<double> glob=rep->getGlobal();
   std::cout<<"glob=("<<glob[0]<<","<<glob[1]<<","
	    <<glob[2]<<","<<glob[3]<<","
	    <<glob[4]<<","<<glob[5]<<")"<<std::endl;
   TMatrixT<double> cov=rep->getGlobalCov();
   cov.Print();
}


ClassImp(GenfitTester)
