/******************************************************

JPsi->e+e-
Reads the TPC tracks and reconstruct the InvariantMass 
of J/Psi: Dipak
*******************************************************/ 

#include "PndJpsi2PiAna.h"

// Fair
#include "FairRootManager.h"

// Rho
#include "RhoCandidate.h"
#include "RhoCandList.h"
#include "FairRecoCandidate.h"

#include "Fitter/Rho4CFitter.h"
#include "RhoMassParticleSelector.h"

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
	jpsimassf=new TH1F("mjpsif","jpsi fit",200,0,3.5);
	
	ppmass=new TH1F("mpp","mpp",200,0,4.5);
	ppmassf=new TH1F("mppf","mpp",200,0,4.5);
	
	mcmass=new TH1F("mmc","mmc",200,0,4.0);
	
	jpsiMSel=new RhoMassParticleSelector("jpsi",3.1,0.2);
    
    fEvtCount=0;
    
    return kSUCCESS;
}

// -------------------------------------------------------------------------

void PndJpsi2PiAna::Exec(Option_t*) 
{
	if (0==(++fEvtCount)%100) cout <<"evt "<<fEvtCount<<endl;
	
    int i,j;
    
	GetEvent();
	
    RhoCandList ep, em, pip, pim, jpsi, pp, mc;
	
	FillList(ep,"ElectronVeryLoosePlus");
	FillList(em,"ElectronVeryLooseMinus");
	FillList(pip,"PionVeryLoosePlus");
	FillList(pim,"PionVeryLooseMinus");
	
	FillList(mc,"McTruth");
	
	TLorentzVector ini(0, 0, 6.231711, 7.240222);
	
/*	cout<<" e+:" <<ep.GetLength();
	cout<<" e-:" <<em.GetLength();
	cout<<" pi+:" <<pip.GetLength();
	cout<<" pi-:" <<pim.GetLength();
	cout <<endl;*/
	
	jpsi.Combine(ep,em);
	
/*	cout <<jpsi.GetLength();*/
	
	for (j=0;j<jpsi.GetLength();++j) jpsimass->Fill(jpsi[j]->M());
	//jpsi.Select(jpsiMSel);
	
	for (j=0;j<mc.GetLength();++j) mcmass->Fill(mc[j]->M());
	
	pp.Combine(jpsi,pip,pim);
		
	for (j=0;j<pp.GetLength();++j)
	{
			ppmass->Fill(pp[j]->M());
			
			//do the 4C fit on the pbar p System
			Rho4CFitter fitter(pp[j],ini);
			
			fitter.FitConserveMasses();
			
            RhoCandidate *ppfit=pp[j]->GetFit();
			ppmassf->Fill(ppfit->M());
			
            RhoCandidate *epfit=(pp[j]->Daughter(0)->Daughter(0))->GetFit();
            RhoCandidate *emfit=(pp[j]->Daughter(0)->Daughter(1))->GetFit();
			
			TLorentzVector sum=epfit->P4()+emfit->P4();
			
			jpsimassf->Fill(sum.Mag());
			
	}

	
}

// -------------------------------------------------------------------------

void PndJpsi2PiAna::Finish()
{
  jpsimass->Write();
  jpsimassf->Write();
  ppmass->Write();
  ppmassf->Write();
  mcmass->Write();
}

