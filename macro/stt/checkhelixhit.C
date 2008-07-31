{
  gROOT->Reset();
  TStopwatch timer;
  timer.Start();
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  
  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  //gSystem->Load("libMvd");
  gSystem->Load("libEmc");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  gSystem->Load("libGen");
//   gSystem->Load("libTrkBase");
//   gSystem->Load("libGeane");
//   //  gSystem->Load("libPGen");
  gSystem->Load("libStt");
  //   gSystem->Load("libgenfit");
  //   gSystem->Load("libtrackrep");
  //   gSystem->Load("libSttReco");
  
  // MCpoints
  TFile filerun("testrun.root");
  TTree *treepnt = (TTree*) filerun.Get("cbmsim");
  TClonesArray *pnt = new TClonesArray("PndSttPoint");
  treepnt->SetBranchAddress("STTPoint",&pnt);
  
  
  // Hits
  TFile filedigi("testdigi.root");
  TTree *treedigi = (TTree*) filedigi.Get("cbmsim");
  TClonesArray *digi = new TClonesArray("PndSttHit");
  treedigi->SetBranchAddress("STTHit",&digi);

  // HelixHits
  TFile filehelix("testreco.root");
  TTree *treereco = (TTree*) filehelix.Get("cbmsim");
  TClonesArray *hh = new TClonesArray("PndSttHelixHit");
  treereco->SetBranchAddress("SttHelixHit",&hh);

  // Helix Tracks
  TClonesArray *track = new TClonesArray("PndSttTrack");
  treereco->SetBranchAddress("STTTrack",&track);

  TCanvas *c = new TCanvas("c", "c", 0, 0, 600, 600);
  c->Divide(1,2);
  TH2F *hxy = new TH2F("hxy","hxy",100,-42,42, 100,-42,42);
  TH2F *hyz = new TH2F("hyz","hyz",100,-42,42, 100,-40,110);
   
  int evt = 0;
 
  treepnt->GetEntry(evt);
  treedigi->GetEntry(evt);
  treereco->GetEntry(evt);

  c->cd(1);
  hxy->Draw();
  c->cd(2);
  hyz->Draw();

  // tracks loop
  for (Int_t k = 0; k < track->GetEntriesFast(); k++) {
    PndSttTrack *stttrack = (PndSttTrack*) track->At(k);
    if(!stttrack) continue;
   

    for (Int_t j = 0; j < stttrack->GetNofHits() ; j++) {
      Int_t iHit = stttrack->GetHitIndex(j);
      PndSttHit *hit = (PndSttHit*) digi->At(iHit);

      // CHECK to be changed
      PndSttHelixHit *helixhit = NULL;
      for (Int_t dj = 0; dj < digi->GetEntriesFast(); dj++) {
	helixhit = (PndSttHelixHit*) hh->At(dj);
	if(helixhit->GetHitIndex() == iHit) break;
      }

      // check if hit <-> helix hit association is correct
      if(helixhit->GetHitIndex() != iHit) cout << "==> " << iHit << " " << helixhit->GetHitIndex() << " " << j << endl;
      //	else cout << "   " << iHit << " " << helixhit->GetHitIndex()<< endl;
	
      PndSttPoint *mcpoint = (PndSttPoint*) pnt->At(hit->GetRefIndex());

      c->cd(1);
      TMarker *mrkpnt = new TMarker(mcpoint->GetXtot(), mcpoint->GetYtot(), 2);
      mrkpnt->SetMarkerColor(4);
      mrkpnt->Draw("SAME");
      TMarker *mrkdigi = new TMarker(hit->GetX(), hit->GetY(), 6);
      mrkdigi->SetMarkerColor(3);
      mrkdigi->Draw("SAME");
      TMarker *mrkhh = new TMarker(helixhit->GetX(), helixhit->GetY(), 5);
      mrkhh->SetMarkerColor(2);
      mrkhh->Draw("SAME");
	
      c->cd(2);
      TMarker *mrkpnt = new TMarker(mcpoint->GetYtot(), mcpoint->GetZtot(), 2);
      mrkpnt->SetMarkerColor(4);
      mrkpnt->Draw("SAME");
      TMarker *mrkdigi = new TMarker(hit->GetY(), hit->GetZ(), 6);
      mrkdigi->SetMarkerColor(3);
      //      mrkdigi->Draw("SAME");
      TMarker *mrkhh = new TMarker(helixhit->GetY(), helixhit->GetZ(), 5);
      mrkhh->SetMarkerColor(2);
      mrkhh->Draw("SAME");
	

    }

  }
} 
