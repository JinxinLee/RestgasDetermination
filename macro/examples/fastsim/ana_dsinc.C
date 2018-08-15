#include "TCanvas.h"
#include "TTree.h"

int ana_dsinc(TString fname,int nevts=0)
{
	gROOT->Reset();
  	TStopwatch timer;
  	timer.Start();
  
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");basiclibs();
	gSystem->Load("libRho");
	
	TCanvas *c1=new TCanvas("c1","c1",600,600);
	c1->Divide(2,2);
	
	TH1F *mphi=new TH1F("mphi","m(phi)",100,0.98,1.06);
	TH1F *mds=new TH1F("meta","m(ds)",100,1.9685-0.10,1.9685+0.10);
	TH1F *miss=new TH1F("miss","miss mass",200,2.317-0.2,2.317+0.2);
	TH1F *sum=new TH1F("sum","m(ds)+miss",200,1.9685+2.317-0.05,1.9685+2.317+0.01);
	
	PndEventReader evr(fname);
	
	if (nevts==0) nevts=evr.GetEntries();
	int i=0,j;
	
	TCandList kp,km,pi,phi,ds;
	
	TPidMassSelector *phiMassSel=new TPidMassSelector("phi",1.0195,0.03);
		
	while (evr.GetEvent() && i++<nevts)
	{
		if (0==i%100) cout <<"evt "<<i<<endl;
		
		PndEventInfo *evtinfo=evr.GetEventInfo();
		
		TLorentzVector cm=evtinfo->CmFrame();
		
		evr.FillList(kp,"KaonVeryLoosePlus");
		evr.FillList(km,"KaonVeryLooseMinus");
		evr.FillList(pi,"PionVeryLoose");	
		
		phi.Combine(kp,km);
		phi.Select(phiMassSel);
		for (j=0;j<phi.GetLength();++j) mphi->Fill(phi[j].M()); 
		
		ds.Combine(phi,pi);
		for (j=0;j<ds.GetLength();++j)
		{
			double dsmass=ds[j].M();
			mds->Fill(dsmass);
			double missmass=(cm-ds[j].P4()).M();
			miss->Fill(missmass);
			sum->Fill(missmass+dsmass);
		}
	}
	
	c1->cd(1); mphi->Draw();
	c1->cd(2); mds->Draw();
	c1->cd(3); miss->Draw();
	c1->cd(4); sum->Draw();
	c1->cd();
	
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    
    printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
	  return 0;

}
