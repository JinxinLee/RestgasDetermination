/**
   @class recoqa_complete
   @brief Quality Assurance task for tracking
   @author Lia Lavezzi <lia.lavezzi@to.infn.it>, University of Torino
   @author Stefano Spataro <stefano.spataro@to.infn.it>, University of Torino
   @version 1.0
   @date Jun 25, 2015
**/
void recoqa_complete(Int_t nEvents = 0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec"; 
  TString  output         = "recoqa";
  TString  friend1        = "digi";
  TString  friend2        = "reco";
  TString  friend3        = "";
  TString  friend4        = "";
  
  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetInput(input);
  fRun->SetOutput(output);
  fRun->SetFriend1(friend1);
  fRun->SetFriend2(friend2);
  fRun->SetFriend3(friend3);
  fRun->SetFriend4(friend4);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup();
  
  // -----   Add tasks   ----------------------------------------------------
  /// Ideal Track finder
  PndIdealTrackFinder* idealTracking = new PndIdealTrackFinder();
  idealTracking->AddBranchName("MVDHitsPixel");
  idealTracking->AddBranchName("MVDHitsStrip");
  idealTracking->AddBranchName("STTHit");
  idealTracking->AddBranchName("GEMHit");
  fRun->AddTask(idealTracking);
  
  /// QA task
  PndTrackingQATask* trackingQA = new PndTrackingQATask("SttMvdGemGenTrack", "IdealTrack");
  fRun->AddTask(trackingQA);
  
  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();

  exit(0); 
}
