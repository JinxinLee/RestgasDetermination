void EffTrks(int nEvents=1000,int nStart=0,TString path="/panda/pandaroot/macro/lmd/testPixel/mom_1_5/"){

  // ---- Input file -------------------------------------------------------------
  TString filename = "Lumi_TrksQA_";
  filename+=nStart;
  filename+=".root";
  //----------------------------------------------------------------------------------

  // ---- Output file -------------------------------------------------------------
  TString out=path+"/Lumi_EffTrk_";
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
  TH1 *hthetaMC_all = new TH1D("hthetaMC_all",";#theta_{MC}, mrad (all)",2e2,0,20);
  TH1 *hthetaMC = new TH1D("hthetaMC",";#theta_{MC}, mrad (acc)",2e2,0,20);
  TH1 *hthetaMC_cutMom = new TH1D("hthetaMC_cutMom",";#theta_{MC}, mrad (acc)",2e2,0,20);
  TH1 *hthetaREC = new TH1D("hthetaREC",";#theta_{REC}, mrad",2e2,0,20);
  TH2 *hthetaMCREC = new TH2D("hthetaMCREC",";#theta_{MC}, mrad;#theta_{REC}, mrad",2e2,0,20,2e2,0,20);
  TH2 *hthetaMCREC_cutMom = new TH2D("hthetaMCREC_cutMom",";#theta_{MC}, mrad;#theta_{REC}, mrad",2e2,0,20,2e2,0,20);
  TH2 *hdthMCREC_dmom = new TH2D("hdthMCREC_dmom",";#theta_{MC}-#theta_{REC}, mrad;(P_{REC}-P_{MC})/P_{MC}",2e3,-12,12,4e3,-0.1,1.5);
  TH1 *hthprimrec = new TH1D("hthprimrec","#theta of rec.trks belong to primary; #theta_{REC}, mrad",2e2,0,20);
  TH1 *hthsecondrec = new TH1D("hthsecondrec","#theta of rec.trks belong to secondaries; #theta_{REC}, mrad",2e2,0,20);
  TH1 *hthfakerec = new TH1D("hthfakerec","#theta of rec.trks don't belong to MC; #theta_{REC}, mrad",7e3,0,3500);
  TH1 *hthprimmissmc = new TH1D("hthprimmissmc","#theta of missed mc.trks belong to primary; #theta_{MC}, mrad",7e3,0,3500);
  TH1 *hthbkgmissmc = new TH1D("hthbkgmissmc","#theta of missed mc.trks belong to secondaries; #theta_{MC}, mrad",7e3,0,3500);
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
      double Prec = trkcur->GetIPmom();
      double Pmc =  trkcur->GetMCmom();
      hthetaMC_all->Fill(thmctrk);
      if(trkStat==0){ //GOOD rec.trks
	hthetaREC->Fill(thtrk);
	hthetaMC->Fill(thmctrk);
	if((Prec-Pmc)/Pmc<0.0005){
	  hthetaMC_cutMom->Fill(thmctrk);
	  hthetaMCREC_cutMom->Fill(thmctrk,thtrk);
	}
	hdthMCREC_dmom->Fill(thmctrk-thtrk,(Prec-Pmc)/Pmc);
	hthetaMCREC->Fill(thmctrk,thtrk);
	hPDGrec->Fill(PDGcode);
	//	if(sumPDGev==4424){//signal
	if(flSecondary==0){//primaries only
	  hthprimrec->Fill(thtrk);
	}
	else{//bkg
	  hthsecondrec->Fill(thtrk);
	}
      }
      else{// not GOOD rec.trk
	if(trkStat<0){//trk was missed
	  //	  if(PDGcode==-2212){//signal
	  //	  if(sumPDGev==4424){//signal
	  if(flSecondary==0){//primaries only
	    hthprimmissmc->Fill(thmctrk);
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
  hthprimrec->Write();
  hthsecondrec->Write();
  hthfakerec->Write();
  hthprimmissmc->Write();
  hthbkgmissmc->Write();
  hPDGrec->Write();
  hMultiMC->Write();
  hMultiREC->Write();
  hthetaREC->Write();
  hthetaMC->Write();
  hthetaMC_cutMom->Write();
  hthetaMC_all->Write();
  hthetaMCREC->Write();
  hthetaMCREC_cutMom->Write();
  hdthMCREC_dmom->Write();
  f->Close();
}
