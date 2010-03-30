// Fit Task for linear tracks in the Bonn Tracking Station (6 sensors)
// The output is a TClonesArray for each event with all the possible tracks
// stored in a PndLinTrack(modifed version with eloss) object

// Simone Bianco 21/03/2010

// This Class' Header ------------------
#include "PndMvdLinFitTask.h"
#include "PndLinTrack.h"

// C/C++ Headers ----------------------
#include <iostream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"

//#include "PndLinTrack.h"
#include "../../../pnddata/TrackData/PndTrackCand.h"
#include "../../../pnddata/SdsData/PndSdsHit.h"
#include "../../../pnddata/MvdData/PndMvdHit.h"
#include "../../../pnddata/TrackData/PndTrackCandHit.h"

#include "TFile.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"

// Fit Classes -----------
#include <TMath.h>
#include <TVector3.h>
#include <TRandom2.h>
//#include <TStyle.h>
//#include <TCanvas.h>
#include <TF2.h>
#include <TH1.h>
#include <TVirtualFitter.h>
#include <TPolyLine3D.h>
#include <Math/Vector3D.h>

using namespace ROOT::Math;

PndMvdLinFitTask::PndMvdLinFitTask()
  : FairTask("3D-Straight-Line-Fit")
{
  fTCandBranchName = "MVDHitsStrip";
  fRecoBranchName = "MVDHitsStrip";
}


PndMvdLinFitTask::~PndMvdLinFitTask()
{

}

InitStatus PndMvdLinFitTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndMvdLinFitTask::Init","RootManager not instantiated!");
      return kERROR;
    }

  //Get input collection
  
  fTCandArray=(TClonesArray*) ioman->GetObject(fTCandBranchName);

  if(fTCandArray==0)
    {
      Error("PndMvdLinFitTask::Init","trackcand-array not found!");
      return kERROR;
    }

  //  fRecoArray=(TClonesArray*) ioman->GetObject(fRecoBranchName);

  //   if(fRecoArray==0)
  //     {
  //       Error("PndMvdLinFitTask::Init","reco-array not found!");
  //       return kERROR;
  //     }

  fTrackArray = new TClonesArray("PndLinTrack");
  ioman->Register("MVDTrack", "PndMvd", fTrackArray, kTRUE);

  std::cout << "-I- PndMvdLinFitTask: Initialisation successfull" << std::endl;

  return kSUCCESS;
}


void PndMvdLinFitTask::Exec(Option_t* opt)
{

  Int_t ntcand=fTCandArray->GetEntriesFast();

  std::cout << "Entries: " << ntcand << std::endl;

  std::map<TString,Int_t> Sensors;

  Double_t eloss[6] = {0.,0.,0.,0.,0.,0.};

  {
    std::cout<<"PndMvdLinFitTask::Exec"<<std::endl;
    // Reset output Array
    if(fTrackArray==0) Fatal("PndMvdLinFitTask::Exec","No TrackArray");
    fTrackArray->Delete();
    



    TString name;

    
    // Detailed output
    std::cout<<" -I- PndMvdLinFitTask: contains "<<ntcand<<" TCandidates"<<std::endl;
    {
      std::cout<< " Detailed Debug info on the candidates:"<<std::endl;
      unsigned int detid=12345, index=12345;
      for(Int_t itr=0;itr<ntcand;++itr){

	PndMvdHit *theHit = (PndMvdHit*)   fTCandArray->At(itr);
	
	index = theHit->GetRefIndex();
	detid = theHit->GetDetectorID();

	name = theHit->GetDetName();
	
	Sensors[name] += 1;

	std::cout << itr << "\t" << index << "\t" << detid << std::endl;
      }
    }
  }

  const Int_t sizeMap = Sensors.size();
  
  Int_t Num[sizeMap];
  TString DetNames[sizeMap];

  Int_t k = 0;

  for (std::map<TString,Int_t>::iterator it=Sensors.begin();it!=Sensors.end();++it)
    {

      Num[k] = it->second;
      DetNames[k] = it->first;
      
      k++;
    }




  // Fitting ----------------------------------------------------------------------------------
  if(fVerbose>1)std::cout<<" -I- PndMvdLinFitTask: start Fitting "<<std::endl;

  Int_t firstHit=-1, lastHit=-1;



  Int_t ihit = 0, track = 0;

  std::cout << "Map size: " << sizeMap << std::endl;

  if (sizeMap>1)
    {
      
      TGraph2DErrors fitme(ntcand); //new graph for fitting
      {
	for(Int_t it1=0; it1<ntcand; it1++) // on first det
	  
	  {
	    ihit = 0;
	    
	    eloss[0] = 0;

	    PndMvdHit *theHit = (PndMvdHit*)   fTCandArray->At(it1);

	    if( (theHit->GetDetName()) != DetNames[0]) continue;
      
	    eloss[0] = theHit->GetEloss();

	    TVector3 addPos = theHit->GetPosition();
      
	    fitme.SetPoint(ihit, addPos.X(), addPos.Y(), addPos.Z());
	    fitme.SetPointError(ihit, theHit->GetDx(), theHit->GetDy(), theHit->GetDz());
      
	    for (Int_t it2=0; it2<ntcand; it2++) // on second det
	      {
		ihit = 1;
	      
		eloss[1] = 0;
	    
		PndMvdHit *theHit1 = (PndMvdHit*)   fTCandArray->At(it2);

		if( (theHit1->GetDetName()) != DetNames[1]) continue;
      
		eloss[1] = theHit1->GetEloss();

		TVector3 addPos1 = theHit1->GetPosition();
      
		fitme.SetPoint(ihit, addPos1.X(), addPos1.Y(), addPos1.Z());
		fitme.SetPointError(ihit, theHit1->GetDx(), theHit1->GetDy(), theHit1->GetDz());
	    
		if (sizeMap==2)
		  {
		    Double_t parFit[4]; //fit-parameter
		    Double_t accuracy = line3Dfit(6, &fitme, parFit);
		    
		    PndLinTrack* trackfit = new PndLinTrack("Mvd", parFit[0], parFit[1], parFit[2], parFit[3], accuracy, eloss[0]+eloss[1], sizeMap, track);

		    new((*fTrackArray)[track]) PndLinTrack(*(trackfit)); //save Track

		    track++;
		  }	
		
		else
		  {
		    for (Int_t it3=0; it3<ntcand; it3++) // on third det
		      {
			ihit = 2;
		    
			eloss[2] = 0;
		      
			PndMvdHit *theHit2 = (PndMvdHit*)   fTCandArray->At(it3);
		      
			if( (theHit2->GetDetName()) != DetNames[2]) continue;
		      
			eloss[2] = theHit2->GetEloss();

			TVector3 addPos2 = theHit2->GetPosition();
      
			fitme.SetPoint(ihit, addPos2.X(), addPos2.Y(), addPos2.Z());
			fitme.SetPointError(ihit, theHit2->GetDx(), theHit2->GetDy(), theHit2->GetDz());
		 
			if (sizeMap==3)
			  {
			    Double_t parFit[4]; //fit-parameter
			    Double_t accuracy = line3Dfit(6, &fitme, parFit);
		    
			    PndLinTrack* trackfit = new PndLinTrack("Mvd", parFit[0], parFit[1], parFit[2], parFit[3], accuracy, eloss[0]+eloss[1]+eloss[2], sizeMap, track);

			    new((*fTrackArray)[track]) PndLinTrack(*(trackfit)); //save Track

			    track++;
			  }	
		
			else
			  {

			    for (Int_t it4=0; it4<ntcand; it4++) // on fourth det
			      {
			  
				ihit = 3;
			  
				eloss[3] = 0;
			  
				PndMvdHit *theHit3 = (PndMvdHit*)   fTCandArray->At(it4);
			  
				if( (theHit3->GetDetName()) != DetNames[3]) continue;

				eloss[3] = theHit3->GetEloss();
			    
				TVector3 addPos3 = theHit3->GetPosition();
      
				fitme.SetPoint(ihit, addPos3.X(), addPos3.Y(), addPos3.Z());
				fitme.SetPointError(ihit, theHit3->GetDx(), theHit3->GetDy(), theHit3->GetDz());
			
				if (sizeMap==4)
				  {
				    Double_t parFit[4]; //fit-parameter
				    Double_t accuracy = line3Dfit(6, &fitme, parFit);
		    
				    PndLinTrack* trackfit = new PndLinTrack("Mvd", parFit[0], parFit[1], parFit[2], parFit[3], accuracy, eloss[0]+eloss[1]+eloss[2]+eloss[3], sizeMap, track);
				    
				    new((*fTrackArray)[track]) PndLinTrack(*(trackfit)); //save Track
				 
				    track++;
				  }	
		
				else
				  {

				    for (Int_t it5=0; it5<ntcand; it5++) // on fifth det
				      {
			      
					ihit = 4;
	      
					eloss[4] = 0;

					PndMvdHit *theHit4 = (PndMvdHit*)   fTCandArray->At(it5);

			      
					if( (theHit4->GetDetName()) != DetNames[4]) continue;

					eloss[4] = theHit4->GetEloss();
      
					TVector3 addPos4 = theHit4->GetPosition();
      
					fitme.SetPoint(ihit, addPos4.X(), addPos4.Y(), addPos4.Z());
					fitme.SetPointError(ihit, theHit4->GetDx(), theHit4->GetDy(), theHit4->GetDz());
			 
					if (sizeMap==5)
					  {
					    Double_t parFit[4]; //fit-parameter
					    Double_t accuracy = line3Dfit(6, &fitme, parFit);
		    
					    PndLinTrack* trackfit = new PndLinTrack("Mvd", parFit[0], parFit[1], parFit[2], parFit[3], accuracy, eloss[0]+eloss[1]+eloss[2]+eloss[3]+eloss[5], sizeMap, track);

					    new((*fTrackArray)[track]) PndLinTrack(*(trackfit)); //save Track

					    track++;
					  }	
		
					else
					  {

					    for (Int_t it6=0; it6<ntcand; it6++) // on sixth det
					      {
				
						ihit = 5;
				  
						eloss[5] = 0;

						PndMvdHit *theHit5 = (PndMvdHit*)   fTCandArray->At(it6);

						if( (theHit5->GetDetName()) != DetNames[5]) continue;
      
						TVector3 addPos5 = theHit5->GetPosition();
				  
						fitme.SetPoint(ihit, addPos5.X(), addPos5.Y(), addPos5.Z());
						fitme.SetPointError(ihit, theHit5->GetDx(), theHit5->GetDy(), theHit5->GetDz());
				  
						Double_t parFit[4]; //fit-parameter
						Double_t accuracy = line3Dfit(6, &fitme, parFit);

						eloss[5] = theHit5->GetEloss();      

						PndLinTrack* trackfit = new PndLinTrack("Mvd", parFit[0], parFit[1], parFit[2], parFit[3], accuracy, eloss[0]+eloss[1]+eloss[2]+eloss[3]+eloss[4]+eloss[5], sizeMap, track);
				  

				  
						new((*fTrackArray)[track]) PndLinTrack(*(trackfit)); //save Track
      
						track++;
				  
						// Done--------------------------------------------------------------------------------------
  
						std::cout<<"Fitting done"<<std::endl;
					      }
					  }
			  
				      }
				  }
			      }		      
			  }
		      }
		  }
	      }
	    
	  }

      }// End of TCand's

    
      

      
    
    }
  return;
}


// define the parameteric line equation 
void PndMvdLinFitTask::line(double t, double *p, double &x, double &y, double &z) { 
  // a parameteric line is define from 6 parameters but 4 are independent
  // x0,y0,z0,z1,y1,z1 which are the coordinates of two points on the line
  // can choose z0 = 0 if line not parallel to x-y plane and z1 = 1; 
  x = p[0] + p[1]*t; 
  y = p[2] + p[3]*t;
  z = t; 
} 

// calculate distance line-point 
double distance2(double x,double y,double z, double *p) { 
  // distance line point is D= | (xp-x0) cross  ux | 
  // where ux is direction of line and x0 is a point in the line (like t = 0) 
  XYZVector xp(x,y,z); 
  XYZVector x0(p[0], p[2], 0. ); 
  XYZVector x1(p[0] + p[1], p[2] + p[3], 1. ); 
  XYZVector u = (x1-x0).Unit(); 
  double d2 = ((xp-x0).Cross(u)) .Mag2(); 
  return d2; 
}

// function to be minimized 
void SumDistance2(int &, double *, double & sum, double * par, int ) { 
  TGraph2D * gr = dynamic_cast<TGraph2D*>( (TVirtualFitter::GetFitter())->GetObjectFit() );
  assert(gr != 0);
  double * x = gr->GetX();
  double * y = gr->GetY();
  double * z = gr->GetZ();
  int npoints = gr->GetN();
  sum = 0;
  for (int i  = 0; i < npoints; ++i) { 
    double d = distance2(x[i],y[i],z[i],par); 
    sum += d;
  }
  //if (firstIt && fVerbose>1) 
  //   std::cout << "Total sum2 = " << sum << std::endl;
  //firstIt = false;
}

double PndMvdLinFitTask::line3Dfit(Int_t nd, TGraph2DErrors* gr, Double_t* fitpar)
{
  //gStyle->SetOptStat(0);
  //gStyle->SetOptFit();
  //firstIt = true;
   
  TVirtualFitter *min = TVirtualFitter::Fitter(0,4);
  min->SetObjectFit(gr);
  min->SetFCN( *SumDistance2 );
  
  Double_t arglist[10];
  arglist[0] = 1;
  min->ExecuteCommand("SET PRINT",arglist,1);
  
  double pStart[4] = {0.001,0.001,0.001,0.001};
  min->SetParameter(0,"x0",pStart[0],0.00001,0,0);
  min->SetParameter(1,"Ax",pStart[1],0.00001,0,0);
  min->SetParameter(2,"y0",pStart[2],0.00001,0,0);
  min->SetParameter(3,"Ay",pStart[3],0.00001,0,0);
  min->SetPrecision(1e-8);
    
  arglist[0] = 1000; // number of function calls 
  arglist[1] = 1e-8; // tolerance 
   
  min->ExecuteCommand("MIGRAD",arglist,2);

  //if (minos) min->ExecuteCommand("MINOS",arglist,0);
  int nvpar,nparx; 
  double amin,edm, errdef;
  min->GetStats(amin,edm,errdef,nvpar,nparx);
  if(fVerbose>1)
    min->PrintResults(1,amin);
  // gr->Draw("p0");

  // get fit parameters
  for (int i = 0; i <4; ++i) 
    fitpar[i] = min->GetParameter(i); 

  return amin; 
}


ClassImp(PndMvdLinFitTask);
