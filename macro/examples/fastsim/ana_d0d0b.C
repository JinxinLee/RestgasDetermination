#include "TCanvas.h"
#include "TTree.h"
#include "TChain.h"

int ana_d0d0b(TString fname,int num=1000000)
{
    TStopwatch timer;
    timer.Start();
    
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libRho");
    
	
	TCanvas *c1=new TCanvas("c1","c1",600,600);
    c1->Divide(2,2);
    
    TH1F *d0mass=new TH1F("d0Mass","D0 mass",100,1.864-0.1,1.864+0.1);
	TH1F *ppmass = new TH1F("ppmass","pbar p mass", 100, 4.4,4.6);
	TH1F *nneut=new TH1F("nneut","neutrals",100,0,30);
	TH1F *nchrg=new TH1F("nchrg","charged",100,0,10);
   
	PndEventReader evr(fname);
	
	TCandList kp,km,pip,pim;
	TCandList d0,d0b,pp;
	
	int i=0,j,k;
	
	TPidMassSelector *msel=new TPidMassSelector("d0 mass",1.864,0.1); 
	
	while (evr.GetEvent() && i<num)
	{
		if (!(++i%100)) cout <<"evt "<<i<<endl;	
		
		int nn=0,nc=0;
		
		PndEventInfo *evtinfo=evr.GetEventInfo();
		
		if (evtinfo)
		{
			nn=evtinfo->GetNeutrals();
			nc=evtinfo->GetCharged();
		}
		
		nneut->Fill(nn);
		nchrg->Fill(nc);
		
		if (nc!=6) continue;
		
		evr.FillList(kp,"KaonVeryLoosePlus");
		evr.FillList(km,"KaonVeryLooseMinus");
		evr.FillList(pip,"PionVeryLoosePlus");
		evr.FillList(pim,"PionVeryLooseMinus");
		
		d0.Combine(km,pip);
		d0b.Combine(kp,pim);
		
		for (j=0;j<d0.GetLength();j++) d0mass->Fill(d0[j].M());
		for (j=0;j<d0b.GetLength();j++) d0mass->Fill(d0b[j].M());
		
		d0.Select(msel);
		d0b.Select(msel);
		
		pp.Combine(d0,d0b,pip,pim);
		
		for (j=0;j<pp.GetLength();j++) ppmass->Fill(pp[j].M());
	}
	
	c1->cd(1);
	d0mass->Draw();
	c1->cd(2);
	ppmass->Draw();
	c1->cd(3);
	nneut->Draw();
	c1->cd(4);
	nchrg->Draw();
	
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    
    printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
    
  return 0;
}
