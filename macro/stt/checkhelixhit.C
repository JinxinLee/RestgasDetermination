{
  gROOT->Reset();
  TStopwatch timer;
  timer.Start();
//   gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
//   rootlogon();
  
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
  TH2F *hyz = new TH2F("hyz","hyz",100,0,50, 100,-40,110);
   
  int evt = 0;
 
  treepnt->GetEntry(evt);
  treedigi->GetEntry(evt);
  treereco->GetEntry(evt);

  c->cd(1);
  hxy->Draw();
  c->cd(2);
  hyz->Draw();

  //  cout << "num tracks " << track->GetEntriesFast() << endl;

  cout << "blue  = Monte Carlo " << endl;
  cout << "red   = Helix Hit " << endl;
  cout << "green = Center Of Tubes " << endl;

  // tracks loop
  for (Int_t k = 0; k < track->GetEntriesFast(); k++) {

    PndSttTrack *stttrack = (PndSttTrack*) track->At(k);
    if(!stttrack) continue;

    Double_t d0  = stttrack->GetDist();
    Double_t phi0 = stttrack->GetPhi();
    Double_t R = stttrack->GetRad();
    Double_t z0 = stttrack->GetZ();
    Double_t tanl = stttrack->GetTanL();
    Double_t h = -(Int_t) stttrack->GetCharge(); 
    Double_t ptran = 0.003 * 2 * R;

    Double_t plong = ptran * tanl;
    Double_t ptot = sqrt(plong*plong + ptran*ptran);

    Double_t x0 = (d0 + R) * cos(phi0);
    Double_t y0 = (d0 + R) * sin(phi0);

    cout << "evt" << evt << " track " << k << " num hits " << stttrack->GetNofHelixHits() << " flag " << stttrack->GetFlag() << " ptran " << ptran << " plong "<< plong << " ptot " << ptot << endl;

  c->cd(1);
      // track
      TArc *circ = new TArc(x0, y0, R, 0, 360);
      circ->SetLineColor(k+1);
      circ->SetFillStyle(0);
      circ->Draw("SAME");
   c->cd(2);
      // track
      Double_t x1 = d0 * cos(phi0);
      Double_t y1 = d0 * sin(phi0);
      Double_t scoslT = stttrack->CalculateScosl(x1, y1);
     
      TLine* line = new TLine(scoslT, scoslT*tanl+ z0, 50, 50*tanl + z0);
      line->SetLineColor(k+1);
      line->Draw("SAME");


    for (Int_t j = 0; j < stttrack->GetNofHelixHits() ; j++) {
      Int_t iHHit = stttrack->GetHelixHitIndex(j);
      PndSttHelixHit *helixhit = (PndSttHelixHit*) hh->At(iHHit);

      Int_t hitindex = helixhit->GetHitIndex();
      PndSttHit* hit = (PndSttHit*) digi->At(hitindex);
      PndSttPoint *mcpoint = (PndSttPoint*) pnt->At(hit->GetRefIndex());

      c->cd(1);
    
      // points/hits
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
    
      /**
	 TMarker *mrkpnt = new TMarker(mcpoint->GetYtot(), mcpoint->GetZtot(), 2);
	 mrkpnt->SetMarkerColor(4);
	 mrkpnt->Draw("SAME");
	 TMarker *mrkdigi = new TMarker(hit->GetY(), hit->GetZ(), 6);
	 mrkdigi->SetMarkerColor(3);
	 mrkdigi->Draw("SAME");
	 TMarker *mrkhh = new TMarker(helixhit->GetY(), helixhit->GetZ(), 5);
	 mrkhh->SetMarkerColor(2);
	 mrkhh->Draw("SAME");
      **/
	
      //    cout << "MC " <<  mcpoint->GetXtot() 
      // 	   << " " <<             mcpoint->GetYtot() 
      // 	   << " " <<             mcpoint->GetZtot() << endl;
      
      //   cout << "helixhit " <<  helixhit->GetX() 
      //        << " " <<             helixhit->GetY() 
      //        << " " <<             helixhit->GetZ() << endl;
      

     
      Double_t scoslMC = stttrack->CalculateScosl(mcpoint->GetXtot(), mcpoint->GetYtot());
      TMarker *mrkpnt = new TMarker(scoslMC, mcpoint->GetZtot(), 2);
      mrkpnt->SetMarkerColor(4);
      mrkpnt->Draw("SAME");

      Double_t scoslH  = stttrack->CalculateScosl(hit->GetX(), hit->GetY());
      TMarker *mrkdigi = new TMarker(scoslH, hit->GetZ(), 6);
      mrkdigi->SetMarkerColor(3);
      //  mrkdigi->Draw("SAME");

      Double_t scoslHH = stttrack->CalculateScosl(helixhit->GetX(), helixhit->GetY());
      TMarker *mrkhh = new TMarker(scoslHH, helixhit->GetZ(),  5);
      mrkhh->SetMarkerColor(2);
      mrkhh->Draw("SAME");
	
      //      cout <<  scoslMC<< " " << mcpoint->GetZtot() << endl;
    }

  }
} 
