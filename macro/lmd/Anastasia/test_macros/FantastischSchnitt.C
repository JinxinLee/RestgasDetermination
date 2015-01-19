// macro for X&Y cut visual check
void FantastischSchnitt(TString path="/panda/pandaroot/macro/lmd/testPixel/mom_1_5/", int nEvents=1000,int nStart=0){

  // ---- Input file -------------------------------------------------------------
  TString filename = "/Lumi_TrksQA_";
  filename+=nStart;
  filename+=".root";
  //----------------------------------------------------------------------------------

  // ---- Output file -------------------------------------------------------------
  TString out=path+"/Lumi_Ana_";
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
  TH2 *hxthrecLMD = new TH2D("hxthrecLMD","; #theta^{REC}_{LMD}, mrad; X^{REC}_{LMD}, cm",1e3,30,50,1e3,10,40);
  TH2 *hyphrecLMD = new TH2D("hyphrecLMD","; #phi^{REC}_{LMD}, mrad; Y^{REC}_{LMD}, cm",1e3,-250,250,1e3,-15,15);

  TH1 *hmod = new TH1I("hmod",";module #",10,0,10);
  //-----------------------------------------------------------------------------------

  // Go over all events ---------------------------------------------------------
  for (Int_t j=0; j<nEvents; j++){
    tTrkRec.GetEntry(j);
    const int nTrksQ = qtrk->GetEntriesFast();//Number of trks/ev
    // Go over all trks ----------------------------------------------------------
    for(int iq=0;iq<nTrksQ;iq++){
      PndLmdTrackQ *trkcur = qtrk->At(iq);
      int trkStat = trkcur->GetTrkRecStatus();

      int moduleID = trkcur->GetModule();//position of 1st hit of the trk
      int halfID = trkcur->GetHalf();//position of 1st hit of the trk
      int modNr = 5*halfID+moduleID;

      TVector3 recLMDpoint;
      trkcur->GetLMDpoint(recLMDpoint);
      double threcLMD = 1e3*(trkcur->GetLMDtheta()); 
      double phrecLMD = 1e3*(trkcur->GetLMDphi()); 

      int flSecondary = trkcur->GetSecondary();
      int PDGcode = trkcur->GetPDGcode();
      int sumPDGev = trkcur->GetSumEvPDG();
      
      if(trkStat==0){ //GOOD rec.trks
	//	if(sumPDGev==4424 && PDGcode==-2212){//signal DPM
	if(PDGcode==-2212){//signal BOX
	  hxthrecLMD->Fill(threcLMD,recLMDpoint.X());
	  hyphrecLMD->Fill(phrecLMD,recLMDpoint.Y());
	  hmod->Fill(modNr);
	}
	else{//bkg
	}
      }
      else{// not GOOD rec.trk
      }
    }
    //--------------------------------------------------------------------------------
 }
  //-----------------------------------------------------------------------------------
  hxthrecLMD->Write();
  hyphrecLMD->Write();
  hmod->Write();
  f->Close();
}
