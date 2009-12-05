#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TTree.h"
#include "TString.h"


void printCand(TLorentzVector l, TVector3 p)
{
  cout <<"vtx("<<p.X()<<"/"<<p.Y()<<"/"<<p.Z()<<") p4("<<l.Px()<<"/"<<l.Py()<<"/"<<l.Pz()<<")="<<l.Vect().Mag()<<endl;
}


void QAmacro_fastsim_2(TString fname="sim_fast.root", int nevts=0)
{
    TStopwatch timer;
    timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");rootlogon();
    
    TCanvas *c1=new TCanvas("c1","c1",600,600);
    c1->Divide(2,2);

	PndEventReader evr(fname);
	if (nevts==0) nevts=evr.GetEntries();
	    
    // **** create and setup some histos for QA plots
    //
    TH1F *jpsimass = new TH1F("jpsimass","J/psi cands",100,3.1-0.3,3.1+0.3);
    TH1F *jpsi2mass = new TH1F("jpsi2mass","J/psi cands 4C fit",100,3.1-0.3,3.1+0.3);
    
    TH1F *ppmass = new TH1F("ppmass","pbarp cands",100,3.68598-0.3,3.68598+0.3);
    TH1F *pp2mass = new TH1F("pp2mass","pbarp fitted",100,3.68598-0.02,3.68598+0.02);
    
   // **** create all the particle lists we'll need for rebuilding the decay tree
    //
    TCandList pip, pim, ep, em;
    TCandList jpsi,pp;
    
    TPidMassSelector *jpsiMSel       = new TPidMassSelector("jpsiSelector" , 3.096 , 0.3);
    
    TLorentzVector ini(0,0,6.23164,7.24015);
    
	int i=0,j=0;
		
    // **** loop over all _events_
    //
	while (evr.GetEvent() && ++i<nevts)
	{
		if (!(i%100)) 
		cout <<"evt "<<i<<endl;
		
		//cout<<"e mass:" <<TRho::Instance()->GetPDG()->GetParticle(11)->Mass()<<endl;
		
		evr.FillList(pip,"PionVeryLoosePlus");
		evr.FillList(pim,"PionVeryLooseMinus");
		evr.FillList(ep, "ElectronVeryLoosePlus");
		evr.FillList(em, "ElectronVeryLooseMinus");
		
/*		for (j=0;j<ep.GetLength();++j) 
		{
			ep[j].SetMass(TRho::Instance()->GetPDG()->GetParticle(11)->Mass());
			cout <<"e: "<<ep[j].Uid()<<" ";
			
		}
		cout <<endl;
		for (j=0;j<em.GetLength();++j) cout <<"e-:"<<em[j].Uid()<<" ";
		cout <<endl;*/
		
		jpsi.Combine(ep,em);
		//jpsi.Select(jpsiMSel);
		for (j=0;j<jpsi.GetLength();++j) 
		{
		   jpsimass->Fill(jpsi[j].M());
		}
		
		// for the 4C fit we need to add the daughters individually
		pp.Combine(jpsi,pip,pim);
		
		for (j=0;j<pp.GetLength();++j)
		{
			ppmass->Fill(pp[j].M());
			
			//do the 4C fit on the pbar p System
			Pnd4CFitter fitter(pp[j],ini);
			
			fitter.FitConserveMasses();
			
			TCandidate *ppfit=fitter.FittedCand(pp[j]);
			pp2mass->Fill(ppfit->M());
			
			TCandidate *epfit=fitter.FittedCand(*(pp[j].Daughter(0)->Daughter(0)));
			TCandidate *emfit=fitter.FittedCand(*(pp[j].Daughter(0)->Daughter(1)));
			
			TLorentzVector sum=epfit->P4()+emfit->P4();
			
			jpsi2mass->Fill(sum.Mag());
			
		}
    }
    
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    
    printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

    Bool_t fTest=kTRUE;

    Double_t mean_jpsimass=jpsimass->GetMean();
    Double_t mean_jpsi2mass=jpsi2mass->GetMean();

    Double_t mean_ppmass=ppmass->GetMean();
    Double_t mean_pp2mass=pp2mass->GetMean();
    
    if (mean_jpsimass>3.0 && mean_jpsimass<3.2 && mean_jpsi2mass>3.0 && mean_jpsi2mass<3.2)
    {
	cout<<"\n J/Psi mass - ACCEPTABLE "<<endl;
    }
    else
    {
	cout<<" \n J/Psi mass - SOMETHING WENT WRONG "<<endl;
	cout<<"    Mean                = " << mean_jpsimass << endl;
	cout<<"    Mean (after kinfit) = " << mean_jpsi2mass << endl;
	fTest=kFALSE;
    }

    if (mean_ppmass>3.5 && mean_ppmass<3.7 && mean_pp2mass>3.5 && mean_pp2mass<3.7)
    {
	cout<<"\n psi(2s) mass - ACCEPTABLE "<<endl;
    }
    else
    {
	cout<<" \n psi(2s) mass - SOMETHING WENT WRONG "<<endl;
	cout<<"    Mean                = " << mean_ppmass << endl;
	cout<<"    Mean (after kinfit) = " << mean_pp2mass << endl;
	fTest=kFALSE;
    }

    if (fTest){
    cout << " Test passed" << endl;
    cout << " All ok " << endl;  
    }
    exit(0);     
}
