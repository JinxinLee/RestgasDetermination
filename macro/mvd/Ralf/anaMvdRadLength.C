// root macro to analyze the simulation output
//void convertMCPoints()
{
#include <map>
#include <vector>
  // -----  Load libraries   ------------------------------------------------
//``gSystem->Load("fstream.h");
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  
  gStyle->SetPalette(1);
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

/*
Convert PndMvdWaferMCPoints to the STAR Format (text, "x y z layer")
for the conformal mapping stuff
*/

//   PndFileNameCreator namecreator("data/mvdsim.root");
  PndFileNameCreator namecreator("Mvd_DPM_Test.root");
  std::string inFile = namecreator.GetSimFileName(false);

  TFile* f = new TFile(inFile.c_str()); // the sim file you want to analyse
  TTree *t=(TTree *) f->Get("cbmsim") ;
 
  TClonesArray* hit_array=new TClonesArray("PndSdsMCPoint");
  t->SetBranchAddress("MVDPoint",&hit_array);//Branch names
  
  TClonesArray* radlen_array = new TClonesArray("FairRadLenPoint");
  t->SetBranchAddress("RadLen", &radlen_array);

  TClonesArray* mc_array=new TClonesArray("PndMCTrack");
  t->SetBranchAddress("MCTrack",&mc_array);//Branch names
  
  TH1D* hisRadLen = new TH1D("hisRadLen","Radiation Length", 1000,0,100);
  TH2D* hisRadLen2D = new TH2D("hisRadLen2D","Radiation Length 2D", 100,-1,1,100, -TMath::Pi(), TMath::Pi());
  TH2D* hisRadLenCount = new TH2D("hisRadLenCount","hisRadLenCount",100,-1,1,100, -TMath::Pi(), TMath::Pi());
  
  TH1D* hisTrackP = new TH1D("hisTrackP","Hits per Track", 51,-0.5,50.5);
  TH2D* hisTrackP2D = new TH2D("hisTrackP2D","Hits per Track 2D", 100,-1,1,100, -TMath::Pi(), TMath::Pi());  
  TH2D* hisTrackPCount = new TH2D("hisTrackPCount","hisTrackPCount", 100,-1,1,100, -TMath::Pi(), TMath::Pi());  

  int  nEvents = 1000;
  int startEvent = 0;
  bool verbose = false;

  TVector3 vecs,veco;
  std::map<int,int> trackHitMap;
     
  for (Int_t j=startEvent; j<(nEvents+startEvent) && j<t->GetEntriesFast(); j++)
  {
    t->GetEntry(j);
    //if (verbose)
    cout<<">>>> Event No "<<j<<endl;
    std::vector<double> RadLengthOnTrack (10000,0.0); //trackID, vector with points on track
     
    for (Int_t i=0; i<radlen_array->GetEntriesFast(); i++)
    {
      if(verbose) cout<<"Point No "<<i<<endl;
       FairRadLenPoint *point=(FairRadLenPoint*)radlen_array->At(i);
      if (verbose) cout << "Track ID: " << point->GetTrackID() << std::endl;
      TVector3 pos, posOut, res;
      pos = point->GetPosition();
      posOut = point->GetPositionOut();
      res = posOut - pos;
      PndMCTrack* myTrack = (PndMCTrack*)(mc_array->At(point->GetTrackID()));
      if (myTrack->GetMotherID() < 0){
	      if (verbose){
	    	  std::cout << "Time: " << point->GetTime() << " Length: " << point->GetLength() << std::endl;
	    	  std::cout << "Pos: " << pos.x() << "/" << pos.y() << "/" << pos.z();
	    	  std::cout << " OutPos: " << posOut.x() << "/" << posOut.y() << "/" << posOut.z() << std::endl;
	    	  std::cout << "TrackLength: " << res.Mag() << " RadLength: " << point->GetRadLength() << " Res: " << res.Mag() / point->GetRadLength() << std::endl;
	      }
	      if (point->GetTrackID() > 0 && point->GetTrackID() < 10000 && fabs(posOut.x()) < 15 && fabs(posOut.y()) < 15 && fabs(posOut.z()) < 20)
	    	  RadLengthOnTrack[point->GetTrackID()] += res.Mag()/point->GetRadLength()*100;
      }
    }
    for (int k = 0; k < RadLengthOnTrack.size(); k++){
	  if (RadLengthOnTrack[k] > 0){
		  if (verbose) std::cout << "Full TrackLength: " << RadLengthOnTrack[k] << std::endl;
		  PndMCTrack* mcTrack = (PndMCTrack*)(mc_array->At(k));
		  hisRadLen->Fill(RadLengthOnTrack[k]);
		  hisRadLen2D->Fill(mcTrack->GetMomentum().CosTheta(), mcTrack->GetMomentum().Phi(), RadLengthOnTrack[k]);
		  hisRadLenCount->Fill(mcTrack->GetMomentum().CosTheta(), mcTrack->GetMomentum().Phi());
	  }
    }
    
    trackHitMap.clear();
    for (int k = 0; k < hit_array->GetEntriesFast(); k++){
    	PndSdsMCPoint* myHit = (PndSdsMCPoint*)(hit_array->At(k));
    	trackHitMap[myHit->GetTrackID()]++;
    }
    
    for (map<int,int>::const_iterator ci = trackHitMap.begin(); ci != trackHitMap.end(); ci++){
    	PndMCTrack* track = (PndMCTrack*)(mc_array->At(ci->first));
    	hisTrackP->Fill(ci->second);
    	hisTrackP2D->Fill(track->GetMomentum().CosTheta(), track->GetMomentum().Phi(), ci->second);
    	hisTrackPCount->Fill(track->GetMomentum().CosTheta(), track->GetMomentum().Phi());
    }
  }// end for j (events)
  
  hisRadLen2D->Divide(hisRadLenCount);
  hisTrackP2D->Divide(hisTrackPCount);
  
  TCanvas* can1 = new TCanvas();
  can1->Divide(2,2);
  gStyle->SetPalette(1);
  can1->cd(1);
  hisRadLen->DrawCopy();
  can1->cd(2);
  hisRadLen2D->DrawCopy("colz");
  can1->cd(3);
  hisTrackP->DrawCopy();
  can1->cd(4);
  hisTrackP2D->DrawCopy("colz");

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  //cout << "Output file is "    << outFile << endl;
  //cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

}
