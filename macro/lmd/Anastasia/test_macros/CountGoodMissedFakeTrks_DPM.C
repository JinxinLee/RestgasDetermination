// macro for counting good, missed and fake reconstructed trks
void CountGoodMissedFakeTrks_DPM(TString path="/panda/pandaroot/macro/lmd/testPixel/mom_1_5/", int nEvents=1000,int nStart=0){

  // ---- Input file -------------------------------------------------------------
  TString filename = "/Lumi_TrksQA_";
  filename+=nStart;
  filename+=".root";
  //----------------------------------------------------------------------------------

  // // ---- Output file -------------------------------------------------------------
  // TString out=path+"/Lumi_Ana_";
  // out += nStart;
  // out += ".root";
  // TFile *f = new TFile(out,"RECREATE");
  // // ---------------------------------------------------------------------------------

  TChain tTrkRec("cbmsim");
  TString in=path+filename;
  tTrkRec.Add(in);

  //--- Tracks -------------------------------------------------------------------
  TClonesArray* qtrk=new TClonesArray("PndLmdTrackQ");
  tTrkRec.SetBranchAddress("LMDTrackQ",&qtrk);  //Tracks
  //----------------------------------------------------------------------------------


  // Go over all events ---------------------------------------------------------

  int Ngood = 0; int Nmiss = 0; int Nfake = 0;
  nEvents = tTrkRec.GetEntries();
  for (Int_t j=0; j<nEvents; j++){
    tTrkRec.GetEntry(j);
    const int nTrksQ = qtrk->GetEntriesFast();//Number of trks/ev
    // Go over all trks ----------------------------------------------------------
    for(int iq=0;iq<nTrksQ;iq++){
      PndLmdTrackQ *trkcur = qtrk->At(iq);
      int trkStat = trkcur->GetTrkRecStatus();
      int trkPDG = trkcur->GetPDGcode();
      int evSumPDG = trkcur->GetSumEvPDG();
      double thMC = trkcur->GetMCtheta();
      double thREC = trkcur->GetIPtheta();
      //      if(trkPDG==-2212 && evSumPDG==4424){ //signal event
      // if(trkPDG==-2212 && evSumPDG==4424 &&fabs(thMC-thREC)>=3.*7.46691e-04 ){//@ 1.5 GeV/c
      if(trkPDG==-2212 && evSumPDG==4424 &&fabs(thMC-thREC)>=3.*1.01792e-04){//@ 15 GeV/c
	if(trkStat<0)
	  Nmiss++;
      }

      //if(trkPDG==-2212 && evSumPDG==4424 && fabs(thMC-thREC)<3.*7.46691e-04){ //signal event @ 1.5 GeV/c
      if(trkPDG==-2212 && evSumPDG==4424 && fabs(thMC-thREC)<3.*1.01792e-04){ //signal event @ 15 GeV/c
	if(trkStat<0)
	  Nmiss++;
	else
	  if(trkStat>0)
	    Nfake++;
	  else
	    Ngood++;
      }
      else
	if(trkStat>=0)
	  Nfake++;
	// else 
	//   if(trkcur->GetPDGcode()==-2212){
	//     if(trkcur->GetSumEvPDG()==4424)
	//       Ngood++;
	//     else
	//       Nfake++;
	//   }
	//   else
	//     Nfake++; //trk was recontructed, but it is not signal
      //  cout<<"trkStat = "<<trkStat<<endl;
      
    }
    //--------------------------------------------------------------------------------
  }
  //-----------------------------------------------------------------------------------

  cout<<"[sim with DPM] Number of good trks = "<<Ngood<<"; missed trks = "<<Nmiss<<" and fake trks = "<<Nfake<<endl;
  cout<<"For "<<nEvents<<" events: Number of good trks = "<<double(Ngood*100)/nEvents<<"; missed trks = "<<double(Nmiss*100)/nEvents<<" and fake trks = "<<double(Nfake*100)/nEvents<<", %"<<endl;
  cout<<"BKG/SIG = "<<double(Nfake*100.)/Ngood<<", %"<<endl;
}
