// THIS MACRO FILLS DEDX / p HISTOS
// ifile = 0 1 2 3 4 = e mu pi k p


int dedx_p_chain(int ifile = 0){
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();//basic libraries


  TString directory[5] = {"production/e/", "production/mu/", "production/pi/", "production/k/", "production/p/"};
  TString directory2[5] = {"production2/e/", "production2/mu/", "production2/pi/", "production2/k/", "production2/p/"};

  TString simname  = "points_sttcombi.root";
  TString diginame = "digi_sttcombi.root";
  TString reconame = "reco_sttcombi.root";

  TH2F *hdedx_p_e = new TH2F("hdedx_p_e", "hdedx_p_e", 100, 0., 1.5, 100, 0., 30);
  TH2F *hdedx_p_mu = new TH2F("hdedx_p_mu", "hdedx_p_mu", 100, 0., 1.5, 100, 0., 30);
  TH2F *hdedx_p_pi = new TH2F("hdedx_p_pi", "hdedx_p_pi", 100, 0., 1.5, 100, 0., 30);
  TH2F *hdedx_p_k = new TH2F("hdedx_p_k", "hdedx_p_k", 100, 0., 1.5, 100, 0., 30);
  TH2F *hdedx_p_p = new TH2F("hdedx_p_p", "hdedx_p_p", 100, 0., 1.5, 100, 0., 30);

  hdedx_p_e->SetMarkerColor(2);  // red
  hdedx_p_mu->SetMarkerColor(4); // blue
  hdedx_p_pi->SetMarkerColor(3); // green
  hdedx_p_k->SetMarkerColor(6);  // fuchsia
  hdedx_p_p->SetMarkerColor(1);  // black

  // pointers
  TChain *chainsim = NULL, *chaindigi = NULL, *chainreco = NULL, *chaingen = NULL;

  TClonesArray *mctrackarray = NULL, *mcpointarray = NULL, *hitarray = NULL, *recotrackarray = NULL, *gentrackarray = NULL;
  PndMCTrack *mctrk = NULL;
  PndTrackCand *ctrk = NULL;
  PndTrack *gtrk = NULL;
  PndSttPoint *mcpoint  = NULL;
  PndSttHit *hit  = NULL;
  PndTrackCandHit candhit;

  TString sn = directory[ifile] + simname;
  TString dn = directory[ifile] + diginame;
  TString rn = directory[ifile] + reconame;

  TString sn2 = directory2[ifile] + simname;
  TString dn2 = directory2[ifile] + diginame;
  TString rn2 = directory2[ifile] + reconame;

  cout << "******************" << endl;
  cout << sn << " " << sn2 << " " <<  endl;
  cout << dn << " " << dn2 << " " <<  endl;
  cout << rn << " " << rn2 << " " <<  endl;



  // MC
  chainsim = new TChain("pndsim");
  chainsim->Add(sn);
  chainsim->Add(sn2);
//   chainsim->Add(sn3);
  mcpointarray = new TClonesArray("PndSttPoint");
  chainsim->SetBranchAddress("STTPoint",&mcpointarray);
  mctrackarray = new TClonesArray("PndMCTrack");
  chainsim->SetBranchAddress("MCTrack",&mctrackarray);

  // digi
  chaindigi = new TChain("pndsim");
  chaindigi->Add(dn);
  chaindigi->Add(dn2);
//   chaindigi->Add(dn3);
  hitarray = new TClonesArray("PndSttHit");
  chaindigi->SetBranchAddress("STTHit",&hitarray);

  // reco
  chainreco = new TChain("pndsim");
  chainreco->Add(rn);
  chainreco->Add(rn2);
//   chainreco->Add(rn3);
  gentrackarray = new TClonesArray("PndTrack");
  chainreco->SetBranchAddress("SttMvdGenTrack",&gentrackarray);
  recotrackarray = new TClonesArray("PndTrackCand");
  chainreco->SetBranchAddress("SttMvdTrackCand",&recotrackarray);

  for(int evt = 0; evt < 20000; evt++) {

    chainsim->GetEntry(evt);
    chaindigi->GetEntry(evt);
    chainreco->GetEntry(evt);

    if(evt%500 == 0) cout << evt << endl;

    TVector3 momentum;

    // loop on reco tracks
    for(int itrk = 0; itrk < gentrackarray->GetEntriesFast(); itrk++) {

      // get the genit track
      gtrk = (PndTrack*) gentrackarray->At(itrk);
      if(!gtrk) continue;

      // get the track candidate
      ctrk = (PndTrackCand*) recotrackarray->At(gtrk->GetRefIndex());
      if(!ctrk) continue;

      int mcIndex = ctrk->getMcTrackId();


      // GET ONLY PRIMARY TRACKS!!!     // CHECK
      if(mcIndex != 0) continue;

      // CUT ON KALMAN FLAG             // CHECK
      if(gtrk->GetFlag() < 0) continue;

      // track momentum ON FIRST PLANE (CHECK: it has
      // to be backprop, but indeed the PndTrack does
      // not contain the backprop momentum)
      momentum = gtrk->GetParamFirst().GetMomentum();

      // CHECK CUT on momentum
      if(momentum.Mag() > 1.2) continue;

      std::vector<double> dedx;

  // loop on track hits
      for(int ihit = 0; ihit < ctrk->GetNHits(); ihit++) {
	PndTrackCandHit candhit = ctrk->GetSortedHit(ihit);
	Int_t hitid = candhit.GetHitId();
	hit = (PndSttHit*) hitarray->At(hitid);
	if(!hit)  continue;

	// dE/dx calculation ==================
	Double_t tuberadius = 0.5; // CHECK 0.5 cm
	Double_t distance = 2 * sqrt(tuberadius * tuberadius - hit->GetIsochrone() * hit->GetIsochrone()); // cm
	Double_t coslam = momentum.Perp() / momentum.Mag();
	distance = distance / coslam;
	double de_dx = 0;
	if (distance != 0)  de_dx = hit->GetDepCharge()/(1000000 * distance);  // in arbitrary units
	// de/dx
	dedx.push_back(de_dx);
	// ====================================

      }

      // from MIN to MAX
      std::sort(dedx.begin(), dedx.end());


      // truncated mean
      Double_t perc = 0.70;                            // CHECK

      //truncated mean
      Double_t sum = 0;
      Int_t lastpnt = int(floor(dedx.size() * perc));


      for(int ihit = 0; ihit < lastpnt; ihit++) sum += dedx[ihit];

      // CHECK CUT on n of hits (> 5 already truncated)
      if(lastpnt > 5 && dedx.size() > 0) {
	//	if(lastpnt > 12) {                             // CHECK

	double mean = sum/(Double_t) lastpnt;

	switch(ifile) {
	case 0:
	  hdedx_p_e->Fill(momentum.Mag(), mean); break;
	case 1:
	  hdedx_p_mu->Fill(momentum.Mag(), mean); break;
	case 2:
	  hdedx_p_pi->Fill(momentum.Mag(), mean); break;
	case 3:
	  hdedx_p_k->Fill(momentum.Mag(), mean); break;
	case 4:
	  hdedx_p_p->Fill(momentum.Mag(), mean);
	}
      }
    }
  }

  TFile output("dedx_out.root", "UPDATE");
  switch(ifile) {
  case 0:
    hdedx_p_e->Write(); break;
  case 1:
    hdedx_p_mu->Write(); break;
  case 2:
    hdedx_p_pi->Write(); break;
  case 3:
    hdedx_p_k->Write(); break;
  case 4:
    hdedx_p_p->Write();
  }
  output.Write();
  output.Close();

  return 0;
}

