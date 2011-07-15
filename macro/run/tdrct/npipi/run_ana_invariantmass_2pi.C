void run_ana_invariantmass_2pi(TString fname="evt_pid_stt.root",int nEntries=0)
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

  TFile *out=TFile::Open("invariantmass_2pi.root","RECREATE");

  PndEventReader evr(inPidFile);

  //Define all histograms
int n_reco=0;

  TH1F *nc=new TH1F("nc","Number of Charged Tracks; Charged Tracks",20,-0.5,19.5);  
  TH1F *invmassnosel=new TH1F("invmassnosel","#pi^{+}#pi^{-} Invariant mass;Invariant Mass (GeV)",100,0,10);
  TH1F *invmassnocut=new TH1F("invmassnocut","#pi^{+}#pi^{-} Invariant mass;Invariant Mass (GeV)",100,2,4);
  TH1F *invmasswithpid=new TH1F("invmasswithpid","#pi^{+}#pi^{-} Invariant mass;Invariant Mass (GeV)",100,0,10); 
  TH1F *invmasswithpid_sel=new TH1F("invmasswithpid_sel","#pi^{+}#pi^{-} Invariant mass;Invariant Mass (GeV)",100,2,4); 
  //  TH1F *invmassvtx=new TH1F("invmassvtx","#pi^{+}#pi^{-} Invariant mass;Invariant Mass (GeV)",100,2,4);
  TH1F *invmass_trackhighmom= new TH1F("invmass_trackhighmom","#pi^{+}#pi^{-} Invariant mass;Invariant Mass (GeV)",100,2,4);

 TH1F *invmasschicut_best=new TH1F("invmasschicut_best","#pi^{+}#pi^{-} Invariant mass;Invariant Mass (GeV)",100,2,4);

 TH2F *hvpos = new TH2F("hvpos","(x,y) projection of fitted decay vertex",100,-5,5,100,-5,5);
 TH1F *hvzpos = new TH1F("hvzpos","z position of fitted decay vertex",100,-10,10);
 
  TH1F *chivtx=new TH1F("chivtx","Chi Square PndKinVtxFitter; Chi Square",100,0,8);
 
  TPidPlusSelector *piplusSel=new TPidPlusSelector("piplus");
  TPidMinusSelector *piminusSel=new TPidMinusSelector("piminus");
  
  TCandList pp, pm, pipi,pipinosel, pipiwithpid;
  
  
  TLorentzVector ini(0,0,4.0,5.04684);
  int i=0,j=0,k=0,l=0,y=0;
  
  TLorentzVector beam;
  beam.SetXYZM (0.0,0.0,4.0,0.938272);
  TLorentzVector target;
  target.SetXYZM (0.0,0.0,0.0,0.938272);
  TLorentzVector pbarp=beam+target;


  float energypip, energypim, momxpip, momypip, momzpip;
  TPidMassSelector *pipisel=new TPidMassSelector("pipisel",3.07,1.0); //energy in the center of mass 3.07 GeV

  TPidPlusSelector *piplusSel=new TPidPlusSelector("piplus");
  TPidMinusSelector *piminusSel=new TPidMinusSelector("piminus");

  if (nEntries==0) nEntries=evr.GetEntries();
  while (evr.GetEvent() && k++<nEntries){
    
    if (!((i+1)%100)) cout<<"evt " << i << "\n";
    
    evr.FillList(pp,"Charged");
    evr.FillList(pm,"Charged");
    pp.Select(piplusSel);
    pm.Select(piminusSel);
    
    int nchrg=pp.GetLength();
    nc->Fill(nchrg);
    float momentumpplus, momentumpminus, theta1, theta2, phi1, phi2;
    
    for (Int_t l=0;l<pp.GetLength();l++){
      pp[l].SetMass(TRho::Instance()->GetPDG()->GetParticle(211)->Mass());
      momentumpplus=pp[l].GetMicroCandidate().GetMomentum().Mag();
      theta1=pp[l].GetMicroCandidate().GetMomentum().Theta()*TMath::RadToDeg();
      phi1=pp[l].GetMicroCandidate().GetMomentum().Phi()*TMath::RadToDeg();
      
    }
    for (Int_t l=0;l<pm.GetLength();l++){
      pm[l].SetMass(TRho::Instance()->GetPDG()->GetParticle(211)->Mass());
      momentumpminus=pm[l].GetMicroCandidate().GetMomentum().Mag();
      theta2=pm[l].GetMicroCandidate().GetMomentum().Theta()*TMath::RadToDeg();
      phi2=pm[l].GetMicroCandidate().GetMomentum().Phi()*TMath::RadToDeg();
      
      
    }
    
    
    pipinosel.Combine(pp,pm);
    
    for (y=0;y<pipinosel.GetLength();++y){
      invmassnosel->Fill(pipinosel[y].M());
    }
    
    pipi.Combine(pp,pm);
    pipi.Select(pipisel); //Mass selector
    
    for (y=0;y<pipi.GetLength();++y){
      invmassnocut->Fill(pipi[y].M());
      if (momentumpplus>0.3 && momentumpminus >0.3){ //All the combination with momentum >0.3 GeV
	invmass_trackhighmom->Fill(pipi[y].M());
      }
      
    }
    
    //MC PID
    
    for (l=0;l<pp.GetLength();++l) { 
      if (pp[l].GetMicroCandidate().GetMcIndex()>-1){
	PndMCTrack *mcTrack = (PndMCTrack*)mc_array->At(pp[l].GetMicroCandidate().GetMcIndex());
	//		float a=mcTrack->GetMomentum().Mag();
	if (mcTrack!=0)
	  {
	    if (mcTrack->GetPdgCode()!=211)
	      pp.Remove(pp[l]);
	    cout<<"Remove candidate"<<endl;
	  }
	else 
	  {
	    std::cout<<"Pion list plus, element "<<l<<" has no assosiated mcTRack"<<std::endl;
	  }
      }
    }
    
    for (l=0;l<pm.GetLength();++l) {
      if (pm[l].GetMicroCandidate().GetMcIndex()>-1){
        PndMCTrack *mcTrack = (PndMCTrack*)mc_array->At(pm[l].GetMicroCandidate().GetMcIndex());
        if (mcTrack!=0)
          {
            if (mcTrack->GetPdgCode()!=-211)
              pm.Remove(pm[l]);
	    cout<<"Remove Candidate"<<endl;
          }
        else
          {
	    std::cout<<"Pion list minus, element "<<l<<" has no assosiated mcTRack"<<std::endl;
          }
      }
    }
    
    
    pipiwithpid.Combine(pp,pm);
    for (y=0;y<pipiwithpid.GetLength();++y){
      invmasswithpid->Fill(pipiwithpid[y].M());
    }
    
    
    pipiwithpid.Combine(pp,pm);
    pipiwithpid.Select(pipisel); //Mass selector
    for (y=0;y<pipiwithpid.GetLength();++y){
      invmasswithpid_sel->Fill(pipiwithpid[y].M());
    }
   
    int best_i=0;
    double best_chi2=1000;
    TCandidate *pipifit_best=0;
 
  for (y=0;y<pipi.GetLength();++y){

      PndKinVtxFitter vtxfitter(pipi[y]);
      vtxfitter.Fit();
      TCandidate *pipifit=vtxfitter.FittedCand(pipi[y]);
      TVector3 pipiVtx=pipifit->Pos();
      double chi2_vtx=vtxfitter.GlobalChi2();
    
      hvpos->Fill(pipiVtx.X(),pipiVtx.Y());   
      hvzpos->Fill(pipiVtx.Z()); 
      if(chi2_vtx<best_chi2)
	{
	  best_chi2=chi2_vtx;
	  best_i=y;
	  pipifit_best=pipifit;
	}
      chivtx->Fill(chi2_vtx/5); // Number degree of freedom 2N-3=5; N=number of charged tracks
    }

    if((best_chi2<6)&&(pipi.GetLength()!=0))
      {
	invmasschicut_best->Fill(pipifit_best.M());
	n_reco++;
      }

       }
  
    
 
  out->cd();
  out->Write();
  out->Save();


}

