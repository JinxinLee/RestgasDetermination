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
  TH1F *nc=new TH1F("nc","Number of Charged Tracks; Charged Tracks",20,-0.5,19.5);  


  TH1F *invmassnosel=new TH1F("invmassnosel","2(#pi^{+}#pi^{-}) Invariant mass;Invariant Mass (GeV)",100,0,10);
  TH1F *invmassnocut=new TH1F("invmassnocut","2(#pi^{+}#pi^{-}) Invariant mass;Invariant Mass (GeV)",100,2,4);
  // TH1F *invmassvtx=new TH1F("invmassvtx","2(#pi^{+}#pi^{-}) Invariant mass;Invariant Mass (GeV)",100,2,4);
 TH1F *invmass_trackhighmom= new TH1F("invmass_trackhighmom","2(#pi^{+}#pi^{-}) Invariant mass;Invariant Mass (GeV)",100,2,4);
 TH1F *invmasschicut=new TH1F("invmasschicut","#2(pi^{+}#pi^{-}) Invariant mass;Invariant Mass (GeV)",100,2,4);
 TH1F *invmasschicut_best=new TH1F("invmasschicut_best","2(#pi^{+}#pi^{-}) Invariant mass;Invariant Mass (GeV)",100,2,4);

 TH2F *hvpos = new TH2F("hvpos","(x,y) projection of fitted decay vertex",100,-5,5,100,-5,5);
 TH1F *hvzpos = new TH1F("hvzpos","z position of fitted decay vertex",100,-10,10);
 
 TH1F *chivtx=new TH1F("chivtx","Chi Square PndKinVtxFitter; Chi Square",100,0,100);
 

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

 int nchrg=pp1.GetLength();
    nc->Fill(nchrg);


    float momentumpplus, momentumpminus, theta1, theta2, phi1, phi2;

   for (Int_t l=0;l<pp1.GetLength();l++){
      pp1[l].SetMass(TRho::Instance()->GetPDG()->GetParticle(211)->Mass());
      momentumpplus=pp1[l].GetMicroCandidate().GetMomentum().Mag();
      
    }
    for (Int_t l=0;l<pm1.GetLength();l++){
      pm1[l].SetMass(TRho::Instance()->GetPDG()->GetParticle(211)->Mass());
      momentumpminus=pm1[l].GetMicroCandidate().GetMomentum().Mag();
      
}


    pipinosel.Combine(pp1,pm1,pp1,pm1);
    
    for (y=0;y<pipinosel.GetLength();++y){
      invmassnosel->Fill(pipinosel[y].M());
    }

    pipi.Combine(pp1,pm1,pp1,pm1);
    pipi.Select(pipisel);

    for (y=0;y<pipi.GetLength();++y){
      invmassnocut->Fill(pipi[y].M());
            if (momentumpplus>0.3 && momentumpminus >0.3){
	invmass_trackhighmom->Fill(pipi[y].M());
	}
	   
}

    int best_i=0;
    double best_chi2=1000;
    TCandidate *pipifit_best=0;
       //vertex fitter                                                                               
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
    
    //invmassvtx->Fill(pipifit.M());
    //if(chi2_vtx<6){
    //invmasschicut->Fill(pipifit.M());
    //}
    
    
    }
  
	
	std::cout<<"Number of reconstructed pipipipi = "<<n_reco<<std::endl;
 
  out->cd();
  out->Write();
  out->Save();


}

