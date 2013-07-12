void MissGhostCount(TString path="/panda/pandaroot/macro/lmd/testPixel/mom_1_5/", TString filename = "/Lumi_TrksQA_CA_4trks.root",TString algo="CA", int nEvents=1000,int nStart=0){

  // ---- Output file ----------------------------------------------------------------
  TString out=path+"/Lumi_missed_ghost_";
  out +=algo;
  out +="_";
  out += nStart;
  out += ".root";
  TFile *f = new TFile(out,"RECREATE");
  // ---------------------------------------------------------------------------------

  TChain tTrkRec("cbmsim");
  TString in=path+filename;
  tTrkRec.Add(in);

  //--- Real tracks -------------------------------------------------------------------
  TClonesArray* qtrk=new TClonesArray("PndLmdTrackQ");
  tTrkRec.SetBranchAddress("LMDTrackQ",&qtrk);  //Tracks
  //----------------------------------------------------------------------------------

  TH1 *hntrkmissed_I = new TH1F("hntrkmissed_I","Number of missed tracks per event (hit losses if MChits>0);N_{trk}",30,0,30);
  TH1 *hntrkmissed_II = new TH1F("hntrkmissed_II","Number of missed tracks per event (trk search losses);N_{trk}",30,0,30);
  TH1 *hntrkgood_II = new TH1F("hntrkgood_II","Number of good tracks per event (hits matching);N_{trk}",30,0,30);
  TH1 *hntrkghost_II = new TH1F("hntrkghost_II","Number of ghost tracks per event  (repeated trk);N_{trk}",30,0,30);
  TH1 *hntrkghost_I = new TH1F("hntrkghost_I","Number of ghost tracks per event (hits mixture);N_{trk}",30,0,30);

  for (Int_t j=0; j<nEvents; j++){
    tTrkRec.GetEntry(j);
    const int nTrksQ = qtrk->GetEntriesFast();
    int missHit=0;//missed due to small amount of trks
    int missTrk=0;//missed due to trk search losses
    int ghostHit=0;//ghost due to hit mixture
    int ghostTrk=0;//doubled trk
    int good = 0;//good trk
    for(int iq=0;iq<nTrksQ;iq++){
      PndLmdTrackQ *trkcur = qtrk->At(iq);
      int trkStat = trkcur->GetTrkRecStatus();
      int flSecondary = trkcur->GetSecondary();
      int numMChits =  trkcur->GetNumMChits();
      int numDoubleMChits = trkcur->GetNumDoubleMChits();
      int MChitslim = 1;
      if(numDoubleMChits>0) MChitslim =2;
      if(trkStat==0) good++;
      if(trkStat==1) ghostHit++;
      if(trkStat==2) ghostTrk++;
      // if((trkStat==-1 || trkStat==-10) && flSecondary==0 && numMChits>MChitslim) missTrk++;
      // if(trkStat==-2 && flSecondary==0 && numMChits<=MChitslim) missHit++; 
      if((trkStat==-1 || trkStat==-10) && flSecondary==0) missTrk++;
      if(trkStat==-2 && flSecondary==0) missHit++;
    }
    if(missHit>0) hntrkmissed_I->Fill(missHit);
    if(missTrk>0) hntrkmissed_II->Fill(missTrk);
    if(ghostHit>0) hntrkghost_I->Fill(ghostHit);
    if(ghostTrk>0) hntrkghost_II->Fill(ghostTrk);
    if(good>0) hntrkgood_II->Fill(good);
  }


hntrkmissed_I->Write();
hntrkmissed_II->Write();
hntrkghost_I->Write();
hntrkghost_II->Write();
hntrkgood_II->Write();
//f->Write();
f->Close();
}
