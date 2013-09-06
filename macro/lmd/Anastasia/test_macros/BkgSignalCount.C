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
      if(trkStat==0){ //GOOD rec.trks
	hPDGrec->Fill(PDGcode);
	if(sumPDGev==4424){//signal
	  hthsigrec->Fill(thtrk);
	}
	else{//bkg
	  hthbkgrec->Fill(thtrk);
	}
      }
      else{// not GOOD rec.trk
	if(trkStat<0){//trk was missed
	  //	  if(PDGcode==-2212){//signal
	  if(sumPDGev==4424){//signal
	    hthsigmissmc->Fill(thmctrk);
	  }
	  else{
	    hthbkgmissmc->Fill(thmctrk);
	  }
	}
	if(trkStat>0){// fake trk
	  hthfakerec->Fill(thtrk);
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
  hPDGrec->Write();
  hMultiMC->Write();
  hMultiREC->Write();
  f->Close();
}
