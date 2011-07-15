void run_ana_singlepion_4pi(TString fname="evt_pid_stt.root",int nEntries=0)
{

  gStyle->SetOptFit(1011);
  
  TStopwatch timer;
  timer.Start();
  
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  //  TString inDigiFile  = "evt_digi_stt.root";
  TString inPidFile  = "evt_pid_stt.root";
  //TString inRecoFile  = "evt_reco_stt.root";                                                         
  TString inSimFile = "evt_points_stt.root";
  TFile *inFile = TFile::Open(inPidFile,"READ");
  TTree *tree=(TTree *) inFile->Get("cbmsim") ;
  tree->AddFriend("cbmsim",inSimFile);
  //tree->AddFriend("cbmsim",inDigiFile); 

  TClonesArray* mc_array=new TClonesArray("PndMCTrack");
  tree->SetBranchAddress("MCTrack",&mc_array);

  TClonesArray* cand_array=new TClonesArray("PndPidCandidate");
  tree->SetBranchAddress("PidChargedCand",&cand_array);

  TFile *out=TFile::Open("singlepion_4pi.root","RECREATE");
  

  PndEventReader evr(inPidFile);

  TH1F *mc_momentum=new TH1F("mc_momentum","Momentum Distribution MonteCarlo;#mom",100,0,10);

  TH1F *momentumx=new TH1F("momentumx","Momentum Distribution X;Momentum (GeV)",100,0,10);
  TH1F *momentumy=new TH1F("momentumy","Momentum Distribution Y;Momentum (GeV)",100,0,10);
  TH1F *momentumz=new TH1F("momentumz","Momentum Distribution Z;Momentum (GeV)",100,0,10);
  TH1F *mom_TOT=new TH1F("mom_TOT","Total Momentum Distribution ;Momentum (GeV)",100,0,10); 
  TH1F *energy=new TH1F("energy","Energy Distribution ;Energy (GeV)",100,0,10);

  TH1F *ptransverseH=new TH1F ("ptransverseH","Transverse Momentum; p_{T} (GeV)",100,0,10);

  TH1F *reco_mc_diff=new TH1F("reco_mc_diff","(Reconstructed Momentum - MC Momentum)/MC Momentum;(Reco-MC)/MC",100,-1,1);
  TH1F *reco_mc_diff_banda=new TH1F("reco_mc_diff_banda","(Reconstructed Momentum - MC Momentum)/MC Momentum;(Reco-MC)/MC",100,-1,1);
  TH1F *reco_mc_diff_highmom=new TH1F("reco_mc_diff_highmom","(Reconstructed Momentum - MC Momentum)/MC Momentum;(Reco-MC)/MC",100,-1,1);


  TH1F *reco_mc_diff_theta=new TH1F("reco_mc_diff_theta","(Reconstructed theta - MC Theta);(Reco-MC)",100,-1,1);
  TH1F *reco_mc_diff_phi=new TH1F("reco_mc_diff_phi","(Reconstructed Phi - MC Phi);(Reco-MC)",100,-10,10);


  TH2F *momvstheta=new TH2F("momvstheta","Reconstructed Momentum VS theta angle; Theta (degree); Reco Momentum",100,0,200,100,0,10);
  TH2F *diffmomvstheta=new TH2F("diffmomvstheta","Reconstructed Momentum - MC Momentum VS theta angle;Theta (degree);Reco-MC",100,0,200,100,-1,1);
  TH2F *diffmomvsmom=new TH2F("diffmomvsmom","Reconstructed Momentum - MC Momentum VS Transverse Momentum;p_{T} (GeV);Reco-MC",100,0,4,100,-1,1);

  TH1F *theta=new TH1F("theta","#theta Angle Distribution;Theta (degree)",100,0,360);
  TH1F *phi=new TH1F("phi","#phi Angle Distribution;Phi (degree)",100,-200,200);
  TH1F *nc=new TH1F("nc","Number of Charged Tracks; Charged Tracks",20,-0.5,19.5);

  TH1F *costh=new TH1F("costh","cos#theta CM Distribution;cos#theta",100,-1,1);

  TPidPlusSelector *piplusSel=new TPidPlusSelector("piplus");
  TPidMinusSelector *piminusSel=new TPidMinusSelector("piminus");
 
  TCandList pp1, pm1, pp2,pm2, pipi,pipinosel, pipiwithpid;

  int n_reco=0;

  TLorentzVector ini(0,0,4.0,5.04684);
  int i=0,j=0,k=0,l=0,y=0;

    
  TLorentzVector beam;
  beam.SetXYZM (0.0,0.0,4.0,0.938272);
  TLorentzVector target;
  target.SetXYZM (0.0,0.0,0.0,0.938272);
  TLorentzVector pbarp=beam+target;
   
  if (nEntries==0) nEntries=tree->GetEntriesFast();

      for (Int_t j=0; j<nEntries; j++)
    {
      tree->GetEntry(j);
  if (!((j+1)%100)) cout<<"evt " <<j<<"\n";
 
  Float_t th=0;
  Float_t momx=0, momy=0, momz=0, Energy=0, momentum_total=0, mc_mom=0, reco_mc_diff_v=0;
  Float_t phiangle=0;
  Float_t ptransverse=0, Float_t thetaCM=0;

  int n=0; 
  for(Int_t mc=0;mc<mc_array->GetEntriesFast();mc++)
    {
      PndMCTrack *mctrack=(PndMCTrack*)mc_array->At(mc);
      if (mctrack->GetMotherID()!=-1) continue;
      if (mctrack->GetPdgCode()==211){
      mc_mom=mctrack->GetMomentum().Mag();
      mc_momentum->Fill(mc_mom);
      mc_theta=mctrack->GetMomentum().Theta()*TMath::RadToDeg();
      mc_phi=mctrack->GetMomentum().Phi()*TMath::RadToDeg();


      Int_t cand_mult=0;

      for (Int_t index=0; index<cand_array->GetEntriesFast(); index++)
	{
	  PndPidCandidate *pidCand=(PndPidCandidate*)cand_array->At(index);
	  if (pidCand->GetMcIndex()!=mc) continue;
	  if ((cand_mult==0) || ((cand_mult>0) && (fabs(momentum_total-mc_mom)>fabs(pidCand->GetMomentum().Mag()-mc_mom))))
	    {
	      
      momx=pidCand->GetMomentum().X();
      momy=pidCand->GetMomentum().Y();
      momz=pidCand->GetMomentum().Z();
      momentum_total=pidCand->GetMomentum().Mag();
      Energy=pidCand->GetEnergy();
      mom_TOT->Fill(momentum_total);
      momentumx->Fill(momx);
      momentumy->Fill(momy);
      momentumz->Fill(momz);
      energy->Fill(Energy);
      th=pidCand->GetMomentum().Theta()*TMath::RadToDeg();
      theta->Fill(th);
      phiangle=pidCand->GetMomentum().Phi()*TMath::RadToDeg();
      phi->Fill(phiangle);

      reco_mc_diff_theta->Fill(th-mc_theta);
      reco_mc_diff_phi->Fill(phiangle-mc_phi);

      TLorentzVector pi_lab(momx,momy,momz,Energy);
      TLorentzVector pi_CM=pi_lab;
      pi_CM.Boost(-pbarp.BoostVector());
      costheta=cos(pi_CM.Vect().Theta());
      thetaCM=(pi_CM.Vect().Theta());
      ptransverse=TMath::Sqrt(momx*momx+momy*momy);
      costh->Fill(costheta);

      reco_mc_diff_v=(momentum_total-mc_mom)/mc_mom;
      reco_mc_diff->Fill(reco_mc_diff_v);

            if(momentum_total>0.3){
	reco_mc_diff_highmom->Fill((momentum_total-mc_mom)/mc_mom);
      }
      
      
      if((mc_mom>0.75) && (mc_mom<1.25)){
	reco_mc_diff_banda->Fill((momentum_total-mc_mom)/mc_mom);
	  }
      

      ptransverseH->Fill(ptransverse);

      momvstheta->Fill(th,momentum_total);
      diffmomvstheta->Fill(th,reco_mc_diff_v);
      diffmomvsmom->Fill(ptransverse,reco_mc_diff_v);

      cand_mult++;
	    } 
	} 
      } 
      } 

    }                
    
      timer.Stop();
 
  out->cd();
  out->Write();
  out->Save();


}

