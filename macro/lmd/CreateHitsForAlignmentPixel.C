//################################################################
//# Macros to write hits point for alignment with Knossos
//# author: Anastasia Karavdina
//# date: May, 2012
//#
//# to compile it add in "# install #" part of CMakeLists.txt lines:
//# add_executable(hits_align_pixel CreateHitsForAlignmentPixel.C)
//# target_link_libraries(hits_align_pixel ${ROOT_LIBRARIES}  Lmd LmdTrk LmdTesting Sds SdsReco Geane Geom Proof TrkBase FairTools PndData VMC EG GeomPainter generalTools Base GeoBase FairDB ParBase MathMore)
//#
//# to run it (and see options): 
//# ${PANDAROOT}/build/bin/./hits_align_pixel --help
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
#include<PndSdsClusterPixel.h>
#include<PndSdsDigiPixel.h>


// needed for geane backtracking
#include<FairRunAna.h>
#include<FairRootManager.h>
#include<FairGeane.h>
#include<FairRtdbRun.h>
#include<FairRuntimeDb.h>
#include<FairParRootFileIo.h>
#include<FairLogger.h>
#include<FairTrackParH.h>

// needed for corditate from\out lumi frame calculation
#include<PndLmdDim.h>

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
    
    std::cout << "This is script for study hits residuals for pixel sensors design \n"
	      << "with parameters\n"
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

  TString simMC=storePath+"/Lumi_MC_";
  simMC += startEvent;
  simMC += ".root";
  TChain tMC("cbmsim");
  tMC.Add(simMC);
  
  // ---------------------------------------------------------------------------------

  //--- Digitization info ------------------------------------------------------------
  TClonesArray* fStripClusterArray;
  fStripClusterArray = new TClonesArray("PndSdsClusterPixel");
  tHits.SetBranchAddress("LMDPixelClusterCand",&fStripClusterArray);
  TClonesArray* fStripDigiArray;
  fStripDigiArray = new TClonesArray("PndSdsDigiPixel");
  tdigiHits.SetBranchAddress("LMDPixelDigis",&fStripDigiArray);
  //----------------------------------------------------------------------------------
  
  //--- Real Hits --------------------------------------------------------------------
  TClonesArray* rechit_array=new TClonesArray("PndSdsHit");
  tHits.SetBranchAddress("LMDHitsPixel",&rechit_array);  //Points for Tracks
  //----------------------------------------------------------------------------------
  
  //--- Track Candidate ---------------------------------------------------------------
  TClonesArray* trkcand_array=new TClonesArray("PndTrackCand");
  tTrkCand.SetBranchAddress("LMDTrackCand",&trkcand_array); //Points for Track Canidates 
  //-----------------------------------------------------------------------------------
  
  //--- Real tracks -------------------------------------------------------------------
  TClonesArray* rec_trk=new TClonesArray("PndLinTrack");
  tTrkRec.SetBranchAddress("LMDTrack",&rec_trk);  //Tracks
  //----------------------------------------------------------------------------------  
  
  //--- MC info -----------------------------------------------------------------
  TClonesArray* true_tracks=new TClonesArray("PndMCTrack");
  tMC.SetBranchAddress("MCTrack",&true_tracks);  //True Track to compare
  
  TClonesArray* true_points=new TClonesArray("PndSdsMCPoint");
  tMC.SetBranchAddress("LMDPoint",&true_points);  //True Points to compare
  //----------------------------------------------------------------------------------

  //Histograms with residuals---------------------------------------------------------
 //Create and fill histogramms 
 TObjArray *m_res_x = new TObjArray;
 TObjArray *m_res_y = new TObjArray;
 TObjArray *m_res_r = new TObjArray;

 TObjArray *m_res_mc_x = new TObjArray;
 TObjArray *m_res_mc_y = new TObjArray;
 TObjArray *m_res_mc_z = new TObjArray;

 double misal_scales[3];
 const double scaleX = 0.05;
 const double scaleY = 0.05;
 const double scaleR = 0.05;




  (scaleX == 0) ? misal_scales[0] = 1. : misal_scales[0] = 1.*scaleX;
  (scaleY == 0) ? misal_scales[1] = 1. : misal_scales[1] = 1.*scaleY;
  (scaleR == 0) ? misal_scales[2] = 1. : misal_scales[2] = 1.*scaleR;

  // TH2F *resx = new TH2F("resx","residuals vs. x; x, cm;#delta_{x}, cm",100,-15,15,5000,-2.5e-1,2.5e-1);
  // TH2F *resy = new TH2F("resy","residuals vs. y; y, cm;#delta_{y}, cm",100,-15,15,5000,-2.5e-1,2.5e-1);

  TH2F *resx = new TH2F("resx","residuals vs. x; x, cm;#delta_{x}, cm",100,-15,15,500,-2.5e-1,2.5e-1);
  TH2F *resy = new TH2F("resy","residuals vs. y; y, cm;#delta_{y}, cm",100,-15,15,500,-2.5e-1,2.5e-1);

  TH2F *x_id = new TH2F("x_id","x vs. sensor id; id ;x, cm",400,0,400,1e3,-15,15);
  TH2F *y_id = new TH2F("y_id","y vs. sensor id; id ;y, cm",400,0,400,1e3,-15,15);
  TH2F *z_id = new TH2F("z_id","z vs. sensor id; id ;z, cm",400,0,400,1e3,-10,60);
  TNtuple *nhits = new TNtuple("nhits","hitsInfo","xrec:yrec:zrec:xmc:ymc:zmc:sens_id:half:plane:module:side:die:sens_pos");
  // 	sens_id = mcpoint->GetSensorID(); // store the sensor id
  //				// calculate the plane and sensor on this plane
  //				lmddim->Get_sensor_by_id(sens_id, ihalf, iplane, imodule, iside, idie, isensor);

  TH2F *resxy = new TH2F("resxy","residuals y vs. residuals x; #delta_{x}, cm; #delta_{y}, cm",5000,-2.5e-1,2.5e-1,5000,-2.5e-1,2.5e-1);
  for (unsigned int histID = 0; histID < 400; ++histID)
    {
      char histoResXName[100];
      char histoResYName[100];
      char histoResRName[100];
      
      char histoResMCXName[100];
      char histoResMCYName[100];
      char histoResMCZName[100];
      sprintf(histoResXName, "residuals_x_%d",histID);
      TH1F* aHisto = new TH1F(histoResXName, histoResXName, 1000, -misal_scales[0], misal_scales[0]);
      m_res_x->Add(aHisto);
      
      sprintf(histoResYName, "residuals_y_%d",histID);
      TH1F* aHisto2 = new TH1F(histoResYName, histoResYName, 1000, -misal_scales[1], misal_scales[1]);
      
      m_res_y->Add(aHisto2);
      
      sprintf(histoResRName, "residuals_r_%d",histID);
      TH1F* aHisto3 = new TH1F(histoResRName, histoResRName, 1000, -misal_scales[2], misal_scales[2]);
      m_res_r->Add(aHisto3);
      
      sprintf(histoResMCXName, "residuals_mc_x_%d",histID);
      TH1F* aHistomc = new TH1F(histoResMCXName, histoResMCXName, 1000, -misal_scales[0], misal_scales[0]);
      m_res_mc_x->Add(aHistomc);
      
      sprintf(histoResMCYName, "residuals_mc_y_%d",histID);
      TH1F* aHistomc2 = new TH1F(histoResMCYName, histoResMCYName, 1000, -misal_scales[1], misal_scales[1]);
      
      m_res_mc_y->Add(aHistomc2);
      
      sprintf(histoResMCZName, "residuals_mc_z_%d",histID);
      TH1F* aHistomc3 = new TH1F(histoResMCZName, histoResMCZName, 1000, -misal_scales[2], misal_scales[2]);
      m_res_mc_z->Add(aHistomc3);
    }
  //----------------------------------------------------------------------------------

    //Load lumi geo params
    PndLmdDim *lmddim = PndLmdDim::Instance();
    // lmddim -> Read_transformation_matrices("matrices.txt", true);
    lmddim -> Read_transformation_matrices("matrices_perfect.txt", false);

  for (Int_t j=0; j<nEvents; j++){
    // Read REC tree -----------------------------------------------------------------
    tTrkCand.GetEntry(j);
    tTrkRec.GetEntry(j);
    tHits.GetEntry(j);
    tdigiHits.GetEntry(j);
    tMC.GetEntry(j);
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
      if(Ntrkcandhits<4) continue; //!!! TEST with 4 hits tracks only !!!
      double phiMCgl;
	for (Int_t iHit = 0; iHit < Ntrkcandhits; iHit++){
	  PndTrackCandHit candhit = (PndTrackCandHit)(trkcand->GetSortedHit(iHit));
	  Int_t hitID = candhit.GetHitId();
	  PndSdsHit* myHit = (PndSdsHit*)(rechit_array->At(hitID));
	  TVector3 HitPos = myHit->GetPosition(); 
	  TVector3 mcTop,mcTopOUT;
	  PndSdsClusterPixel* myCluster = (PndSdsClusterPixel*)(fStripClusterArray->At(myHit->GetClusterIndex()));
	  PndSdsDigiPixel* astripdigi = (PndSdsDigiPixel*)(fStripDigiArray->At(myCluster->GetDigiIndex(0)));
	  PndSdsMCPoint* MCPoint = (PndSdsMCPoint*)(true_points->At(astripdigi->GetIndex(0)));
	  mcTop = MCPoint->GetPosition(); 
	  mcTopOUT = MCPoint->GetPositionOut();
	  mcTop = lmddim->Transform_global_to_lmd_local(mcTop, false, false);
	  mcTopOUT = lmddim->Transform_global_to_lmd_local(mcTopOUT, false, false);
	  TVector3 mcMid = mcTop+(mcTopOUT-mcTop)*0.5;
	  TVector3 ResRecMC = HitPos - mcMid;
	

	// cout<<"ResRecMC: ";
	// ResRecMC.Print();
	// cout<<""<<endl;
	  double trkX = startlintrk.X()+dirlintrk.X()*(HitPos.Z()-startlintrk.Z());
	  double trkY = startlintrk.Y()+dirlintrk.Y()*(HitPos.Z()-startlintrk.Z());
	  double trkR = sqrt(trkX*trkX+trkY*trkY);
	  double hitR = sqrt(HitPos.X()*HitPos.X()+HitPos.Y()*HitPos.Y());
	  int sensorID = myHit->GetSensorID();
	  ((TH1F*)m_res_x->At(sensorID))->Fill(HitPos.X()-trkX);
	  ((TH1F*)m_res_y->At(sensorID))->Fill(HitPos.Y()-trkY);
	  ((TH1F*)m_res_r->At(sensorID))->Fill(hitR-trkR);
	  double resx_d = HitPos.X()-trkX;
	  double resy_d = HitPos.Y()-trkY;
	  resx->Fill(HitPos.X(),resx_d);
	  resy->Fill(HitPos.Y(),resy_d);
	  resxy->Fill(resx_d,resy_d);
	  if(fabs(resx_d)>0.1 || fabs(resy_d)>0.1)
	    cout<<"Track #"<<iN<<" from event #"<<j<<" has: resx="<<resx_d<<", resy="<<resy_d<<endl;
	  ((TH1F*)m_res_mc_x->At(sensorID))->Fill(ResRecMC.X());
	  ((TH1F*)m_res_mc_y->At(sensorID))->Fill(ResRecMC.Y());
	  ((TH1F*)m_res_mc_z->At(sensorID))->Fill(ResRecMC.Z());
	  x_id->Fill(sensorID,HitPos.X());
	  y_id->Fill(sensorID,HitPos.Y());
	  z_id->Fill(sensorID,HitPos.Z());


  int ihalf, iplane, imodule, iside, idie, isensor;
  // calculate the plane and sensor on this plane
  lmddim->Get_sensor_by_id(sensorID, ihalf, iplane, imodule, iside, idie, isensor);
  nhits->Fill(HitPos.X(),HitPos.Y(),HitPos.Z(),mcMid.X(),mcMid.Y(),mcMid.Z(),sensorID,ihalf,iplane,imodule,iside,idie,isensor);
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

  for (unsigned int histID = 0; histID < 400; ++histID){
    ((TH1F*)m_res_x->At(histID))->Write();
    ((TH1F*)m_res_y->At(histID))->Write();
    ((TH1F*)m_res_r->At(histID))->Write();
    ((TH1F*)m_res_mc_x->At(histID))->Write();
    ((TH1F*)m_res_mc_y->At(histID))->Write();
    ((TH1F*)m_res_mc_z->At(histID))->Write();
  }
 

  TCanvas* c0 = new TCanvas("respl1x","residuals, pl#1",200,500,700,800);
  c0->SetFillColor(0);
  c0->SetBorderMode(0); 
  c0->Divide(10,10);     
  for (unsigned int histID = 0; histID < 100; ++histID){
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
  c1->Divide(10,10);     
  for (unsigned int histID = 100; histID < 200; ++histID){
    c1->cd(histID+1-100);
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
  c2->Divide(10,10);     
  for (unsigned int histID = 200; histID < 300; ++histID){
    c2->cd(histID+1-200);
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
  c3->Divide(10,10);     
  for (unsigned int histID = 300; histID < 400; ++histID){
    c3->cd(histID+1-300);
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
  c01->Divide(10,10);     
  for (unsigned int histID = 0; histID < 100; ++histID){
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
  c11->Divide(10,10);     
  int i11=1;
  for (unsigned int histID = 100; histID < 200; ++histID){
    c11->cd(i11);
    i11++;
    ((TH1F*)m_res_y->At(histID))->GetXaxis()->SetTitle("(trk-hit)[y], cm");
    ((TH1F*)m_res_y->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_y->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_y->At(histID))->Draw();
  }
 c11->Update();
 c11->Write();

 i11=1;
TCanvas* c21 = new TCanvas("respl3y","residuals, pl#3",200,500,700,800);
  c21->SetFillColor(0);
  c21->SetBorderMode(0); 
  c21->Divide(10,10);     
  for (unsigned int histID = 200; histID < 300; ++histID){
    c21->cd(i11);
    i11++;
    ((TH1F*)m_res_y->At(histID))->GetXaxis()->SetTitle("(trk-hit)[y], cm");
    ((TH1F*)m_res_y->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_y->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_y->At(histID))->Draw();
  }
 c21->Update();
 c21->Write();

 i11=1;
TCanvas* c31 = new TCanvas("respl4y","residuals, pl#4",200,500,700,800);
  c31->SetFillColor(0);
  c31->SetBorderMode(0); 
  c31->Divide(10,10);     
  for (unsigned int histID = 300; histID < 400; ++histID){
    c31->cd(i11);
    i11++;
    ((TH1F*)m_res_y->At(histID))->GetXaxis()->SetTitle("(trk-hit)[y], cm");
    ((TH1F*)m_res_y->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_y->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_y->At(histID))->Draw();
  }
 c31->Update();
 c31->Write();

 //---------------------------------------------

  TCanvas* c0mc = new TCanvas("resmcpl1x","residuals, pl#1",200,500,700,800);
  c0mc->SetFillColor(0);
  c0mc->SetBorderMode(0); 
  c0mc->Divide(10,10);     
  for (unsigned int histID = 0; histID < 100; ++histID){
    c0mc->cd(histID+1);
    ((TH1F*)m_res_mc_x->At(histID))->GetXaxis()->SetTitle("(rec-mc)[x], cm");
    ((TH1F*)m_res_mc_x->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_mc_x->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_mc_x->At(histID))->Draw();
  }
 c0mc->Update();
 c0mc->Write();

 TCanvas* c1mc = new TCanvas("resmcpl2x","residuals, pl#2",200,500,700,800);
  c1mc->SetFillColor(0);
  c1mc->SetBorderMode(0); 
  c1mc->Divide(10,10);     
  for (unsigned int histID = 100; histID < 200; ++histID){
    c1mc->cd(histID+1-100);
    ((TH1F*)m_res_mc_x->At(histID))->GetXaxis()->SetTitle("(rec-mc)[x], cm");
    ((TH1F*)m_res_mc_x->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_mc_x->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_mc_x->At(histID))->Draw();
  }
 c1mc->Update();
 c1mc->Write();

TCanvas* c2mc = new TCanvas("resmcpl3x","residuals, pl#3",200,500,700,800);
  c2mc->SetFillColor(0);
  c2mc->SetBorderMode(0); 
  c2mc->Divide(10,10);     
  for (unsigned int histID = 100; histID < 200; ++histID){
    c2mc->cd(histID+1-100);
    ((TH1F*)m_res_mc_x->At(histID))->GetXaxis()->SetTitle("(rec-mc)[x], cm");
    ((TH1F*)m_res_mc_x->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_mc_x->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_mc_x->At(histID))->Draw();
  }
 c2mc->Update();
 c2mc->Write();

TCanvas* c3mc = new TCanvas("resmcpl4x","residuals, pl#4",200,500,700,800);
  c3mc->SetFillColor(0);
  c3mc->SetBorderMode(0); 
  c3mc->Divide(10,10);     
  for (unsigned int histID = 200; histID < 300; ++histID){
    c3mc->cd(histID+1-200);
    ((TH1F*)m_res_mc_x->At(histID))->GetXaxis()->SetTitle("(rec-mc)[x], cm");
    ((TH1F*)m_res_mc_x->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_mc_x->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_mc_x->At(histID))->Draw();
  }
 c3mc->Update();
 c3mc->Write();


 TCanvas* c01mc = new TCanvas("resmcpl1y","residuals, pl#1",200,500,700,800);
  c01mc->SetFillColor(0);
  c01mc->SetBorderMode(0); 
  c01mc->Divide(10,10);     
  for (unsigned int histID = 0; histID < 100; ++histID){
    c01mc->cd(histID+1);
    ((TH1F*)m_res_mc_y->At(histID))->GetXaxis()->SetTitle("(rec-mc)[y], cm");
    ((TH1F*)m_res_mc_y->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_mc_y->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_mc_y->At(histID))->Draw();
  }
 c01mc->Update();
 c01mc->Write();

 TCanvas* c11mc = new TCanvas("resmcpl2y","residuals, pl#2",200,500,700,800);
  c11mc->SetFillColor(0);
  c11mc->SetBorderMode(0); 
  c11mc->Divide(10,10);     
  i11=1;
  for (unsigned int histID = 100; histID < 200; ++histID){
    c11mc->cd(i11);
    i11++;
    ((TH1F*)m_res_mc_y->At(histID))->GetXaxis()->SetTitle("(rec-mc)[y], cm");
    ((TH1F*)m_res_mc_y->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_mc_y->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_mc_y->At(histID))->Draw();
  }
 c11mc->Update();
 c11mc->Write();

 i11=1;
TCanvas* c21mc = new TCanvas("resmcpl3y","residuals, pl#3",200,500,700,800);
  c21mc->SetFillColor(0);
  c21mc->SetBorderMode(0); 
  c21mc->Divide(10,10);     
  for (unsigned int histID = 200; histID < 300; ++histID){
    c21mc->cd(i11);
    i11++;
    ((TH1F*)m_res_mc_y->At(histID))->GetXaxis()->SetTitle("(rec-mc)[y], cm");
    ((TH1F*)m_res_mc_y->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_mc_y->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_mc_y->At(histID))->Draw();
  }
 c21mc->Update();
 c21mc->Write();

 i11=1;
TCanvas* c31mc = new TCanvas("resmcpl4y","residuals, pl#4",200,500,700,800);
  c31mc->SetFillColor(0);
  c31mc->SetBorderMode(0); 
  c31mc->Divide(10,10);     
  for (unsigned int histID = 300; histID < 400; ++histID){
    c31mc->cd(i11);
    i11++;
    ((TH1F*)m_res_mc_y->At(histID))->GetXaxis()->SetTitle("(rec-mc)[y], cm");
    ((TH1F*)m_res_mc_y->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_mc_y->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_mc_y->At(histID))->Draw();
  }
 c31mc->Update();
 c31mc->Write();


 TCanvas* c02mc = new TCanvas("resmcpl1z","residuals, pl#1",200,500,700,800);
  c02mc->SetFillColor(0);
  c02mc->SetBorderMode(0); 
  c02mc->Divide(10,10);     
  for (unsigned int histID = 0; histID < 100; ++histID){
    c02mc->cd(histID+1);
    ((TH1F*)m_res_mc_z->At(histID))->GetXaxis()->SetTitle("(rec-mc)[y], cm");
    ((TH1F*)m_res_mc_z->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_mc_z->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_mc_z->At(histID))->Draw();
  }
 c02mc->Update();
 c02mc->Write();

 TCanvas* c12mc = new TCanvas("resmcpl2z","residuals, pl#2",200,500,700,800);
  c12mc->SetFillColor(0);
  c12mc->SetBorderMode(0); 
  c12mc->Divide(10,10);     
  i11=1;
  for (unsigned int histID = 100; histID < 200; ++histID){
    c12mc->cd(i11);
    i11++;
    ((TH1F*)m_res_mc_z->At(histID))->GetXaxis()->SetTitle("(rec-mc)[y], cm");
    ((TH1F*)m_res_mc_z->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_mc_z->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_mc_z->At(histID))->Draw();
  }
 c12mc->Update();
 c12mc->Write();

 i11=1;
TCanvas* c22mc = new TCanvas("resmcpl3z","residuals, pl#3",200,500,700,800);
  c22mc->SetFillColor(0);
  c22mc->SetBorderMode(0); 
  c22mc->Divide(10,10);     
  for (unsigned int histID = 200; histID < 300; ++histID){
    c22mc->cd(i11);
    i11++;
    ((TH1F*)m_res_mc_z->At(histID))->GetXaxis()->SetTitle("(rec-mc)[y], cm");
    ((TH1F*)m_res_mc_z->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_mc_z->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_mc_z->At(histID))->Draw();
  }
 c22mc->Update();
 c22mc->Write();

 i11=1;
TCanvas* c32mc = new TCanvas("resmcpl4z","residuals, pl#4",200,500,700,800);
  c32mc->SetFillColor(0);
  c32mc->SetBorderMode(0); 
  c32mc->Divide(10,10);     
  for (unsigned int histID = 300; histID < 400; ++histID){
    c32mc->cd(i11);
    i11++;
    ((TH1F*)m_res_mc_z->At(histID))->GetXaxis()->SetTitle("(rec-mc)[y], cm");
    ((TH1F*)m_res_mc_z->At(histID))->GetXaxis()->SetTitleSize(0.05);
    ((TH1F*)m_res_mc_z->At(histID))->SetLineColor(1);
    ((TH1F*)m_res_mc_z->At(histID))->Draw();
  }
 c32mc->Update();
 c32mc->Write();
 resx->Write();
 resy->Write();
 resxy->Write();
 x_id->Write();
 y_id->Write();
 z_id->Write();
 nhits->Write();
 f->Close();

}
