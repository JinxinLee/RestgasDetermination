//################################################################
//# Macros for Background study
//# !need sum file with generator information
//# author: Anastasia Karavdina
//# date: Jan, 2013
//#
//# to compile it add in "# install #" part of CMakeLists.txt lines:
//# add_executable(bkg_gen_info BkgStudies_GenInfo_exe.C)
//# target_link_libraries(bkg_gen_info ${ROOT_LIBRARIES})
//#
//# to run it (and see options): 
//# ${PANDAROOT}/build/bin/./bkg_gen_info --help
//################################################################

#include <iostream>
#include <sstream>

#include<TApplication.h>
#include<TCanvas.h>
#include<TROOT.h>
#include<TString.h>
#include<TChain.h>
#include<TFile.h>
#include<TClonesArray.h>
#include<TSystem.h>
#include<TH1.h>
#include<TH2.h>
#include<TRotation.h>
#include<TVector3.h>
#include<TMath.h>
#include<TGaxis.h>
#include<TNtuple.h>

#include<TStopwatch.h>

#include<PndMCTrack.h>
#include<PndSdsMCPoint.h>
#include<string>

using namespace std;
int main(int __argc,char *__argv[]) {
  bool fillBkgTree = true;
  //TODO: read this like params!
  TString storePath="/data/FAIRsorf/pandaroot/trunk/macro/lmd/tmpOutputBkg";
  int verboseLevel=0;
  double pmom=0;
  int finp=0;
  std::string pathStr="", momStr="", finStr="";
  // decode arguments
  if( __argc>1 && ( strcmp( __argv[1], "-help" ) == 0
		    || strcmp( __argv[1], "--help" ) == 0 ) ){
    
    std::cout << "This is Background study script with parameters\n"
	      // <<"-s start event \n"
	      // <<"-n Number of events \n"
	      <<"-mom Beam Momentum \n"
	      <<"-path path to the file \n"
	      <<"-fin sim of rec file: 0=sim, 1=rec \n"
      //	      <<"-v verbose Level (if>0, print out some information)"
	      <<"Have fun! \n"
	      << std::endl;
    return 0;
  } 
  while ((optind < (__argc-1) ) && (__argv[optind][0]=='-')) {
    bool found=false;
    std::string sw = __argv[optind];
    if (sw=="-fin") {
      optind++;
      finStr = __argv[optind];
      found=true;
    }
    // if (sw=="-n"){
    //   optind++;
    //   nStr = __argv[optind];
    //   found=true;
    // }
    if (sw=="-path"){
      optind++;
      pathStr = __argv[optind];
      found=true;
    }
    if (sw=="-mom"){
      optind++;
      momStr = __argv[optind];
      found=true;
    }
    // if (sw=="-v"){
    //   optind++;
    //   verbStr = __argv[optind];
    //   found=true;
    // }
   
    if (!found){
      std::cout<< "Unknown switch: "
	       << __argv[optind] <<std::endl;
      optind++;
    }
  
  while ( (optind < __argc ) && __argv[optind][0]!='-' ) optind++; 
  }

  std::stringstream pathSStr(pathStr), momSStr(momStr),finSStr(finStr);
  pathSStr >> storePath;
  momSStr >> pmom;
  finSStr >> finp;
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libLmdTrk");
  // gROOT->LoadMacro("line3Dfit.C");
  // ------------------------------------------------------------------------
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  
  // ---- Input files --------------------------------------------------------
  TString sumSimFileName = storePath;
  if(finp>0)
    sumSimFileName +="/sumAll.root";
  else
    sumSimFileName +="/sum_MC_All.root";
  //TString sumSimFileName = storePath;
  TFile *fin = new TFile(sumSimFileName,"READ");
  TNtuple *ntupMCTrk = (TNtuple*)fin->Get("ntupMCTrk");

  // ---------------------------------------------------------------------------------
  
  // ---- Output file ----------------------------------------------------------------
  TString out=storePath;
  if(finp>0)
    out+="/sumAll_MCforREC_plots.root";
  else
    out+="/sum_MC_All_plots.root";
  TFile *f = new TFile(out,"RECREATE");
  // ---------------------------------------------------------------------------------
  
  //--- Hehe we go! -----------------------------------------------------
  TCanvas *celyz = new TCanvas("eYZ");
  celyz->Divide(3,2);
  celyz->cd(1);
  TH2F* hemYZ = new TH2F("hemYZ","e^{-};z, cm;y, cm",2400,0,1200,400,-100,100);
  ntupMCTrk->Project("hemYZ","y:z","pdgid==11");
  hemYZ->Draw("colz");
  celyz->cd(2);
  TH2F* hemYZ_small = new TH2F("hemYZ_small","e^{-};z, cm;y, cm",2400,1000,1200,400,-100,100);
  ntupMCTrk->Project("hemYZ_small","y:z","pdgid==11");
  hemYZ_small->Draw("colz");
  celyz->cd(3);
  TH2F* hemYZmom_small = new TH2F("hemYZmom_small","e^{-};z, cm;mom, GeV/c",2400,1000,1200,1e3,0,pmom+1.1);
  ntupMCTrk->Project("hemYZmom_small","mom:z","pdgid==11");
  hemYZmom_small->Draw("colz");

  celyz->cd(4);
  TH2F* hepYZ = new TH2F("hepYZ","e^{+};z, cm;y, cm",2400,0,1200,400,-100,100);
  ntupMCTrk->Project("hepYZ","y:z","pdgid==-11");
  hepYZ->Draw("colz");
  celyz->cd(5);
  TH2F* hepYZ_small = new TH2F("hepYZ_small","e^{+};z, cm;y, cm",2400,1000,1200,400,-100,100);
  ntupMCTrk->Project("hepYZ_small","y:z","pdgid==-11");
  hepYZ_small->Draw("colz");
  celyz->cd(6);
  TH2F* hepYZmom_small = new TH2F("hepYZmom_small","e^{+};z, cm;mom, GeV/c",2400,1000,1200,1e3,0,pmom+1.1);
  ntupMCTrk->Project("hepYZmom_small","mom:z","pdgid==-11");
  hepYZmom_small->Draw("colz");
  //  cSumTheta->SetLogy();

  TCanvas *cpilyz = new TCanvas("piYZ");
  cpilyz->Divide(3,2);
  cpilyz->cd(1);
  TH2F* hpimYZ = new TH2F("hpimYZ","pi^{-};z, cm;y, cm",2400,0,1200,400,-100,100);
  ntupMCTrk->Project("hpimYZ","y:z","pdgid==-211");
  hpimYZ->Draw("colz");
  cpilyz->cd(2);
  TH2F* hpimYZ_small = new TH2F("hpimYZ_small","pi^{-};z, cm;y, cm",2400,1000,1200,400,-100,100);
  ntupMCTrk->Project("hpimYZ_small","y:z","pdgid==-211");
  hpimYZ_small->Draw("colz");
  cpilyz->cd(3);
  TH2F* hpimYZmom_small = new TH2F("hpimYZmom_small","pi^{-};z, cm;mom, GeV/c",2400,1000,1200,1e3,0,pmom+1.1);
  ntupMCTrk->Project("hpimYZmom_small","mom:z","pdgid==-211");
  hpimYZmom_small->Draw("colz");

  cpilyz->cd(4);
  TH2F* hpipYZ = new TH2F("hpipYZ","pi^{+};z, cm;y, cm",2400,0,1200,400,-100,100);
  ntupMCTrk->Project("hpipYZ","y:z","pdgid==211");
  hpipYZ->Draw("colz");
  cpilyz->cd(5);
  TH2F* hpipYZ_small = new TH2F("hpipYZ_small","pi^{+};z, cm;y, cm",2400,1000,1200,400,-100,100);
  ntupMCTrk->Project("hpipYZ_small","y:z","pdgid==211");
  hpipYZ_small->Draw("colz");
  cpilyz->cd(6);
  TH2F* hpipYZmom_small = new TH2F("hpipYZmom_small","pi^{+};z, cm;mom, GeV/c",2400,1000,1200,1e3,0,pmom+1.1);
  ntupMCTrk->Project("hpipYZmom_small","mom:z","pdgid==211");
  hpipYZmom_small->Draw("colz");
  //  cSumTheta->SetLogy();


  TCanvas *cnyz = new TCanvas("nYZ");
  cnyz->Divide(3,2);
  cnyz->cd(1);
  TH2F* hnYZ = new TH2F("hnYZ","pi^{-};z, cm;y, cm",2400,0,1200,400,-100,100);
  ntupMCTrk->Project("hnYZ","y:z","abs(pdgid)==2112");
  hnYZ->Draw("colz");
  cnyz->cd(2);
  TH2F* hnYZ_small = new TH2F("hnYZ_small","pi^{-};z, cm;y, cm",2400,1000,1200,400,-100,100);
  ntupMCTrk->Project("hnYZ_small","y:z","abs(pdgid)==2112");
  hnYZ_small->Draw("colz");
  cnyz->cd(3);
  TH2F* hnYZmom_small = new TH2F("hnYZmom_small","pi^{-};z, cm;mom, GeV/c",2400,1000,1200,1e3,0,pmom+1.1);
  ntupMCTrk->Project("hnYZmom_small","mom:z","abs(pdgid)==2112");
  hnYZmom_small->Draw("colz");

  cnyz->cd(4);
  TH2F* hprotonYZ = new TH2F("hprotonYZ","pi^{+};z, cm;y, cm",2400,0,1200,400,-100,100);
  ntupMCTrk->Project("hprotonYZ","y:z","abs(pdgid)==2212");
  hprotonYZ->Draw("colz");
  cnyz->cd(5);
  TH2F* hprotonYZ_small = new TH2F("hprotonYZ_small","pi^{+};z, cm;y, cm",2400,1000,1200,400,-100,100);
  ntupMCTrk->Project("hprotonYZ_small","y:z","abs(pdgid)==2212");
  hprotonYZ_small->Draw("colz");
  cnyz->cd(6);
  TH2F* hprotonYZmom_small = new TH2F("hprotonYZmom_small","pi^{+};z, cm;mom, GeV/c",2400,1000,1200,1e3,0,pmom+1.1);
  ntupMCTrk->Project("hprotonYZmom_small","mom:z","abs(pdgid)==2212");
  hprotonYZmom_small->Draw("colz");
  //  cSumTheta->SetLogy();


 TCanvas *cpdgid = new TCanvas("pdgid");
 // cpdgid->Divide(1,2);
 // cpdgid->cd(1);
 // TH1F* hpdgid = new TH1F("hpdgid","PDG_{ID};ID;# events",1e4,-5e3,5e3);
 // ntupMCTrk->Project("hpdgid","pdgid");
 // cpdgid->SetLogy();
 // hpdgid->Draw();
 // cpdgid->cd(2);
TH2F* hpdgidmom = new TH2F("hpdgidmom","PDG_{ID} vs. momentum;p, GeV/c;ID;# events",1e3,0,pmom+1.1,1e4,-5e3,5e3);
 ntupMCTrk->Project("hpdgidmom","pdgid:mom");
 hpdgidmom->Draw("colz");

 
 // TH2F* hpdgidmom1 = new TH2F("hpdgidmom1","PDG_{ID} vs. momentum;p, GeV/c;ID;# events",1e3,0,pmom+1.1,1e3,-5e2,5e2);
 // ntupMCTrk->Project("hpdgidmom1","pdgid:mom");
 // hpdgidmom1->Draw("colz");
 // cpdgid->cd(3);
 // TH2F* hpdgidmom2 = new TH2F("hpdgidmom2","PDG_{ID} vs. momentum;p, GeV/c;ID;# events",1e3,0,pmom+1.1,2e3,1e3,3e3);
 // ntupMCTrk->Project("hpdgidmom2","pdgid:mom");
 // hpdgidmom2->Draw("colz");
 // cpdgid->cd(4);
 // TH2F* hpdgidmom3 = new TH2F("hpdgidmom3","PDG_{ID} vs. momentum;p, GeV/c;ID;# events",1e3,0,pmom+1.1,2e3,-3e3,-1e3);
 // ntupMCTrk->Project("hpdgidmom3","pdgid:mom");
 // hpdgidmom3->Draw("colz");
  //--- Save result --------------------------------------------------------------
  celyz->Write();
  cpdgid->Write();
  cpilyz->Write();
  cnyz->Write();
  f->Close();
 if(verboseLevel>0)  cout<<"Succsefully finished!"<<endl;
}
