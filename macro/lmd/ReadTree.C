#define ReadTree_cxx
#include "ReadTree.h"
#include <TH2.h>
#include <TVector.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TLatex.h>
#include <TVector3.h>
using namespace std;
//   In a ROOT session, you can do:
//      Root > .L ReadTree.C
//      Root > ReadTree t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

void ReadTree::Loop()
{
  TString in="tmpOutput/ThetaRes.root";
  fout = new TFile(in,"RECREATE");
  cout<<in<<endl;
  //     This is the loop skeleton where:
  //    jentry is the global entry number in the chain
  //    ientry is the entry number in the current Tree
  //  Note that the argument to GetEntry must be:
  //    jentry for TChain::GetEntry
  //    ientry for TTree::GetEntry and TBranch::GetEntry
  //
  //       To read only selected branches, Insert statements like:
  // METHOD1:
  //    fChain->SetBranchStatus("*",0);  // disable all branches
  //    fChain->SetBranchStatus("branchname",1);  // activate branchname
  // METHOD2: replace line
  //    fChain->GetEntry(jentry);       //read all branches
  //by  b_branchname->GetEntry(ientry); //read only this branch
  if (fChain == 0) return;
  
  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"ReadTree::Loop() nentries="<<nentries<<endl;

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    //  cout<<"ientry = "<<ientry<<endl;
    if (ientry < 0) continue;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    //MC
    b_PxMC->GetEntry(ientry);
    b_PyMC->GetEntry(ientry);
    b_PzMC->GetEntry(ientry);
    double deltapx = fPx-fPxMC;  double deltapy = fPy-fPyMC;  double deltapz = fPz-fPzMC;
    ResmomX->Fill(deltapx);
    ResmomY->Fill(deltapy);
    ResmomZ->Fill(deltapz);

    TVector3 *fMomMC = new TVector3(0,0,0);
    fMomMC->SetX(fPxMC);  fMomMC->SetY(fPyMC);  fMomMC->SetZ(fPzMC);

    //  if(fabs(fMomMC.Mag()-fMomRec.Mag())>5e-08) continue; ///TEST!!!

    Double_t fthetaMC = fMomMC->Theta();
    Double_t fphiMC = fMomMC->Phi();
    if(fphiMC<0) fphiMC+=2*TMath::Pi();
    //  if(fabs(fthetaMC-0.45)<1e-3 || fphiMC>1e-3) continue;
    
    //REC
    // //read braches with vertex position and momentum in vertex
    b_Xv->GetEntry(ientry);
    vertexX->Fill(fXv);
    b_Yv->GetEntry(ientry);
    vertexY->Fill(fYv);
    b_Zv->GetEntry(ientry);
    vertexZ->Fill(fZv);

    b_errXv->GetEntry(ientry);
    ErrvertexX->Fill(ferrXv);
    b_errYv->GetEntry(ientry);
    ErrvertexY->Fill(ferrYv);
    b_errZv->GetEntry(ientry);
    ErrvertexZ->Fill(ferrZv);

    b_Px->GetEntry(ientry);
    momX->Fill(fPx);
    b_Py->GetEntry(ientry);
    momY->Fill(fPy);
    b_Pz->GetEntry(ientry);
    momZ->Fill(fPz);

    b_errPx->GetEntry(ientry);
    b_errPy->GetEntry(ientry);
    b_errPz->GetEntry(ientry);
    ErrmomX->Fill(ferrPx);
    ErrmomY->Fill(ferrPy);
    ErrmomZ->Fill(ferrPz);
    TVector3 *fMomRec = new TVector3(0,0,0);
    fMomRec->SetX(fPx); fMomRec->SetY(fPy); fMomRec->SetZ(fPz);


    Double_t fthetaRec = fMomRec->Theta();
    Double_t fphiRec = fMomRec->Phi();
    if(fphiRec<0) fphiRec+=2*TMath::Pi();

    dTheta->Fill(fthetaMC,fthetaRec-fthetaMC);
    dPhi->Fill(fphiMC,fphiRec-fphiMC);
    dThetavsPhi->Fill(fphiMC,fthetaRec-fthetaMC);
    ThetavsPhi->Fill(fphiRec,fthetaRec);
    ThetavsPhiMC->Fill(fphiMC,fthetaMC);
    dThetavsdPhi->Fill(fphiRec-fphiMC,fthetaRec-fthetaMC);
     
    //   cout<<"fMomMC.Mag() = "<<fMomMC->Mag()<<endl;
    dMomMomMC->Fill(fMomMC->Mag(),fMomMC->Mag()-fMomRec->Mag());

    sigmaPxvsPhi->Fill(fphiRec,ferrPx);
    sigmaPxvsTheta->Fill(fthetaRec,ferrPx);
    sigmaXvsPhi->Fill(fphiRec,ferrXv);
    sigmaXvsTheta->Fill(fthetaRec,ferrXv);
    sigmaPxvssigmaX->Fill(ferrXv,ferrPx);
   
  }
  
  // ThetaRecvsThetaMC->Write();
  //  PhiRecvsPhiMC->Write();
  dTheta->Write();
  dPhi->Write();
  dThetavsPhi->Write();
  ThetavsPhi->Write();
  ThetavsPhiMC->Write();
  dThetavsdPhi->Write();

  vertexX->Write(); vertexY->Write(); vertexZ->Write();
  ErrvertexX->Write();ErrvertexY->Write();ErrvertexZ->Write();
  momX->Write();momY->Write();momZ->Write();
  ResmomX->Write();ResmomY->Write();ResmomZ->Write();
  ErrmomX->Write();ErrmomY->Write();ErrmomZ->Write();
  dMomMomMC->Write();
  sigmaPxvsPhi->Write();
  sigmaPxvsTheta->Write();
  sigmaXvsPhi->Write();
  sigmaXvsTheta->Write();
  sigmaPxvssigmaX->Write();
  fout->Write();
}
