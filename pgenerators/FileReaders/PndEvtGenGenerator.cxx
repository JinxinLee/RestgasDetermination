// -------------------------------------------------------------------------
// -----                PndEvtGenGenerator source file                  -----
// -------------------------------------------------------------------------


#include <iostream>
#include "TClonesArray.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "TVector3.h"
#include "TParticle.h"
#include "PndEvtGenGenerator.h"
#include "FairPrimaryGenerator.h"

#include "TF1.h"
#include "TRandom.h"

#define MAX 200


using std::cout;
using std::endl;
using std::max;


// -----   Default constructor   ------------------------------------------
PndEvtGenGenerator::PndEvtGenGenerator() : 
  FairGenerator(), iEvent(0), fFileName(""), fInputRootFile(), fInputTree(),
  fInputAsciiFile(), fRPx(), fRPy(), fRPz(), fRVx(), fRVy(), fRVz(),
  fRPdg(), fRDF(), fRDL(), fRNTrk(0), fFileType(kFALSE), fGasmode(0), 
  fRsigma(0.), fDensityFunction()
					  
{
}
// ------------------------------------------------------------------------

// -----   Standard constructor   -----------------------------------------
PndEvtGenGenerator::PndEvtGenGenerator(const Char_t* fileName) : 
  FairGenerator(), iEvent(0), fFileName(fileName), fInputRootFile(), fInputTree(),
  fInputAsciiFile(), fRPx(), fRPy(), fRPz(), fRVx(), fRVy(), fRVz(),
  fRPdg(), fRDF(), fRDL(), fRNTrk(0), fFileType(kFALSE), fGasmode(0), 
  fRsigma(0.), fDensityFunction()
{
  Init();
}
// ------------------------------------------------------------------------

PndEvtGenGenerator::PndEvtGenGenerator(const Char_t* fileName, Double_t Rsigma, TF1 * DensityFunction) : 
  FairGenerator(), iEvent(0), fFileName(fileName), fInputRootFile(), fInputTree(),
  fInputAsciiFile(), fRPx(), fRPy(), fRPz(), fRVx(), fRVy(), fRVz(),
  fRPdg(), fRDF(), fRDL(), fRNTrk(0), fFileType(kFALSE), fGasmode(1), 
  fRsigma(Rsigma), fDensityFunction(DensityFunction)
{
  Init();
}


// -----   Destructor   ---------------------------------------------------
PndEvtGenGenerator::~PndEvtGenGenerator() {
  CloseInput();
  if (fFileType==1)
  {
    delete[] fRPx;
    delete[] fRPy;
    delete[] fRPz;
    delete[] fRVx;
    delete[] fRVy;
    delete[] fRVz;
    delete[] fRPdg;
  }
}
// ------------------------------------------------------------------------

Bool_t PndEvtGenGenerator::Init()
{
  iEvent     = 0;
  
  cout << "-I PndEvtGenGenerator: Opening input file " << fFileName.Data() << endl;
  
  // open a ROOT input file
  
  if (fFileName.EndsWith(".root"))  
  {
    fFileType = 1;  // ROOT file
    
    // create the arrays for the branches
    
    fRPx  = new Double_t[MAX];
    fRPy  = new Double_t[MAX];
    fRPz  = new Double_t[MAX];  
    fRVx  = new Double_t[MAX];
    fRVy  = new Double_t[MAX];
    fRVz  = new Double_t[MAX];  
    fRPdg = new Int_t[MAX];             
    fRDF  = new Int_t[MAX];
    fRDL  = new Int_t[MAX];  
      
    fInputRootFile = new TFile(fFileName,"READ");
    if (fInputRootFile->IsZombie())
      Fatal("PndEvtGenGenerator","Cannot open ROOT input file.");
    
    fInputTree = (TTree*) fInputRootFile->Get("ntp");
    if (!SetBranchAddresses()) 
      Fatal("PndEvtGenGenerator","Incompatible ROOT input file!");
    
  }
  // or an ASCII input file
  else
  {
    fFileType = 0;  // ASCII file
    
    if ((fInputAsciiFile = fopen(fFileName.Data(),"r"))==NULL)
      Fatal("PndEvtGenGenerator","Cannot open ASCII input file.");
  }
  return kTRUE;
}

// ------------------------------------------------------------------------

Bool_t PndEvtGenGenerator::SetBranchAddresses() 
{
    if (0==fInputTree) return false;
    
    fInputTree->SetBranchStatus("*",0);
    
    fInputTree->SetBranchStatus("nTrk",1);
    
    fInputTree->SetBranchStatus("px",1);
    fInputTree->SetBranchStatus("py",1);
    fInputTree->SetBranchStatus("pz",1);
    fInputTree->SetBranchStatus("x",1);
    fInputTree->SetBranchStatus("y",1);
    fInputTree->SetBranchStatus("z",1);
    fInputTree->SetBranchStatus("Id",1);
    fInputTree->SetBranchStatus("DF",1);
    fInputTree->SetBranchStatus("DL",1);
    
    
    fInputTree->SetBranchAddress("nTrk",&fRNTrk);
    fInputTree->SetBranchAddress("px",fRPx);
    fInputTree->SetBranchAddress("py",fRPy);
    fInputTree->SetBranchAddress("pz",fRPz);
    fInputTree->SetBranchAddress("x",fRVx);
    fInputTree->SetBranchAddress("y",fRVx);
    fInputTree->SetBranchAddress("z",fRVz);
    fInputTree->SetBranchAddress("Id",fRPdg);
    fInputTree->SetBranchAddress("DF",fRDF);
    fInputTree->SetBranchAddress("DL",fRDL);
   
    return true;
}

// -----   Public method ReadEvent   --------------------------------------
Bool_t PndEvtGenGenerator::ReadEvent(FairPrimaryGenerator* primGen) 
{
  if (fFileType==0) // fFileType is boolean
  { 
    if ( !fInputAsciiFile ) {
      cout << "-E PndEvtGenGenerator: Input ASCII file not open!" << endl;
      return kFALSE;
    }
    return ReadAsciiEvent(primGen);
  }
  else 
    {
      if ( ! fInputRootFile ) {
	cout << "-E PndEvtGenGenerator: Input ROOT file not open!" << endl;
	return kFALSE;
      }
      return ReadRootEvent(primGen);
    }
  
  return kFALSE; // It cannot happen
  
}
  
  
// ------------------------------------------------------------------------

Bool_t PndEvtGenGenerator::ReadRootEvent(FairPrimaryGenerator* primGen)
{
  // Check for number of events in input file
  if ( iEvent > fInputTree->GetEntries() ) {
    cout << "-E PndEvtGenGenerator: No more events in input file!" << endl;
    CloseInput();
    return kFALSE;
  }
  
  // preserve orginal TDirectory
  TFile  *g=gFile;
  fInputRootFile->cd();
  fInputTree->GetEntry(iEvent++);
  g->cd();

  for (Int_t i=0; i<fRNTrk;i++)
    {
      //cout << fRPdg[i]<<" "<<fRDF[i]<<" "<<fRDL[i]<<" "<<fRPx[i]<<" "<< fRPy[i] 
      //<<" "<<fRPz[i] <<" "<<fRVx[i] <<" "<<fRVy[i]<<" "<< fRVz[i]<<endl;
      
      // add all final state particles
      if ( -1==fRDF[i] && -1==fRDL[i] ) 
	{
	  Double_t fVx = fRVx[i]/10.; // mm -> cm conversion
	  Double_t fVy = fRVy[i]/10.;
	  Double_t fVz = fRVz[i]/10.;
	  /* Check if fGasmode is set */
	  if (fGasmode == 1) 
	    {
	      // Random 2D point in a circle of radius r (simple beamprofile)
	      Double_t fX, fY, fZ, radius;
	      radius = gRandom->Gaus(0,fRsigma);
	      gRandom->Circle(fX, fY, radius);
	      fVx = fVx + fX;
	      fVy = fVy + fY;
	      
	      // calculate fZ according to some (probability) density function of the
	      // gas
	      fZ=fDensityFunction->GetRandom();
	      fVz = fVz + fZ;
	    }	
	  
	  primGen->AddTrack(fRPdg[i],fRPx[i],fRPy[i],fRPz[i],fVx,fVy,fVz);
	  //cout <<"P = "<<fRPx[i]<<" "<<fRPy[i]<<" "<<fRPz[i]<<endl;
	} 
    }
  
  return kTRUE;
  
}


// ------------------------------------------------------------------------

Bool_t PndEvtGenGenerator::ReadAsciiEvent(FairPrimaryGenerator* primGen)
{
  // Define event variable to be read from file
   Int_t ntracks = 0, eventID = 0, ncols = 0;
 
  // Define track variables to be read from file
  Int_t nLine = 0, pdgID = 0, nDecay = 0, nM1 = -1, nM2 = -1, nDF = -1, nDL = -1;
  Double_t fPx = 0., fPy = 0., fPz = 0., fE = 0.;
  Double_t fVx = 0., fVy = 0., fVz = 0., fT = 0.;
  
  // Read event header line from input file

  Int_t max_nr = 0;
  
  Text_t buffer[400];
  ncols = fscanf(fInputAsciiFile,"%d\t%d", &eventID, &ntracks);
  
  if (ncols && ntracks>0) 
  {
    
    // cout << "Event number: " << eventID << "\tNtracks: " << ntracks << endl;
    for (Int_t ii=0; ii<15; ii++) {
      ncols = fscanf(fInputAsciiFile,"%s",buffer);
    //   cout << buffer << "\t" ;
    }
    //cout << endl;
     
    for (Int_t ll=0; ll<ntracks; ll++) 
    {
	  ncols = fscanf(fInputAsciiFile,"%d %d %d %d %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf", 
	           &nLine, &pdgID, &nDecay, &nM1, &nM2, &nDF, &nDL, &fPx, &fPy, &fPz, &fE, &fT, &fVx, &fVy, &fVz);
	  max_nr = max(max_nr, nDF);
	  max_nr = max(max_nr, nDL);
	  if ((nDF==-1) && (nDL==-1)) 
      {
	// Conversion mm -> cm for vertex position
	fVx = fVx / 10.; 
	fVy = fVy / 10.;
 	fVz = fVz / 10.; 
        /* Check if fGasmode is set */
        if (fGasmode == 1) 
        {
		    // Random 2D point in a circle of radius r (simple beamprofile)
			Double_t fX, fY, fZ, radius;
			radius = gRandom->Gaus(0,fRsigma);
			gRandom->Circle(fX, fY, radius);
			fVx = fVx + fX;
			fVy = fVy + fY;
				
			// calculate fZ according to some (probability) density function of the
            // gas
            fZ=fDensityFunction->GetRandom();
            fVz = fVz + fZ;
     	
         }	
         //printf("Insert coordinates: %f, %f, %f ...\n", fVx, fVy, fVz);
         //cout <<"P = "<<fPx<<" "<<fPy<<" "<<fPz<<endl;
         primGen->AddTrack(pdgID, fPx, fPy, fPz, fVx, fVy, fVz);     
	  
       }	  
    }
  }
  else 
  {
    cout << "-I FairEvtGenGenerator: End of input file reached " << endl;
    CloseInput();
    return kFALSE;
  }
  

  // If end of input file is reached : close it and abort run
  if ( feof(fInputAsciiFile) ) 
  {
    cout << "-I FairEvtGenGenerator: End of input file reached " << endl;
    CloseInput();
    return kFALSE;
  }
  
  /*
    cout << "-I FairEvtGenGenerator: Event " << eventID << ",  vertex = ("
    << vx << "," << vy << "," << vz << ") cm,  multiplicity "
    << ntracks << endl;
  */
  
  return kTRUE;
}



// -----   Private method CloseInput   ------------------------------------
void PndEvtGenGenerator::CloseInput() 
{
  if (fFileType==0) // fFileType is boolean
    {
      if (fInputAsciiFile) 
	{
	  cout << "-I PndEvtGenGenerator: Closing ASCII input file " << fFileName.Data() << endl;
	  
	  fclose(fInputAsciiFile);
	  delete fInputAsciiFile;
	  fInputAsciiFile = NULL;
	}
    }
  else
    {
      if (fInputRootFile) 
	{
	  cout << "-I PndEvtGenGenerator: Closing ROOT input file " << fFileName.Data() << endl;
	  fInputRootFile->Close();
	  delete fInputRootFile;
	  fInputRootFile = NULL;
	}
    }
  
}
// ------------------------------------------------------------------------



ClassImp(PndEvtGenGenerator)
