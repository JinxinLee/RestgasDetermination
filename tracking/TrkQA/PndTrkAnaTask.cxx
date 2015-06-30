////////////////////////////////////////////////////////////
//
// PndTrkAnaTask
//
// Class for Quality Assurance
//
// authors: Lia Lavezzi - University of Torino (2015)
//          from a macro by Karin Schoenning /thanks!/
////////////////////////////////////////////////////////////

#include "PndTrkAnaTask.h"
#include "PndSttMapCreator.h"
#include "PndSttHit.h"
#include "PndSttTube.h"
#include "PndTrack.h"
#include "PndTrackID.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include "FairHit.h"
#include "FairMCPoint.h"
#include "PndSttPoint.h"
#include "TDatabasePDG.h"
#include "RhoCandidate.h"
#include <iostream>

using namespace std;


// -----   Default constructor   -------------------------------------------
PndTrkAnaTask::PndTrkAnaTask() : FairTask("QualityAssurance", 0), fPersistence(kTRUE) {
  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
  sprintf(fGemBranch,"GEMHit");
  sprintf(fSciTBranch,"SciTHit");
}

// -----   Destructor   ----------------------------------------------------
PndTrkAnaTask::~PndTrkAnaTask() {

}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndTrkAnaTask::Init() {

  fEventCounter = 0;

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndTrkAnaTask::Init: "
	 << "RootManager not instantiated, return!" << endl;
    return kFATAL;
  }


//   fMCTrackInfo = (TClonesArray*) ioman->GetObject("MCTrackInfo");
//   if (!fMCTrackInfo ) return kERROR;
//   if(fVerbose > 9) cout << "Found fMCTrackInfo\n";

  fRecoTrackInfo = (TClonesArray*) ioman->GetObject("RecoTrackInfo");
  if (!fRecoTrackInfo ) return kERROR;
  if(fVerbose > 9) cout << "Found fRecoTrackInfo\n";


  fAnalysis = new PndAnalysis();  // *** initialize analysis object   



  // TDatabasePDG::Instance()->AddParticle("pbarpSystem","pbarpSystem",3.07744,kFALSE,0.1,0,"",88888);
  double pbarmom=4; // 1.64;
  // *** the lorentz vector of the initial pbarp
  double m0_p    = TDatabasePDG::Instance()->GetParticle("proton")->Mass();   // Get nominal PDG mass of the proton
  // cms
  double Ene=sqrt(m0_p*m0_p + pbarmom*pbarmom) + m0_p;
  fIni = new TLorentzVector(0, 0, pbarmom, Ene);
  

  // **** create and setup some histos for QA plots
  //
  lambdamass = new TH1F("lambdamass","#Lambda cands",500,0.01,2.5);
  lambdamassv = new TH1F("lambdamassv","#Lambda cands vertex fit",500,0.01,2.5);
  lambdacmass = new TH1F("lambdacmass","clean #Lambda cands",500,0.01,2.5);
  lambdabmass = new TH1F("lambdabmass","#bar{#Lambda} cands",500,0.01,2.5);

  lambdamassbkg = new TH1F("lambdamassbkg","bkg #Lambda cands",500,0.01,2.5);
  lambdabmassbkg = new TH1F("lambdabmassbkg","bkg #bar{#Lambda} cands",500,0.01,2.5);

  lbmassv = new TH1F("lbmassv","#bar{#Lambda} cands vertex fit",500,0.01,2.5);
  lambdabcmass = new TH1F("lambdabcmass","clean #bar{#Lambda} cands",500,0.01,2.5);
  l4C_chi2  = new TH1F("l4C_chi2","Prob. 4C fit",500,0,500);
  hmv_lamb = new TH1F("hmv_lamb","#bar{#Lambda} mass, 4C fit",500,0.01,2.5);
  hmv_lam = new TH1F("hmv_lam","#Lambda mass, 4C fit",500,0.01,2.5);
  //lb4Cmass = new TH1F("lbmass4C","Lambdab cands, 4C fit ",500,0.01,2.5);
  //llbmass = new TH1F("llbmass","LLbar mass before fit ",500,0.01,2.5);
  //llbar2mass = new TH1F("llbar2mass","LLbar mass, 4Cfit ",500,0.01,2.5);

  //hjpsim_vf   = new TH1F("hjpsim_vf","J/#psi mass vertex fit",200,0,4);
  hlvpos = new TH2F("hlvpos","(x,y) projection of fitted #Lambda decay vertex",100,-2,2,100,-2,2);
  hlbvpos = new TH2F("hlbvpos","(x,y) projection of fitted #bar{#Lambda} decay vertex",100,-2,2,100,-2,2);
  hlam_prob_vf  = new TH1F("hlam_prob_vf","#Lambda, prob. vertex fit",500,0,1);
  hlamb_prob_vf  = new TH1F("hlamb_prob_vf","#bar{#Lambda}, prob. vertex fit",500,0,1);
  hOvpos = new TH2F("hOvpos","(x,y) projection of fitted #Lambda decay vertex",100,-2,2,100,-2,2);
  hObvpos = new TH2F("hObvpos","(x,y) projection of fitted #bar{#Lambda} decay vertex",100,-2,2,100,-2,2);

  hlvposz  = new TH1F("hlvposz","#Lambda vertex z pos",200,-2.0,50.0);
  hlbvposz  = new TH1F("hlvbposz","#bar{#Lambda} vertex z pos",200,-2.0,50.0);

  h0_chi2_vf  = new TH1F("h0_chi2_vf","#Lambda, #chi^{2} vertex fit",100,0,10);
  h0b_chi2_vf  = new TH1F("h0b_chi2_vf","#bar{#Lambda}, #chi^{2} vertex fit",100,0,10);

  //hmv_lamb = new TH1F("hmv_lamb", "lambdabar, mass & vertex fit", 100, 0.01,2.5);
    
  hnfw = new TH1F("hnfw", "No. of forward going tracks (< 5 degrees)", 10, -1,9);

  hpip_p = new TH1F("hpip_p", "Momentum of #pi+", 100, 0.01,2.0);

  hpim_p = new TH1F("hpim_p", "Momentum of #pi-", 100, 0.01,2.0);
  hp_p = new TH1F("hp_p", "Momentum of p", 100, 0.01,10.0);
  hp_pimp = new TH2F("p_pimp","p of proton versus p of pi-",100,0,1.0,100,0,0.4);
  hp_pibar = new TH2F("p_pibar","p of pbar versus p of pi+",100,0,4.0,100,0,4.0);

  hp_rpimp = new TH2F("p_rpimp","rec. p of proton versus p of #pi-",100,0,1.0,100,0,0.4);
  hp_rpibar = new TH2F("p_rpibar","rec. p of pbar versus p of #pi+",100,0,4.0,100,0,4.0);

  hp_thpip = new TH2F("hp_thpip","P versus #theta, #pi+",100,0,1,100,0,180);
  hp_thpim = new TH2F("hp_thpim","P versus #theta, #pi-",100,0,1,100,0,180);
  hp_thp = new TH2F("hp_thp","P versus theta, p",100,0,4,100,0,180);
  hp_thpbar = new TH2F("hp_thpbar","P versus #theta, pbar",100,0,4,100,0,40);

  hv_lam_diff = new TH1F("hv_lam_diff","|fitted vertex - MC truth|, #Lambda",50,0,10);
  hv_lam_diff1 = new TH1F("hv_lam_diff1","|fitted vertex - reco track origin|, #Lambda",50,0,25);

  hv_lamb_diff = new TH1F("hv_lamb_diff","|fitted vertex - MC truth|, #bar{#Lambda}",50,0,10);
  hv_lamb_diff1 = new TH1F("hv_lamb_diff1","|fitted vertex - track origin|, #bar{#Lambda}",50,0,25);

  hpbar_p = new TH1F("hpbar_p", "Momentum of p", 100, 0.01,10.0);

  hlamb_diff = new TH1F("hlamb_diff", "(P_rec-P_truth)/(P_truth) for #bar{#Lambda}",100,-0.5,0.5);
  hlamb_diff_vf = new TH1F("hlamb_diff_vf", "(P_rec-P_truth)/(P_truth) for #bar{#Lambda}, vertex fit",100,-0.5,0.5);

  hlam_diff = new TH1F("hlam_diff", "(P_rec-P_truth)/(P_truth) for @Lambda",100,-0.5,0.5);
  hlam_diff_vf = new TH1F("hlam_diff_vf", "(P_rec-P_truth)/(P_truth) for #Lambda, vertex fit",100,-0.5,0.5);

  hpip_diff = new TH1F("hpip_diff", "(P_rec-P_truth)/(P_truth) for pi+",100,-0.5,0.5);
  hpim_diff = new TH1F("hpim_diff", "(P_rec-P_truth)/(P_truth) for pi-",100,-0.5,0.5);
  hp_diff = new TH1F("hp_diff", "(P_rec-P_truth)/(P_truth)for p",100,-0.5,0.5);
  hpbar_diff = new TH1F("hpbar_diff", "(P_rec-P_truth)/(P_truth) for pbar",100,-0.5,0.5);

  hp_stt = new TH1F("hp_stt", "(P_rec-P_truth)/(P_truth)for p, STT",100,-0.5,0.5);
  hp_sttmvd = new TH1F("hp_sttmvd", "(P_rec-P_truth)/(P_truth)for p, STT & MVD",100,-0.5,0.5);
  hp_sttmvdgem = new TH1F("hp_sttmvdgem", "(P_rec-P_truth)/(P_truth)for p, STT, MVD & GEM",100,-0.5,0.5);
  hp_mvdgem = new TH1F("hp_mvdgem", "(P_rec-P_truth)/(P_truth)for p, MVD & GEM",100,-0.5,0.5);


  hpip_diff_vf = new TH1F("hpip_diff_vf", "(P_rec-P_truth)/(P_truth) for #pi+ after vertex fit",100,-0.5,0.5);
  hpim_diff_vf = new TH1F("hpim_diff_vf", "(P_rec-P_truth)/(P_truth) for #pi- after vertex fit",100,-0.5,0.5);
  hp_diff_vf = new TH1F("hp_diff_vf", "(P_rec-P_truth)/(P_truth)for p after vertex fit",100,-0.5,0.5);
  hpbar_diff_vf = new TH1F("hpbar_diff_vf", "(P_rec-P_truth)/(P_truth) for pbar after vertex fit",100,-0.5,0.5);
    
  hpip_diff_4c = new TH1F("hpip_diff_4c", "(P_rec-P_truth)/(P_truth) for #pi+ after 4C fit",100,-0.5,0.5);
  hpim_diff_4c = new TH1F("hpim_diff_4c", "(P_rec-P_truth)/(P_truth) for #pi- after 4C fit",100,-0.5,0.5);
  hp_diff_4c = new TH1F("hp_diff_4c", "(P_rec-P_truth)/(P_truth)for p after 4C fit",100,-0.5,0.5);
  hpbar_diff_4c = new TH1F("hpbar_diff_4c", "(P_rec-P_truth)/(P_truth) for pbar after 4C fit",100,-0.5,0.5);
     
  hl_cos = new TH1F("hl_cos", "Cos(theta_l)", 50, -1,1); 
  hlb_cos = new TH1F("hlb_cos", "Cos(theta_lb)", 50, -1,1); 

  hl_cosr = new TH1F("hl_cosr", "Cos(theta_l)", 50, -1,1); 
  hlb_cosr = new TH1F("hlb_cosr", "Cos(theta_lb)", 50, -1,1); 

  // ----------------------------------------   maps of STT tubes
  PndSttMapCreator *mapperStt = new PndSttMapCreator(fSttParameters);
  fSttTubeArray = mapperStt->FillTubeArray();
  // ----------------------------------------------------  end map


  nofcleanpim = 0;
  nofcleanp = 0;
  nofcleanpip = 0;
  nofcleanpbar = 0;

  return kSUCCESS;

}


// -------------------------------------------------------------------------

void PndTrkAnaTask::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

// -------------------------------------------------------------------------


void PndTrkAnaTask::Exec(Option_t* opt) {

  if(fEventCounter%100 == 0)  cout << "event " << fEventCounter << endl;
  fEventCounter++;
  
  fAnalysis->GetEvent();
   
  // **** create all the particle lists we'll need for rebuilding the decay tree
  RhoCandList pip, pim, Kp, Km, pp, pm, lam, lamb, cleanlam, cleanlamb, bkglam, bkglamb, goodlam, goodlamb, truepip, truepim, truep, truepbar,mclist,llbar; //Omv, Ombv;
  RhoCandList mcplist, mcpbarlist, mcpiplist, mcpimlist;
  RhoCandList cleanplist, cleanpbarlist, cleanpiplist, cleanpimlist;
  RhoCandList bkgplist, bkgpbarlist, bkgpiplist, bkgpimlist;
  
  fAnalysis->FillList(mclist,"McTruth");
  //  cout <<  mclist.GetLength() << endl;
  //  for(int i = 0; i < 7; i++)  cout << i << " " << mclist[i]->M() << " " << mclist[i]->Charge() << " " << mclist[i]->Origin().X() << " " <<  mclist[i]->Origin().Y() << endl;
  //  cout << endl;

  // fwd tracks, theta < 5 deg
  int infw = 0;
  
  // theta
  double thetapip,thetapim,thetap,thetapbar;
  thetapbar=mclist[1]->Daughter(0)->GetMomentum().Theta() * TMath::RadToDeg();
  if(thetapbar<5.0) infw=infw+1;
  
  thetapip=mclist[1]->Daughter(1)->GetMomentum().Theta() * TMath::RadToDeg();
  if(thetapip<5.0) infw=infw+1;

  thetap=mclist[2]->Daughter(0)->GetMomentum().Theta() * TMath::RadToDeg();
  if(thetap<5.0) infw=infw+1;
    
  thetapim=mclist[2]->Daughter(1)->GetMomentum().Theta()  * TMath::RadToDeg();
  if(thetapim<5.0) infw=infw+1;
    
  hp_pimp->Fill(mclist[2]->Daughter(0)->P(),mclist[2]->Daughter(1)->P());
  hp_pibar->Fill(mclist[1]->Daughter(0)->P(),mclist[1]->Daughter(1)->P());
  hnfw->Fill(infw);

  // true pid
  fAnalysis->FillList(truepip,"PionAllPlus");
  fAnalysis->FillList(truepim,"PionAllMinus");
  fAnalysis->FillList(truep, "ProtonAllPlus");
  fAnalysis->FillList(truepbar, "ProtonAllMinus");
 
  SelectTruePid(fAnalysis, truepip);
  SelectTruePid(fAnalysis, truepim);
  SelectTruePid(fAnalysis, truep);
  SelectTruePid(fAnalysis, truepbar);

    int itp =0;
    int itpbar = 0;
    int itpip = 0;
    int itpim = 0;
    mcplist.Cleanup();
    mcpiplist.Cleanup();
    mcpimlist.Cleanup();
    mcpbarlist.Cleanup();

    // costheta
    TVector3 cmboost = -fIni->BoostVector();
    TLorentzVector lamcm4=mclist[2]->P4();
    TLorentzVector lambcm4=mclist[1]->P4();
    lamcm4.Boost(cmboost);
    lambcm4.Boost(cmboost);
    TLorentzVector llb=lamcm4+lambcm4;
    double cosl = lamcm4.CosTheta();
    double coslb = lambcm4.CosTheta();
    hl_cos->Fill(cosl);
    hlb_cos->Fill(coslb);

    // loop on pi+
    int j =0;

   //  cout <<  truepip.GetLength() << " " << truepim.GetLength() << " " << truep.GetLength() << " " << truepbar.GetLength() << endl;

    for (j=0;j<truepip.GetLength();++j) 
      {
	itpip++;

	// reco 
	TVector3 pip3=truepip[j]->GetMomentum();
	double theta=pip3.Theta() * TMath::RadToDeg();
	hpip_p->Fill(truepip[j]->P());
	FairRecoCandidate* recpip=truepip[j]->GetRecoCandidate();
	hp_thpip->Fill(truepip[j]->P(),theta); // CHECK
	//	hp_thpip->Fill(pip3.Mag(), theta);// CHECK
	
	// retrieve mc  
	RhoCandidate* mcpip = truepip[j]->GetMcTruth();
	mcpiplist.Add(mcpip);
	TVector3 mcpipmom = mcpip->GetMomentum();
	double pipdiff = truepip[j]->P() - mcpip->P();
	pipdiff /= mcpip->P();
	hpip_diff->Fill(pipdiff);

	// clean
	int recotrackid = recpip->GetTrackIndex();
// 	cout << "recotrackid " << recotrackid << " " << fRecoTrackInfo->GetEntriesFast() << " " << endl;
	PndTrkRecoTrackInfo *recoinfo = (PndTrkRecoTrackInfo*) fRecoTrackInfo->At(recotrackid);
 	if(recoinfo->GetRecoTrackID() != recotrackid) cout << "EEEEEEEEEERRRRRRRRRRROOOOOOOOOOOOORRRRRRRRR" << endl;
	//	if(recoinfo->IsTrue() == kTRUE && recoinfo->GetMCTrackID() == 4 && recoinfo->GetEfficiency() > 0.8) {
	if(recoinfo->GetFlag() >= 0 && recoinfo->IsTrue() == kTRUE && recoinfo->GetMCTrackID() == 4) {
	  cleanpiplist.Add(truepip[j]);
	  nofcleanpip++;
	}
	else if(recoinfo->IsTrue() == kTRUE && recoinfo->GetMCTrackID() != 4) bkgpiplist.Add(truepip[j]);


      }

    // loop on pi-
    for (j=0;j<truepim.GetLength();++j) 
      {
	itpim++;
	
	// reco 
	TVector3 pim3=truepim[j]->GetMomentum();
	double theta=pim3.Theta() * TMath::RadToDeg();
	hpim_p->Fill(truepim[j]->P());
	FairRecoCandidate *recpim=truepim[j]->GetRecoCandidate();
	hp_thpim->Fill(truepim[j]->P(),theta);

	// retrieve mc  
	RhoCandidate* mcpim = truepim[j]->GetMcTruth(); 
	mcpimlist.Add(mcpim);
	double pimdiff=truepim[j]->P()-mcpim->P();
	pimdiff /= mcpim->P();
	hpim_diff->Fill(pimdiff);

   	// clean
	int recotrackid = recpim->GetTrackIndex();

  	PndTrkRecoTrackInfo* recoinfo = (PndTrkRecoTrackInfo*) fRecoTrackInfo->At(recotrackid);
	if(recoinfo->GetRecoTrackID() != recotrackid) cout << "EEEEEEEEEERRRRRRRRRRROOOOOOOOOOOOORRRRRRRRR" << endl;
 // 	if(recoinfo->IsTrue() == kTRUE && recoinfo->GetMCTrackID() == 6 && recoinfo->GetEfficiency() > 0.8)   {
	if(recoinfo->GetFlag() >= 0 && recoinfo->IsTrue() == kTRUE && recoinfo->GetMCTrackID() == 6) {
	  cleanpimlist.Add(truepim[j]);
	  nofcleanpim++;
	}
  	else if(recoinfo->IsTrue() == kTRUE && recoinfo->GetMCTrackID() != 6) bkgpimlist.Add(truepim[j]);
    }

    // loop on p
    for (j=0;j<truep.GetLength();++j) 
      {
	itp++;

	// reco 
	TVector3 prot3=truep[j]->GetMomentum();
	double theta=prot3.Theta() * TMath::RadToDeg();
	hp_p->Fill(truep[j]->P());
	FairRecoCandidate* recp=truep[j]->GetRecoCandidate();
	hp_thp->Fill(truep[j]->P(),theta);

	// mc	
	RhoCandidate* mcp = truep[j]->GetMcTruth();
	mcplist.Add(mcp);
	double pdiff = truep[j]->P()- mcp->P();
	pdiff /= mcp->P();
	hp_diff->Fill(pdiff);


	if(theta>145.0 && theta<159.5) hp_stt->Fill(pdiff);
	if(theta>20.9  && theta<145.0) hp_sttmvd->Fill(pdiff);
	if(theta>7.8   && theta<20.9) hp_sttmvdgem->Fill(pdiff);
	if(theta>5.0   && theta<7.8) hp_mvdgem->Fill(pdiff);
 
  	// clean
	int recotrackid = recp->GetTrackIndex();

	PndTrkRecoTrackInfo*recoinfo = (PndTrkRecoTrackInfo*) fRecoTrackInfo->At(recotrackid);
	if(recoinfo->GetRecoTrackID() != recotrackid) cout << "EEEEEEEEEERRRRRRRRRRROOOOOOOOOOOOORRRRRRRRR" << endl;
	// 	if(recoinfo->IsTrue() == kTRUE && recoinfo->GetMCTrackID() == 5 && recoinfo->GetEfficiency() > 0.8) {
	if(recoinfo->GetFlag() >= 0 && recoinfo->IsTrue() == kTRUE && recoinfo->GetMCTrackID() == 5) {
	  cleanplist.Add(truep[j]);
	  nofcleanp++;
	}
	else if(recoinfo->IsTrue() == kTRUE && recoinfo->GetMCTrackID() != 5) bkgplist.Add(truep[j]);
     }

    for (j=0;j<truepbar.GetLength();++j) 
      {
	itpbar++;

	// reco 
	TVector3 p3bar=truepbar[j]->GetMomentum();
	double theta=p3bar.Theta() * TMath::RadToDeg();
	hpbar_p->Fill(truepbar[j]->P());
	FairRecoCandidate *recpbar=truepbar[j]->GetRecoCandidate();
	hp_thpbar->Fill(truepbar[j]->P(),theta*180/3.14159);
 
	// mc
	RhoCandidate* mcpbar = truepbar[j]->GetMcTruth();
	mcpbarlist.Add(mcpbar);
	double pbardiff=truepbar[j]->P()-mcpbar->P();
	pbardiff /= mcpbar->P();
	hpbar_diff->Fill(pbardiff);

     	// clean
	int recotrackid = recpbar->GetTrackIndex();

  	PndTrkRecoTrackInfo*recoinfo = (PndTrkRecoTrackInfo*) fRecoTrackInfo->At(recotrackid);
	if(recoinfo->GetRecoTrackID() != recotrackid) cout << "EEEEEEEEEERRRRRRRRRRROOOOOOOOOOOOORRRRRRRRR" << endl;
	// if(recoinfo->IsTrue() == kTRUE && recoinfo->GetMCTrackID() == 3 && recoinfo->GetEfficiency() > 0.8) {
	if(recoinfo->GetFlag() >= 0 && recoinfo->IsTrue() == kTRUE && recoinfo->GetMCTrackID() == 3) {
	  cleanpbarlist.Add(truepbar[j]);
	  nofcleanpbar++;
	}
	else if(recoinfo->IsTrue() == kTRUE && recoinfo->GetMCTrackID() != 3) bkgpbarlist.Add(truepbar[j]);
    }
		
    lam.Combine(truep,truepim);
    lamb.Combine(truepbar,truepip);
   
    //     cout << "......................" << endl;
    //     if(cleanplist.GetLength() == 1) cout << "PROTON" << endl;
    //     if(cleanpimlist.GetLength() == 1) cout << "PI-" << endl;
    //     if(cleanpbarlist.GetLength()  == 1)  cout << "PBAR" << endl;
    //     if(cleanpiplist.GetLength()  == 1 ) cout << "PI+" << endl;
    //     if(cleanplist.GetLength() == 1 && cleanpimlist.GetLength() == 1) cout << "LAM" << endl;
    //     if(cleanpbarlist.GetLength()  == 1 && cleanpiplist.GetLength()  == 1 ) cout << "LAMBAR" << endl;
    
    if(cleanplist.GetLength() > 1) {
      cout << "ERROR proton " << cleanplist.GetLength() << endl;
      for(int k = 0; k < cleanplist.GetLength(); k++) {
	FairRecoCandidate *cp=cleanplist[k]->GetRecoCandidate();
	RhoCandidate* mccp = cleanplist[k]->GetMcTruth();
	cout << "mc proton " << mccp->GetTrackNumber() << endl;
      }
    }
    if(cleanpimlist.GetLength() > 1) {
      cout << "ERROR pi- " << cleanpimlist.GetLength() << endl;
    }
    if(cleanpbarlist.GetLength() > 1) {
      cout << "ERROR antiproton " << cleanpbarlist.GetLength() << endl;
    }
    if(cleanpiplist.GetLength() > 1) {
      cout << "ERROR pi+ " << cleanpiplist.GetLength() << endl;
    }

    cleanlam.Combine(cleanplist,cleanpimlist);
    cleanlamb.Combine(cleanpbarlist, cleanpiplist);

    bkglam.Combine(bkgplist, bkgpimlist);
    bkglamb.Combine(bkgpbarlist, bkgpiplist);

    // loop over lam
    for (j=0;j<lam.GetLength();++j) 
      {
	lambdamass->Fill(lam[j]->M());
	double lamp=lam[j]->P();
	double mclamp=mclist[2]->P();
	double lamdiff=(lamp-mclamp)/mclamp;
	hlam_diff->Fill(lamdiff);
      }

    // loop over lambar
    for (j=0;j<lamb.GetLength();++j) 
      {
	lambdabmass->Fill(lamb[j]->M());
	double lambp=lamb[j]->P();
	double mclambp=mclist[1]->P();
	double lambdiff=(lambp-mclambp)/mclambp;
	hlamb_diff->Fill(lambdiff);
      }


    // loop over clean lam
    for (j=0;j<cleanlam.GetLength();++j) 
      {
	lambdacmass->Fill(cleanlam[j]->M());
      }

    // loop over lambar
    for (j=0;j<cleanlamb.GetLength();++j) 
      {
	lambdabcmass->Fill(cleanlamb[j]->M());
      }

    // loop over bkg lam
    for (j=0;j< bkglam.GetLength();++j) 
      {
	lambdamassbkg->Fill(bkglam[j]->M());
      }

    // loop over lambar bkg
    for (j=0;j< bkglamb.GetLength();++j) 
      {
	lambdabmassbkg->Fill(bkglamb[j]->M());
      }


    // do vertex fit (lambda)	
    goodlam.Cleanup();
    
    // loop over lam
    int jj = 0;
    for (jj=0;jj<lam.GetLength();++jj) 
      {
	PndVtxPRG vtxfitterl(lam[jj]); 
	bool check = vtxfitterl.Fit();
	double chi2_vtxl = vtxfitterl.GetChi2();	// access chi2 of fit
	double prob_vtxl = vtxfitterl.GetProb();	// access probability of fit
	hlam_prob_vf->Fill(prob_vtxl);
	h0_chi2_vf->Fill(chi2_vtxl);
					    
	if(check)
	  { 
	    RhoCandidate *lamv = lam[jj]->GetFit();	// access the fitted cand
	    TVector3 lVtxl=lamv->Pos();

	    // get a mass window
	    if(lamv->M()>0.01 && lamv->M()<2.5)
	      {
		lambdamassv->Fill(lamv->M());
		hlvpos->Fill(lVtxl.X(),lVtxl.Y());
		hlvposz->Fill(lVtxl.Z());

		RhoCandidate *vp = lamv->Daughter(0);
		RhoCandidate *vpim = lamv->Daughter(1);
		hp_rpimp->Fill(vp->P(),vpim->P());

		// mc p
		RhoCandidate* mcvp = vp->GetMcTruth();
		double vpdiff=vp->P()- mcvp->P();
		vpdiff /= mcvp->P();
		hp_diff_vf->Fill(vpdiff);
			
		// mc pi	    
		RhoCandidate* mcvpim = vpim->GetMcTruth();
		double vpimdiff=vpim->P()-mcvpim->P();
		vpimdiff /= mcvpim->P();
		hpim_diff_vf->Fill(vpimdiff);

		double lampv=lamv->P();
		double lammcvp=mclist[2]->P();
		double lamdiffv=(lampv-lammcvp)/lammcvp;
		hlam_diff_vf->Fill(lamdiffv);

		// get a mass window
		if(lamv->M()>1.05&&lamv->M()<1.15){
		  goodlam.Add(lamv);

		  if(itp==1 && lam.GetLength()==1){
		    TVector3 dv0=lVtxl-mcvp->GetPosition();
		    double dvert0=dv0.Mag();

		    TVector3 dv1=lVtxl-truep[0]->GetPosition();
		    double dvert1=dv1.Mag();

		    hv_lam_diff->Fill(dvert0);
		    hv_lam_diff1->Fill(dvert1);
		  }
		}
	      }
	  }
      }
	
    // *** do vertex fitting (lambdabar)
    goodlamb.Cleanup();
		
    for (j=0;j<lamb.GetLength();++j) 
      {
	PndVtxPRG vtxfitterlb(lamb[j]);
	bool checkb=vtxfitterlb.Fit();
	double chi2_vtx = vtxfitterlb.GetChi2();	// access chi2 of fit
	double prob_vtx = vtxfitterlb.GetProb();	// access probability of fit
	h0b_chi2_vf->Fill(chi2_vtx);
	hlamb_prob_vf->Fill(prob_vtx);	
			
	if(checkb)
	  {
	    RhoCandidate *lambv = lamb[j]->GetFit();	// access the fitted cand
	    TVector3 lVtx=lambv->Pos();
	    // mass wihw
	    if(lambv->M()>0.01&&lambv->M()<2.5)
	      {
		lbmassv->Fill(lambv->M());
		hlbvpos->Fill(lVtx.X(),lVtx.Y());
		hlbvposz->Fill(lVtx.Z());

		RhoCandidate *vpbar = lambv->Daughter(0);
		RhoCandidate *vpip = lambv->Daughter(1);
		hp_rpibar->Fill(vpbar->P(),vpip->P());

		// mc p
		RhoCandidate* mcvpbar = vpbar->GetMcTruth();
		double vpbardiff=vpbar->P()-mcvpbar->P();
		vpbardiff/= mcvpbar->P();
		hpbar_diff_vf->Fill(vpbardiff);

		// mc pi+
		RhoCandidate* mcvpip = vpip->GetMcTruth();
		double vpipdiff=vpip->P()-mcvpip->P();
		vpipdiff /= mcvpip->P();
		hpip_diff_vf->Fill(vpipdiff);

		double lambpv=lambv->P();
		double lambmcvp=mclist[1]->P();
		double lambdiffv=(lambpv-lambmcvp)/lambmcvp;
		hlamb_diff_vf->Fill(lambdiffv);
		
		// get a mass window
		if(lamb[j]->M()>1.05&&lamb[j]->M()<1.15){
		  goodlamb.Add(lambv);
		  
		  if(itpbar==1&&lamb.GetLength()==1){
		    TVector3 dvb0=lVtx-mcvpbar->GetPosition();
		    double dvertb0=dvb0.Mag();
						     
		    TVector3 dvb1=lVtx-truepbar[0]->GetPosition();
		    double dvertb1=dvb1.Mag();
						     
		    hv_lamb_diff->Fill(dvertb0);
		    hv_lamb_diff1->Fill(dvertb1);
		  }
		}
	      	
	      }
	    
	  }
	
      }
    
	
    llbar.Cleanup();			
    llbar.Combine(goodlam,goodlamb);
  
    for (j=0;j<llbar.GetLength();++j) 
      {
    
	Pnd4CFitter fitter(llbar[j],*fIni);     // *** instantiate the 4C fitter
   	fitter.FitConserveMasses();           // *** perform the fit
	
	double chi2_4c = fitter.GetChi2();    // *** access the chi2 of the fit
        l4C_chi2->Fill(chi2_4c);
	//cout<<"Chi2: "<<chi2_4c<<endl;
	
	RhoCandidate *lfit = llbar[j]->Daughter(0)->GetFit(); // *** access the fitted candidate            
	RhoCandidate *lbfit = llbar[j]->Daughter(1)->GetFit();

	RhoCandidate *pfit = llbar[j]->Daughter(0)->Daughter(0)->GetFit(); 
	RhoCandidate *pimfit = llbar[j]->Daughter(0)->Daughter(1)->GetFit();

	RhoCandidate *pbfit = llbar[j]->Daughter(1)->Daughter(0)->GetFit(); 
	RhoCandidate *pipfit = llbar[j]->Daughter(1)->Daughter(1)->GetFit();

	if (chi2_4c<100 && itp >0 && itpip > 0 && itpim > 0 && itpbar > 0){               // *** when good enough, fill some histos
 
	  hmv_lam->Fill(lfit->M());    
	  hmv_lamb->Fill(lbfit->M()); 
       
	  if(mcplist.GetLength()==1){
	    double pfdiff = pfit->P()-mcplist[0]->P();
	    pfdiff=pfdiff/mcplist[0]->P();
	    hp_diff_4c->Fill(pfdiff);}
      
	  if(mcpbarlist.GetLength()==1){
	    double pbarfdiff=pbfit->P()-mcpbarlist[0]->P();
	    pbarfdiff /=mcpbarlist[0]->P();
	    hpbar_diff_4c->Fill(pbarfdiff);
	  }

	  if(mcpiplist.GetLength()==1){
	    double pipfdiff=pipfit->P()-mcpiplist[0]->P();
	    pipfdiff /=mcpiplist[0]->P();
	    hpip_diff_4c->Fill(pipfdiff);}

	  if(mcpimlist.GetLength()==1){
	    double pimfdiff=pimfit->P()-mcpimlist[0]->P();
	    pimfdiff /= mcpimlist[0]->P();
	    hpim_diff_4c->Fill(pimfdiff);}
	}
      }
}


void PndTrkAnaTask::Finish()
{

  cout << "nof p    " << nofcleanp << endl;
  cout << "nof pi-  " << nofcleanpim << endl;
  cout << "nof pbar " << nofcleanpbar << endl;
  cout << "nof pi+  " << nofcleanpip << endl;

  lambdamass->Write();
  lambdabmass->Write();
  lambdamassv->Write();
  lbmassv->Write();
  
  lambdacmass->Write();
  lambdabcmass->Write();
 
  lambdamassbkg->Write();
  lambdabmassbkg->Write();
 
  hlbvposz->Write();
  hlvposz->Write();
  hmv_lam->Write();
  hpip_p->Write();
  hpim_p->Write();
  hp_p->Write();
  hpbar_p->Write();

  hpip_diff->Write();
  hpim_diff->Write();	
  hp_diff->Write();	
  hpbar_diff->Write();
	
  hlam_diff_vf->Write();
  hlamb_diff_vf->Write();
  hlam_diff->Write();
  hlamb_diff->Write();

  hv_lam_diff->Write();
  hv_lamb_diff->Write();
  hv_lam_diff1->Write();
  hv_lamb_diff1->Write();
		
  h0b_chi2_vf->Write();
  h0_chi2_vf->Write();

  hp_stt->Write();
  hp_sttmvd->Write();
  hp_sttmvdgem->Write();
  hp_mvdgem->Write();

  hpip_diff_vf->Write();
  hpim_diff_vf->Write();	
  hp_diff_vf->Write();	
  hpbar_diff_vf->Write();
		
  hpip_diff_4c->Write();
  hpim_diff_4c->Write();	
  hp_diff_4c->Write();	
  hpbar_diff_4c->Write();
		
  hlam_prob_vf->Write();
  hlamb_prob_vf->Write();

			
  l4C_chi2->Write();
	
  //llbar2mass->Write();
  hmv_lamb->Write();	
  hp_thpip->Write();
  hp_thpim->Write();
  hp_thp->Write();
  hp_thpbar->Write();
  hnfw->Write();
  hl_cos->Write();
  hlb_cos->Write();
  hp_pimp->Write();
  hp_pibar->Write();
  hl_cosr->Write();
  hlb_cosr->Write();
  hp_rpimp->Write();
  hp_rpibar->Write();
}

 int PndTrkAnaTask::SelectTruePid(PndAnalysis *ana, RhoCandList &l)
{
  int removed = 0;
	
  for (int ii=l.GetLength()-1;ii>=0;--ii)
    {
      if ( !(ana->McTruthMatch(l[ii])) )
	{
	  l.Remove(l[ii]);
	  removed++;
	}
    }
	
  return removed;
}

ClassImp(PndTrkAnaTask)

