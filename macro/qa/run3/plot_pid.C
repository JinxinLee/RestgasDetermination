void plot_pid()
{
  
  gStyle->SetHistMinimumZero();
  TStopwatch fTimer;
  TFile *file_in = TFile::Open("psi2s_jpsi2pi_jpsi_mumu_pid.root","READ");
  TFile *file_out = TFile::Open("pid_plot.root","RECREATE");
  TTree *cbmsim = (TTree*)file_in->Get("cbmsim");
  
  
  TH1F *hMomentum = new TH1F("hMomentum","Momentum",100,0,10);
  cbmsim->Draw("PidChargedCand.GetMomentum().Mag()>>hMomentum","","goff");
  
  TH1F *hTheta = new TH1F("hTheta","Theta",100,0,180);
  cbmsim->Draw("PidChargedCand.GetMomentum().Theta()*TMath::RadToDeg()>>hTheta","","goff");

  TH1F *hPhi = new TH1F("hPhi","Phi",100,-180,180);
  cbmsim->Draw("PidChargedCand.GetMomentum().Phi()*TMath::RadToDeg()>>hPhi","","goff");
  
  TH1F *hMvdHits = new TH1F("hMvdHits","Number of MVD hits",15,-0.5,14.5);
  cbmsim->Draw("PidChargedCand.GetMvdHits()>>hMvdHits","","goff");
 
  TH1F *hMvdDEDX = new TH1F("hMvdDEDX","MVD DEDX [MeV/cm]",200,0.,40.);
  cbmsim->Draw("PidChargedCand.GetMvdDEDX()*1000>>hMvdDEDX","","goff");
 
  TH1F *hSttHits = new TH1F("hSttHits","Number of STT hits",45,-0.5,44.5);
  cbmsim->Draw("PidChargedCand.GetSttHits()>>hSttHits","PidChargedCand.GetTrackBranch()==50","goff");
 
  TH1F *hSttDEDX = new TH1F("hSttDEDX","STT DEDX [u.a.]",200,0.,50.);
  cbmsim->Draw("PidChargedCand.GetSttMeanDEDX()>>hSttDEDX","PidChargedCand.GetTrackBranch()==50","goff");

  TH1F *hFtsHits = new TH1F("hFtsHits","Number of FTS hits",80,-0.5,79.5);
  cbmsim->Draw("PidChargedCand.GetSttHits()>>hFtsHits","PidChargedCand.GetTrackBranch()==55","goff");
 
  TH1F *hFtsDEDX = new TH1F("hFtsDEDX","FTS DEDX [u.a.]",200,0.,50.);
  cbmsim->Draw("PidChargedCand.GetSttMeanDEDX()>>hFtsDEDX","PidChargedCand.GetTrackBranch()==55","goff");

  TH1F *hGemHits = new TH1F("hGemHits","Number of GEM hits",10,-0.5,9.5);
  cbmsim->Draw("PidChargedCand.GetGemHits()>>hGemHits","","goff");

  TH1F *hTofTof = new TH1F("hTofTof","SciTil time-of-flight [ns]",100,0.,10.);
  cbmsim->Draw("PidChargedCand.GetTofStopTime()>>hTofTof","PidChargedCand.GetTrackBranch()==50&&PidChargedCand.GetTofIndex()>-1","goff");

  TH1F *hTofLength = new TH1F("hTofLength","SciTil track length [m]",100,0.,150.);
  cbmsim->Draw("PidChargedCand.GetTofTrackLength()>>hTofLength","PidChargedCand.GetTrackBranch()==50&&PidChargedCand.GetTofIndex()>-1","goff");

  TH1F *hTofM2 = new TH1F("hTofM2","SciTil mass^{2} [GeV^{2}/c^{4}]",200,-2.,2.);
  cbmsim->Draw("PidChargedCand.GetTofM2()>>hTofM2","PidChargedCand.GetTrackBranch()==50&&PidChargedCand.GetTofIndex()>-1","goff");
   
  TH1F *hTofQ = new TH1F("hTofQ","SciTil Quality",200,0.,20.);
  cbmsim->Draw("PidChargedCand.GetTofQuality()>>hTofQ","PidChargedCand.GetTrackBranch()==50&&PidChargedCand.GetTofIndex()>-1","goff");
   
  TH1F *hFTofTof = new TH1F("hFTofTof","FTOF time-of-flight [ns]",200,0.,50.);
  cbmsim->Draw("PidChargedCand.GetTofStopTime()>>hFTofTof","PidChargedCand.GetTrackBranch()==55&&PidChargedCand.GetTofIndex()>-1","goff");

  TH1F *hFTofLength = new TH1F("hFTofLength","FTOF track length [m]",100,600.,900.);
  cbmsim->Draw("PidChargedCand.GetTofTrackLength()>>hFTofLength","PidChargedCand.GetTrackBranch()==55&&PidChargedCand.GetTofIndex()>-1","goff");
 
  TH1F *hFTofM2 = new TH1F("hFTofM2","FTOF mass^{2} [GeV^{2}/c^{4}]",200,-2.,2.);
  cbmsim->Draw("PidChargedCand.GetTofM2()>>hFTofM2","PidChargedCand.GetTrackBranch()==55&&PidChargedCand.GetTofIndex()>-1","goff");
   
  TH1F *hFTofQ = new TH1F("hFTofQ","FTOF Quality",200,0.,2000.);
  cbmsim->Draw("PidChargedCand.GetTofQuality()>>hFTofQ","PidChargedCand.GetTrackBranch()==55&&PidChargedCand.GetTofIndex()>-1","goff");

  TH1F *hDrcThetaC = new TH1F("hDrcThetaC","DIRC Cherenkov angle",100,0.,1.);
  cbmsim->Draw("PidChargedCand.GetDrcThetaC()>>hDrcThetaC","PidChargedCand.GetDrcIndex()>-1","goff");
 
  TH1F *hDrcThetaCErr = new TH1F("hDrcThetaCErr","DIRC Cherenkov angle error",100,0.,1.);
  cbmsim->Draw("PidChargedCand.GetDrcThetaCErr()>>hDrcThetaCErr","PidChargedCand.GetDrcIndex()>-1","goff");

  TH1F *hDrcQ = new TH1F("hDrcQ","DIRC Quality",100,0,.00001);
  cbmsim->Draw("PidChargedCand.GetDrcQuality()>>hDrcQ","PidChargedCand.GetDrcIndex()>-1","goff");

  TH1F *hDrcNPhotons = new TH1F("hDrcNPhotons","DIRC Number of Photons",20,-0.5,19.5);
  cbmsim->Draw("PidChargedCand.GetDrcNumberOfPhotons()>>hDrcNPhotons","PidChargedCand.GetDrcIndex()>-1","goff");

  TH1F *hDiscThetaC = new TH1F("hDiscThetaC","DISC Cherenkov angle",100,0.,1.);
  cbmsim->Draw("PidChargedCand.GetDiscThetaC()>>hDiscThetaC","PidChargedCand.GetDiscIndex()>-1","goff");
 
  TH1F *hDiscThetaCErr = new TH1F("hDiscThetaCErr","DISC Cherenkov angle error",100,0.,1.);
  cbmsim->Draw("PidChargedCand.GetDiscThetaCErr()>>hDiscThetaCErr","PidChargedCand.GetDiscIndex()>-1","goff");

  TH1F *hDiscQ = new TH1F("hDiscQ","DISC Quality",100,0,.01);
  cbmsim->Draw("PidChargedCand.GetDiscQuality()>>hDiscQ","PidChargedCand.GetDiscIndex()>-1","goff");

  TH1F *hDiscNPhotons = new TH1F("hDiscNPhotons","DISC Number of Photons",20,-0.5,19.5);
  cbmsim->Draw("PidChargedCand.GetDiscNumberOfPhotons()>>hDiscNPhotons","PidChargedCand.GetDiscIndex()>-1","goff");

  TH1F *hEmcRawEnergy = new TH1F("hEmcRawEnergy","EMC Raw Energy",100,0,1.);
  cbmsim->Draw("PidChargedCand.GetEmcRawEnergy()>>hEmcRawEnergy","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==50","goff");

  TH1F *hEmcCalEnergy = new TH1F("hEmcCalEnergy","EMC Cal Energy",100,0,1.);
  cbmsim->Draw("PidChargedCand.GetEmcCalEnergy()>>hEmcCalEnergy","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==50","goff");

  TH1F *hEmcQ = new TH1F("hEmcQ","EMC Quality",100,0,20.);
  cbmsim->Draw("PidChargedCand.GetEmcQuality()>>hEmcQ","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==50","goff");

  TH1F *hEmcNCrystals = new TH1F("hEmcNCrystals","EMC Number Of Crystals",50,-0.5,49.5);
  cbmsim->Draw("PidChargedCand.GetEmcNumberOfCrystals()>>hEmcNCrystals","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==50","goff");

  TH1F *hEmcNBumps = new TH1F("hEmcNBumps","EMC Number Of Bumps",5,-0.5,4.5);
  cbmsim->Draw("PidChargedCand.GetEmcNumberOfBumps()>>hEmcNBumps","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==50","goff");

  TH1F *hEmcModule = new TH1F("hEmcModule","EMC Module",10,-0.5,9.5);
  cbmsim->Draw("PidChargedCand.GetEmcModule()>>hEmcModule","PidChargedCand.GetEmcIndex()>-1","goff");

  TH1F *hEmcZ20 = new TH1F("hEmcZ20","EMC Z20",100,0.,1.1);
  cbmsim->Draw("PidChargedCand.GetEmcClusterZ20()>>hEmcZ20","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==50","goff");

  TH1F *hEmcZ53 = new TH1F("hEmcZ53","EMC Z53",100,0.,0.02);
  cbmsim->Draw("PidChargedCand.GetEmcClusterZ53()>>hEmcZ53","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==50","goff");

  TH1F *hEmcLat = new TH1F("hEmcLat","EMC Lat",100,0.,1);
  cbmsim->Draw("PidChargedCand.GetEmcClusterLat()>>hEmcLat","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==50","goff");

   TH1F *hEmcE1 = new TH1F("hEmcE1","EMC E1",100,0.,1);
  cbmsim->Draw("PidChargedCand.GetEmcClusterE1()>>hEmcE1","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==50","goff");

  TH1F *hEmcE9 = new TH1F("hEmcE9","EMC E9",100,0.,1);
  cbmsim->Draw("PidChargedCand.GetEmcClusterE9()>>hEmcE9","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==50","goff");
  
  TH1F *hEmcE25 = new TH1F("hEmcE25","EMC E25",100,0.,1);
  cbmsim->Draw("PidChargedCand.GetEmcClusterE25()>>hEmcE25","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==50","goff");

   TH1F *hFscRawEnergy = new TH1F("hFscRawEnergy","FSC Raw Energy",100,0,1.);
  cbmsim->Draw("PidChargedCand.GetEmcRawEnergy()>>hFscRawEnergy","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==55","goff");

  TH1F *hFscCalEnergy = new TH1F("hFscCalEnergy","FSC Cal Energy",100,0,1.);
  cbmsim->Draw("PidChargedCand.GetEmcCalEnergy()>>hFscCalEnergy","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==55","goff");

  TH1F *hFscQ = new TH1F("hFscQ","FSC Quality",100,0,20.);
  cbmsim->Draw("PidChargedCand.GetEmcQuality()>>hFscQ","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==55","goff");

  TH1F *hFscNCrystals = new TH1F("hFscNCrystals","FSC Number Of Crystals",55,-0.5,49.5);
  cbmsim->Draw("PidChargedCand.GetEmcNumberOfCrystals()>>hFscNCrystals","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==55","goff");

  TH1F *hFscNBumps = new TH1F("hFscNBumps","FSC Number Of Bumps",5,-0.5,4.5);
  cbmsim->Draw("PidChargedCand.GetEmcNumberOfBumps()>>hFscNBumps","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==55","goff");

  TH1F *hFscZ20 = new TH1F("hFscZ20","FSC Z20",100,0.,1.1);
  cbmsim->Draw("PidChargedCand.GetEmcClusterZ20()>>hFscZ20","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==55","goff");

  TH1F *hFscZ53 = new TH1F("hFscZ53","FSC Z53",100,0.,0.02);
  cbmsim->Draw("PidChargedCand.GetEmcClusterZ53()>>hFscZ53","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==55","goff");

  TH1F *hFscLat = new TH1F("hFscLat","FSC Lat",100,0.,1);
  cbmsim->Draw("PidChargedCand.GetEmcClusterLat()>>hFscLat","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==55","goff");

   TH1F *hFscE1 = new TH1F("hFscE1","FSC E1",100,0.,1);
  cbmsim->Draw("PidChargedCand.GetEmcClusterE1()>>hFscE1","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==55","goff");

  TH1F *hFscE9 = new TH1F("hFscE9","FSC E9",100,0.,1);
  cbmsim->Draw("PidChargedCand.GetEmcClusterE9()>>hFscE9","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==55","goff");
  
  TH1F *hFscE25 = new TH1F("hFscE25","FSC E25",100,0.,1);
  cbmsim->Draw("PidChargedCand.GetEmcClusterE25()>>hFscE25","PidChargedCand.GetEmcIndex()>-1&&PidChargedCand.GetTrackBranch()==55","goff");

  TH1F *hMuoNLayers = new TH1F("hMuoNLayers","MUO Number Of Layers",20,-0.5,19.5);
  cbmsim->Draw("PidChargedCand.GetMuoNumberOfLayers()>>hMuoNLayers","PidChargedCand.GetMuoIndex()>-1&&PidChargedCand.GetTrackBranch()==50","goff");

  TH1F *hMuoQ = new TH1F("hMuoQ","MUO Quality [cm]",100,0.,50);
  cbmsim->Draw("sqrt(PidChargedCand.GetMuoQuality())>>hMuoQ","PidChargedCand.GetMuoIndex()>-1&&PidChargedCand.GetTrackBranch()==50","goff");
  
  TH1F *hMuoIron = new TH1F("hMuoIron","MUO Crossed Iron Thickness [cm]",100,0.,100);
  cbmsim->Draw("PidChargedCand.GetMuoIron()>>hMuoIron","PidChargedCand.GetMuoIndex()>-1&&PidChargedCand.GetTrackBranch()==50","goff");
  
  TH1F *hMuoMomentumIn = new TH1F("hMuoMomentumIn","MUO Momentum at ingress point [GeV/c]",100,0.,10);
  cbmsim->Draw("PidChargedCand.GetMuoIron()>>hMuoMomentumIn","PidChargedCand.GetMuoIndex()>-1&&PidChargedCand.GetTrackBranch()==50","goff");
  
  TH1F *hMuoModule = new TH1F("hMuoModule","MUO Module",6,-1.5,4.5);
  cbmsim->Draw("PidChargedCand.GetMuoModule()>>hMuoModule","PidChargedCand.GetMuoIndex()>-1","goff");
  
  TH1F *hMuoHits = new TH1F("hMuoHits","MUO Number Of Hits",25,-0.5,24.5);
  cbmsim->Draw("PidChargedCand.GetMuoHits()>>hMuoHits","PidChargedCand.GetMuoIndex()>-1&&PidChargedCand.GetTrackBranch()==50","goff");
 
  TH1F *hFMuoNLayers = new TH1F("hFMuoNLayers","MUO Number Of FWD Layers",20,-0.5,19.5);
  cbmsim->Draw("PidChargedCand.GetMuoNumberOfLayers()>>hFMuoNLayers","PidChargedCand.GetMuoIndex()>-1&&PidChargedCand.GetTrackBranch()==55","goff");

  TH1F *hFMuoQ = new TH1F("hFMuoQ","MUO Quality [cm]",100,0.,10);
  cbmsim->Draw("sqrt(PidChargedCand.GetMuoQuality())>>hFMuoQ","PidChargedCand.GetMuoIndex()>-1&&PidChargedCand.GetTrackBranch()==55","goff");
  
  TH1F *hFMuoIron = new TH1F("hFMuoIron","MUO Crossed Iron Thickness [cm]",100,0.,110);
  cbmsim->Draw("PidChargedCand.GetMuoIron()>>hFMuoIron","PidChargedCand.GetMuoIndex()>-1&&PidChargedCand.GetTrackBranch()==55","goff");
  
  TH1F *hFMuoMomentumIn = new TH1F("hFMuoMomentumIn","MUO Momentum at ingress point [GeV/c]",100,0.,15);
  cbmsim->Draw("PidChargedCand.GetMuoMomentumIn()>>hFMuoMomentumIn","PidChargedCand.GetMuoIndex()>-1&&PidChargedCand.GetTrackBranch()==55","goff");
  
  TH1F *hFMuoHits = new TH1F("hFMuoHits","MUO Number Of Hits",35,-0.5,34.5);
  cbmsim->Draw("PidChargedCand.GetMuoHits()>>hFMuoHits","PidChargedCand.GetMuoIndex()>-1&&PidChargedCand.GetTrackBranch()==55","goff");


  file_out->cd();

  hMomentum->Write();
  hTheta->Write();
  hPhi->Write();
  hMvdHits->Write();
  hMvdDEDX->Write(); 
  hSttHits->Write();
  hSttDEDX->Write();
  hFtsHits->Write();
  hFtsDEDX->Write(); 
  hGemHits->Write();
  hTofTof->Write();  
  hFTofTof->Write(); 
  hTofLength->Write();  
  hFTofLength->Write();
  hTofM2->Write();  
  hFTofM2->Write(); 
  hTofQ->Write();  
  hFTofQ->Write();
  hDrcThetaC->Write();
  hDrcThetaCErr->Write();
  hDrcQ->Write();
  hDrcNPhotons->Write();
  hDiscThetaC->Write();
  hDiscThetaCErr->Write();
  hDiscQ->Write();
  hDiscNPhotons->Write();
  hEmcRawEnergy->Write();
  hEmcCalEnergy->Write();
  hEmcQ->Write();
  hEmcNCrystals->Write();
  hEmcNBumps->Write();
  hEmcModule->Write();
  hEmcZ20->Write();
  hEmcZ53->Write();
  hEmcLat->Write();
  hEmcE1->Write();
  hEmcE9->Write();
  hEmcE25->Write();
  hFscRawEnergy->Write();
  hFscCalEnergy->Write();
  hFscQ->Write();
  hFscNCrystals->Write();
  hFscNBumps->Write();
  hFscZ20->Write();
  hFscZ53->Write();
  hFscLat->Write();
  hFscE1->Write();
  hFscE9->Write();
  hFscE25->Write();
  hMuoNLayers->Write();
  hMuoQ->Write();
  hMuoIron->Write();
  hMuoMomentumIn->Write();
  hMuoModule->Write();
  hMuoHits->Write();
  hFMuoNLayers->Write();
  hFMuoQ->Write();
  hFMuoIron->Write();
  hFMuoMomentumIn->Write();
  hFMuoHits->Write();

  // Extract the maximal used memory an add is as Dart measurement
  // This line is filtered by CTest and the value send to CDash
  FairSystemInfo sysInfo;
  Float_t maxMemory=sysInfo.GetMaxMemory();
  cout << "<DartMeasurement name=\"MaxMemory\" type=\"numeric/double\">";
  cout << maxMemory;
  cout << "</DartMeasurement>" << endl;
  
  fTimer.Stop();
  Double_t rtime = fTimer.RealTime();
  Double_t ctime = fTimer.CpuTime();
  
  Float_t cpuUsage=ctime/rtime;
  cout << "<DartMeasurement name=\"CpuLoad\" type=\"numeric/double\">";
  cout << cpuUsage;
  cout << "</DartMeasurement>" << endl;
  
  cout << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime
       << "s" << endl;
  cout << "CPU usage " << cpuUsage*100. << "%" << endl;
  cout << "Max Memory " << maxMemory << " MB" << endl;
   
  cout << "Macro finished successfully." << endl; 
 
  exit(kTRUE);
}
