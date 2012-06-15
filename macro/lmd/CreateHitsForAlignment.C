//################################################################
//# Macros to write hits point for alignment with Knossos
//# author: Anastasia Karavdina
//# date: May, 2012
//#
//# to compile it add in "# install #" part of CMakeLists.txt lines:
//# add_executable(hits_align CreateHitsForAlignment.C)
//# target_link_libraries(hits_align ${ROOT_LIBRARIES} Geane Geom Proof TrkBase FairTools PndData VMC EG GeomPainter generalTools Base GeoBase FairDB ParBase LmdTrk)
//#
//# to run it (and see options): 
//# ${PANDAROOT}/build/bin/./hits_align --help
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
#include<TF1.h>
#include<TRotation.h>
#include<TVector3.h>
#include<TMath.h>
#include<TGaxis.h>
#include<TNtuple.h>
#include<TMatrixD.h>
#include<TStopwatch.h>
#include<TNtuple.h>
#include<PndMCTrack.h>
#include<PndSdsMCPoint.h>

//lmd track
#include<PndLinTrack.h>
#include<PndTrackCand.h>
#include<PndTrackCandHit.h>
#include<PndSdsHit.h>
#include<PndSdsClusterStrip.h>
#include<PndSdsDigiStrip.h>

// needed for geane backtracking
#include<FairRunAna.h>
#include<FairRootManager.h>
#include<FairGeane.h>
#include<FairRtdbRun.h>
#include<FairRuntimeDb.h>
#include<FairParRootFileIo.h>
#include<FairLogger.h>
#include<FairTrackParH.h>

using namespace std;
int main(int __argc,char *__argv[]) {
  //TODO: read this like params!
  //  const int nEvents=500000;
  ofstream output;
  int nEvents=1000;
  int startEvent=0;
  TString storePath="/data/FAIRsorf/pandaroot/trunk/macro/lmd/tmpOutput";
  double Plab=15.;
  int verboseLevel=0;
  std::string startStr="", momStr="", nStr="", pathStr="", verbStr="", outnStr="", outrootStr="";
  // decode arguments
  if( __argc>1 && ( strcmp( __argv[1], "-help" ) == 0
		    || strcmp( __argv[1], "--help" ) == 0 ) ){
    
    std::cout << "This is script for comparision reconstructed and simulated tracks with parameters\n"
	      <<"-s start event \n"
	      <<"-n Number of events \n"
	      <<"-path path to the file(s) \n"
	      <<"-out name for output file \n"
	      <<"-outhist name for output file with residuals histogram \n"
	      <<"-v verbose Level (if>0, print out some information) \n"
	      <<"Have fun! \n"
	      << std::endl;
    return 0;
  } 
  while ((optind < (__argc-1) ) && (__argv[optind][0]=='-')) {
    bool found=false;
    std::string sw = __argv[optind];
    if (sw=="-s") {
      optind++;
      startStr = __argv[optind];
      found=true;
    }
    if (sw=="-n"){
      optind++;
      nStr = __argv[optind];
      found=true;
    }
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
    if (sw=="-v"){
      optind++;
      verbStr = __argv[optind];
      found=true;
    }
    if(sw=="-out"){
      optind++;
      outnStr= __argv[optind];
      found=true;
    }
    if(sw=="-outhist"){
      optind++;
      outrootStr= __argv[optind];
      found=true;
    }
    if (!found){
      std::cout<< "Unknown switch: "
	       << __argv[optind] <<std::endl;
      optind++;
    }
  
  while ( (optind < __argc ) && __argv[optind][0]!='-' ) optind++; 
  }

  std::stringstream startSStr(startStr), nSStr(nStr), pathSStr(pathStr), verbSStr(verbStr);

  startSStr >> startEvent; 
  nSStr >> nEvents;
  pathSStr >> storePath;
  verbSStr >> verboseLevel;

  cout<<"For data files will be used path: "<<storePath<<endl;

  // ---- Open output file ------------------------------------------------
  output.open(outnStr.c_str());
  TFile *f = new TFile(outrootStr.c_str(),"RECREATE");
   // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libLmdTrk");
  // ------------------------------------------------------------------------
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  
  // ---- Input files --------------------------------------------------------
 
  TString DigiFile = storePath+"/Lumi_digi_";
  DigiFile += startEvent;
  DigiFile += ".root";
  TChain tdigiHits("cbmsim");
  tdigiHits.Add(DigiFile);
  
  TString recHit=storePath+"/Lumi_reco_";
  recHit += startEvent;
  recHit += ".root";
  TChain tHits("cbmsim");
  tHits.Add(recHit);
  
  TString trkCand = storePath+"/Lumi_TCand_";
  trkCand += startEvent;
  trkCand += ".root";
  TChain tTrkCand("cbmsim");
  tTrkCand.Add(trkCand);
  
  TString recTrack = storePath+"/Lumi_Track_";
  recTrack += startEvent;
  recTrack += ".root";
  TChain tTrkRec("cbmsim");
  tTrkRec.Add(recTrack);
  
  // ---------------------------------------------------------------------------------

  //--- Digitization info ------------------------------------------------------------
  TClonesArray* fStripClusterArray = new TClonesArray("PndSdsClusterStrip");
  tHits.SetBranchAddress("LMDStripClusterCand",&fStripClusterArray); 
  
  TClonesArray* fStripDigiArray = new TClonesArray("PndSdsDigiStrip");
  tdigiHits.SetBranchAddress("LMDStripDigis",&fStripDigiArray); 
  //----------------------------------------------------------------------------------
  
  //--- Real Hits --------------------------------------------------------------------
  TClonesArray* rechit_array=new TClonesArray("PndSdsHit");
  tHits.SetBranchAddress("LMDHitsStrip",&rechit_array);  //Points for Tracks
  //----------------------------------------------------------------------------------
  
  //--- Track Candidate ---------------------------------------------------------------
  TClonesArray* trkcand_array=new TClonesArray("PndTrackCand");
  tTrkCand.SetBranchAddress("LMDTrackCand",&trkcand_array); //Points for Track Canidates 
  //-----------------------------------------------------------------------------------
  
  //--- Real tracks -------------------------------------------------------------------
  TClonesArray* rec_trk=new TClonesArray("PndLinTrack");
  tTrkRec.SetBranchAddress("LMDTrack",&rec_trk);  //Tracks
  //----------------------------------------------------------------------------------  
  
  //Histograms with residuals---------------------------------------------------------
 //Create and fill histogramms 
 TObjArray *m_res_x = new TObjArray;
 TObjArray *m_res_y = new TObjArray;
 TObjArray *m_res_r = new TObjArray;


 double misal_scales[3];
 const double scaleX = 0.05;
 const double scaleY = 0.05;
 const double scaleR = 0.05;




  (scaleX == 0) ? misal_scales[0] = 1. : misal_scales[0] = 1.*scaleX;
  (scaleY == 0) ? misal_scales[1] = 1. : misal_scales[1] = 1.*scaleY;
  (scaleR == 0) ? misal_scales[2] = 1. : misal_scales[2] = 1.*scaleR;


 for (unsigned int histID = 0; histID < 32; ++histID)
 {
    char histoResXName[100];
    char histoResYName[100];
    char histoResRName[100];
   
    sprintf(histoResXName, "residuals_x_%d",histID);
    TH1F* aHisto = new TH1F(histoResXName, histoResXName, 1000, -misal_scales[0], misal_scales[0]);
    m_res_x->Add(aHisto);

    sprintf(histoResYName, "residuals_y_%d",histID);
    TH1F* aHisto2 = new TH1F(histoResYName, histoResYName, 1000, -misal_scales[1], misal_scales[1]);

    m_res_y->Add(aHisto2);

    sprintf(histoResRName, "residuals_r_%d",histID);
    TH1F* aHisto3 = new TH1F(histoResRName, histoResRName, 1000, -misal_scales[2], misal_scales[2]);
    m_res_r->Add(aHisto3);
 }
  //----------------------------------------------------------------------------------

  for (Int_t j=0; j<nEvents; j++){
    // Read REC tree -----------------------------------------------------------------
    tTrkCand.GetEntry(j);
    tTrkRec.GetEntry(j);
    tHits.GetEntry(j);
    tdigiHits.GetEntry(j);
    ///-----------------------------------------------------------------------------------------

    const int nRecTrks = rec_trk->GetEntriesFast();
    /// Read info about hits from reconstructed tracks ----------------------------------------------------
    for (Int_t iN=0; iN<nRecTrks; iN++){
      PndLinTrack *trk_lin = (PndLinTrack*)rec_trk->At(iN);
      TVector3 startlintrk = trk_lin->GetStartVec();
      TVector3 dirlintrk = trk_lin->GetDirectionVec();
      Int_t candID = trk_lin->GetTCandID();
      PndTrackCand *trkcand = (PndTrackCand*)trkcand_array->At(candID);
      const int Ntrkcandhits= trkcand->GetNHits();
      double phiMCgl;
	for (Int_t iHit = 0; iHit < Ntrkcandhits; iHit++){
	  PndTrackCandHit candhit = (PndTrackCandHit)(trkcand->GetSortedHit(iHit));
	  Int_t hitID = candhit.GetHitId();
	  PndSdsHit* myHit = (PndSdsHit*)(rechit_array->At(hitID));
	  TVector3 HitPos = myHit->GetPosition(); 
	  double trkX = startlintrk.X()+dirlintrk.X()*(HitPos.Z()-startlintrk.Z());
	  double trkY = startlintrk.Y()+dirlintrk.Y()*(HitPos.Z()-startlintrk.Z());
	  double trkR = sqrt(trkX*trkX+trkY*trkY);
	  double hitR = sqrt(HitPos.X()*HitPos.X()+HitPos.Y()*HitPos.Y());
	  int sensorID = myHit->GetSensorID();
	  ((TH1F*)m_res_x->At(sensorID))->Fill(HitPos.X()-trkX);
	  ((TH1F*)m_res_y->At(sensorID))->Fill(HitPos.Y()-trkY);
	  ((TH1F*)m_res_r->At(sensorID))->Fill(hitR-trkR);
	  int endtrk=1;
	  if((Ntrkcandhits-iHit)>1) endtrk=0;
	  if(output>0){
	    output<<HitPos.X()<<" "<<myHit->GetDx()<<" ";
	    output<<HitPos.Y()<<" "<<myHit->GetDy()<<" ";
	    output<<HitPos.Z()<<" "<<myHit->GetDz()<<" ";
	    output<<myHit->GetSensorID()<<" "<<endtrk<<endl;
	  }
	}
    }/// end rec-trks
    ///-----------------------------------------------------------------------------------------
  }/// end events
  output.close();

  for (unsigned int histID = 0; histID < 32; ++histID){
    ((TH1F*)m_res_x->At(histID))->Write();
    ((TH1F*)m_res_y->At(histID))->Write();
    ((TH1F*)m_res_r->At(histID))->Write();
  }
 

  TCanvas* c0 = new TCanvas("respl1x","residuals, pl#1",200,500,700,800);
  c0->SetFillColor(0);
  c0->SetBorderMode(0); 
  c0->Divide(4,2);     
  for (unsigned int histID = 0; histID < 8; ++histID){
    c0->cd(histID+1);
    ((TH1F*)m_res_x->At(histID))->GetXaxis()->SetTitle("(trk-hit)[x], cm");
    ((TH1F*)m_res_x->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_x->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_x->At(histID))->Draw();
  }
 c0->Update();
 c0->Write();

 TCanvas* c1 = new TCanvas("respl2x","residuals, pl#2",200,500,700,800);
  c1->SetFillColor(0);
  c1->SetBorderMode(0); 
  c1->Divide(4,2);     
  for (unsigned int histID = 8; histID < 16; ++histID){
    c1->cd(histID+1);
    ((TH1F*)m_res_x->At(histID))->GetXaxis()->SetTitle("(trk-hit)[x], cm");
    ((TH1F*)m_res_x->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_x->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_x->At(histID))->Draw();
  }
 c1->Update();
 c1->Write();

TCanvas* c2 = new TCanvas("respl3x","residuals, pl#3",200,500,700,800);
  c2->SetFillColor(0);
  c2->SetBorderMode(0); 
  c2->Divide(4,2);     
  for (unsigned int histID = 16; histID < 24; ++histID){
    c2->cd(histID+1);
    ((TH1F*)m_res_x->At(histID))->GetXaxis()->SetTitle("(trk-hit)[x], cm");
    ((TH1F*)m_res_x->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_x->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_x->At(histID))->Draw();
  }
 c2->Update();
 c2->Write();

TCanvas* c3 = new TCanvas("respl4x","residuals, pl#4",200,500,700,800);
  c3->SetFillColor(0);
  c3->SetBorderMode(0); 
  c3->Divide(4,2);     
  for (unsigned int histID = 24; histID < 32; ++histID){
    c3->cd(histID+1);
    ((TH1F*)m_res_x->At(histID))->GetXaxis()->SetTitle("(trk-hit)[x], cm");
    ((TH1F*)m_res_x->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_x->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_x->At(histID))->Draw();
  }
 c3->Update();
 c3->Write();


 TCanvas* c01 = new TCanvas("respl1y","residuals, pl#1",200,500,700,800);
  c01->SetFillColor(0);
  c01->SetBorderMode(0); 
  c01->Divide(4,2);     
  for (unsigned int histID = 0; histID < 8; ++histID){
    c01->cd(histID+1);
    ((TH1F*)m_res_y->At(histID))->GetXaxis()->SetTitle("(trk-hit)[y], cm");
    ((TH1F*)m_res_y->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_y->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_y->At(histID))->Draw();
  }
 c01->Update();
 c01->Write();

 TCanvas* c11 = new TCanvas("respl2y","residuals, pl#2",200,500,700,800);
  c11->SetFillColor(0);
  c11->SetBorderMode(0); 
  c11->Divide(4,2);     
  for (unsigned int histID = 8; histID < 16; ++histID){
    c11->cd(histID+1);
    ((TH1F*)m_res_y->At(histID))->GetXaxis()->SetTitle("(trk-hit)[y], cm");
    ((TH1F*)m_res_y->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_y->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_y->At(histID))->Draw();
  }
 c11->Update();
 c11->Write();

TCanvas* c21 = new TCanvas("respl3y","residuals, pl#3",200,500,700,800);
  c21->SetFillColor(0);
  c21->SetBorderMode(0); 
  c21->Divide(4,2);     
  for (unsigned int histID = 16; histID < 24; ++histID){
    c21->cd(histID+1);
    ((TH1F*)m_res_y->At(histID))->GetXaxis()->SetTitle("(trk-hit)[y], cm");
    ((TH1F*)m_res_y->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_y->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_y->At(histID))->Draw();
  }
 c21->Update();
 c21->Write();

TCanvas* c31 = new TCanvas("respl4y","residuals, pl#4",200,500,700,800);
  c31->SetFillColor(0);
  c31->SetBorderMode(0); 
  c31->Divide(4,2);     
  for (unsigned int histID = 24; histID < 32; ++histID){
    c31->cd(histID+1);
    ((TH1F*)m_res_y->At(histID))->GetXaxis()->SetTitle("(trk-hit)[y], cm");
    ((TH1F*)m_res_y->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_y->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_y->At(histID))->Draw();
  }
 c31->Update();
 c31->Write();

 f->Close();

}
