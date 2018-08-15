int TB_checkHitReconstruction(Double_t pntHitDist=0.1) {

  TFile* simFile = TFile::Open("Gem_MvdStt_3Stations_DPM_n10000.root");
  TTree* simTree = (TTree*)simFile->Get("pndsim");
  TFile* digFile = TFile::Open("Gem_MvdStt_3Stations_DPM_n10000_digiSorted_a10000_c1000x10.root");
  TTree* digTree = (TTree*)digFile->Get("pndsim");
  TFile* cluFile = TFile::Open("Gem_MvdStt_3Stations_DPM_n10000_CluHi_a10000_c1000x10.root");
  TTree* cluTree = (TTree*)cluFile->Get("pndsim");

  cout << "FILES FETCHED" << endl;

  FairEventHeader* evHeader = NULL;
  TClonesArray* mcTrackArray  = new TClonesArray("PndMCTrack");
  TClonesArray* gemPointArray = new TClonesArray("PndGemMCPoint");
  TClonesArray* gemHitArray   = new TClonesArray("PndGemHit");

  simTree->SetBranchAddress("MCTrack",&mcTrackArray);
  simTree->SetBranchAddress("GEMPoint",&gemPointArray);
  digTree->SetBranchAddress("EventHeader.",&evHeader);
  cluTree->SetBranchAddress("GEMHit",&gemHitArray);

  cout << "GOT ALL THE BRANCHES" << endl;

  TH2F* hist_YT = new TH2F("hist_YT","hist_YT",200,-10,10,200,-75,125);
  TH2F* hist_XT = new TH2F("hist_XT","hist_XT",200,-10,10,200,-75,125);
  TH2F* hist_XY = new TH2F("hist_XY","hist_XY",200,-10,10,200,-10,10);
  TH1F* hist_T  = new TH1F("hist_T", "hist_T", 500,-200,300);
  TH1F* hist_Y  = new TH1F("hist_Y", "hist_Y", 500,-25,25);
  TH1F* hist_X  = new TH1F("hist_X", "hist_X", 500,-25,25);
  TH1F* hist_N  = new TH1F("hist_N","Nof of matched hits",101,-0.5,100.5);

  const Int_t kNofStat = 3;

  const Int_t kNofEvents  = 10000;//cluTree->GetEntries();
  const Int_t kMaxNofHits = 10000;
  //  cout << "what's not static here?"<< endl;
  Bool_t hitAssignedToPoint[kNofEvents][kMaxNofHits];
  //  cout << "really?" << endl
  for ( Int_t iev = 0 ; iev < kNofEvents ; iev++ ) {
    for ( Int_t ihit = 0 ; ihit < kMaxNofHits ; ihit++ ) {
      hitAssignedToPoint[iev][ihit] = kFALSE;
    }
  }
  cout << "DONE" << endl;

  Double_t statZPos[kNofStat] = {117.,153.,189.};

  TH1F* fhPointRadNof    [kNofStat][2];
  TH1F* fhPointRadReco   [kNofStat][2];
  TH1F* fhPointRadRecoEff[kNofStat][2];
  TH2F* fhPointNof       [kNofStat][2];
  TH2F* fhPointReco      [kNofStat][2];
  TH2F* fhPointRecoEff   [kNofStat][2];

  TH1F* fhHitRadNof     [kNofStat][2];
  TH1F* fhHitRadFake    [kNofStat][2];
  TH1F* fhHitRadFakeProb[kNofStat][2];
  TH2F* fhHitNof        [kNofStat][2];
  TH2F* fhHitFake       [kNofStat][2];
  TH2F* fhHitFakeProb   [kNofStat][2];

  for ( Int_t istat = 0 ; istat < kNofStat ; istat++ ) {
    for ( Int_t isens = 0 ; isens < 2 ; isens++ ) {
      fhPointRadNof      [istat][isens] = new TH1F(Form("fhPointRadNof_s%d_s%d",istat,isens),
						   Form("Number of points, station %d, sensor %d",istat,isens),
						   100,0,100);
      fhPointRadReco     [istat][isens] = new TH1F(Form("fhPointRadReco_s%d_s%d",istat,isens),
						   Form("Number of reconstructed points (point-hit %.2f cm), station %d, sensor %d",pntHitDist,istat,isens),
						   100,0,100);
      fhPointRadRecoEff  [istat][isens] = new TH1F(Form("fhPointRadRecoEff_s%d_s%d",istat,isens),
						   Form("Hit finding efficiency (point-hit %.2f cm), station %d, sensor %d",pntHitDist,istat,isens),
						   100,0,100);
      fhPointNof         [istat][isens] = new TH2F(Form("fhPointNof_s%d_s%d",istat,isens),
						   Form("Number of points, station %d, sensor %d",istat,isens),
						   140,-70,70,
						   140,-70,70);
      fhPointReco        [istat][isens] = new TH2F(Form("fhPointReco_s%d_s%d",istat,isens),
						   Form("Number of reconstructed points (point-hit %.2f cm), station %d, sensor %d",pntHitDist,istat,isens),
						   140,-70,70,
						   140,-70,70);
      fhPointRecoEff     [istat][isens] = new TH2F(Form("fhPointRecoEff_s%d_s%d",istat,isens),
						   Form("Hit finding efficiency (point-hit %.2f cm), station %d, sensor %d",pntHitDist,istat,isens),
						   140,-70,70,
						   140,-70,70);
      fhHitRadNof      [istat][isens] = new TH1F(Form("fhHitRadNof_s%d_s%d",istat,isens),
						   Form("Number of hits, station %d, sensor %d",istat,isens),
						   100,0,100);
      fhHitRadFake     [istat][isens] = new TH1F(Form("fhHitRadFake_s%d_s%d",istat,isens),
						   Form("Number of fake hits (point-hit %.2f cm), station %d, sensor %d",pntHitDist,istat,isens),
						   100,0,100);
      fhHitRadFakeProb [istat][isens] = new TH1F(Form("fhHitRadFakeProb_s%d_s%d",istat,isens),
						   Form("Fake hit probability (point-hit %.2f cm), station %d, sensor %d",pntHitDist,istat,isens),
						   100,0,100);
      fhHitNof      [istat][isens] = new TH2F(Form("fhHitNof_s%d_s%d",istat,isens),
						   Form("Number of hits, station %d, sensor %d",istat,isens),
						   140,-70,70,
						   140,-70,70);
      fhHitFake     [istat][isens] = new TH2F(Form("fhHitFake_s%d_s%d",istat,isens),
						   Form("Number of fake hits (point-hit %.2f cm), station %d, sensor %d",pntHitDist,istat,isens),
						   140,-70,70,
						   140,-70,70);
      fhHitFakeProb [istat][isens] = new TH2F(Form("fhHitFakeProb_s%d_s%d",istat,isens),
						   Form("Fake hit probability (point-hit %.2f cm), station %d, sensor %d",pntHitDist,istat,isens),
						   140,-70,70,
						   140,-70,70);
    }
  }

  PndGemMCPoint* tempPoint = NULL;
  PndGemHit*     tempHit   = NULL;

  Int_t startRev = 0;

  for ( Int_t imev = 0 ; imev < simTree->GetEntries() ; imev++ ) {
    //  for ( Int_t imev = 0 ; imev < 100 ; imev++ ) {
    simTree->GetEntry(imev);
    digTree->GetEntry(imev);
       cout << "\r MC EVENT " << imev << flush;

    for ( Int_t ipoint = 0 ; ipoint < gemPointArray->GetEntries() ; ipoint++ ) {
      tempPoint = (PndGemMCPoint*)gemPointArray->At(ipoint);
      // cout << "POINT @ ( " << tempPoint->GetX() << " , " << tempPoint->GetY() << " , " << tempPoint->GetZ() << " )"
      // 	   << " at time " << evHeader->GetEventTime() << " + " << tempPoint->GetTime() << " ns" << endl;
      Int_t pntStat = -1;
      for ( Int_t istat = 0 ; istat < kNofStat ; istat++ ) {
	if ( TMath::Abs(statZPos[istat]-tempPoint->GetZ()) < 5. ) {
	  pntStat = istat;
	}
      }
      if ( pntStat == -1 ) cout << "Station at " << tempPoint->GetZ() << " not identified" << endl;

      Int_t    pntSens = (tempPoint->GetZ()>=statZPos[pntStat]?1:0);
      Double_t pntX    = (tempPoint->GetX()+tempPoint->GetXOut())/2.;
      Double_t pntY    = (tempPoint->GetY()+tempPoint->GetYOut())/2.;
      Double_t pntRad  = TMath::Sqrt(pntX*pntX+pntY*pntY);

      Int_t irev = 0;

      //	cout << "\r Event " << imev << " check from " << startRev << flush;
      Int_t nofMatchedHits = 0;

      for ( irev = startRev ; irev < cluTree->GetEntries() ; irev++ ) {
	//	if ( verbose )
	// if ( ipoint == 0 )
	//   cout << "   EVENT " << irev << endl;

	Int_t nofUsableHits = 0;

	cluTree->GetEntry(irev);

	if ( gemHitArray->GetEntries() > 0 ) { // check if first hit has time much bigger than point time
	  tempHit = (PndGemHit*)gemHitArray->At(0);
	  if ( tempHit->GetTimeStamp() > evHeader->GetEventTime()+tempPoint->GetTime() + 200. )
	    break;
	}

	if ( gemHitArray->GetEntries() > kMaxNofHits )
	  cout << "THERE ARE MORE HITS THAN EXPECTED, " << gemHitArray->GetEntries() << " IN EVENT " << irev << "." << endl;

	for ( Int_t ihit = 0 ; ihit < gemHitArray->GetEntries() ; ihit++ ) {
	  tempHit = (PndGemHit*)gemHitArray->At(ihit);

	  if ( ipoint == 0 ) {
	    if ( tempHit->GetTimeStamp() > evHeader->GetEventTime() )
	      nofUsableHits ++;
	  }

	  // cout << "HIT @ ( " << tempHit->GetX() << " , " << tempHit->GetY() << " , " << tempHit->GetZ() << " )"
	  //      << " at time " << tempHit->GetTimeStamp() << endl;

	  if ( TMath::Abs(tempHit->GetZ()-tempPoint->GetZ()) > 1. ) continue;// same station

	  Bool_t goodT = kFALSE;
	  Bool_t goodX = kFALSE;
	  Bool_t goodY = kFALSE;

	  // if ( TMath::Abs(tempHit->GetX()-tempPoint->GetX()) < pntHitDist ) goodX = kTRUE;
	  // if ( TMath::Abs(tempHit->GetY()-tempPoint->GetY()) < pntHitDist ) goodY = kTRUE;
	  if ( TMath::Abs(tempHit->GetX()-pntX) < pntHitDist ) goodX = kTRUE;
	  if ( TMath::Abs(tempHit->GetY()-pntY) < pntHitDist ) goodY = kTRUE;
	  if ( TMath::Abs(tempHit->GetTimeStamp()-(evHeader->GetEventTime()+tempPoint->GetTime())-5.5) < 8. ) goodT = kTRUE;

	  if ( goodX ) {
	    hist_YT->Fill(tempHit->GetY()-pntY,
			  tempHit->GetTimeStamp()-(evHeader->GetEventTime()+tempPoint->GetTime()));
	    if ( goodY ) {
	      hist_T->Fill(tempHit->GetTimeStamp()-(evHeader->GetEventTime()+tempPoint->GetTime()));
	      if ( goodT ) {
		//		((PndGemHit*)gemHitArray->At(ihit))->SetCharge(-100);
		hitAssignedToPoint[irev][ihit] = kTRUE;
		nofMatchedHits++;
	      }
	    }
	    if ( goodT )
	      hist_Y->Fill(tempHit->GetY()-pntY);//tempPoint->GetY());
	  }
	  if ( goodY ) {
	    hist_XT->Fill(tempHit->GetX()-pntX,//tempPoint->GetX(),
			  tempHit->GetTimeStamp()-(evHeader->GetEventTime()+tempPoint->GetTime()));
	    if ( goodT )
	      hist_X->Fill(tempHit->GetX()-pntX);//tempPoint->GetX());
	  }
	  if ( goodT ) {
	    hist_XY->Fill(tempHit->GetX()-pntX,//tempPoint->GetX(),
			  tempHit->GetY()-pntY);//tempPoint->GetY());
	  }

	}
	if ( ipoint == 0 && nofUsableHits == 0 && gemHitArray->GetEntries() > 0 )
	  startRev++;
      }
      hist_N->Fill(nofMatchedHits);

      fhPointRadNof[pntStat][pntSens]->Fill(pntRad);
      fhPointNof   [pntStat][pntSens]->Fill(pntX,pntY);
      if ( nofMatchedHits > 0 ) {
	fhPointRadReco[pntStat][pntSens]->Fill(pntRad);
	fhPointReco   [pntStat][pntSens]->Fill(pntX,pntY);
      }
    }
  }

  for ( irev = 0 ; irev < cluTree->GetEntries() ; irev++ ) {
  //  for ( irev = 0 ; irev < 100 ; irev++ ) {
    cluTree->GetEntry(irev);
    for ( Int_t ihit = 0 ; ihit < gemHitArray->GetEntries() ; ihit++ ) {
      //cout << "event " << irev << " hit " << ihit << " - " << tempHit->GetCharge() << " ----> " << (hitAssignedToPoint[irev][ihit]?"MATCHED":"") << end;l
      tempHit = (PndGemHit*)gemHitArray->At(ihit);

      fhHitRadNof      [tempHit->GetStationNr()-1][tempHit->GetSensorNr()-1]->Fill(TMath::Sqrt(tempHit->GetX()*tempHit->GetX()+tempHit->GetY()*tempHit->GetY()));
      fhHitNof         [tempHit->GetStationNr()-1][tempHit->GetSensorNr()-1]->Fill(tempHit->GetX(),tempHit->GetY());
      if ( !hitAssignedToPoint[irev][ihit] ) {
	fhHitRadFake     [tempHit->GetStationNr()-1][tempHit->GetSensorNr()-1]->Fill(TMath::Sqrt(tempHit->GetX()*tempHit->GetX()+tempHit->GetY()*tempHit->GetY()));
	fhHitFake        [tempHit->GetStationNr()-1][tempHit->GetSensorNr()-1]->Fill(tempHit->GetX(),tempHit->GetY());
      }
    }
  }

  for ( Int_t istat = 0 ; istat < kNofStat ; istat++ ) {
    for ( Int_t isens = 0 ; isens < 2 ; isens++ ) {
      fhPointRadNof      [istat][isens]->Sumw2();
      fhPointRadReco     [istat][isens]->Sumw2();
      fhPointRadRecoEff  [istat][isens]->Divide(fhPointRadReco     [istat][isens],fhPointRadNof      [istat][isens],1.,1.,"B");
      fhPointRadRecoEff  [istat][isens]->Scale(100.);
      fhHitRadNof      [istat][isens]->Sumw2();
      fhHitRadFake     [istat][isens]->Sumw2();
      fhHitRadFakeProb [istat][isens]->Divide(fhHitRadFake     [istat][isens],fhHitRadNof      [istat][isens],1.,1.,"B");
      fhHitRadFakeProb [istat][isens]->Scale(100.);

      fhPointNof    [istat][isens]->Sumw2();
      fhPointReco   [istat][isens]->Sumw2();
      fhPointRecoEff[istat][isens]->Divide(fhPointReco[istat][isens],fhPointNof[istat][isens],1.,1.,"B");
      fhPointRecoEff[istat][isens]->Scale(100.);
      fhHitNof      [istat][isens]->Sumw2();
      fhHitFake     [istat][isens]->Sumw2();
      fhHitFakeProb [istat][isens]->Divide(fhHitFake     [istat][isens],fhHitNof      [istat][isens],1.,1.,"B");
      fhHitFakeProb [istat][isens]->Scale(100.);
    }
  }

  TCanvas* canv_XY = new TCanvas("canv_XY","canv_XY",10,10,1000,800);
  hist_XY->DrawClone("colz");

  TCanvas* canv_XT = new TCanvas("canv_XT","canv_XT",10,10,1000,800);
  hist_XT->DrawClone("colz");

  TCanvas* canv_YT = new TCanvas("canv_YT","canv_YT",10,10,1000,800);
  hist_YT->DrawClone("colz");

  TCanvas* canv_X = new TCanvas("canv_X","canv_X",10,10,1000,800);
  hist_X->DrawClone();

  TCanvas* canv_Y = new TCanvas("canv_Y","canv_Y",10,10,1000,800);
  hist_Y->DrawClone();

  TCanvas* canv_T = new TCanvas("canv_T","canv_T",10,10,1000,800);
  hist_T->DrawClone();

  TCanvas* canv_N = new TCanvas("canv_N","canv_N",10,10,1000,800);
  hist_N->DrawClone();

  TCanvas* canv_E = new TCanvas("canv_E","canv_E",10,10,1000,800);
  TString drawString = "";
  for ( Int_t istat = 0 ; istat < kNofStat ; istat++ ) {
    for ( Int_t isens = 0 ; isens < 2 ; isens++ ) {
      fhPointRadRecoEff  [istat][isens]->SetLineColor(2+istat+isens*4);
      fhPointRadRecoEff  [istat][isens]->SetLineWidth(2);
      fhPointRadRecoEff  [istat][isens]->DrawClone(drawString.Data());
      drawString = "kSAME";
    }
  }

  TCanvas* canv_F = new TCanvas("canv_F","canv_F",10,10,1000,800);
  TString drawString = "";
  for ( Int_t istat = 0 ; istat < kNofStat ; istat++ ) {
    for ( Int_t isens = 0 ; isens < 2 ; isens++ ) {
      fhHitRadFakeProb  [istat][isens]->SetLineColor(2+istat+isens*4);
      fhHitRadFakeProb  [istat][isens]->SetLineWidth(2);
      fhHitRadFakeProb  [istat][isens]->DrawClone(drawString.Data());
      drawString = "kSAME";
    }
  }

  TFile* outFile = new TFile(Form("hit_reco_%.1f.root",pntHitDist),"RECREATE");

  hist_YT->Write();
  hist_XT->Write();
  hist_XY->Write();
  hist_T ->Write();
  hist_Y ->Write();
  hist_X ->Write();
  hist_N ->Write();

  for ( Int_t istat = 0 ; istat < kNofStat ; istat++ ) {
    for ( Int_t isens = 0 ; isens < 2 ; isens++ ) {
      fhPointRadNof    [istat][isens]->Write();
      fhPointRadReco   [istat][isens]->Write();
      fhPointRadRecoEff[istat][isens]->Write();
      fhHitRadNof      [istat][isens]->Write();
      fhHitRadFake     [istat][isens]->Write();
      fhHitRadFakeProb [istat][isens]->Write();
      fhPointNof       [istat][isens]->Write();
      fhPointReco      [istat][isens]->Write();
      fhPointRecoEff   [istat][isens]->Write();
      fhHitNof         [istat][isens]->Write();
      fhHitFake        [istat][isens]->Write();
      fhHitFakeProb    [istat][isens]->Write();
    }
    }
  outFile->Write();
  outFile->Close();
  return 0;
}
