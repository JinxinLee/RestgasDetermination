int track_check(Int_t nEntries = 0)
{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  TString inDigiFile  = "digi_sttcombi.root";
  TString inPidFile  = "pid_sttcombi.root";
  TString inRecoFile  = "reco_sttcombi.root";
  TString inAlgoFile = "algo_sttcombi.root";
  TString inSimFile = "points_sttcombi.root";

  TFile *inFile = TFile::Open(inPidFile,"READ");

  TTree *tree=(TTree *) inFile->Get("pndsim") ;
  tree->AddFriend("pndsim",inSimFile);
  tree->AddFriend("pndsim",inDigiFile);

  TClonesArray* cand_array=new TClonesArray("PndPidCandidate");
  tree->SetBranchAddress("PidChargedCand", &cand_array);

  TClonesArray* mc_array=new TClonesArray("PndMCTrack");
  tree->SetBranchAddress("MCTrack", &mc_array);
  TClonesArray* mdtp_array=new TClonesArray("PndMdtPoint");
  tree->SetBranchAddress("MdtPoint", &mdtp_array);
  TClonesArray* mdth_array=new TClonesArray("PndMdtHit");
  tree->SetBranchAddress("MdtHit", &mdth_array);
  TClonesArray* mdtt_array=new TClonesArray("PndMdtTrk");
  tree->SetBranchAddress("MdtTrk", &mdtt_array);

  TFile *out = TFile::Open("out_test.root","RECREATE");
  TNtuple *nt = new TNtuple("nt","nt","evt:mc_p:mc_theta:mc_phi:mc_pid:p:theta:phi:mult");

  if (nEntries==0) nEntries =  tree->GetEntriesFast();
  for (Int_t j=0; j< nEntries; j++){
    tree->GetEntry(j);
    //if (cand_array->GetEntriesFast()==0) continue;
    cout << "processing event " << j << "\n";

    Float_t mc_mom = 0, mc_theta = 0, mc_phi = 0;
    Float_t rec_mom = 0, rec_theta = 0, rec_phi = 0;
    for (Int_t mc = 0; mc < mc_array->GetEntriesFast(); mc++)
      {
	PndMCTrack *mctrack = (PndMCTrack*)mc_array->At(mc);
	if (mctrack->GetMotherID()!=-1) continue;
	mc_mom = mctrack->GetMomentum().Mag();
	mc_theta = mctrack->GetMomentum().Theta()*TMath::RadToDeg();
	mc_phi = mctrack->GetMomentum().Phi()*TMath::RadToDeg();
	Int_t mc_pid = mctrack->GetPdgCode();
	Int_t cand_mult = 0;
	for (Int_t pp=0; pp<cand_array->GetEntriesFast(); pp++)
	  {
	    PndPidCandidate *pidCand = (PndPidCandidate*)cand_array->At(pp);
	    if (pidCand->GetMcIndex()!=mc) continue;
	    if ( (cand_mult==0) || ((cand_mult>0) && (fabs(rec_mom-mc_mom)> fabs(pidCand->GetMomentum().Mag()-mc_mom))) )
	      {
		rec_mom = pidCand->GetMomentum().Mag();
		rec_theta = pidCand->GetMomentum().Theta();
		rec_phi = pidCand->GetMomentum().Phi();
	      }
	    cand_mult++;

	  } // end of candidate loop

	Float_t ntuple[] = {j,mc_mom,mc_theta,mc_phi, mc_pid,
			    rec_mom, rec_theta*TMath::RadToDeg(), rec_phi*TMath::RadToDeg(), cand_mult};
	nt->Fill(ntuple);

      } // end of MC loop

  } // end of event loop


  out->cd();

  nt->Write();
  out->Save();

  return 0;
}
