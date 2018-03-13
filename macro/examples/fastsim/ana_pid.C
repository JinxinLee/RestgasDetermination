#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"
#include "TCanvas.h"
#include "TStopwatch.h"

void config_histo(TH1F *h, TString tx, TString ty,double offy=1.65)
{
  h->SetLineWidth(1);
  h->SetLineColor(1);
  h->SetFillColor(3);
  //h->Sumw2();

  h->GetXaxis()->SetTitleOffset(1.3);
  h->GetXaxis()->SetTitleColor(1);
  h->GetXaxis()->SetLabelSize(0.05);
  h->GetXaxis()->SetTitleSize(0.05);
  h->GetXaxis()->SetNdivisions(505);

  h->GetYaxis()->SetTitleOffset(offy);
  h->GetYaxis()->SetTitleFont(42);
  h->GetYaxis()->SetLabelSize(0.05);
  h->GetYaxis()->SetTitleSize(0.05);

  h->SetXTitle(tx);
  h->SetYTitle(ty);

}


int ana_pid(TString fname="dsdsj_10k.root",int num=0)
{
  TStopwatch timer;
  timer.Start();

  int i;

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();


  gSystem->Load("libRho");
  /*
  TCanvas *c1=new TCanvas("c1","2-dim",1100 ,750);
  c1->Divide(3,2);

  TCanvas *c2=new TCanvas("c2","1-dim",1100 ,750);
  c2->Divide(3,2);

  TCanvas *c4=new TCanvas("c4","LH",750 ,750);
  c4->Divide(5,5);

  TCanvas *c3=new TCanvas("c3","mom+tht",600,350);
  c3->Divide(2,1);
*/

  TFile* f = new TFile(fname.Data());
  TTree *t=f->Get("pndsim") ;
  //TClonesArray *fTr=new TClonesArray("TParticle");
  //TClonesArray *fTrMc=new TClonesArray("TParticle");
  TClonesArray *fCands=new TClonesArray("TCandidate");

  //t->SetBranchAddress("PndParticles",&fTr) ;
  //t->SetBranchAddress("PndMcParticles",&fTrMc);
  t->SetBranchStatus("*",0);
  t->SetBranchStatus("PndChargedCandidates*",1);
  t->SetBranchAddress("PndChargedCandidates",&fCands);

  //TParticle  *tr1;
  //TParticle  *tr2;
  TCandidate *tc;


  TH1F *hmom= new TH1F("hmom","momentum charged tracks",100,0,6);
  TH1F *htht= new TH1F("htht","theta charged tracks",100,0,180);

  TH2F *thtcbar = new TH2F("thtcbar","#theta_{c} DrcBarrel",200,0,8,200,0.,0.9);
  thtcbar->SetMarkerSize(0.1);

  TH2F *thtcdsc = new TH2F("thtcdsc","#theta_{c} DrcDisc",200,0,8,200,0.0,0.9);
  thtcdsc->SetMarkerSize(0.1);

  TH2F *thtcrch = new TH2F("thtcrch","#theta_{c} Rich",200,0.,8,200,0.,0.35);
  thtcrch->SetMarkerSize(0.1);

  TH2F *m2tof = new TH2F("m2tof","m^{2} Tof",100,0,1.5,100,0.,1.2);
  m2tof->SetMarkerSize(0.1);

  TH2F *dedxmvd = new TH2F("dedxmvd","dE/dx Mvd",100,0,1.5,100,0.,20);
  dedxmvd->SetMarkerSize(0.1);

  TH2F *dedxstt = new TH2F("dedxstt","dE/dx Stt",100,0,1.5,100,0.,20);
  dedxstt->SetMarkerSize(0.1);

  TH1F *thtcB[5], *thtcD[5], *thtcR[5];
  TH1F *m2T[5], *dedxM[5],*dedxS[5];

  TH1F *LHe[5],*LHmu[5],*LHpi[5],*LHk[5],*LHp[5];

  int colors[5]={1,2,4,6,8};

  for (i=0;i<5;i++)
  {
    char tmp[50];
    sprintf(tmp,"%d",i);
    TString nums(tmp);
    thtcB[i]=new TH1F(("h1"+nums).Data(),("h1"+nums).Data(),100,0,0.85);
    thtcD[i]=new TH1F(("h2"+nums).Data(),("h2"+nums).Data(),100,0,0.85);
    thtcR[i]=new TH1F(("h3"+nums).Data(),("h3"+nums).Data(),100,0,0.85);

    m2T[i]=new TH1F(("h4"+nums).Data(),("h4"+nums).Data(),100,0,1.4);
    dedxM[i]=new TH1F(("h5"+nums).Data(),("h5"+nums).Data(),100,0,40);
    dedxS[i]=new TH1F(("h6"+nums).Data(),("h6"+nums).Data(),100,0,40);

    LHe[i]=new TH1F(("LHe"+nums).Data(),("LH for being electron"+nums).Data(),50,0,1);
    LHmu[i]=new TH1F(("LHmu"+nums).Data(),("LH for being muons"+nums).Data(),50,0,1);
    LHpi[i]=new TH1F(("LHpi"+nums).Data(),("LH for being pions"+nums).Data(),50,0,1);
    LHk[i]=new TH1F(("LHk"+nums).Data(),("LH for being kaons"+nums).Data(),50,0,1);
    LHp[i]=new TH1F(("LHp"+nums).Data(),("LH for being protons"+nums).Data(),50,0,1);

    LHe[i]->SetFillColor(colors[0]);
    LHmu[i]->SetFillColor(colors[1]);
    LHpi[i]->SetFillColor(colors[2]);
    LHk[i]->SetFillColor(colors[3]);
    LHp[i]->SetFillColor(colors[4]);

    thtcB[i]->SetLineColor(colors[i]);
    thtcR[i]->SetLineColor(colors[i]);
    thtcD[i]->SetLineColor(colors[i]);
    m2T[i]->SetLineColor(colors[i]);
    dedxM[i]->SetLineColor(colors[i]);
    dedxS[i]->SetLineColor(colors[i]);
  }

  if (num==0) num= t->GetEntriesFast();
  cout <<"\n####### Processing "<<num <<" events...\n"<<endl;

  TCandList allCands,neutralCands,chargedCands, plusCands,minusCands;

  TCandList kpCands,kmCands,piCands;

   TPidChargedSelector *chargedSel = new TPidChargedSelector;

  //  TPidSimpleKaonSelector *kSel    = new TPidSimpleKaonSelector();
  //  kSel->SetCriterion("loose");
  //  TPidSimplePionSelector *piSel   = new TPidSimplePionSelector();
  //  piSel->SetCriterion("loose");

  int i2;

  int pidhypo[2213];
  pidhypo[11]=0;
  pidhypo[13]=1;
  pidhypo[211]=2;
  pidhypo[321]=3;
  pidhypo[2212]=4;

  TFile *tf = new TFile(fname+"_ana.root","RECREATE");
  TNtuple *ntp=new TNtuple("ntp","ntp","pid:px:py:pz:e:p:pt:tht:ctht:le:lmu:lpi:lk:lp:tcb:tcd:tcr:mt:dem:des:det:tcbr:tcdr:tcrr:mtr:demr:desr:detr");


  for (Int_t j=0; j< num;j++)
  {
    t->GetEntry(j);
    if (!(j%100)) cout <<"evt:"<<j<<endl;

    //allCands.Cleanup();
    //chargedCands.Cleanup();

    for (Int_t i1=0; i1<fCands->GetEntriesFast(); i1++)
    {
      tc = (TCandidate *)fCands->At(i1);
      // allCands.Add(*tc);


      //chargedCands.Select(allCands, chargedSel);

      float valvect[28];

      double pmin=0.5;
      double pmax=0.7;

      double thtmin=0./180.*3.1415;
      double thtmax=180./180.*3.1415;

      //TCandListIterator iterch(chargedCands);
      //while (tc=iterch.Next()){
      double *pi=tc->GetPidInfo();
      double p=tc->P();
      double tht=tc->GetVect().Theta();

      double pt = p*sin(tht);
      int pid=abs((int)pi[29]);
      int hypo=pidhypo[pid];

      //  TNtuple *ntp=new TNtuple("ntp","ntp","pid:p:px:py:pz:e:pt:tht:ctht:le:lmu:lpi:lk:lp:tcb:tcd:tcr:mt:dem:des:det:tcbr:tcdr:tcrr:mtr:demr:desr:detr");
      valvect[0]=(float)hypo;
      valvect[1]=(float)p;
      valvect[2]=(float)tc->Px() ;
      valvect[3]=(float)tc->Py()  ;
      valvect[4]=(float)tc->Pz()  ;
      valvect[5]=(float)tc->E()  ;
      valvect[6]=(float)tc->Pt()  ;
      valvect[7]=(float)tht  ;
      valvect[8]=(float)cos(tht)  ;

      valvect[9]=(float)pi[0]  ; // LH e
      valvect[10]=(float)pi[1]  ;// LH mu
      valvect[11]=(float)pi[2]  ;// LH pi
      valvect[12]=(float)pi[3]  ;// LH K
      valvect[13]=(float)pi[4]  ;// LH p

      valvect[14]=(float)pi[5]  ;// tht_c barrel
      valvect[15]=(float)pi[6]  ;// tht_c disc
      valvect[16]=(float)pi[7]  ;// tht_c rich
      valvect[17]=(float)pi[8]  ;// m squared tof
      valvect[18]=(float)pi[9]  ;// dEdx mvd
      valvect[19]=(float)pi[10]  ;// dEdx stt
      valvect[20]=(float)pi[11]  ;// dEdx tpc

      valvect[21]=(float)pi[12]  ;// tht_c barrel error
      valvect[22]=(float)pi[13]  ;// tht_c disc error
      valvect[23]=(float)pi[14]  ;// tht_c rich error
      valvect[24]=(float)pi[15]  ;// m squared tof error
      valvect[25]=(float)pi[16]  ;// dEdx mvd error
      valvect[26]=(float)pi[17]  ;// dEdx stt error
      valvect[27]=(float)pi[18]  ;// dEdx tpc error

      ntp->Fill(valvect);
    }


  }


  ntp->Write();
  tf->Close();

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  return 0;
}
