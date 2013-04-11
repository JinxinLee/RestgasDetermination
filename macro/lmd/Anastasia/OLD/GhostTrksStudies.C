#include "TString.h"
#include "TStopwatch.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TFile.h"
using namespace std;
void GhostTrksStudies(const int nEvents=2, const int startEvent=0, TString storePath="tmpOutput", const int verboseLevel=1, const int particle=-2212, const double mom=15, const int simTrk=5, TString algo="CA")
{
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libLmdTrk");
  // gROOT->LoadMacro("line3Dfit.C");
  // ------------------------------------------------------------------------
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  
  // ---- Input files --------------------------------------------------------
  TString simMC=storePath+"/Lumi_MC_";
  simMC += startEvent;
  simMC += ".root";
  TChain tMC("cbmsim");
  tMC.Add(simMC);

  TString DigiFile = storePath+"/Lumi_digi_";
  DigiFile += startEvent;
  DigiFile += ".root";
  TChain tdigiHits("cbmsim");
  tdigiHits.Add(DigiFile);
  
  TString recHit=storePath+"/Lumi_reco_";
  recHit += startEvent;
  recHit += ".root";
  TChain tHits("cbmsim");
  tHits.Add(recHit);
  
  TString trkCand = storePath+"/Lumi_TCand_";
  trkCand += startEvent;
  trkCand += ".root";
  TChain tTrkCand("cbmsim");
  tTrkCand.Add(trkCand);
  
  TString recTrack = storePath+"/Lumi_Track_";
  recTrack += startEvent;
  recTrack += ".root";
  TChain tTrkRec("cbmsim");
  tTrkRec.Add(recTrack);
 
  TString geaneFile = storePath+"/Lumi_Geane_";
  geaneFile += startEvent;
  geaneFile += ".root";
  TChain tgeane("cbmsim");
  tgeane.Add(geaneFile);
  
  // ---------------------------------------------------------------------------------
  
  // ---- Output file ----------------------------------------------------------------
  TString out=storePath+"/Ghost_trks_";
  out += startEvent;
  out +="_Mom_";
  out +=mom;
  out +="_NsimTrks_";
  out +=simTrk;
  out +="_algo_";
  out +=algo;
  out += ".root";
  TFile *f = new TFile(out,"RECREATE");
  // ---------------------------------------------------------------------------------
  
  //--- MC info -----------------------------------------------------------------
  TClonesArray* true_tracks=new TClonesArray("PndMCTrack");
  tMC.SetBranchAddress("MCTrack",&true_tracks);  //True Track to compare
  
  TClonesArray* true_points=new TClonesArray("PndSdsMCPoint");
  tMC.SetBranchAddress("LMDPoint",&true_points);  //True Points to compare
  //----------------------------------------------------------------------------------
//--- Digitization info ------------------------------------------------------------
  TClonesArray* fStripClusterArray = new TClonesArray("PndSdsClusterStrip");
  tHits.SetBranchAddress("LMDStripClusterCand",&fStripClusterArray); 
  
  TClonesArray* fStripDigiArray = new TClonesArray("PndSdsDigiStrip");
  tdigiHits.SetBranchAddress("LMDStripDigis",&fStripDigiArray); 
  //----------------------------------------------------------------------------------
  
  //--- Real Hits --------------------------------------------------------------------
  TClonesArray* rechit_array=new TClonesArray("PndSdsHit");
  tHits.SetBranchAddress("LMDHitsStrip",&rechit_array);  //Points for Tracks
  //----------------------------------------------------------------------------------
  
  
  //--- Track Candidate ---------------------------------------------------------------
  TClonesArray* trkcand_array=new TClonesArray("PndTrackCand");
  tTrkCand.SetBranchAddress("LMDTrackCand",&trkcand_array); //Points for Track Canidates 
  //-----------------------------------------------------------------------------------
  
  //--- Real tracks -------------------------------------------------------------------
  TClonesArray* rec_trk=new TClonesArray("PndLinTrack");
  tTrkRec.SetBranchAddress("LMDTrack",&rec_trk);  //Tracks
  //----------------------------------------------------------------------------------  

  //--- Geane info ------------------------------------------------------------------
  TClonesArray* geaneArray =new TClonesArray("FairTrackParH");
  tgeane.SetBranchAddress("GeaneTrackFinal",&geaneArray);  //Tracks with parabolic parametrisation
  
  TH2 *hMCntrkRECntrk = new TH2I("hMCntrkRECntrk","; number of MC trks; number of rec. trks",100,0,100,100,0,100);
  TH1 *hmissMC = new TH1I("hmissMC"," Number of missed MC trks; Number of trks; ", 25,0,25);
  TH1 *hghostRec = new TH1I("hghostRec"," Number of ghost trks; Number of trks; ", 100,0,100);
  TH1 *hThetaMissMC = new TH1F("hThetaMissMC","#theta for miss MC trks;#theta_{MC}, rad;",1e2,0,0.01);
  TH1 *hPhiMissMC = new TH1F("hPhiMissMC","#phi for missed MC trks ;#phi_{MC}, rad;",1e2,-3.15,3.15);
  TH1 *hThetaGhostRec = new TH1F("hThetaGhostRec","#theta for ghost REC trks;#theta_{REC}, rad;",1e2,0,0.01);
  TH1 *hPhiGhostRec = new TH1F("hPhiGhostRec","#phi for ghost REC trks ;#phi_{REC}, rad;",1e2,-3.15,3.15);
  TH1 *hdThetaGhostRec = new TH1F("hdThetaGhostRec","#delta#theta for ghost REC trks;#delta#theta_{REC}/#sigma;",1e3,0,1e2);
  TH1 *hdPhiGhostRec = new TH1F("hdPhiGhostRec","#delta#phi for ghost REC trks ;#delta#phi_{REC}/#sigma;",1e3,-2.,1e2);
  TH2 *hdPhidThetaGhostRec = new TH2F("hdPhidThetaGhostRec","#delta#phi vs. #delta#theta for ghost REC trks; #delta#theta_{REC}/#sigma; #delta#phi_{REC}/#sigma;",1e3,0,1e2,1e3,0,1e2);
  int MCevents = 0;
  // int RECevents[100]; 
  //RECevents[0] - less trks in REC then in MC
  //RECevents[1] - number of REC trks = MC trks
  //RECevents[2] number REC trks = (MC trks + 1) etc.
  int goodRec=0, totmissMC=0, totghostRec=0;
  for (Int_t j=0; j<nEvents; j++){

    double TotCharge = 0;
    if(verboseLevel>0)  cout<<"Event #"<<j<<endl;

    // Read GEANE & MC tree -----------------------------------------------------------------
    tgeane.GetEntry(j);
    tMC.GetEntry(j);
    tTrkCand.GetEntry(j);
    tTrkRec.GetEntry(j);
    tHits.GetEntry(j);
    tdigiHits.GetEntry(j);

    const int nGeaneTrks = geaneArray->GetEntriesFast();
    const int nParticles = true_tracks->GetEntriesFast();
    const int numTrk = rec_trk->GetEntriesFast();
    const int numHits = rechit_array->GetEntriesFast();
    if(numHits<3) continue;
    if(verboseLevel>0) cout<<"number of MC trks = "<<nParticles<<" number of recHist = "<<numHits
			   <<" number of GEANEtrk = "<<nGeaneTrks<<endl;
    //    cout<<"nParticles "<<nParticles<<" simTrk "<<simTrk<<endl;
    //    if(fabs(nParticles-simTrk)>1e-3) break;
    //    cout<<"Event#"<<j<<endl;
    if(nParticles==simTrk){
      /// check hits: all MC trks must be presented in hits array--------
      vector<int> HitMCids;
      for(int iu=0;iu<nParticles;iu++)
	HitMCids.push_back(0);
      for (Int_t iH=0; iH<numHits; iH++){
	  PndSdsHit* myHit = (PndSdsHit*)(rechit_array->At(iH));
	  ///Top cluster
	  PndSdsClusterStrip* myCluster =  (PndSdsClusterStrip*)(fStripClusterArray->At(myHit->GetClusterIndex()));
	  PndSdsDigiStrip* astripdigi = (PndSdsDigiStrip*)(fStripDigiArray->At(myCluster->GetDigiIndex(0)));
	  if (astripdigi->GetIndex(0) == -1) continue; // sort out noise
	  PndSdsMCPoint* MCPoint = (PndSdsMCPoint*)(true_points->At(astripdigi->GetIndex(0)));
	  int MCidTOP = MCPoint->GetTrackID();
	  HitMCids[MCidTOP]++;
	  ///Bottom cluster
	  Int_t  botIndex = myHit->GetBotIndex();
	  PndSdsClusterStrip* myClusterBot =  (PndSdsClusterStrip*)(fStripClusterArray->At(botIndex));
	  PndSdsDigiStrip* astripdigiBot = (PndSdsDigiStrip*)(fStripDigiArray->At(myClusterBot->GetDigiIndex(0)));
	  if (astripdigiBot->GetIndex(0) == -1) continue; // sort out noise
	  PndSdsMCPoint* MCPointBot = (PndSdsMCPoint*)(true_points->At(astripdigiBot->GetIndex(0)));
	  int MCidBOT = MCPointBot->GetTrackID();
	  HitMCids[MCidBOT]++;
      }
      bool hitsok=true;
      for(int iu=0;iu<nParticles;iu++){
	//	cout<<"HitMCids["<<iu<<"]="<<HitMCids[iu]<<endl;
	if(HitMCids[iu]<6) hitsok=false;
      }
      if(!hitsok) continue;
      /// end of hits check---------------------------------------------

      MCevents++;
      // if(nGeaneTrks<nParticles) RECevents[0]++;
      // else{
      // 	if(nParticles==nGeaneTrks) RECevents[1]++;
      // 	else{
      // 	  int pos = (nGeaneTrks-nParticles)+1;
      // 	  RECevents[pos]++;
      // 	}
      // }
      hMCntrkRECntrk->Fill(nParticles,nGeaneTrks);

      vector<double> MCphi, MCtheta;
      vector<int> MCrec;//was this MC trk reconstructed? -1=no,+value=yes in rec trk #value
      /// Read MC track parameters -----------------------------------------------------------
      for (Int_t iN=0; iN<nParticles; iN++){
	PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(iN);
	Int_t mcID = mctrk->GetPdgCode();
	//	if(fabs(mcID)>1e4) continue;
	if(mcID!=-2212) continue;
	TVector3 MomMC_all = mctrk->GetMomentum();
	MCtheta.push_back(MomMC_all.Theta());
	MCphi.push_back(MomMC_all.Phi());
	MCrec.push_back(-1);
      }
      /// end MC trks -------------------------------------------------------------------------

      vector<double> RECphi, RECtheta ;
      vector<double> errRECphi, errRECtheta;
      vector<int> RecMC;//has this trk MC? -1=no, +value=yes to MC trk#value
      /// Read INFO from reconstructed tracks ----------------------------------------------------
      for (Int_t iNr=0; iNr<nGeaneTrks; iNr++){
	FairTrackParH *fRes = (FairTrackParH*)geaneArray->At(iNr);
	Double_t lyambda = fRes->GetLambda();
	double thetaRec = TMath::Pi()/2. - lyambda;
	RECtheta.push_back(thetaRec);
	Double_t err_lyambda = fRes->GetDLambda();
	errRECtheta.push_back(err_lyambda); 
	double phiRec = fRes->GetPhi();
	RECphi.push_back(phiRec);
	Double_t err_phi = fRes->GetDPhi();
	errRECphi.push_back(err_phi);
	RecMC.push_back(-1);
      }
      /// end REC trks ---------------------------------------------------------------------------

      // cout<<"	RecMC.size() = "<<RecMC.size()<<endl;
      /// compare MC and REC ---------------------------------------------------------------------
      int oldgoodREC=goodRec-1;
      int goodthis=0, ghostthis=0, missthis=0;
      while(goodRec!=oldgoodREC && goodthis<nParticles){
	//	cout<<"goodRec = "<<<<endl;
	oldgoodREC = goodRec;
	for(int imc=0;imc<MCphi.size();imc++){
	  // if(MCrec[imc]>0) continue;
	  for(int irec=0;irec<RECphi.size();irec++){
	    if(RecMC[irec]>0)  continue;
	  
	    // if(MCphi[imc]*RECphi[irec]<0){
	    //   if(RECphi[irec]<0) RECphi[irec]+=TMath::Pi();
	    //   else MCphi[imc]+=TMath::Pi();
	    // }
	    //	    cout<<"fabs(MCphi[imc]-RECphi[irec])="<<fabs(MCphi[imc]-RECphi[irec])<<" 8*errRECphi[irec]="<<8*errRECphi[irec]<<endl;
	    //	    cout<<"fabs(MCtheta[imc]-RECtheta[irec])="<<fabs(MCtheta[imc]-RECtheta[irec])<<" 8*errRECtheta[irec]="<<8*errRECtheta[irec]<<endl;
	    // if(fabs(MCphi[imc])<1e-2 || fabs(MCphi[imc])>3.12) MCphi[imc]=fabs(MCphi[imc]);
	    // if(fabs(RECphi[irec])<1e-2 || fabs(RECphi[irec])>3.12) RECphi[irec]=fabs(RECphi[irec]);
	    // cout<<"MCphi[imc] = "<<MCphi[imc]<<" RECphi[irec] = "<<RECphi[irec]<<endl;
	    // cout<<"MCtheta[imc] = "<<MCtheta[imc]<<" RECtheta[irec] = "<<RECtheta[irec]<<endl;
	    // cout<<"fabs(MCphi[imc]-RECphi[irec])="<<fabs(MCphi[imc]-RECphi[irec])<<" 12*errRECphi[irec]="<<12*errRECphi[irec]<<endl;
	    // cout<<"fabs(MCtheta[imc]-RECtheta[irec])="<<fabs(MCtheta[imc]-RECtheta[irec])<<" 8*errRECtheta[irec]="<<8*errRECtheta[irec]<<endl;
	    double deltaPhi = fabs(MCphi[imc]-RECphi[irec])/(6*errRECphi[irec]);
	    //   if(fabs(MCphi[imc])<2e-1 || fabs(MCphi[imc])>2.9) deltaPhi=-1;
	    double deltaTheta = fabs(MCtheta[imc]-RECtheta[irec])/(6*errRECtheta[irec]);
	    if(deltaPhi<1. && deltaTheta<1.){
	      goodRec++;
	      goodthis++;
	      MCrec[imc]=irec;
	      RecMC[irec]=imc;
	    }
	  }
	}
	
	//check double reconstructed MC trks!
	for(int irec1=0;irec1<RecMC.size();irec1++){
	  if(RecMC[irec1]<0) continue;
	  for(int irec2=0;irec2<RecMC.size();irec2++){
	    if(RecMC[irec2]<0 || irec1==irec2) continue;
	    if(RecMC[irec1]==RecMC[irec2]){
	      int imcd=RecMC[irec1];
	      double deltaPhi1 = fabs(MCphi[imcd]-RECphi[irec1])/(6*errRECphi[irec1]);
	      double deltaTheta1 = fabs(MCtheta[imcd]-RECtheta[irec1])/(6*errRECtheta[irec1]);
	      double deltaPhi2 = fabs(MCphi[imcd]-RECphi[irec2])/(6*errRECphi[irec2]);
	      double deltaTheta2 = fabs(MCtheta[imcd]-RECtheta[irec2])/(6*errRECtheta[irec2]);
	      // if(fabs(MCphi[imcd])<2e-1 || fabs(MCphi[imcd])>2.9){
	      // 	deltaPhi1=-1;
	      // 	deltaPhi2=-1;
	      // }
	      //cout<<"deltaPhi1 = "<<deltaPhi1<<" deltaPhi2 = "<<deltaPhi2<<" deltaTheta1 = "<<deltaTheta1<<" deltaTheta2="<<deltaTheta2<<endl;
	      // cout<<"deltaPhi1+deltaTheta1 = "<<deltaPhi1+deltaTheta1<<" deltaPhi2+deltaTheta2 = "<<deltaPhi2+deltaTheta2<<endl;
	      if((deltaPhi1+deltaTheta1)<(deltaPhi2+deltaTheta2)){
		goodRec--;
		goodthis--;
		RecMC[irec2]=-1;
		MCtheta[imcd]=irec1;
	      }
	      else{
		goodRec--;
		goodthis--;
		RecMC[irec1]=-1;
		MCtheta[imcd]=irec2;
	      }
	    }
	  }
	}
	//	cout<<"goodRec "<<goodRec<<" oldgoodREC "<<oldgoodREC<<endl;
      }

      //fill missMC
      for(int imc=0;imc<MCphi.size ();imc++){
	if(MCrec[imc]>=0) continue;
	totmissMC++;
	missthis++;
	hPhiMissMC->Fill(MCphi[imc]);
	hThetaMissMC->Fill(MCtheta[imc]);
	if(verboseLevel>0){
	  cout<<"MCphi[imc] = "<<MCphi[imc]<<endl;
	}
      }
      if(missthis>0){
	hmissMC->Fill(missthis);
	if(verboseLevel>0) cout<<"Number of missed MC trks="<<totmissMC<<endl;
      }

      //fill ghostREC
      for(int irec=0;irec<RECphi.size();irec++){
	if(RecMC[irec]>=0) continue;
	totghostRec++;
	ghostthis++;
	hThetaGhostRec->Fill(RECtheta[irec]);
	hPhiGhostRec->Fill(RECphi[irec]);
	for(int imc=0;imc<MCphi.size ();imc++){
	  double deltaPhi = fabs(MCphi[imc]-RECphi[irec])/(6*errRECphi[irec]);
	  double deltaTheta = fabs(MCtheta[imc]-RECtheta[irec])/(6*errRECtheta[irec]);
	  hdThetaGhostRec->Fill(deltaTheta);
	  hdPhiGhostRec->Fill(deltaPhi);
	  hdPhidThetaGhostRec->Fill(deltaTheta,deltaPhi);
	  //	  if(verboseLevel>0) 
	  //cout<<" RECphi[irec] = "<<RECphi[irec]<<endl;
	}
      }
      if(ghostthis>0){
	hghostRec->Fill(ghostthis);
	if(verboseLevel>0) cout<<"Number of ghost trks="<<totghostRec<<endl;
      }
      //  cout<<"gooRec = "<<goodRec<<" nParticles = "<<nParticles<<" totmissMC = "
      //	  <<totmissMC<<" totghostRec="<<totghostRec
	      //	  <<" RecMC.size()="<<RecMC.size()<<" MCrec.size()="<<MCrec.size()<<endl;
    }
      /// end compare MC and REC------------------------------------------------------------------
    }/// end events

  cout<<"MCevents = "<<MCevents<<" hMCntrkRECntrk->GetEntries() = "<<hMCntrkRECntrk->GetEntries()<<endl;
  //    int allMissedMC = hmissMC->GetEntries();
  int allMissedMC = totmissMC;
    if(allMissedMC!=0){
      int missMCbin = 
	((hmissMC->GetXaxis()->GetXmax()) - (hmissMC->GetXaxis()->GetXmin()))/(hmissMC->GetXaxis()->GetNbins());
      int valMostMissMC = missMCbin*hmissMC->GetMaximumBin() - 1;
      int numMostMissMC = hmissMC->GetBinContent(hmissMC->GetMaximumBin());
      cout<<"missed MC = "<<allMissedMC<<"("<<100.*allMissedMC/(simTrk*MCevents)<<" %), most often are missed "<<valMostMissMC<<" trks in event ("
	  <<numMostMissMC<<"="<<100.*numMostMissMC*(hmissMC->GetMaximumBin()-1)/allMissedMC<<" %)"<<endl;
      
      for(int imiss=0;imiss<(hmissMC->GetXaxis()->GetNbins());imiss++){
	double currval = 100.*(hmissMC->GetBinContent(imiss))/hmissMC->GetEntries();
	//      cout<<"currval = "<<currval<<endl;
	if(currval>1){
	  cout<<(imiss*missMCbin-1)<<" MC trks are missed in "<<currval<<" % of all events with missed trks"<<endl;
	}
      }
    }

    //   int allGhostREC =  hghostRec->GetEntries();
    int allGhostREC = totghostRec;
    if(allGhostREC!=0){
      int ghostRECbin = ((hghostRec->GetXaxis()->GetXmax()) - (hghostRec->GetXaxis()->GetXmin()))/(hghostRec->GetXaxis()->GetNbins());
      int valMostGhostREC = ghostRECbin*hghostRec->GetMaximumBin() - 1;
      int numMostGhostREC = hghostRec->GetBinContent(hghostRec->GetMaximumBin());
      cout<<"ghost REC = "<<allGhostREC<<"("<<100.*allGhostREC/(simTrk*MCevents)<<" %), most often ghost "<<valMostGhostREC<<" trks in event ("
	  <<numMostGhostREC<<"="<<100.*numMostGhostREC*(hghostRec->GetMaximumBin()-1)/allGhostREC<<" %)"<<endl;
      
      for(int ighost=0;ighost<(hghostRec->GetXaxis()->GetNbins());ighost++){
	double currval = 100.*(hghostRec->GetBinContent(ighost))/hghostRec->GetEntries();
	if(currval>1){
	  cout<<(ighost*ghostRECbin-1)<<" REC trks are ghost "<<currval<<" % of all events with ghost trks"<<endl;
	}
      }
    }
    //count number of reconstructed trks for algorithm speed calculation
    int itogoRecTrks=0;
    TH1 *hRecN = hMCntrkRECntrk->ProjectionY();
    //  cout<<"hRecN->GetEntries()="<<hRecN->GetEntries()<<endl;
    for(int irec=0;irec<(hRecN->GetXaxis()->GetNbins());irec++){
      int coef = irec-simTrk;
      int Nrecbin = hRecN->GetBinContent(irec);
      //   cout<<"coef = "<<coef<<" Nrecbin = "<<Nrecbin<<endl;
      if(coef!=0) itogoRecTrks+=coef*Nrecbin;
      else itogoRecTrks+=simTrk*Nrecbin;
      //   cout<<"itogoRecTrks = "<<itogoRecTrks<<endl;
    }
    cout<<"Total number of reconstructed tracks: "<<itogoRecTrks<<endl;
  hMCntrkRECntrk->Write();
  hmissMC->Write();
  hghostRec->Write();
  hThetaMissMC->Write();
  hPhiMissMC->Write();
  hThetaGhostRec->Write();
  hPhiGhostRec->Write();
  hdThetaGhostRec->Write();
  hdPhiGhostRec->Write();
  hdPhidThetaGhostRec->Write();
  TCanvas *c1 = new TCanvas("angular_distr","angular distributions",800,800);
  c1->Divide(2,2);
  c1->cd(1);
  hThetaMissMC->Draw();
  c1->cd(2);
  hPhiMissMC->Draw();
  c1->cd(3);
  hThetaGhostRec->Draw();
  c1->cd(4);
  hPhiGhostRec->Draw();
  c1->Write();
  c1->Close();
  f->Close();
  if(verboseLevel>0)  cout<<"Macro succsefully finished!"<<endl;
}
