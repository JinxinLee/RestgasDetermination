{
  gROOT->Reset();
 #include <vector>
 TStopwatch timer;
  timer.Start();
//   gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
//   rootlogon();

  // MCpoints
  TFile filerun("testrun.root");
  TTree *treemc = (TTree*) filerun.Get("pndsim");
  TClonesArray *pnt = new TClonesArray("PndSttPoint");
  treemc->SetBranchAddress("STTPoint",&pnt);
  TClonesArray *mctrack = new TClonesArray("PndMCTrack");
  treemc->SetBranchAddress("MCTrack",&mctrack);

  // Hits
  TFile filedigi("testdigi.root");
  TTree *treedigi = (TTree*) filedigi.Get("pndsim");
  TClonesArray *digi = new TClonesArray("PndSttHit");
  treedigi->SetBranchAddress("STTHit",&digi);

  // HelixHits
  TFile filehelix("testreco.root");
  TTree *treereco = (TTree*) filehelix.Get("pndsim");
  TClonesArray *hh = new TClonesArray("PndSttHelixHit");
  treereco->SetBranchAddress("SttHelixHit",&hh);

  // Helix Tracks
  TClonesArray *track = new TClonesArray("PndSttTrack");
  treereco->SetBranchAddress("STTTrack",&track);
  TClonesArray *matchtrack = new TClonesArray("PndSttTrackMatch");
  treereco->SetBranchAddress("STTTrackMatch",&matchtrack);

  TCanvas *c = new TCanvas("c", "c", 0, 0, 600, 600);
  c->Divide(1,2);
  TH1F *samplenum = new TH1F("samplenum","number of sampling",50,0.,50.);
  TH2F *hdedxvsp = new TH2F("hdedxvsp","dedx vs p",100,0.,1.5, 100,0.,40.);

  TCanvas *c1 = new TCanvas("c1", "c1", 0, 0, 600, 600);
  c1->Divide(2,2);
  TH2F *hdedxvsp_p = new TH2F("hdedxvsp_p","dedx vs p for p",100,0.,1.5, 100,0.,40.);
  TH2F *hdedxvsp_pi = new TH2F("hdedxvsp_pi","dedx vs p for #pi",100,0.,1.5, 100,0.,40.);
  TH2F *hdedxvsp_e = new TH2F("hdedxvsp_e","dedx vs p for e",100,0.,1.5, 100,0.,40.);
  TH2F *hdedxvsp_k = new TH2F("hdedxvsp_k","dedx vs p for K",100,0.,1.5, 100,0.,40.);
  TH2F *hdedxvsp_mu = new TH2F("hdedxvsp_mu","dedx vs p for #mu",100,0.,1.5, 100,0.,40.);

  TCanvas *c2 = new TCanvas("c2", "c2", 0, 0, 600, 600);
  TH2F *hdedxvsp_reco = new TH2F("hdedxvsp_reco","dedx vs reco p",100,0.,1.5, 100,0.,40.);

 cout << treereco->GetEntriesFast() << " events" << endl;

  // loop on evts
 for(Int_t evt = 0; evt < treereco->GetEntriesFast(); evt++) {
    if(evt%100 == 0) cout << evt << endl;

    treemc->GetEntry(evt);
    treedigi->GetEntry(evt);
    treereco->GetEntry(evt);

    // tracks loop
    for (Int_t k = 0; k < track->GetEntriesFast(); k++) {

      PndSttTrack *stttrack = (PndSttTrack*) track->At(k);
      if(!stttrack) continue;
      PndSttTrackMatch *mtrack = (PndSttTrackMatch *) matchtrack->At(k);
      if(!mtrack) continue;
      PndMCTrack *MCtrack = (PndMCTrack*) mctrack->At(mtrack->GetMCTrackID());
      if(!MCtrack) continue;
      Int_t PDGcode = MCtrack->GetPdgCode();

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

      Int_t hitcounter = stttrack->GetNofHelixHits();
      std::vector<double> dedxvec;
      dedxvec.clear();
      TVector3 momentum = MCtrack->GetMomentum();
      Int_t losthit = 0;
      for (Int_t j = 0; j < hitcounter; j++) {
	Int_t iHHit = stttrack->GetHelixHitIndex(j);
	PndSttHelixHit *helixhit = (PndSttHelixHit*) hh->At(iHHit);
	if(!helixhit) continue;

	Int_t hitindex = helixhit->GetHitIndex();
// 	PndSttHit* hit = (PndSttHit*) digi->At(hitindex);
// 	PndSttPoint *point = (PndSttPoint*) pnt->At(hit->GetRefIndex());

	if(helixhit->GetdEdx() != 0) dedxvec.push_back(helixhit->GetdEdx());
	else losthit++;
      }

      hitcounter -= losthit;

      if(hitcounter > 0) {
	// truncated mean
	Double_t perc = 0.60;
	// sort
	std::sort(dedxvec.begin(), dedxvec.end());

	//truncated mean
	Double_t sum = 0;
	Int_t endnum = floor(hitcounter * perc);


	for(Int_t m = 0; m < endnum; m++) sum += dedxvec[m];
	Double_t tmean;
	if(endnum > 0) {
	  tmean = sum/(Double_t) endnum;
	  hdedxvsp->Fill(momentum->Mag(), tmean);
	  hdedxvsp_reco->Fill(ptot, tmean);

	  samplenum->Fill(hitcounter);

 	  if(abs(PDGcode) == 11)       { hdedxvsp_e->Fill(momentum->Mag(), tmean);}
	  else if(abs(PDGcode) == 13)  { hdedxvsp_mu->Fill(momentum->Mag(), tmean);}
	  else if(abs(PDGcode) == 211) { hdedxvsp_pi->Fill(momentum->Mag(), tmean);}
     	  else if(abs(PDGcode) == 321) { hdedxvsp_k->Fill(momentum->Mag(), tmean); }
    	  else if(abs(PDGcode) == 2212){ hdedxvsp_p->Fill(momentum->Mag(), tmean);}

	}
      }
    }
  }

  c->cd(1);
  hdedxvsp->Draw();
  c->cd(2);
  samplenum->Draw();

  c1->cd(1);
  hdedxvsp_e->SetMarkerColor(2); // red = electron
  hdedxvsp_e->Draw();
  c1->cd(2);
  hdedxvsp_k->SetMarkerColor(3); // green = kaon
  hdedxvsp_k->Draw();
  c1->cd(3);
  hdedxvsp_pi->SetMarkerColor(4); // blue = pion
  hdedxvsp_mu->SetMarkerColor(7); // light blue = muon
  hdedxvsp_pi->Draw();
  hdedxvsp_mu->Draw("SAME");
  c1->cd(4);
  hdedxvsp_p->SetMarkerColor(5); // yellow = proton
  hdedxvsp_p->Draw();

  c2->cd();
  hdedxvsp_reco->Draw();

  TFile *out_dedx = new TFile("out_dedx_helixhit.root", "RECREATE");
  hdedxvsp->Write();
  hdedxvsp_e->Write();
  hdedxvsp_mu->Write();
  hdedxvsp_pi->Write();
  hdedxvsp_k->Write();
  hdedxvsp_p->Write();
  hdedxvsp_reco->Write();
  samplenum->Write();
  out_dedx->Write();
  out_dedx->Close();



}
