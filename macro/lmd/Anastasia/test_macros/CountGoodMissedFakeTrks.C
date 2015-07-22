// macro for counting good, missed and fake reconstructed trks
void CountGoodMissedFakeTrks(TString path="/panda/pandaroot/macro/lmd/testPixel/mom_1_5/", int nEvents=1000,int nStart=0){

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
      //  if(trkcur->GetPDGcode()!=-2212) continue;
      if(trkStat<0){
	if(trkcur->GetPDGcode()==-2212)
	  Nmiss++;
      }
      else
	if(trkStat>0)
	  Nfake++;
	else 
	  if(trkcur->GetPDGcode()==-2212)
	    Ngood++;
      //  cout<<"trkStat = "<<trkStat<<endl;
    }
    //--------------------------------------------------------------------------------
  }
  //-----------------------------------------------------------------------------------

  cout<<"Number of good trks = "<<Ngood<<"; missed trks = "<<Nmiss<<" and fake trks = "<<Nfake<<endl;
  cout<<"For "<<nEvents<<" events , 1trk/ev: Number of good trks = "<<double(Ngood*100)/nEvents<<"; missed trks = "<<double(Nmiss*100)/nEvents<<" and fake trks = "<<double(Nfake*100)/nEvents<<"%"<<endl;
}
