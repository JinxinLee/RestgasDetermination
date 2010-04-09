/******************************************************

JPsi->e+e-
Reads the TPC tracks and reconstruct the InvariantMass 
of J/Psi: Dipak
*******************************************************/ 

#include "PndJpsi2PiAna.h"

// Fair
#include "FairRootManager.h"

// Rho
#include "TPidSelector.h"
#include "TCandidate.h"
#include "TCandList.h"
#include "VAbsMicroCandidate.h"

// Root
#include "TH1F.h"

// general
#include <string>
#include <iostream>
		
		
using std::cout;
using std::endl;

		
// -----   Default constructor   -------------------------------------------
PndJpsi2PiAna::PndJpsi2PiAna() : PndAnaTask() 
{ 
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndJpsi2PiAna::~PndJpsi2PiAna() 
{
}

// -------------------------------------------------------------------------

InitStatus PndJpsi2PiAna::Init() 
{
  	std::cout << "-I- PndJpsi2PiAna: Initialization" << std::endl;
  	
  	InitArrays();
  
  
	jpsimass=new TH1F("mjpsi","jpsi",200,0,3.5);
	ppmass=new TH1F("mpp","mpp",200,0,4.5);
	mcmass=new TH1F("mmc","mmc",200,0,4.0);
	
	jpsiMSel=new TPidMassSelector("jpsi",3.1,0.1);
    
    fEvtCount=0;
    
    return kSUCCESS;
}

// -------------------------------------------------------------------------

void PndJpsi2PiAna::Exec(Option_t* opt) 
{
	if (0==(++fEvtCount)%100) cout <<"evt "<<fEvtCount<<endl;
	
    int i,j;
    
	GetEvent();
	
	TCandList ep, em, pip, pim, jpsi, pp, mc;
	
	FillList(ep,"ElectronVeryLoosePlus");
	FillList(em,"ElectronVeryLooseMinus");
	FillList(pip,"PionVeryLoosePlus");
	FillList(pim,"PionVeryLooseMinus");
	
	FillList(mc,"McTruth");
	
/*	cout<<" e+:" <<ep.GetLength();
	cout<<" e-:" <<em.GetLength();
	cout<<" pi+:" <<pip.GetLength();
	cout<<" pi-:" <<pim.GetLength();
	cout <<endl;*/
	
	jpsi.Combine(ep,em);
	
	for (j=0;j<jpsi.GetLength();++j) jpsimass->Fill(jpsi[j].M());
	//jpsi.Select(jpsiMSel);
	
	pp.Combine(jpsi,pip,pim);
	for (j=0;j<pp.GetLength();++j) ppmass->Fill(pp[j].M());
	
	for (j=0;j<mc.GetLength();++j) mcmass->Fill(mc[j].M());
	
}

// -------------------------------------------------------------------------

void PndJpsi2PiAna::Finish()
{
  jpsimass->Write();
  ppmass->Write();
}

