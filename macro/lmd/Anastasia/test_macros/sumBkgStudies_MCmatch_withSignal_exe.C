//################################################################
//# Macros for summary of Background\Signal study
//# author: Anastasia Karavdina
//# date: May, 2012
//#
//# to compile it add in "# install #" part of CMakeLists.txt lines:
//# add_executable(sum_bkg_rec_mc_match sumBkgStudies_MCmatch_withSignal_exe.C)
//# target_link_libraries(sum_bkg_rec_mc_match ${ROOT_LIBRARIES})
//#
//# to run it (and see options): 
//# ${PANDAROOT}/build/bin/./sum_bkg_rec_mc_match --help
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
  TFile *f = new TFile(in,"UPDATE");
  TH1F *hMCpdg_sum_f = (TH1F*)f->Get("hMCpdg");
  TH1F *hMCpdgSUM_sum_f = (TH1F*)f->Get("hMCpdgSUM");
  TH1F *hThetaRecAP_sum_f = (TH1F*)f->Get("hThetaRecAP");
  TH2 *hMCtrkNhits_sum_f = (TH2I*)f->Get("hMCtrkNhits");
  TH2 *hMCtrkPhits_sum_f = (TH2D*)f->Get("hMCtrkPhits");
  TNtuple *nrecbkg = (TNtuple*)f->Get("nrecbkg");
  TNtuple *nrecsig = (TNtuple*)f->Get("nrecsig");

  /// Read SumIDs info bin by bin ----------------------------------------------
  double totBkg = 0;
  double sumMin = hMCpdgSUM_sum_f->GetXaxis()->GetXmin();
  double sumMax = hMCpdgSUM_sum_f->GetXaxis()->GetXmax();
  double sumStep = (sumMax-sumMin)/(hMCpdgSUM_sum_f->GetNbinsX());
  int barpevents = hThetaRecAP_sum_f->GetEntries();
  for(int xi=0;xi<hMCpdgSUM_sum_f->GetNbinsX();xi++){
    double numEv =  hMCpdgSUM_sum_f->GetBinContent(xi);
    if(numEv!=0) {
      //      if(numEv>20) 
      cout<<sumStep*xi-1<<" & "<<numEv<<"  ( "<<100*numEv/barpevents <<" \\%)"<<" \\"<<endl;
      if((sumStep*xi-1)!=4424) totBkg+=numEv;
    }
  }
  cout<<" TotSig = "<<barpevents <<" TotBkg = "<< totBkg<<" ("<<100*totBkg/barpevents <<" )\\%"<<endl;
  cout<<""<<endl;
  /// ---------------------------------------------------------------------------
  
  /// Read PDGid info bin by bin ----------------------------------------------
  cout<<"Particles:---------------------------------------"<<endl;
  double iDMin = hMCpdg_sum_f->GetXaxis()->GetXmin();
  double iDMax = hMCpdg_sum_f->GetXaxis()->GetXmax();
  sumStep = (iDMax-iDMin)/(hMCpdg_sum_f->GetNbinsX());
  // cout<<"iDMin = "<<iDMin<<" iDMax = "<<iDMax<<" sumStep = "<<sumStep<<endl;
  for(int xi=0;xi<hMCpdg_sum_f->GetNbinsX();xi++){
    double numEv =  hMCpdg_sum_f->GetBinContent(xi);
    if(numEv!=0) {
      int pdgID= (iDMin+sumStep*xi-1);
      if(fabs(pdgID)==3122){
	if(pdgID>0) cout<<"\$\\Laymbda$"; 
	else cout<<"\$\\bar{\\Laymbda}$"; 
      }
      else{
	if(fabs(pdgID)==2212) {
	  if(pdgID>0) cout<<"$p$";
	  else cout<<"\$\\bar{p}$";
	}
	else{
	  if(fabs(pdgID)==2112){
	    if(pdgID>0) cout<<"$n$";
	    else cout<<"\$\\bar{n}$";
	  }
	  else{
	    if(fabs(pdgID)==321){ 
	      if(pdgID>0) cout<<"\$K^{+}$";
	      else cout<<"\$K^{-}$";
	    }
	    else{
	      if(fabs(pdgID)==211) {
		if(pdgID>0) cout<<"\$\\pi^{+}";
		else  cout<<"\$\\pi^{-}$";
	      }
	      else{
		cout<<pdgID;
	      }
	    }
	  }
	}
      }
      cout<<" & "<<numEv<<" \\"<<"\\"<<endl;
    }
  }
 cout<<"-----------------------------------------------"<<endl;
 /// ---------------------------------------------------------------------------


  ///How many hits from MCtrk participate in one RECtrk? -----------------------
  TH1D *h1MChit = hMCtrkNhits_sum_f->ProjectionY("h1MChit",1,2);
  TH1D *h2MChit = hMCtrkNhits_sum_f->ProjectionY("h2MChit",2,3);
  TH1D *h3MChit = hMCtrkNhits_sum_f->ProjectionY("h3MChit",3,4);
  TH1D *h4MChit = hMCtrkNhits_sum_f->ProjectionY("h4MChit",4,5);
  TH1D *h5MChit = hMCtrkNhits_sum_f->ProjectionY("h5MChit",5,6);
  TH1D *h6MChit = hMCtrkNhits_sum_f->ProjectionY("h6MChit",6,7);
  TH1D *h7MChit = hMCtrkNhits_sum_f->ProjectionY("h7MChit",7,8);
  TH1D *h8MChit = hMCtrkNhits_sum_f->ProjectionY("h8MChit",8,9);

  TCanvas *c1 = new TCanvas("participHitsINFOp1");
  c1->Divide(2,2);
  c1->cd(1);
  h1MChit->Draw();
  c1->cd(2);
  h2MChit->Draw();
  c1->cd(3);
  h3MChit->Draw();
  c1->cd(4);
  h4MChit->Draw();
  c1->Write();

  TCanvas *c2 = new TCanvas("participHitsINFOp2");
  c2->Divide(2,2);
  c2->cd(1);
  h5MChit->Draw();
  c2->cd(2);
  h6MChit->Draw();
  c2->cd(3);
  h7MChit->Draw();
  c2->cd(4);
  h8MChit->Draw();
  c2->Write();
  
  TH1D *h1MCPhit = hMCtrkPhits_sum_f->ProjectionY("h1MCPhit",10,14);// 1 from 8
  TH1D *h2MCPhit = hMCtrkPhits_sum_f->ProjectionY("h2MCPhit",15,19);// 1 from 6
  TH1D *h3MCPhit = hMCtrkPhits_sum_f->ProjectionY("h3MCPhit",23,27);// 2 from 8
  TH1D *h4MCPhit = hMCtrkPhits_sum_f->ProjectionY("h4MCPhit",31,35);// 2 from 6
  TH1D *h5MCPhit = hMCtrkPhits_sum_f->ProjectionY("h5MCPhit",35,39);// 3 from 8
  TH1D *h6MCPhit = hMCtrkPhits_sum_f->ProjectionY("h6MCPhit",48,52);// 4 from 8 or 3 from 6
  TH1D *h7MCPhit = hMCtrkPhits_sum_f->ProjectionY("h7MCPhit",60,65);// 5 from 8
  TH1D *h8MCPhit = hMCtrkPhits_sum_f->ProjectionY("h8MCPhit",65,69);// 4 from 6
  TH1D *h9MCPhit = hMCtrkPhits_sum_f->ProjectionY("h9MCPhit",73,77);// 6 from 8
  TH1D *h10MCPhit = hMCtrkPhits_sum_f->ProjectionY("h10MCPhit",85,89);// 7 from 8
  TH1D *h11MCPhit = hMCtrkPhits_sum_f->ProjectionY("h11MCPhit",98,102);// 8 from 8 or 6 from 6
  
  TCanvas *c3 = new TCanvas("participHitsINFO");
  c3->Divide(4,3);
  c3->cd(1);
  h1MCPhit->SetTitle("10% - 14% (1 from 8)");
  h1MCPhit->Draw();
  c3->cd(2);
  h2MCPhit->SetTitle("15% - 19% (1 from 6)");
  h2MCPhit->Draw();
  c3->cd(3);
  h3MCPhit->SetTitle("23% - 27% (2 from 8)");
  h3MCPhit->Draw();
  c3->cd(4);
  h4MCPhit->SetTitle("31% - 35% (2 from 6)");
  h4MCPhit->Draw();
  c3->cd(5);
  h5MCPhit->SetTitle("35% - 39% (3 from 8)");
  h5MCPhit->Draw();
  c3->cd(6);
  h6MCPhit->SetTitle("48% - 52% (4 from 8 or 3 from 6)");
  h6MCPhit->Draw();
  c3->cd(7);
  h7MCPhit->SetTitle("60% - 65% (5 from 8)");
  h7MCPhit->Draw();
  c3->cd(8);
  h8MCPhit->SetTitle("65% - 69% (4 from 6)");
  h8MCPhit->Draw();
  c3->cd(9);
  h9MCPhit->SetTitle("73% - 77% (6 from 8)");
  h9MCPhit->Draw();
  c3->cd(10);
  h10MCPhit->SetTitle("85% - 89% (7 from 8)");
  h10MCPhit->Draw();
  c3->cd(11);
  h11MCPhit->SetTitle("98% - 102% (8 from 8 or 6 from 6)");
  h11MCPhit->Draw();
  c3->cd(12);
  // TPaveLabel lfunction = new TPaveLabel(5,39,9.8,46,"% of MC hit participation in one REC trk");
  //  lfunction->Draw();
  TLatex latex;
  latex.SetTextSize(0.05);
  latex.SetTextAlign(13);  //align at top
  latex.DrawLatex(.1,.9,"% of MC hit participation in one REC trk");
  latex.DrawLatex(.2,.8,"PDG id code:");
  latex.DrawLatex(.2,.7,"3122 = #Lambda");
  latex.DrawLatex(.2,.6,"2212 = p");
  latex.DrawLatex(.2,.5,"321 = K^{+}");
  latex.DrawLatex(.2,.4,"211 = #pi^{+}");
  latex.DrawLatex(.2,.3,"13 = #mu^{-}");
  latex.DrawLatex(.2,.2,"11 = e^{-}");
  //c3->Write();

  TCanvas *cBkgpbar = new TCanvas("cBkgpbar");
  cBkgpbar->Divide(3,2);
  cBkgpbar->cd(1);
  nrecbkg->Draw("sumid","id==-2212 && sumid<1e5");
  cBkgpbar->cd(2);
  nrecbkg->Draw("sumid","id==-2212 && sumid>2000 && sumid<4000");
  cBkgpbar->cd(3);
  nrecbkg->Draw("sumid","id==-2212 && sumid>4000 && sumid<5000");
  cBkgpbar->cd(4);
  nrecbkg->Draw("sumid","id==-2212 && sumid>5000 && sumid<6000");
  cBkgpbar->cd(5);
  nrecbkg->Draw("sumid","id==-2212 && sumid>6000 && sumid<7000");
  cBkgpbar->cd(6);
  nrecbkg->Draw("sumid","id==-2212 && sumid>7000 && sumid<1e5");
  


  TCanvas *cBkgTheta = new TCanvas("cBkgTheta");
  cBkgTheta->Divide(3,2);
  cBkgTheta->cd(1);
  nrecbkg->Draw("theta:x","theta>0.003 && theta<0.009","colz");
  cBkgTheta->cd(2);
  nrecbkg->Draw("theta:y","theta>0.003 && theta<0.009","colz");
  cBkgTheta->cd(3);
  nrecbkg->Draw("theta:z","theta>0.003 && theta<0.009","colz");
  cBkgTheta->cd(4);
  nrecbkg->Draw("theta:px","theta>0.003 && theta<0.009","colz");
  cBkgTheta->cd(5);
  nrecbkg->Draw("theta:py","theta>0.003 && theta<0.009","colz");
  cBkgTheta->cd(6);
  nrecbkg->Draw("theta:pz","theta>0.003 && theta<0.009","colz");
 

  TCanvas *cSigTheta = new TCanvas("cSigTheta");
  cSigTheta->Divide(3,2);
  cSigTheta->cd(1);
  nrecsig->Draw("theta:x","theta>0.003 && theta<0.009","colz");
  cSigTheta->cd(2);
  nrecsig->Draw("theta:y","theta>0.003 && theta<0.009","colz");
  cSigTheta->cd(3);
  nrecsig->Draw("theta:z","theta>0.003 && theta<0.009","colz");
  cSigTheta->cd(4);
  nrecsig->Draw("theta:px","theta>0.003 && theta<0.009","colz");
  cSigTheta->cd(5);
  nrecsig->Draw("theta:py","theta>0.003 && theta<0.009","colz");
  cSigTheta->cd(6);
  nrecsig->Draw("theta:pz","theta>0.003 && theta<0.009","colz");

  TCanvas *cSumTheta = new TCanvas("cSumTheta");
  TH1F* hthetasig = new TH1F("hthetasig",";#theta, rad",100,0,0.03);
  nrecsig->Project("hthetasig","theta");
  hthetasig->SetLineColor(2);
  hthetasig->Draw();
  TH1F* hthetabkg = new TH1F("hthetabkg",";#theta, rad",100,0,0.03);
  nrecbkg->Project("hthetabkg","theta");
  hthetabkg->SetLineColor(4);
  hthetabkg->Draw("same");
  cSumTheta->SetLogy();

  TString resname_pdf =storePath+"/summary.pdf";
  TString resname_pdf_o = resname_pdf+"(";
  cSumTheta->Print(resname_pdf_o); 
  cSigTheta->Print(resname_pdf_o); 
  cBkgTheta->Print(resname_pdf_o);
  TString resname_pdf_c = resname_pdf+")";
  // cBkgpbar->Print(resname_pdf_c);
  c3->Print(resname_pdf_c);



  // // cSigTheta->Write();
  // // cBkgTheta->Write();
  // // cBkgpbar->Write();
  f->Write();
  
  //TCanvas *ctmp = new TCanvas("ctmp");
  //  for(int isum=900;isum<7000;isum++){
  for(int isum=0;isum<7000;isum++){
  //  for(int isum=900;isum<5000;isum++){
  // for(int isum=5258;isum<7000;isum++){
  //  for(int isum=5268;isum<7000;isum++){
  // for(int isum=4534;isum<4537;isum++){
    const int isumcur = isum;
    TString exprsum = "sumid==";
    exprsum+=isumcur;
    //   cout<<"cut: "<<exprsum<<endl;
    Long64_t numSum = nrecbkg->Draw("sumid",exprsum);
    //  cout<<"numSum = "<<numSum<<endl;
    if(numSum>20){
    // if(numSum>0){
      cout<<isum<<" & "<<numSum<<" [";
      int totpart=0;
      int iid[12]={-2212,2212,-211,211,3122,321,-321,13,-13,11,-11};
      for(int iiid=0;iiid<12;iiid++){
	const int iidcur = iid[iiid];
	TString exprid = "id==";
	exprid+=iidcur;
	exprid+=" && ";
	exprid+=exprsum;
	Long64_t num = nrecbkg->Draw("id",exprid);
	if(num!=0){
	  if(iidcur==-2212){
	    cout<<" barp "<<num;
	    totpart+=num;
	  }
	  if(iidcur==2212){
	    cout<<" p "<<num;
	    totpart+=num;
	  }
	  if(iidcur==-211){
	    cout<<" pi- "<<num;
	    totpart+=num;
	  }
	  if(iidcur==211){
	    cout<<" pi+ "<<num;
	    totpart+=num;
	  }
	  if(iidcur==3122){
	    cout<<" Lambda "<<num;
	    totpart+=num;
	  }
	  if(iidcur==321){
	    cout<<" K+ "<<num;
	    totpart+=num;
	  }
	  if(iidcur==-321){
	    cout<<" K- "<<num;
	    totpart+=num;
	  }
	  if(iidcur==13){
	    cout<<" mu- "<<num;
	    totpart+=num;
	  }
	  if(iidcur==-13){
	    cout<<" mu+ "<<num;
	    totpart+=num;
	  }
	  if(iidcur==11){
	    cout<<" e- "<<num;
	    totpart+=num;
	  }
	  if(iidcur==-11){
	    cout<<" e+ "<<num;
	    totpart+=num;
	  }
	}
      }
      // cout<<"totpart = "<<totpart<<" numSum = "<<numSum<<endl;
      if(totpart!=numSum) cout<<" + "<<numSum-totpart<<" are missing!";
      cout<<"]"<<endl;
    }
  }

  ///---------------------------------------------------------------------------
 
}
