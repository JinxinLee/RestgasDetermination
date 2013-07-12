#include "TString.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TH2.h"
using namespace std;
void AnaTrkCutsAndSecondaries(TString path="/panda/pandaroot/macro/lmd/secondaries/beampipe_201303/BOX/"){
  gStyle->SetOptStat("e");
  TString finname = path+"/Lumi_Track_0_1_5GeV_Large_2_11mrad_secondariesFullLMD.root";
  TFile * fin = new TFile(finname,"READ");
  fin->ls();
  TNtuple *ntgthetatgphiFit = (TNtuple*)fin->Get("htthetatphiTrkFit");
  Long64_t  nAll = ntgthetatgphiFit->GetEntries();//trk
  Long64_t nLeftTFitCut = ntgthetatgphiFit ->GetEntries("abs(tg_theta-0.0400)<=11 && abs(tg_phi)<=0.250");
  fin->Close();
  TString fin2name = path+"/Lumi_TCand_0_1_5GeV_Large_2_11mrad_secondariesFullLMD.root";
  TFile * fin2 = new TFile(fin2name,"READ");
  TNtuple *ntgthetatgphi2 = (TNtuple*)fin2->Get("htthetatphiTrk");
  TNtuple *ntgthetatgphiC = (TNtuple*)fin2->Get("htthetatphiCells");
  Long64_t  nAll2 = ntgthetatgphi2->GetEntries();//trk-cand
  Long64_t nLeftTCandCut = ntgthetatgphi2 ->GetEntries("abs(tg_theta-0.0400)<=11 && abs(tg_phi)<=0.250");
  Long64_t  nAllC = ntgthetatgphiC->GetEntries();
  Long64_t nLeftTCandCellsCut = ntgthetatgphiC ->GetEntries("abs(tg_theta-0.0400)<=11 && abs(tg_phi)<=0.250");
  fin2->Close();
  cout<<" --------- Summary ----------- "<<endl;
  cout<<"There are "<<nAll<<" fitted(filtered) trks from "<<nAll2<<" trk-cands form "<<nAllC<<" cells"<<endl;
  cout<<"Cut 11 mrad & 250 mrad has left:"<<endl;
  double effTfit = 100.*(double(nLeftTFitCut)/double(nAll));
  double erreffTfit = 100.*sqrt(double(nLeftTFitCut)*(1-nLeftTFitCut/double(nAll)))/nAll;
  //  cout<<"After fit&filter "<<effTfit<<" +\- "<<erreffTfit<<endl;
  double effTCand = 100.*(double(nLeftTCandCut)/double(nAll2));
  double erreffTCand = 100.*sqrt(double(nLeftTCandCut)*(1-nLeftTCandCut/double(nAll2)))/nAll2;
  //  cout<<"After TrkSearch "<<effTCand<<" +\- "<<erreffTCand<<endl;
  double effTCandCell = 100.*(double(nLeftTCandCellsCut)/double(nAllC));
  double erreffTCandCell = 100.*sqrt(double(nLeftTCandCellsCut)*(1-nLeftTCandCellsCut/double(nAllC)))/nAllC;
  //  cout<<"After TrkSearch Cells "<<effTCandCell<<" +\- "<<erreffTCandCell<<endl;
  TString fin3name = path+"/Lumi_MC_and_REC_trks_matches_CA_1trks_trkCandCutfalse_psi0_000005_2_11mrad_secondariesFullLMD_1_5GeV.root";
  TFile * fin3 = new TFile(fin3name,"READ");
  TTree *trecmcinfo = (TTree*)fin3->Get("tRECMC");
  Long64_t numSim = trecmcinfo->GetEntries();
  Long64_t numSecondaries = trecmcinfo->GetEntries("trkstMC>0 && trkstREC<0 && abs(trkstMC)<1000");
  Long64_t numRec = trecmcinfo->GetEntries("trkstREC>=0 && abs(trkstMC)<1000");
  Long64_t numRecAfterCut = trecmcinfo->GetEntries("trkstREC>=0 && abs(trkstMC)<1000 && abs(thetarecLMD-0.040)<11 && abs(phirecLMD)<0.25");
  Long64_t numRecGood = trecmcinfo->GetEntries("trkstREC==0 && trkstMC==0");
  Long64_t numRecGoodAfterCut = trecmcinfo->GetEntries("trkstREC==0 && trkstMC==0 && abs(thetarecLMD-0.040)<11 && abs(phirecLMD)<0.25");
  Long64_t numNotBP = trecmcinfo->GetEntries("trkstREC==-10");
  Long64_t numSecondariesRec = trecmcinfo->GetEntries("trkstMC>0 && trkstREC>=0");
  Long64_t numSecondariesRecAfterCut = trecmcinfo->GetEntries("trkstMC>0 && trkstREC>=0 && abs(thetarecLMD-0.040)< 11 && abs(phirecLMD)<0.25");
  TH1D *hthetacut = new TH1D("hthetacut",";#theta_{REC}, mrad",1e2,0,1e2);
  trecmcinfo->Project("hthetacut","1e3*thetarec","trkstMC==0 && trkstREC>=0 && (abs(thetarecLMD-0.040)> 11 || abs(phirecLMD)>0.25)");
  TCanvas c1;
  hthetacut->Draw();
  TString outfile =  path+"theta_good_trk_cut_1_5GeV.root";
  c1.SaveAs(outfile);
  Long64_t  numScattPrim = trecmcinfo->GetEntries("trkstMC==0 && zmc!=0");
  Long64_t  numScattPrimRec = trecmcinfo->GetEntries("trkstMC==0 && zmc!=0 && (trkstREC>-1 || trkstREC==-10)");

  fin3->Close();
  cout<<" numSim = "<<numSim<<" numRec = "<<numRec<<" numRecGood = "<<numRecGood<<" numNotBP = "<<numNotBP
      <<" numSecondariesRec = "<<numSecondariesRec <<" numSecondariesRecAfterCut = "<<numSecondariesRecAfterCut<<" numScattPrim = "<<numScattPrim
      <<" numScattPrimRec = "<<numScattPrimRec<<endl;
  double effTfitAll = 100.*(double(numRec)/double(numRecGood));
  double erreffTfitAll = 100.*sqrt(double(numRec-numRecGood)*(1-(numRec-numRecGood)/double(numRecGood)))/numRecGood;
 

  double effTfitGood = 100.*(double(numRecAfterCut)/double(numRecGood));
  double erreffTfitGood = 100.*sqrt(double(numRec-numRecAfterCut)*(1-(numRec-numRecAfterCut)/double(numRecGood)))/numRecGood;

  // double secondfrac = 100.*double(numSecondaries)/numSim;
  // double errsecondfrac = 100.*sqrt(double(numSecondaries)*(1-numSecondaries/double(numSim)))/numSim;
  // cout<<"Secondaries: "<< secondfrac<<" +/- "<<errsecondfrac <<endl;

  double secondfracRec = 100.*double(numSecondariesRec)/numRecGood;
  double errsecondfracRec = 100.*sqrt(double(numSecondariesRec)*(1-numSecondariesRec/double(numRecGood)))/numRecGood;

  double secondfracRecCut = 100.*double(numSecondariesRecAfterCut)/numRecGood;
  double errsecondfracRecCut = 100.*sqrt(double(numSecondariesRecAfterCut)*(1-numSecondariesRecAfterCut/double(numRecGood)))/numRecGood;

  double scattPrim = 100.*double(numScattPrim)/numSim;
  double errscattPrim = 100.*sqrt(double(numScattPrim)*(1-numScattPrim/double(numSim)))/numSim;
  double scattPrimRec = 100.*double(numScattPrimRec)/numRecGood;
  double errscattPrimRec = 100.*sqrt(double(numScattPrimRec)*(1-numScattPrimRec/double(numRecGood)))/numRecGood;
  //  cout<<"Scattered Primaries rec: "<<scattPrimRec<<" +/- "<<errscattPrimRec<<endl;
  double primaryfracgoodafterCut = 100.*double(numRecGoodAfterCut)/numRecGood;
  double errprimaryfracgoodafterCut = 100.*sqrt(double(numRecGoodAfterCut)*(1-numRecGoodAfterCut/double(numRecGood)))/numRecGood;
  //  cout<<"Cut on good trks: "<<primaryfracgoodafterCut<<" +/- "<<errprimaryfracgoodafterCut <<endl;

  cout<<"fit&filter "<<effTfitAll<<" +\- "<<erreffTfitAll<<endl;
  cout<<"Secondaries rec: "<< secondfracRec<<" +/- "<<errsecondfracRec<<endl;
  cout<<"Cut on all rec. trks: "<<effTfitGood<<" +\- "<<erreffTfitGood<<endl;
  cout<<"Cut on bad trks: "<< secondfracRecCut<<" +/- "<<errsecondfracRecCut<<endl;
  cout<<"Cut on good trks: "<<primaryfracgoodafterCut<<" +/- "<<errprimaryfracgoodafterCut <<endl;
 
  //  c1.Setxlog(1);
  //  c1.Setylog(1);
}
