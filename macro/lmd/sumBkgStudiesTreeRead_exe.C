//################################################################
//# Macros for summary of Background\Signal study
//# author: Anastasia Karavdina
//# date: May, 2012
//#
//# to compile it add in "# install #" part of CMakeLists.txt lines:
//# add_executable(sum_bkg_tree_read sumBkgStudiesTreeRead_exe.C)
//# target_link_libraries(sum_bkg_tree_read ${ROOT_LIBRARIES})
//#
//# to run it (and see options): 
//# ${PANDAROOT}/build/bin/./sum_bkg_tree_read --help
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
#include<TLatex.h>
#include<TStopwatch.h>
#include<TTree.h>
#include<PndMCTrack.h>
#include<PndSdsMCPoint.h>

// //lmd track
// #include<PndLinTrack.h>
// #include<PndTrackCand.h>
// #include<PndTrackCandHit.h>
// #include<PndSdsHit.h>
// #include<PndSdsClusterStrip.h>
// #include<PndSdsDigiStrip.h>

// // needed for geane backtracking
// #include<FairRunAna.h>
// #include<FairRootManager.h>
// #include<FairGeane.h>
// #include<FairRtdbRun.h>
// #include<FairRuntimeDb.h>
// #include<FairParRootFileIo.h>
// #include<FairLogger.h>
// #include<FairTrackParH.h>

#include<string>

using namespace std;
int main(int __argc,char *__argv[]) {
  TString storePath="/data/FAIRsorf/pandaroot/trunk/macro/lmd/tmpOutputBkg";
  std::string pathStr="";
  // decode arguments
  if( __argc>1 && ( strcmp( __argv[1], "-help" ) == 0
		    || strcmp( __argv[1], "--help" ) == 0 ) ){
    
    std::cout << "This is summary Background study script with parameter\n"
	      <<"-path path to the file(s)"
	      <<"Have fun! \n"
	      << std::endl;
    return 0;
  } 
  while ((optind < (__argc-1) ) && (__argv[optind][0]=='-')) {
    bool found=false;
    std::string sw = __argv[optind];
    if (sw=="-path"){
      optind++;
      pathStr = __argv[optind];
      found=true;
    }
    if (!found){
      std::cout<< "Unknown switch: "
	       << __argv[optind] <<std::endl;
      optind++;
    }
  while ( (optind < __argc ) && __argv[optind][0]!='-' ) optind++; 
  }

  std::stringstream pathSStr(pathStr);

  pathSStr >> storePath;


  //void sumBkgStudies_MCmatch_withSignal(TString storePath= "/data/FAIRsorf/pandaroot/trunk/macro/lmd/tmpHIMster/results/15GeV")
  //{
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libLmdTrk");
  // gROOT->LoadMacro("line3Dfit.C");
  // ------------------------------------------------------------------------
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  TString in=storePath+"/sumAll.root";
  //  TString in=storePath+"/compMC_and_REC_withSignal0.root";
  cout<<in<<endl;
  TFile *f = new TFile(in,"READ");
  //TTree *tAll_sum = (TTree *)f->Get("tAll");
  TTree *tSig_sum = (TTree *)f->Get("tSig");
  TTree *tBkg_sum = (TTree *)f->Get("tBkg");
  TNtuple *nmcall_sum = ( TNtuple *)f->Get("nmcall");
 
  // TH1F *hMCpdg_sum_f = (TH1F*)f->Get("hMCpdg");
  // TH1F *hMCpdgSUM_sum_f = (TH1F*)f->Get("hMCpdgSUM");
  // TH1F *hThetaRecAP_sum_f = (TH1F*)f->Get("hThetaRecAP");
  // TH2 *hMCtrkNhits_sum_f = (TH2I*)f->Get("hMCtrkNhits");
  // TH2 *hMCtrkPhits_sum_f = (TH2D*)f->Get("hMCtrkPhits");
  // TNtuple *nrecbkg = (TNtuple*)f->Get("nrecbkg");
  // TNtuple *nrecsig = (TNtuple*)f->Get("nrecsig");

  int totBkg = tBkg_sum->GetEntries();
  int totSig = tSig_sum->GetEntries();

  //try to find upper limit for sumID and ID
  int maxsumid=5e3;
  Long64_t  nmore=1;
  while(nmore>0){
    maxsumid*=1.1;
    TString condLim = "sumid>";
    condLim+=maxsumid;
    nmore = tBkg_sum->GetEntries(condLim.Data());
  }
  cout<<"(max  SUMid<"<<maxsumid<<")"<<endl;

  int maxid=4e2;
  Long64_t  nmoreid=1;
  while(nmoreid>0){
    maxid*=1.1;
    TString condLim2 = "abs(id)>";
    condLim2+=maxid;
    nmoreid = tBkg_sum->GetEntries(condLim2.Data());
  }
  cout<<"(max  id<"<<maxid<<")"<<endl;

  //Read SUM ID info --------------------------------------------------------------------
  cout<<"==== Summary for SUM_PDG_IDs: ==== "<<endl;
  cout<<"sumID: #events(% from tot.bkg), include [particles content]"<<endl;
  int glnumCh=0;
  for(int isumid=10;isumid<maxsumid;isumid++){
    TString cond = "sumid==";
    cond +=isumid;
    Long64_t  nbkg_ch = tBkg_sum->GetEntries(cond.Data());
    //  cout<<" =========== isumid = "<<isumid<<endl;
    // if(0<1) continue;//TEST
    if(nbkg_ch<1) continue;
    if(nbkg_ch>1){
      glnumCh++;
      cout<<isumid<<": "<<nbkg_ch<<"  ("<<100.*nbkg_ch/totBkg<<" %)"<<endl;
      if(nbkg_ch<0.01*totBkg) continue;
      // if(nbkg_ch<totBkg) continue; //TEST
      cout<<"REC incl.: ";
      for(int iid=-maxid;iid<maxid;iid++){
	//	cout<<"%%%% iid = "<<iid<<endl;
	TString cond2 = "sumid==";
	cond2 +=isumid;
	cond2 +=" && id==";
	cond2+=iid;
	Long64_t  nbkg_ch2 = tBkg_sum->GetEntries(cond2.Data());
	//	cout<<"nbkg_ch2 = "<<nbkg_ch2<<endl;
	if(nbkg_ch2<1) continue;
	if(nbkg_ch2>0){
	switch (iid){
	  case 2212:
	    cout<<"$p$";
	    break;
	  case -2212:
	    cout<<"$\\bar{p}$";
	    break;
	  case 3122:
	    cout<<"$\\Lambda$"; 
	    break;
	  case -3122:
	    cout<<"$\\bar{\\Lambda}$"; 
	    break;
	  case 2112:
	    cout<<"$n$";
	    break;
	  case -2112:
	    cout<<"$\\bar{n}$";
	    break;
	  case 321:
	    cout<<"$K^{+}$";
	    break;
	  case -321:
	    cout<<"$K^{-}$";
	    break;
	  case 211:
	    cout<<"$\\pi^{+}$";
	    break;
	  case -211:
	    cout<<"$\\pi^{-}$";
	    break;
	  case 11:
	    cout<<"$e^{-}$";
	    break;
	  case -11:
	    cout<<"$e^{+}$";
	    break;
	  case 13:
	    cout<<"$\\mu^{-}$";
	    break;
	  case -13:
	    cout<<"$\\mu^{+}$";
	    break;
	  case 15:
	    cout<<"$\\tau^{-}$";
	    break;
	  case -15:
	    cout<<"$\\tau^{+}$";
	    break;
	  case 22:
	    cout<<"$\\gamma$";
	    break;
	  case 111:
	    cout<<"$\\pi^{0}$";
	    break;
	  default:
	    cout<<iid;
	    break;
	  }
	cout<<": "<<nbkg_ch2<<", ";
	}
      }
    }
      cout<<""<<endl;
     //  cout<<"MC  incl.: ";
    //   for(int iid=-maxid;iid<maxid;iid++){
    // 	cout<<" MC iid = "<<iid<<endl;
    // 	TString cond2 = "sumpdgid==";
    // 	cond2 +=isumid;
    // 	cond2 +=" && pdgid==";
    // 	cond2+=iid;
    // 	Long64_t  nbkg_ch2 = nmcall_sum->GetEntries(cond2.Data());
    // 	if(nbkg_ch2<1) continue;
    // 	if(nbkg_ch2>0){
    // 	switch (iid){
    // 	  case 2212:
    // 	    cout<<"$p$";
    // 	    break;
    // 	  case -2212:
    // 	    cout<<"$\\bar{p}$";
    // 	    break;
    // 	  case 3122:
    // 	    cout<<"$\\Lambda$"; 
    // 	    break;
    // 	  case -3122:
    // 	    cout<<"$\\bar{\\Lambda}$"; 
    // 	    break;
    // 	  case 2112:
    // 	    cout<<"$n$";
    // 	    break;
    // 	  case -2112:
    // 	    cout<<"$\\bar{n}$";
    // 	    break;
    // 	  case 321:
    // 	    cout<<"$K^{+}$";
    // 	    break;
    // 	  case -321:
    // 	    cout<<"$K^{-}$";
    // 	    break;
    // 	  case 211:
    // 	    cout<<"$pi^{+}$";
    // 	    break;
    // 	  case -211:
    // 	    cout<<"$pi^{-}$";
    // 	    break;
    // 	  case 11:
    // 	    cout<<"$e^{-}$";
    // 	    break;
    // 	  case -11:
    // 	    cout<<"$e^{+}$";
    // 	    break;
    // 	  case 13:
    // 	    cout<<"$\\mu^{-}$";
    // 	    break;
    // 	  case -13:
    // 	    cout<<"$\\mu^{+}$";
    // 	    break;
    // 	  case 15:
    // 	    cout<<"$\\tau^{-}$";
    // 	    break;
    // 	  case -15:
    // 	    cout<<"$\\tau^{+}$";
    // 	    break;
    // 	  case 22:
    // 	    cout<<"$\\gamma$";
    // 	    break;
    // 	  case 111:
    // 	    cout<<"$\\pi^{0}$";
    // 	    break;
    // 	  default:
    // 	    cout<<iid;
    // 	    break;
    // 	  }
    // 	cout<<": "<<nbkg_ch2<<", ";
    // 	}
    //   }
    //   cout<<""<<endl;
    // }
  }
  cout<<" TotSig = "<<totSig <<" TotBkg = "<< totBkg<<" ("<<100.*totBkg/totSig<<" %)"<<" number of Bkg channels: "<<glnumCh<<endl;
  cout<<"================================="<<endl;
  //Read of SUM ID info (end) --------------------------------------------------------
  cout<<""<<endl;
  cout<<"---------- particles content --------"<<endl;
 //Read ID info --------------------------------------------------------------------
 for(int iid=-maxid;iid<maxid;iid++){
   TString cond3 = "id==";
   cond3 +=iid;
   Long64_t  nbkg_ch3 = tBkg_sum->GetEntries(cond3.Data());
   if(nbkg_ch3<1) continue;
   if(nbkg_ch3>0){
     switch (iid){
       case 2212:
	 cout<<"$p$";
	 break;
       case -2212:
	 cout<<"$\\bar{p}$";
	 break;
       case 3122:
	 cout<<"$\\Lambda$"; 
	 break;
       case -3122:
	 cout<<"$\\bar{\\Lambda}$"; 
	 break;
       case 2112:
	 cout<<"$n$";
	 break;
       case -2112:
	 cout<<"$\\bar{n}$";
	 break;
       case 321:
	 cout<<"$K^{+}$";
	 break;
       case -321:
	 cout<<"$K^{-}$";
	 break;
       case 211:
	 cout<<"$\\pi^{+}$";
	 break;
       case -211:
	 cout<<"$\\pi^{-}$";
	 break;
       case 11:
	 cout<<"$e^{-}$";
	 break;
       case -11:
	 cout<<"$e^{+}$";
	 break;
       case 13:
	 cout<<"$\\mu^{-}$";
	 break;
       case -13:
	 cout<<"$\\mu^{+}$";
	 break;
       case 15:
	 cout<<"$\\tau^{-}$";
	 break;
       case -15:
	 cout<<"$\\tau^{+}$";
	 break;
       case 22:
	 cout<<"$\\gamma$";
	 break;
       case 111:
	 cout<<"$\\pi^{0}$";
	 break;
       default:
	 cout<<iid;
	 break;
       }
     cout<<": "<<nbkg_ch3<<" ("<<100.*nbkg_ch3/totBkg<<" %)"<<endl;
   }
 }
cout<<"---------------------------- --------"<<endl;
 //Read ID info (end) --------------------------------------------------------
  ///---------------------------------------------------------------------------
 
}
