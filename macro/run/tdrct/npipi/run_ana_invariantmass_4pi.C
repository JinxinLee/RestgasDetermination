void run_ana_invariantmass_4pi(TString fname="evt_pid_stt.root",int nEntries=0)
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

  TFile *out=TFile::Open("invariantmass_4pi.root","RECREATE");


  PndEventReader evr(inPidFile);


  TH1F *invmassnosel=new TH1F("invmassnosel","#pi^{+}#pi^{-} Invariant mass;Invariant Mass (GeV)",100,0,10);
  TH1F *invmassnocut=new TH1F("invmassnocut","#pi^{+}#pi^{-} Invariant mass;Invariant Mass (GeV)",100,2,4);
 TH1F *invmassvtx=new TH1F("invmassvtx","#pi^{+}#pi^{-} Invariant mass;Invariant Mass (GeV)",100,2,4);
 TH1F *invmass_trackhighmom= new TH1F("invmass_trackhighmom","#pi^{+}#pi^{-} Invariant mass;Invariant Mass (GeV)",100,2,4);
 TH1F *invmasschicut=new TH1F("invmasschicut","#pi^{+}#pi^{-} Invariant mass;Invariant Mass (GeV)",100,2,4);


  TH1F *chivtx=new TH1F("chivtx","Chi Square PndKinVtxFitter; Chi Square",100,0,100);
  TH1F *hpullvtx=new TH1F("hpullvtx","Pull PndKinVtxFitter",100,-1,1);
  TH1F *hprobvtx=new TH1F("hprobvtx","Prob PndKinVtxFitter; prob",100,0,1);

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



  float energypip, energypim, momxpip, momypip, momzpip;
  TPidMassSelector *pipisel=new TPidMassSelector("pipisel",3.07,0.5); //energy in the center of mass 3.07GeV
  TPidPlusSelector *piplusSel=new TPidPlusSelector("piplus");
  TPidMinusSelector *piminusSel=new TPidMinusSelector("piminus");

    if (nEntries==0) nEntries=evr.GetEntries();
    while (evr.GetEvent() && k++<nEntries){
   
      if (!((i+1)%100)) cout<<"evt " << i << "\n";

      evr.FillList(pp1,"Charged");
    evr.FillList(pm1,"Charged");
    evr.FillList(pp2,"Charged");
    evr.FillList(pm2,"Charged");

    pp1.Select(piplusSel);
    pm1.Select(piminusSel);
    pp2.Select(piplusSel);
    pm2.Select(piminusSel);


    float momentumpplus, momentumpminus, theta1, theta2, phi1, phi2;

   for (Int_t l=0;l<pp1.GetLength();l++){
      pp1[l].SetMass(TRho::Instance()->GetPDG()->GetParticle(211)->Mass());
      momentumpplus=pp1[l].GetMicroCandidate().GetMomentum().Mag();
      
    }
    for (Int_t l=0;l<pm1.GetLength();l++){
      pm1[l].SetMass(TRho::Instance()->GetPDG()->GetParticle(211)->Mass());
      momentumpminus=pm1[l].GetMicroCandidate().GetMomentum().Mag();
      
}

    for (Int_t l=0;l<pp2.GetLength();l++){
      pp2[l].SetMass(TRho::Instance()->GetPDG()->GetParticle(211)->Mass());
      momentumpplus=pp2[l].GetMicroCandidate().GetMomentum().Mag();
      
    }
    for (Int_t l=0;l<pm2.GetLength();l++){
      pm2[l].SetMass(TRho::Instance()->GetPDG()->GetParticle(211)->Mass());
      momentumpminus=pm2[l].GetMicroCandidate().GetMomentum().Mag();

    }


    pipinosel.Combine(pp1,pm1,pp2,pm2);
    
    for (y=0;y<pipinosel.GetLength();++y){
      invmassnosel->Fill(pipinosel[y].M());
    }

    pipi.Combine(pp1,pm1,pp2,pm2);
    pipi.Select(pipisel);

    for (y=0;y<pipi.GetLength();++y){
      invmassnocut->Fill(pipi[y].M());
            if (momentumpplus>0.3 && momentumpminus >0.3){
	invmass_trackhighmom->Fill(pipi[y].M());
	}
	   
}

       
       //vertex fitter                                                                               
    for (y=0;y<pipi.GetLength();++y){

       PndKinVtxFitter vtxfitter(pipi[y]);
      vtxfitter.Fit();
      
      double chi2vtx=vtxfitter.GlobalChi2();
      double pullvtx=vtxfitter.GetPull();
      unsigned ndfvtx=1;
      
      TChisqConsistency cons(chi2vtx,ndfvtx);
      double probvtx=cons.Likelihood();
      
      chivtx->Fill(chi2vtx);
      hprobvtx->Fill(probvtx);
      hpullvtx->Fill(pullvtx);
      
      TCandidate *pipifit=vtxfitter.FittedCand(pipi[y]);
     
      invmassvtx->Fill(pipifit.M());
      if(chi2vtx<6){
	invmasschicut->Fill(pipifit.M());
      }

    }
    }
  
 
  out->cd();
  out->Write();
  out->Save();


}

