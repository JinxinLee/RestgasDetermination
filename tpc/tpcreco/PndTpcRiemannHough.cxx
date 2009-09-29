//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcRiemannHough
//      see PndTpcRiemannHough.h for details
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
#include "PndTpcRiemannHough.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include "TH3D.h"
#include "TH2D.h"
#include "TVector3.h"
#include "TMath.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TCanvas.h"


// Collaborating Class Headers --------
#include "DebugLogger.h"
#include "PndTpcClusterRadius.h"
#include "PndTpcClusterZ.h"
#include "TrackCand.h"
#include "PndTpcCluster.h"
#include "PndTpcRiemannTrack.h"
#include "PndTpcRiemannHit.h"
#include "PndTpcProximityHTCorrelator.h"
#include "PndTpcRiemannHTCorrelator.h"

// Class Member definitions -----------


PndTpcRiemannHough::PndTpcRiemannHough()
  : _minHitsForFit(5)
{
  // correlators in decreasing priority!
  
  //addCorrelator(new PndTpcProximityHTCorrelator(1.));
  //addCorrelator(new PndTpcRiemannHTCorrelator(5.E-4));
  
  // phi, theta, c
  histo=new TH3D("hough","hough",
		 200,-TMath::Pi(),TMath::Pi(), // phi
		 100,0,TMath::Pi(),        // theta
		 100,-0.5,0.5);                     // c

}


PndTpcRiemannHough::~PndTpcRiemannHough()
{
  delete histo;
}




unsigned int
PndTpcRiemannHough::buildTracks(std::vector<PndTpcCluster*>& cll,
				   std::vector<PndTpcRiemannTrack*>& candlist)
{
  //std::sort(cll.begin(),cll.end(),PndTpcClusterRadius());
  unsigned int ncl=cll.size();

    TH2D* pz=new TH2D("pz","phi vs theta", 
		      200,-TMath::Pi(),TMath::Pi(), // phi
		      100,0,TMath::Pi());        // theta

    TH2D* px=new TH2D("px","theta vs c",
		      100,0,TMath::Pi(),
		       100,-0.5,0.5);                     // c

    TH2D* py=new TH2D("py","phi vs c",
		      200,-TMath::Pi(),TMath::Pi(), // phi
		      100,-0.5,0.5);                     // c


  for(unsigned int icl=0;icl<ncl;++icl){ // loop over clusters
    //if(icl<30)cll[icl]->pos().Print();
    PndTpcRiemannHit* rhit=new PndTpcRiemannHit(cll[icl]);
    // Fill hough histogram. Hyperplane given by the equation
    // c=n*x
    // where c and n are part of the feature space and x is the hit on
    // the riemannsphere
    // n (parameterized by CosTheta,Phi) the normal to the trackplane
    // c the distance of the plane to the origin

    double phi;
    double theta;
    double minphi=-TMath::Pi()*1.005;
    TVector3 n(1,0,0);
    for(int i=0;i<200;++i){ // loop over phi
      phi=minphi+(double)i*(TMath::Pi()*0.01);
      for(int j=0;j<99;++j){ // loop over costheta
	theta=0.00001+(double)j*(TMath::Pi()*0.01);
	// fill cell
	n.SetMagThetaPhi(1,theta,phi);
	double c=n*rhit->x();
	histo->Fill(phi,theta,c);
	pz->Fill(phi,theta);
	px->Fill(theta,c);
	py->Fill(phi,c);
      } // end loop over costheta
    }// end loop over phi


  } // end loop over cluster
  
   TCanvas *c1 = new TCanvas("c1","c1",10,10,800,800);
   c1->Divide(2,2);
   c1->cd(1);
   histo->Draw("BOX");
   c1->cd(2);
   py->Draw("COL");

   c1->cd(3);
   px->Draw("COL");

   c1->cd(4);
   pz->Draw("COL");

   gApplication->SetReturnFromRun(kTRUE);
   gSystem->Run();
   delete c1;
   delete px;
   delete py;
   delete pz;


   histo->Clear();
   //std::cout<<candlist.size()<<" Riemann Tracks found."<<std::endl;
  
  
 return candlist.size();
}

