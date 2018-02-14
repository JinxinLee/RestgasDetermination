int DecayConfig() {
     
  KFPartEfficiencies eff;
  
  int index[4] = {48, 49, 50, 63};
  
  for(Int_t iIndex = 0; iIndex<4; iIndex++)
  {
    iParticle = index[iIndex];
    Double_t lifetime = eff.partLifeTime[iParticle]; // lifetime
    Double_t mass = eff.partMass[iParticle];
    Int_t PDG = eff.partPDG[iParticle];
    Double_t charge = eff.partCharge[iParticle];
  
    std::cout << "Particle pdg: " << PDG << " mass " << mass << " charge " << charge << " lifetime "<< lifetime << std::endl;
    TPythia6Decayer* decayer = TPythia6Decayer::Instance();
    
    decayer->SetDecayTableFile("/u/mzyzak/panda_06.2014/macro/kf/D0/PythiaDecayTable.txt");
    decayer->ReadDecayTable();

    decayer->SetForceDecay(TPythia6Decayer::kAll);
    Int_t products[4] = {0};
    Int_t mult[4] = {0}; 
    Int_t npart=eff.GetNDaughters(iParticle);
    
    for(int iD=0; iD<npart; iD++)
    {
      products[iD] = eff.GetDaughterPDG(iParticle, iD);
      mult[iD] = 1;
    }
    decayer->ForceParticleDecay(PDG,products,mult,npart);
    
    gMC->SetExternalDecayer(decayer);
    
    TPythia6& pythia6 = *(TPythia6::Instance());
//     pythia6.Pyupda(1,6);     
    gMC->SetUserDecay(PDG);
    pythia6.SetMDCY(pythia6.Pycomp(PDG),1,1);
  }
  //make pi0 stable for pythia, Geant will decay it
  pythia6.SetMDCY(pythia6.Pycomp(111),1,0);
  return 0;
}


