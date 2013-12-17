void BkgSignalCount(TString path="/panda/pandaroot/macro/lmd/testPixel/mom_1_5/", int nEvents=1000,int nStart=0){

  // ---- Input file -------------------------------------------------------------
  TString filename = "Lumi_TrksQA_";
  filename+=nStart;
  filename+=".root";
  //----------------------------------------------------------------------------------

  // ---- Output file -------------------------------------------------------------
  TString out=path+"/Lumi_BkgSig_";
  out += nStart;
  out += ".root";
  TFile *f = new TFile(out,"RECREATE");
  // ---------------------------------------------------------------------------------

  TChain tTrkRec("cbmsim");
  TString in=path+filename;
  tTrkRec.Add(in);

  //--- Tracks -------------------------------------------------------------------
  TClonesArray* qtrk=new TClonesArray("PndLmdTrackQ");
  tTrkRec.SetBranchAddress("LMDTrackQ",&qtrk);  //Tracks
  //----------------------------------------------------------------------------------

  //--- histogramms for human ananlysis ---------------------------------
  TH1 *hthsigrec = new TH1D("hthsigrec","#theta of rec.trks belong to signal; #theta_{REC}, mrad",2e2,0,20);
  TH1 *hthbkgrec = new TH1D("hthbkgrec","#theta of rec.trks belong to bkg; #theta_{REC}, mrad",2e2,0,20);
  TH1 *hthfakerec = new TH1D("hthfakerec","#theta of rec.trks don't belong to MC; #theta_{REC}, mrad",2e3,0,2000);
  TH1 *hthsigmissmc = new TH1D("hthsigmissmc","#theta of missed mc.trks belong to signal; #theta_{MC}, mrad",3.5e3,0,3500);
  TH1 *hthbkgmissmc = new TH1D("hthbkgmissmc","#theta of missed mc.trks belong to bkg; #theta_{MC}, mrad",3.5e3,0,3500);

 TH1 *hphsigrec = new TH1D("hphsigrec","#phi of rec.trks belong to signal; #phi_{REC}, rad",2e3,-TMath::Pi(),TMath::Pi());
  TH1 *hphbkgrec = new TH1D("hphbkgrec","#phi of rec.trks belong to bkg; #phi_{REC}, rad",2e3,-TMath::Pi(),TMath::Pi());
  TH1 *hphfakerec = new TH1D("hphfakerec","#phi of rec.trks don't belong to MC; #phi_{REC}, rad",2e3,-TMath::Pi(),TMath::Pi());
  TH1 *hphsigmissmc = new TH1D("hphsigmissmc","#phi of missed mc.trks belong to signal; #phi_{MC}, rad",2e3,-TMath::Pi(),TMath::Pi());
  TH1 *hphbkgmissmc = new TH1D("hphbkgmissmc","#phi of missed mc.trks belong to bkg; #phi_{MC}, rad",2e3,-TMath::Pi(),TMath::Pi());

  TH1 *hxsigrec = new TH1D("hxsigrec","x of rec.trks belong to signal; x_{REC}, cm",2e3,-2e2,2e2);
  TH1 *hxbkgrec = new TH1D("hxbkgrec","x of rec.trks belong to bkg; x_{REC}, cm",2e3,-2e2,2e2);
  TH1 *hxfakerec = new TH1D("hxfakerec","x of rec.trks don't belong to MC; x_{REC}, cm",2e3,-2e2,2e2);
  TH1 *hxsigmissmc = new TH1D("hxsigmissmc","x of missed mc.trks belong to signal; x_{MC}, cm",2e3,-2e2,2e2);
  TH1 *hxbkgmissmc = new TH1D("hxbkgmissmc","x of missed mc.trks belong to bkg; x_{MC}, cm",2e3,-2e2,2e2);

  TH1 *hysigrec = new TH1D("hysigrec","y of rec.trks belong to signal; y_{REC}, cm",2e3,-2e2,2e2);
  TH1 *hybkgrec = new TH1D("hybkgrec","y of rec.trks belong to bkg; y_{REC}, cm",2e3,-2e2,2e2);
  TH1 *hyfakerec = new TH1D("hyfakerec","y of rec.trks don't belong to MC; y_{REC}, cm",2e3,-2e2,2e2);
  TH1 *hysigmissmc = new TH1D("hysigmissmc","y of missed mc.trks belong to signal; y_{MC}, cm",2e3,-2e2,2e2);
  TH1 *hybkgmissmc = new TH1D("hybkgmissmc","y of missed mc.trks belong to bkg; y_{MC}, cm",2e3,-2e2,2e2);

 TH1 *hmomsigrec = new TH1D("hmomsigrec","mom of rec.trks belong to signal; P_{REC}, GeV/c",2e3,-20,20);
  TH1 *hmombkgrec = new TH1D("hmombkgrec","mom of rec.trks belong to bkg; P_{REC}, GeV/c",2e3,-20,20);
  TH1 *hmomfakerec = new TH1D("hmomfakerec","mom of rec.trks don't belong to MC; P_{REC}, GeV/c",2e3,-20,20);
  TH1 *hmomsigmissmc = new TH1D("hmomsigmissmc","mom of missed mc.trks belong to signal; P_{MC}, GeV/c",2e3,-20,20);
  TH1 *hmombkgmissmc = new TH1D("hmombkgmissmc","mom of missed mc.trks belong to bkg; P_{MC}, GeV/c",2e3,-20,20);

  TH1 *hPDGrec  = new TH1I("hPDGrec","PDG code of rec.trks",1e4,-5e3,5e3);
  TH1 *hMultiMC  = new TH1I("hMultiMC","multiplicity of sim ev",1e2,0,1e2);
  TH1 *hMultiREC  = new TH1I("hMultiREC","multiplicity of rec ev",1e2,0,1e2);
  //-----------------------------------------------------------------------------------

  // Go over all events ---------------------------------------------------------
  for (Int_t j=0; j<nEvents; j++){
    tTrkRec.GetEntry(j);
    const int nTrksQ = qtrk->GetEntriesFast();//Number of trks/ev
    // Go over all trks ----------------------------------------------------------
    for(int iq=0;iq<nTrksQ;iq++){
      PndLmdTrackQ *trkcur = qtrk->At(iq);
      int trkStat = trkcur->GetTrkRecStatus();
      int flSecondary = trkcur->GetSecondary();
      int PDGcode = trkcur->GetPDGcode();
      int sumPDGev = trkcur->GetSumEvPDG();
      int multimc = trkcur->GetEvMCMulti();
      hMultiMC->Fill(multimc);
      int multirec = trkcur->GetEvRECMulti();
      hMultiREC->Fill(multirec);
      double thtrk = 1e3*(trkcur->GetIPtheta());
      double thmctrk = 1e3*(trkcur->GetMCtheta());
      double phtrk = (trkcur->GetIPphi());
      double phmctrk = (trkcur->GetMCphi());
      TVector3 pcaTrkrec;
      trkcur->GetIPpoint(pcaTrkrec);
      double xtrk = pcaTrkrec.X();
      double ytrk = pcaTrkrec.Y();
      TVector3 pcaTrkmc;
      trkcur->GetMCpoint(pcaTrkmc);
      double xmctrk = pcaTrkmc.X();
      double ymctrk = pcaTrkmc.Y();
      double momtrk = trkcur->GetIPmom();
      double mommctrk = trkcur->GetMCmom();
      if(trkStat==0){ //GOOD rec.trks
	hPDGrec->Fill(PDGcode);
	if(sumPDGev==4424 && PDGcode==-2212){//signal
	  hthsigrec->Fill(thtrk);
	  hphsigrec->Fill(phtrk);
	  hxsigrec->Fill(xtrk);
	  hysigrec->Fill(ytrk);
	  hmomsigrec->Fill(momtrk);
	}
	else{//bkg
	  hthbkgrec->Fill(thtrk);
	  hphbkgrec->Fill(phtrk);
	  hxbkgrec->Fill(xtrk);
	  hybkgrec->Fill(ytrk);
	  hmombkgrec->Fill(momtrk);
	}
      }
      else{// not GOOD rec.trk
	if(trkStat<0){//trk was missed
	  //	  if(PDGcode==-2212){//signal
	  if(sumPDGev==4424){//signal
	    hthsigmissmc->Fill(thmctrk);
	    hphsigmissmc->Fill(phmctrk);
	    hxsigmissmc->Fill(xmctrk);
	    hysigmissmc->Fill(ymctrk);
	    hmomsigmissmc->Fill(mommctrk);
	  }
	  else{
	    hthbkgmissmc->Fill(thmctrk);
	    hphbkgmissmc->Fill(phmctrk);
	    hxbkgmissmc->Fill(xmctrk);
	    hybkgmissmc->Fill(ymctrk);
	    hmombkgmissmc->Fill(mommctrk);
	  }
	}
	if(trkStat>0){// fake trk
	  hthfakerec->Fill(thtrk);
	  hphfakerec->Fill(phtrk);
	  hxfakerec->Fill(xtrk);
	  hyfakerec->Fill(ytrk);
	  hmomfakerec->Fill(momtrk);
	}
      }
    }
    //--------------------------------------------------------------------------------
 }
  //-----------------------------------------------------------------------------------
  hthsigrec->Write();
  hthbkgrec->Write();
  hthfakerec->Write();
  hthsigmissmc->Write();
  hthbkgmissmc->Write();

  hphsigrec->Write();
  hphbkgrec->Write();
  hphfakerec->Write();
  hphsigmissmc->Write();
  hphbkgmissmc->Write();

  hxsigrec->Write();
  hxbkgrec->Write();
  hxfakerec->Write();
  hxsigmissmc->Write();
  hxbkgmissmc->Write();

  hysigrec->Write();
  hybkgrec->Write();
  hyfakerec->Write();
  hysigmissmc->Write();
  hybkgmissmc->Write();

  hmomsigrec->Write();
  hmombkgrec->Write();
  hmomfakerec->Write();
  hmomsigmissmc->Write();
  hmombkgmissmc->Write();

  hPDGrec->Write();
  hMultiMC->Write();
  hMultiREC->Write();
  f->Close();
}
