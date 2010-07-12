#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TCanvas.h>
#include <ReadTree.h>
#include <sstream>
using namespace std;

void TestThetaRes(){
  

  ReadTree *ktree = new ReadTree();
  ktree->Loop();
  TFile *f = new TFile("tmpOutput/ThetaRes.root","READ");
  // TH2F *fThetaRec;
  // TH1F *fThetaRecMean;
  // TH1F *fThetaRecSigma;
  // fThetaRec = (TH2F*)gDirectory->Get("ThetaRec");
  // // std::cout<<"ThetaRec is succsefully read"<<std::endl;
  // fThetaRec->FitSlicesY();
  // fThetaRecMean = (TH1F*)gDirectory->Get("ThetaRec_1");
  // fThetaRecSigma = (TH1F*)gDirectory->Get("ThetaRec_2");
  
  TH2F *fdiffTheta;
  TH1F *fdiffThetaMean;
  TH1F *fdiffThetaSigma;
  fdiffTheta = (TH2F*)gDirectory->Get("diffTheta");
  // std::cout<<"ThetaRec is succsefully read"<<std::endl;
  fdiffTheta->FitSlicesY();
  fdiffThetaMean = (TH1F*)gDirectory->Get("diffTheta_1");
  fdiffThetaSigma = (TH1F*)gDirectory->Get("diffTheta_2");
  
  TH2F *fdiffThetaPhi;
  TH1F *fdiffThetaPhiMean;
  TH1F *fdiffThetaPhiSigma;
  fdiffThetaPhi = (TH2F*)gDirectory->Get("diffThetaPhi");
  fdiffThetaPhi->FitSlicesY();
  fdiffThetaPhiMean = (TH1F*)gDirectory->Get("diffThetaPhi_1");
  fdiffThetaPhiSigma = (TH1F*)gDirectory->Get("diffThetaPhi_2");

  // TH2F *fThetaPhi;
  // fThetaPhi = (TH2F*)gDirectory->Get("ThetaPhi");
  // TH2F *fThetaPhiMC;
  // fThetaPhiMC = (TH2F*)gDirectory->Get("ThetaPhiMC");
  // TH2F *fdThetadPhi;
  // fdThetadPhi = (TH2F*)gDirectory->Get("diffThetadiffPhi");

  // TH2F *fPhiRec;
  // TH1F *fPhiRecMean;
  // TH1F *fPhiRecSigma;
  // fPhiRec = (TH2F*)gDirectory->Get("PhiRec");
  // // std::cout<<"PhiRec is succsefully read"<<std::endl;
  // fPhiRec->FitSlicesY();
  // fPhiRecMean = (TH1F*)gDirectory->Get("PhiRec_1");
  // fPhiRecSigma = (TH1F*)gDirectory->Get("PhiRec_2");
  
  TH2F *fdiffPhi;
  TH1F *fdiffPhiMean;
  TH1F *fdiffPhiSigma;
  fdiffPhi = (TH2F*)gDirectory->Get("diffPhi");
  // std::cout<<"PhiRec is succsefully read"<<std::endl;
  fdiffPhi->FitSlicesY();
  fdiffPhiMean = (TH1F*)gDirectory->Get("diffPhi_1");
  fdiffPhiSigma = (TH1F*)gDirectory->Get("diffPhi_2");
  

  // TCanvas *c1 = new TCanvas();
  // c1->Divide(2,2);
  // // c1->cd(1);
  // // fThetaRec->Draw();
  // // c1->cd(2);
  // // fThetaRecMean->Draw();
  // // c1->cd(3);
  // // fThetaRecSigma->Draw();
  // // c1->cd(4);
  // // fdiffTheta->Draw();
  
  // c1->cd(1);
  // fdiffTheta->Draw();
  // c1->cd(2);
  // fdiffPhi->Draw();
  // // fdiffThetaMean->Draw();
  // // fdiffThetaPhi->Draw();
  // c1->cd(3);
  // //  fThetaPhiMC->Draw();
  // //  fThetaPhi->Draw();
  // // fdiffThetaSigma->Draw();
  // c1->cd(4);
  // // fdThetadPhi->Draw();
  

  // fdiffThetaPhiMean->Draw();

 
  // TCanvas *c2 = new TCanvas();
  // c2->Divide(2,2);
  // // c2->cd(1);
  // // fPhiRec->Draw();
  // // c2->cd(2);
  // // fPhiRecMean->Draw();
  // // c2->cd(3);
  // // fPhiRecSigma->Draw();
  
  // c2->cd(1);
  // // fdiffPhi->Draw();
  // c2->cd(2);
  // // fdiffPhiMean->Draw();
  // c2->cd(3);
  // //  fdiffPhiSigma->Draw();



  TH1F *fPx,*fPy,*fPz;
  TH1F *ferrPx,*ferrPy,*ferrPz;
  TH1F *fVertx,*fVerty,*fVertz;
  TH1F *ferrVertx,*ferrVerty,*ferrVertz;

  fPx =  (TH1F*)gDirectory->Get("MomX");
  fPy =  (TH1F*)gDirectory->Get("MomY");
  fPz =  (TH1F*)gDirectory->Get("MomZ");
  ferrPx =  (TH1F*)gDirectory->Get("ErrMomX");
  ferrPy =  (TH1F*)gDirectory->Get("ErrMomY");
  ferrPz =  (TH1F*)gDirectory->Get("ErrMomZ");

  fVertx =  (TH1F*)gDirectory->Get("VertexX");
  fVerty =  (TH1F*)gDirectory->Get("VertexY");
  fVertz =  (TH1F*)gDirectory->Get("VertexZ");
  ferrVertx =  (TH1F*)gDirectory->Get("ErrVertexX");
  ferrVerty =  (TH1F*)gDirectory->Get("ErrVertexY");
  ferrVertz =  (TH1F*)gDirectory->Get("ErrVertexZ");

  TH1F *fresPx,*fresPy,*fresPz;
  fresPx = (TH1F*)gDirectory->Get("ResMomX");
  fresPy = (TH1F*)gDirectory->Get("ResMomY");
  fresPz = (TH1F*)gDirectory->Get("ResMomZ");

  TCanvas *c3 = new TCanvas();
  c3->Divide(3,3);

  c3->cd(1);
  fPx->Draw();
  c3->cd(2);
  fPy->Draw();
  c3->cd(3);
  fPz->Draw();
  c3->cd(4);
  ferrPx->Draw();
  c3->cd(5);
  ferrPy->Draw();
  c3->cd(6);
  ferrPz->Draw();
  c3->cd(7);
  fresPx->Draw();
  c3->cd(8);
  fresPy->Draw();
  c3->cd(9);
  fresPz->Draw();

  TCanvas *c4 = new TCanvas();
  c4->Divide(3,2);

  c4->cd(1);
  fVertx->Draw();
  c4->cd(2);
  fVerty->Draw();
  c4->cd(3);
  fVertz->Draw();
  c4->cd(4);
  ferrVertx->Draw();
  c4->cd(5);
  ferrVerty->Draw();
  c4->cd(6);
  ferrVertz->Draw();

  TH2F *fdiffMom;
  fdiffMom =  (TH2F*)gDirectory->Get("dMom");

  TCanvas *c5 = new TCanvas();
  c5->Divide(2,2);
  c5->cd(1);
  fdiffMom->Draw();
  // fdiffMom->SetDrawOption("color");
  c5->cd(2);
  fdiffPhi->Draw();
  // fdiffPhi->SetDrawOption("color");
  c5->cd(3);
  fdiffTheta->Draw();
  // fdiffTheta->SetDrawOption("color");
  c5->cd(4);
  TPaveText *pt = new TPaveText(0.02,0.85,0.98,0.98);
  pt->AddText("Dipole+Solenoid+Trans Magnetic Filed");
  // pt->AddText("In absence of Magnetic Filed");
  pt->Draw();

  TCanvas *c6 = new TCanvas();
  c6->Divide(3,2);
  c6->cd(1);
  fdiffTheta->SetDrawOption("color");
  fdiffTheta->Draw();
  c6->cd(2);
  fdiffThetaMean->Draw();
  c6->cd(3);
  fdiffThetaSigma->Draw();
  c6->cd(4);
  fdiffPhi->SetDrawOption("color");
  fdiffPhi->Draw();
  c6->cd(5);
  fdiffPhiMean->Draw();
  c6->cd(6);
  fdiffPhiSigma->Draw();

  TH2F *fsPxPhi,*fsPxTheta;
  fsPxPhi =  (TH2F*)gDirectory->Get("sPxPhi");
  fsPxTheta =  (TH2F*)gDirectory->Get("sPxTheta");

  TH2F *fsXPhi,*fsXTheta;
  fsXPhi =  (TH2F*)gDirectory->Get("sXPhi");
  fsXTheta =  (TH2F*)gDirectory->Get("sXTheta");

  TH2F *fsPxX;
  fsPxX = (TH2F*)gDirectory->Get("sPxX");
  TCanvas *c7 = new TCanvas();
  c7->Divide(3,2);
  c7->cd(1);
  fsPxPhi->Draw();
  fsPxPhi->SetDrawOption("color");
  c7->cd(2);
  fsPxTheta->Draw();
  fsPxTheta->SetDrawOption("color");
  c7->cd(3);
  fsXPhi->Draw();
  fsXPhi->SetDrawOption("color");
  c7->cd(4);
  fsXTheta->Draw();
  fsXTheta->SetDrawOption("color");
  c7->cd(5);
  fsPxX->Draw();
  // fsPxX->SetDrawOption("color");
  // ---- Output file ----------------------------------------------------------------
  TFile *fPlots = new TFile("tmpOutput/Plots.root","RECREATE");
  // ---------------------------------------------------------------------------------
  //  c1->Write(); 
  // c2->Write();  
  c3->Write();  c4->Write(); c5->Write(); c6->Write(); c7->Write();
  fPlots->Close();
}
