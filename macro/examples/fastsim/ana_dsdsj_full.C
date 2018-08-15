
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TTree.h"
#include "TString.h"
/*
#include "RhoSelector/TPidSelector.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandListIterator.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/TFactory.h"
*/

void config_histo(TH1 *h, TString tx, TString ty)
{
  h->SetLineWidth(2);
  //h->SetLineColor(1);
  //h->SetFillColor(1);

  h->GetXaxis()->SetTitleOffset(1.2);
  h->GetXaxis()->SetTitleColor(1);
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetTitleSize(0.05);
  h->GetXaxis()->SetNdivisions(505);

  h->GetYaxis()->SetTitleOffset(1.6);
  h->GetYaxis()->SetTitleFont(42);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetTitleSize(0.05);
  h->SetMinimum(0);

  h->SetFillStyle(1000);
  h->SetFillColor(3);
  h->SetLineWidth(1);

  h->SetXTitle(tx);
  h->SetYTitle(ty);

}

void printCand(TLorentzVector l, TVector3 p)
{
  cout <<"vtx("<<p.X()<<"/"<<p.Y()<<"/"<<p.Z()<<") p4("<<l.Px()<<"/"<<l.Py()<<"/"<<l.Pz()<<")="<<l.Vect().Mag()<<endl;
}

void propagate(TLorentzVector &l, TVector3 &p, float charge, TH2F* hpro=0)
{
  double x0=p.X()/100;
  double y0=p.Y()/100;
  double z0=p.Z()/100;

  double px0=l.Px();
  double py0=l.Py();
  double pz0=l.Pz();

  double B=2;

  double pt=sqrt(px0*px0+py0*py0);
  double lambda=pz0/pt;
  double s_t=z0/lambda;
  double a=-0.2998*B*charge/3;
  double rho=a/pt;

  //cout <<hpro<<endl;

  if (hpro)
  {
    int steps=100;
    double dz=z0/steps;
    for (int i=0;i<steps;i++)
    {
      if (i==90) i=99;
      double s_tt=(i+1)*dz/lambda;
      double cosrst=cos(rho*s_tt);
      double sinrst=sin(rho*s_tt);

      double px = px0*cosrst + py0*sinrst;
      double py = py0*cosrst - px0*sinrst;
      //double pz = pz0;

      double xt=x0 - px/a*sinrst + py/a*(1-cosrst);
      double yt=y0 - py/a*sinrst - px/a*(1-cosrst);
      //double z=z0 - lambda*s_tt;

       hpro->Fill(xt,yt);
    }
  }

  //cout <<"lam="<<atan(1/lambda)/3.1416*180<<" "<<rho<<" "<<s_t<<endl;

  double cosrs=cos(rho*s_t);
  double sinrs=sin(rho*s_t);

  double px = px0*cosrs + py0*sinrs;
  double py = py0*cosrs - px0*sinrs;
  double pz = pz0;

  double x=x0 - px/a*sinrs + py/a*(1-cosrs);
  double y=y0 - py/a*sinrs - px/a*(1-cosrs);
  double z=z0 - lambda*s_t;

  l.SetX(px);
  l.SetY(py);
  l.SetZ(pz);

  p.SetX(x*100);
  p.SetY(y*100);
  p.SetZ(z*100);
}

int ana_dsdsj_full(TString fname="dsdsj.full.root",int num=10, int pr=1)
{
    TStopwatch timer;
    timer.Start();

    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();

    // **** the library is needed in order to use the TCandidate and TCandList etc
    gSystem->Load("libRho");

    TCanvas *c1=new TCanvas("c1","c1",1000,700);
    c1->Divide(3,2);

    // **** access the datastructure holding the particle lists
    //
    TFile* f = new TFile(fname.Data());
    TTree *t=(TTree*)f->Get("pndsim") ;

    // **** for every event, a TCLonesArray with the candidates is stored in pndsim
    //
    TClonesArray *fChrgCands=new TClonesArray("TCandidate");
    TClonesArray *fNeutCands=new TClonesArray("TCandidate");
    TClonesArray *fMcCands=new TClonesArray("TCandidate");

    t->SetBranchAddress("PndChargedCandidates",&fChrgCands);
    t->SetBranchAddress("PndNeutralCandidates",&fNeutCands);
    t->SetBranchAddress("PndMcTracks",&fMcCands);

    TCandidate *tc;

    // **** create and setup some histos for QA plots
    //
    TH1F *phimass = new TH1F("phimass","phi cands",100,0.98,1.1);
    TH1F *pi0mass = new TH1F("pi0mass","pi0 cands",100,0.130-0.05,0.130+0.05);
    TH1F *dsmass  = new TH1F("dsmass","Ds cands",100,1.968-0.06,1.968+0.06);
    TH1F *ds0mass = new TH1F("ds0mass","Ds0 cands",100,2.317-0.1,2.317+0.1);
    TH1F *ppmass = new TH1F("ppmass","pbarp cands",100,4.291-0.1,4.291+0.1);
    TH1F *pp2mass = new TH1F("pp2mass","sum m_{miss}+m_{Ds}",100,4.2855-0.02,4.2855+0.02);

    TH1F *nmult=new TH1F("nmult","# neutrals",15,0,15);

    TH1F *pdiff=new TH1F("pdiff","momentum difference",100,0,2);
    TH2F *pdiff2=new TH2F("pdiff2","gamma corr",50,0,2.5,50,0,2.5);

    config_histo(phimass,"m(K^{+} K^{-}) [GeV/c^{2}]","entries");
    config_histo(pi0mass,"m(#gamma#gamma) [GeV/c^{2}]","entries");
    config_histo(dsmass,"m(#phi#pi^{#pm}) [GeV/c^{2}]","entries");
    config_histo(ds0mass,"m(D_{s}#pi^{0}) [GeV/c^{2}]","entries");
    config_histo(ppmass,"m(D_{s} D_{s0}*) [GeV/c^{2}]","entries");
    config_histo(pp2mass,"m_{miss}+m_{D_{s}} [GeV/c^{2}]","entries");

    /*phimass->SetMinimum(0);
    pi0mass->SetMinimum(0);
    dsmass->SetMinimum(0);
    ds0mass->SetMinimum(0);
    ppmass->SetMinimum(0);
    */
    if (num==0) num= t->GetEntriesFast();
    cout <<"\n####### Processing "<<num <<" events...\n"<<endl;

    // **** create all the particle lists we'll need for rebuilding the decay tree
    //
    TCandList mcCands;

    TCandList allCands,neutralCands,chargedCands,chargedCands2, plusCands,minusCands;

    TCandList kpCands,kmCands,piCands;

    TCandList phiCands,pi0Cands,dsCands,ds0Cands,ppCands;

    // **** create and configure the selectors/filters we'd like to use later
    //
    TPidChargedSelector *chargedSel = new TPidChargedSelector;
    TPidNeutralSelector *neutralSel = new TPidNeutralSelector;
    TPidPlusSelector    *plusSel    = new TPidPlusSelector;
    TPidMinusSelector   *minusSel   = new TPidMinusSelector;

    // **** mass selectors for the resonances/composites
    //
    TPidMassSelector *phiMSel       = new TPidMassSelector("phiSelector" , 1.0195 , 0.04);
    TPidMassSelector *pi0MSel       = new TPidMassSelector("pi0Selector" , 0.130  , 0.02);
    TPidMassSelector *dsMSel        = new TPidMassSelector("dsSelector"  , 1.9685 , 0.02);

    TPidSimpleKaonSelector *kSel    = new TPidSimpleKaonSelector();
    kSel->SetCriterion("veryLoose");
    TPidSimplePionSelector *piSel   = new TPidSimplePionSelector();
    piSel->SetCriterion("veryLoose");

    TH2F *hpro=new TH2F("hpro","",300,-1,1,300,-1,1);


    int i1,i2;

    // **** loop over all _events_
    //
    for (Int_t j=0; j< num;j++)
    {
       if ((j%100)==0) cout <<"evt "<<j<<endl;

       if (pr) cout <<endl<<"************** evt "<<j<<endl;

	t->GetEntry(j);
	TFactory::Instance()->Reset();

	mcCands.Cleanup();
	chargedCands.Cleanup();
	chargedCands2.Cleanup();
	neutralCands.Cleanup();

	// **** loop over all Candidates and add them to the list allCands
	//

	for (i1=0; i1<fChrgCands->GetEntriesFast(); i1++){
	    tc = (TCandidate *)fChrgCands->At(i1);

	    TLorentzVector l=tc->P4();
	    TVector3 p=tc->Pos();

	    if (pr!=0)
	    {
	    cout <<"1 --> ";
            cout <<"c("<<tc->Charge()<<")  vtx("<<p.X()<<"/"<<p.Y()<<"/"<<p.Z()<<")  p4("<<l.Px()<<"/"<<l.Py()<<"/"<<l.Pz()<<")="<<l.Vect().Mag()<<endl;
	    //printCand(l,pos);
	    }

	    chargedCands.Add(*tc);
	    //pdiff->Fill(p.Mag());
	    propagate(l,p,tc->GetCharge());

	    //if (j=1 && i1==1)
	    //{
	    //  propagate(l,p,tc->GetCharge(), hpro);
	    //}
	    //else
	    //  propagate(l,p,tc->GetCharge());



	    if (pr!=0)
	    {
	    cout <<"2 --> ";
            cout <<"c("<<tc->Charge()<<")  vtx("<<p.X()<<"/"<<p.Y()<<"/"<<p.Z()<<")  p4("<<l.Px()<<"/"<<l.Py()<<"/"<<l.Pz()<<")="<<l.Vect().Mag()<<endl;
	    //printCand(l,pos);
	    cout <<endl;
	    }
	    tc->SetP4(l);
	    tc->SetPos(p);
	    //cout <<"2 -->"<<*tc<<endl;

	    //pdiff->Fill(p.Mag());

	    chargedCands2.Add(*tc);
	}

	for (i1=0; i1<fNeutCands->GetEntriesFast(); i1++){
	    tc = (TCandidate *)fNeutCands->At(i1);

	    double minang=10;

	    for (i2=0;i2<chargedCands.GetLength();i2++)
	    {
	      TVector3 gam=tc->P3();
	      TVector3 chrg=chargedCands[i2].Pos();

	      double ang=gam.Angle(chrg);

	      //pdiff2->Fill(gam.Angle(chrg), tc->P4().E());
	      if (minang>ang) minang=ang;

	    }
	    pdiff2->Fill(minang, tc->P4().E());
	    pdiff->Fill(tc->P4().E());
	    //tc->SetEnergy(tc->P4().E()*1.07);
	    if (minang>0.3)
	      neutralCands.Add(*tc);
	}

	for (i1=0; i1<fMcCands->GetEntriesFast(); i1++){
	    tc = (TCandidate *)fMcCands->At(i1);
	    mcCands.Add(*tc);
	}


	// **** select all the basic list
	//

	cout <<"c:"<<chargedCands.GetLength()<<" n:"<<neutralCands.GetLength()<<endl;

	nmult->Fill(neutralCands.GetLength());

	plusCands.Select(chargedCands2  ,plusSel);
	minusCands.Select(chargedCands2 ,minusSel);

	// **** pid selection
	//
	kpCands.Select(plusCands    ,kSel);
	kmCands.Select(minusCands   ,kSel);
	piCands.Select(chargedCands2 ,piSel);

	cout <<"chrg:"<<chargedCands.GetLength()<<" ";
	cout <<"plus:"<<plusCands.GetLength()<<" ";
	cout <<"minus:"<<minusCands.GetLength()<<" ";
	cout <<"K+:"<<kpCands.GetLength()<<" ";
	cout <<"K-:"<<kmCands.GetLength()<<" ";
	cout <<"pi+-:"<<piCands.GetLength()<<" ";
	//cout <<"pi-:"<<piminusCands.GetLength()<<endl;

	// **** now start combining all composits; inbetween plot masses
	//      before using the mass selectors
	//
	phiCands.Combine(kpCands,kmCands);

	//phiCands.Select(phiMSel);
	TCandListIterator iterPhi(phiCands);
	while (tc=iterPhi.Next()) phimass->Fill(tc->M());
	phiCands.Select(phiMSel);
	cout <<"phi:"<<phiCands.GetLength()<<" "<<endl;
	dsCands.Combine(phiCands,piCands);

	TCandListIterator iterDs(dsCands);
	while (tc=iterDs.Next())
	{
	  dsmass->Fill(tc->M());

	  TLorentzVector lds=tc->P4();
	  TLorentzVector ini(0,0,8.824,9.812);

	  TLorentzVector miss=ini-lds;
	  //ds0mass->Fill(miss.M());

	  pp2mass->Fill(miss.M()+lds.M());

	}
	dsCands.Select(dsMSel);

	pi0Cands.Combine(neutralCands,neutralCands);

	TCandListIterator iterPi0(pi0Cands);
	while (tc=iterPi0.Next()) pi0mass->Fill(tc->M());
	pi0Cands.Select(pi0MSel);

	ds0Cands.Combine(dsCands,pi0Cands);

	TCandListIterator iterDs0(ds0Cands);
	while (tc=iterDs0.Next()) ds0mass->Fill(tc->M());

	ppCands.Combine(ds0Cands,dsCands);
	//cout << ds0Cands.GetLength()<<" "<<dsCands.GetLength()<<" "<<ppCands.GetLength()<<endl;
	//ppCands.Select(neutralSel);

	// **** since we didn't care about the D_s charge, combinations might appear in two
	//      different ways; RemoveClones removes double candidates based on the same final states
	//ppCands.RemoveClones();

	TCandListIterator iterPp(ppCands);

	while (tc=iterPp.Next())
	{
	    ppmass->Fill(tc->M());
	    //printTree(tc);
	}

	TCandListIterator iterChrg(chargedCands);
	while(tc=iterChrg.Next())
	{
	    int idx=tc->GetMcIdx();
	    //if (idx!=-1) pdiff->Fill(tc->P()-mcCands[idx]->P());
	}


    }

    // **** plot all that stuff
    //

    c1->cd(1);
    phimass->Draw();
    c1->cd(2);
    pi0mass->Draw();
    c1->cd(3);
    dsmass->Draw();
    c1->cd(4);
    ds0mass->Draw();
    c1->cd(5);
    ppmass->Draw();
    //pdiff->Draw();
    c1->cd(6);
    pp2mass->Draw();
    //pdiff2->Draw("colz");
    //hpro->Draw();

    c1->cd();

    //timer.Stop();
    //Double_t rtime = timer.RealTime();
    //Double_t ctime = timer.CpuTime();

    //printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
      return 0;

}

/*void printTree(TCandidate *tc, int level=0)
{
  int i=0;
  int nd=tc->NDaughters();
  if (nd==0) return;
  cout <<tc->Uid()<<"("<<level<<") -> ";
  for (i=0;i<nd;i++) cout<<tc->Daughter(i)->Uid()<<"  ";
  cout <<endl;
  for (i=0;i<nd;i++) printTree(tc->Daughter(i),level+1);
  if (level==0) cout <<endl;
}
*/
