class RhoCandList;
class RhoCandidate;
class PndAnaPidSelector;
class PndAnaPidCombiner;
class PndAnalysis;

void ana_complete(int nevts = 100000, TString prefix = "barrel", Bool_t use_mvd_hvmaps = false, TString figure_path = ".", TString figure_name = "figure", Double_t config_x = 0.0, Double_t config_y = 0.0, Double_t config_z = 0.0, TString stats_output_dir = ".")
{
  //-----User Settings:------------------------------------------------------
  TString parAsciiFile = use_mvd_hvmaps ? "all_hvmaps.par" : "all.par";
  //TString input = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  TString output = "ana_final";
  //TString friend1 = "reco";
  TString friend2 = "pid_final";
  //TString friend3 = "sim";
  //TString friend4 = "digi";

  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun = new PndMasterRunAna();
  fRun->SetInput("dummy");
  //fRun->SetInput(input);
  fRun->SetOutput(output);
  //fRun->AddFriend(friend1);
  fRun->AddFriend(friend2);
  //fRun->AddFriend(friend3);
  //fRun->AddFriend(friend4);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup(prefix);

  // *** some variables
  int i = 0, j = 0, k = 0, l = 0, m = 0, n = 0;
  gStyle->SetOptFit(1011);

  /*  gStyle -> SetOptFit(1011);

  TString OutFile = "out_dummy.root";
  
  TString inPidFile = prefix + "_pid.root";
  TString inParFile = prefix + "_par.root";

//  TString pidParFile = TString(gSystem->Getenv("VMCWORKDIR"))+"/macro/params/all.par";
  TString pidParFile = TString(gSystem->Getenv("VMCWORKDIR"))+"/macro/params/all_hvmaps.par";

  FairLogger::GetLogger()->SetLogToFile(kFALSE);
  FairRunAna* fRun = new FairRunAna();
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  fRun->SetSource(new FairFileSource(inPidFile));
  fRun->SetUseFairLinks(kTRUE);

  FairParRootFileIo* parIO = new FairParRootFileIo();
  parIO->open(inParFile);
  FairParAsciiFileIo* parIOPid = new FairParAsciiFileIo();
  parIOPid->open(pidParFile.Data(),"in");
 
  rtdb->setFirstInput(parIO);
  rtdb->setSecondInput(parIOPid);
  rtdb->setOutput(parIO);  

  fRun->SetOutputFile(OutFile);
*/
  fRun->Init(); 

  TFile *out = TFile::Open(prefix + "_ana_final.root","RECREATE");

  RhoTuple *ntpDp = new RhoTuple("ntpDp","Dp Analysis");

  PndAnalysis* theAnalysis = new PndAnalysis();
  if (nevts == 0)
  nevts = theAnalysis->GetEntries();

  double pbarmom = 4.06;
  TLorentzVector ini( 0, 0, 4.060, 5.105 );
  //TLorentzVector ini( 0, 0, 8.9, 9.888 );
  double mp = 0.938272;
  //TLorentzVector ini;
  //ini.SetXYZT(0, 0, pbarmom, sqrt(pbarmom * pbarmom + mp * mp) + mp);

  RhoCandList Proton, Proton_match, Pbar, Pbar_match, All, pbarp;

//  Dplus.Cleanup();
//  Dminus.Cleanup();
//  kminus.Cleanup();
//  kplus.Cleanup();
//  piplus.Cleanup();
//  pipminus.Cleanup();
//  pbarp.Cleanup();

  RhoMassParticleSelector *ProtonMassSel=new RhoMassParticleSelector("proton",0.938,1);
  RhoMassParticleSelector *PbarMassSel=new RhoMassParticleSelector("pbar",0.938,1);
  TString pidSelection= "PidAlgoIdealCharged";

  while (theAnalysis->GetEvent() && i++<nevts)
     {
       if ((i%100)==0) cout<<"evt "<<i<<endl;

       Proton.Cleanup();
       Pbar.Cleanup();
       All.Cleanup();
  
       theAnalysis->FillList(Pbar, "ProtonBestMinus",pidSelection);
       theAnalysis->FillList(Proton, "ProtonBestPlus",pidSelection);
       //theAnalysis->FillList(Pbar, "ProtonVeryTightMinus",pidSelection);
       //theAnalysis->FillList(Proton, "ProtonVeryTightPlus",pidSelection);
       theAnalysis->FillList(All, "Charged");

       // --- START OF CHANGE ---
       // 1. Create a new RhoCandList to store the combined protons and antiprotons
       RhoCandList ProtonAndPbar;

       // 2. Add all particles from the Proton list to the new list
       for (int p_idx = 0; p_idx < Proton.GetLength(); ++p_idx) {
           ProtonAndPbar.Put(Proton[p_idx]);
       }

       // 3. Add all particles from the Pbar list to the new list
       for (int pbar_idx = 0; pbar_idx < Pbar.GetLength(); ++pbar_idx) {
           ProtonAndPbar.Put(Pbar[pbar_idx]);
       }

       // 4. Use the combined list to calculate the total POCA vertex
       if (ProtonAndPbar.GetLength() >= 2) {
           RhoVtxPoca pocaProtonPbar;
           TVector3 vtxProtonPbar;
           Double_t docaProtonPbar = pocaProtonPbar.GetPocaVtx(vtxProtonPbar, ProtonAndPbar);

           ntpDp->Column("proton_pbar_vtx_x", (Float_t)vtxProtonPbar.X(), -999.0f);
           ntpDp->Column("proton_pbar_vtx_y", (Float_t)vtxProtonPbar.Y(), -999.0f);
           ntpDp->Column("proton_pbar_vtx_z", (Float_t)vtxProtonPbar.Z(), -999.0f);
           ntpDp->Column("proton_pbar_doca", (Float_t)docaProtonPbar, -999.0f);
           ntpDp->DumpData();
       }
       // --- END OF CHANGE ---

       RhoVtxPoca pocaAllCharged;
       TVector3 vtxAllCharged;
       Double_t docaAllCharged = pocaAllCharged.GetPocaVtx(vtxAllCharged, All);
        //cout << "docaAllCharged: " << docaAllCharged << endl;
        ntpDp->Column("allcharged_vtx_x", (Float_t)vtxAllCharged.X(), -999.0f);
        ntpDp->Column("allcharged_vtx_y", (Float_t)vtxAllCharged.Y(), -999.0f);
        ntpDp->Column("allcharged_vtx_z", (Float_t)vtxAllCharged.Z(), -999.0f);
        ntpDp->Column("allcharged_doca", (Float_t)docaAllCharged, -999.0f);
        ntpDp->DumpData();

       for (int p_idx = 0; p_idx < Proton.GetLength(); ++p_idx) 
       {
           for (int pbar_idx = 0; pbar_idx < Pbar.GetLength(); ++pbar_idx) 
           {
            RhoVtxPoca pocaVtxCalculator;   
            RhoCandidate* currentProton = Proton[p_idx];
            RhoCandidate* currentPbar = Pbar[pbar_idx];
                
            // --- START OF CHANGE ---
            // Create a temporary particle list
            RhoCandList pairList;
            pairList.Put(currentProton); // Put the proton into the list
            pairList.Put(currentPbar);  // Put the antiproton into the list

            TVector3 vtx;
                
            // Call the public GetPocaVtx method
            Double_t doca = pocaVtxCalculator.GetPocaVtx(vtx, pairList);
            // --- END OF CHANGE ---

            // Store the result in the ntuple
            ntpDp->Column("poca_vtx_x", (Float_t)vtx.X(), -999.0f);
            ntpDp->Column("poca_vtx_y", (Float_t)vtx.Y(), -999.0f);
            ntpDp->Column("poca_vtx_z", (Float_t)vtx.Z(), -999.0f);
            ntpDp->Column("poca_doca", (Float_t)doca, -999.0f);
            ntpDp->DumpData();
           // --- END OF CHANGE ---
           }
       }

       int flag1 = 0;
       int flag2 = 0;
       for(j = 0; j < Proton.GetLength(); ++j)
       {
         Proton.SetType(2212);

         double betacm = Proton[j]->P()/Proton[j]->E();
         //Proton[j]->Boost(0,0,-betacm);
         //Proton[j]->Boost(-ini.BoostVector());

         RhoCandidate *proton_mc = Proton[j]->GetMcTruth();
         if(!proton_mc) continue;

         //proton_mc->Boost(0,0,-betacm);
         //proton_mc->Boost(-ini.BoostVector());

         double momentum = Proton[j] -> P();
         double mcmomentum = proton_mc -> P();
         TVector3 Vp = Proton[j] -> GetPosition();
         TVector3 Vp_mc = proton_mc -> GetPosition();
         TLorentzVector Ppos = Proton[j] -> P4();
         TLorentzVector Ppos_mc = proton_mc -> P4();
   
         ntpDp->Column("Proton_P", (Float_t) Proton[j]->P(), -999.0f);
         ntpDp->Column("Proton_Pmc", (Float_t) proton_mc->P(), -999.0f);
         ntpDp->Column("Proton_E", (Float_t) Proton[j]->E(), -999.0f);
         ntpDp->Column("Proton_Emc", (Float_t) proton_mc->E(), -999.0f);
         ntpDp->Column("Proton_M", (Float_t) Proton[j]->M(), -999.0f);
         ntpDp->Column("Proton_Mmc", (Float_t) proton_mc->M(), -999.0f);
         ntpDp->Column("Proton_Vx", (Float_t) Vp.X(), -999.0f);
         ntpDp->Column("Proton_Vy", (Float_t) Vp.Y(), -999.0f);
         ntpDp->Column("Proton_Vz", (Float_t) Vp.Z(), -999.0f);
         ntpDp->Column("Proton_Vxmc", (Float_t) Vp_mc.X(), -999.0f);
         ntpDp->Column("Proton_Vymc", (Float_t) Vp_mc.Y(), -999.0f);
         ntpDp->Column("Proton_Vzmc", (Float_t) Vp_mc.Z(), -999.0f);
         ntpDp->Column("Proton_theta", (Float_t) Ppos.Theta(), -999.0f);
         ntpDp->Column("Proton_theta_mc", (Float_t) Ppos_mc.Theta(), -999.0f);
         ntpDp->Column("Proton_costheta", (Float_t) Ppos.CosTheta(), -999.0f);
         ntpDp->Column("Proton_costheta_mc", (Float_t) Ppos_mc.CosTheta(), -999.0f);
         ntpDp->Column("Proton_phi", (Float_t) Ppos.Phi(), -999.0f);
         ntpDp->Column("Proton_phi_mc", (Float_t) Ppos_mc.Phi(), -999.0f);
      
         ntpDp->DumpData();
         
         if(!theAnalysis->McTruthMatch(Proton[j])) continue;
         //Proton_match.Append(Proton[j]);
         
         ntpDp->Column("Proton_match_P", (Float_t) Proton[j]->P(), -999.0f);
         ntpDp->Column("Proton_match_Vx", (Float_t) Vp.X(), -999.0f);
         ntpDp->Column("Proton_match_Vy", (Float_t) Vp.Y(), -999.0f);
         ntpDp->Column("Proton_match_Vz", (Float_t) Vp.Z(), -999.0f);
         ntpDp->Column("Proton_match_Vxmc", (Float_t) Vp_mc.X(), -999.0f);
         ntpDp->Column("Proton_match_Vymc", (Float_t) Vp_mc.Y(), -999.0f);
         ntpDp->Column("Proton_match_Vzmc", (Float_t) Vp_mc.Z(), -999.0f);
         ntpDp->Column("Proton_match_theta", (Float_t) Ppos.Theta(), -999.0f);
      
         ntpDp->DumpData();
         flag1 = 1;
       }
/*       
       for(j = 0; j < Proton_match.GetLength(); ++j)
       {
         RhoCandidate *proton_match_mc = Proton_match[j]->GetMcTruth();
         if(!proton_match_mc) continue;
         double momentum = Proton_match[j] -> P();
         double mcmomentum = proton_match_mc -> P();
         TVector3 Vp = Proton_match[j] -> GetPosition();
         TVector3 Vp_mc = proton_match_mc -> GetPosition();
         TLorentzVector Ppos = Proton_match[j] -> P4();
   
         ntpDp->Column("Proton_match_P", (Float_t) Proton_match[j]->P(), -999.0f);
         ntpDp->Column("Proton_match_Vx", (Float_t) Vp.X(), -999.0f);
         ntpDp->Column("Proton_match_Vy", (Float_t) Vp.Y(), -999.0f);
         ntpDp->Column("Proton_match_Vz", (Float_t) Vp.Z(), -999.0f);
         ntpDp->Column("Proton_match_Vxmc", (Float_t) Vp_mc.X(), -999.0f);
         ntpDp->Column("Proton_match_Vymc", (Float_t) Vp_mc.Y(), -999.0f);
         ntpDp->Column("Proton_match_Vzmc", (Float_t) Vp_mc.Z(), -999.0f);
         ntpDp->Column("Proton_match_costheta", (Float_t) Ppos.CosTheta(), -999.0f);
      
         ntpDp->DumpData();
       }
*/  
       for(k = 0; k < Pbar.GetLength(); ++k)
       { 
         Proton.SetType(-2212);

         double betacm = Pbar[k]->P()/Pbar[k]->E();
         //Pbar[k]->Boost(0,0,-betacm);
         //Pbar[k]->Boost(-ini.BoostVector());

         RhoCandidate *pbar_mc = Pbar[k]->GetMcTruth();
         if(!pbar_mc) continue;

         //pbar_mc->Boost(0,0,-betacm);
         //pbar_mc->Boost(-ini.BoostVector());
         
         double momentum = Pbar[k] -> P();
         double mcmomentum = pbar_mc -> P();
         TVector3 Vpbar = Pbar[k] -> GetPosition();
         TVector3 Vpbar_mc = pbar_mc -> GetPosition();
         TLorentzVector Pbarpos = Pbar[k] -> P4();
         TLorentzVector Pbarpos_mc = pbar_mc -> P4();
         
         ntpDp->Column("Pbar_P", (Float_t) Pbar[k]->P(), -999.0f);
         ntpDp->Column("Pbar_Pmc", (Float_t) pbar_mc->P(), -999.0f);
         ntpDp->Column("Pbar_E", (Float_t) Pbar[k]->E(), -999.0f);
         ntpDp->Column("Pbar_Emc", (Float_t) pbar_mc->E(), -999.0f);
         ntpDp->Column("Pbar_M", (Float_t) Pbar[k]->M(), -999.0f);
         ntpDp->Column("Pbar_Mmc", (Float_t) pbar_mc->M(), -999.0f);
         ntpDp->Column("Pbar_Vx", (Float_t) Vpbar.X(), -999.0f);
         ntpDp->Column("Pbar_Vy", (Float_t) Vpbar.Y(), -999.0f);
         ntpDp->Column("Pbar_Vz", (Float_t) Vpbar.Z(), -999.0f);
         ntpDp->Column("Pbar_Vxmc", (Float_t) Vpbar_mc.X(), -999.0f);
         ntpDp->Column("Pbar_Vymc", (Float_t) Vpbar_mc.Y(), -999.0f);
         ntpDp->Column("Pbar_Vzmc", (Float_t) Vpbar_mc.Z(), -999.0f);
         ntpDp->Column("Pbar_theta", (Float_t) Pbarpos.Theta(), -999.0f);
         ntpDp->Column("Pbar_theta_mc", (Float_t) Pbarpos_mc.Theta(), -999.0f);
         ntpDp->Column("Pbar_phi", (Float_t) Pbarpos.Phi(), -999.0f);
         ntpDp->Column("Pbar_phi_mc", (Float_t) Pbarpos_mc.Phi(), -999.0f);
      
         ntpDp->DumpData();
         
         if(!theAnalysis->McTruthMatch(Pbar[k])) continue;
         //Pbar_match.Append(Pbar[k]);
         
         ntpDp->Column("Pbar_match_P", (Float_t) Pbar[k]->P(), -999.0f);
         ntpDp->Column("Pbar_match_Vx", (Float_t) Vpbar.X(), -999.0f);
         ntpDp->Column("Pbar_match_Vy", (Float_t) Vpbar.Y(), -999.0f);
         ntpDp->Column("Pbar_match_Vz", (Float_t) Vpbar.Z(), -999.0f);
         ntpDp->Column("Pbar_match_Vxmc", (Float_t) Vpbar_mc.X(), -999.0f);
         ntpDp->Column("Pbar_match_Vymc", (Float_t) Vpbar_mc.Y(), -999.0f);
         ntpDp->Column("Pbar_match_Vzmc", (Float_t) Vpbar_mc.Z(), -999.0f);
         ntpDp->Column("Pbar_match_theta", (Float_t) Pbarpos.Theta(), -999.0f);
      
         ntpDp->DumpData();
         flag2 = 1;
       }
       
/*       for(k = 0; k < Pbar_match.GetLength(); ++k)
       { 
         RhoCandidate *pbar_mc = Pbar_match[k]->GetMcTruth();
         if(!pbar_mc) continue;
         double momentum = Pbar_match[k] -> P();
         double mcmomentum = pbar_mc -> P();
         TVector3 Vpbar = Pbar_match[k] -> GetPosition();
         TVector3 Vpbar_mc = pbar_mc -> GetPosition();
         TLorentzVector Pbarpos = Pbar_match[k] -> P4();
         
         ntpDp->Column("Pbar_match_P", (Float_t) Pbar_match[k]->P(), -999.0f);
         ntpDp->Column("Pbar_match_Vx", (Float_t) Vpbar.X(), -999.0f);
         ntpDp->Column("Pbar_match_Vy", (Float_t) Vpbar.Y(), -999.0f);
         ntpDp->Column("Pbar_match_Vz", (Float_t) Vpbar.Z(), -999.0f);
         ntpDp->Column("Pbar_match_Vxmc", (Float_t) Vpbar_mc.X(), -999.0f);
         ntpDp->Column("Pbar_match_Vymc", (Float_t) Vpbar_mc.Y(), -999.0f);
         ntpDp->Column("Pbar_match_Vzmc", (Float_t) Vpbar_mc.Z(), -999.0f);
         ntpDp->Column("Pbar_match_costheta", (Float_t) Pbarpos.CosTheta(), -999.0f);
      
         ntpDp->DumpData();
       }
*/

       int flag_bc = 0;
       double deltaE_tmp=9999;
       double pbarpP = -999.0;
       double pbarpE = -999.0;
       double pbarpM = -999.0;
       double pbarP = -999.0;
       double pbarE = -999.0;
       double pbarM = -999.0;
       double protonP = -999.0;
       double protonE = -999.0;
       double protonM = -999.0;
       RhoCandidate *BestProton = 0;
       RhoCandidate *BestProton_mc = 0;
       RhoCandidate *BestPbar = 0;
       RhoCandidate *BestPbar_mc = 0;
       RhoCandidate *BestConb = 0;
       RhoCandidate *BestConb_mc = 0;
       TVector3 pbarpP3;
       TVector3 pbarP3;
       TVector3 protonP3;
       TLorentzVector pbarpP4;
       TLorentzVector protonP4;
       TLorentzVector pbarP4;
       TLorentzVector protonP4_mc;
       TLorentzVector pbarP4_mc;
       TLorentzVector pbarpP4_mc;
       for(j = 0; j < Proton.GetLength(); ++j)
       {
         for(k = 0; k < Pbar.GetLength(); ++k)
         {
           RhoCandidate *conbcand = Proton[j]->Combine(Pbar[k]);
           //best candidate selection
           double deltaE = conbcand->P() - pbarmom;
           if(fabs(deltaE)<deltaE_tmp)
           {
              deltaE_tmp = fabs(deltaE);
              ntpDp->Column("deltaE", (Float_t) deltaE, -999.0f);
              ntpDp->DumpData();
              //RhoCandidate *BestProton = Proton[j];
              //RhoCandidate *BestPbar = Pbar[k];
              //RhoCandidate *BestConb = conbcand;
              BestProton = Proton[j];
              BestPbar = Pbar[k];
              BestConb = conbcand;
              BestProton_mc = BestProton->GetMcTruth();
              BestPbar_mc = BestPbar->GetMcTruth();
              BestConb_mc = BestConb->GetMcTruth();
              //if(!BestProton_mc) continue;
              //if(!BestPbar_mc) continue;
              //if(!BestConb_mc) continue;

              pbarpP3 = BestConb -> Pos();          // and the decay vertex position
              pbarpP4 = BestConb -> P4();
              //pbarpP4_mc = BestConb_mc -> P4();
              pbarP3 = BestPbar -> Pos();          // and the decay vertex position
              pbarP4 = BestPbar -> P4();
              //pbarP4_mc = BestPbar_mc -> P4();
              protonP3 = BestProton -> Pos();          // and the decay vertex position
              protonP4 = BestProton -> P4();
              //protonP4_mc = BestProton_mc -> P4();
              pbarpP = BestConb -> P();
              pbarP = BestPbar -> P();
              protonP = BestProton -> P();
              pbarpE = BestConb -> E();
              pbarE = BestPbar -> E();
              protonE = BestProton -> E();
              pbarpM = BestConb -> M();
              pbarM = BestPbar -> M();
              protonM = BestProton -> M();
              flag_bc = 1;
           }
         } 
       }
       if(flag_bc == 1) 
       {
          ntpDp->Column("pbarp_bc_P", (Float_t) pbarpP, -999.0f);
          ntpDp->Column("pbarp_bc_E", (Float_t) pbarpE, -999.0f);
          ntpDp->Column("pbarp_bc_M", (Float_t) pbarpM, -999.0f);
          ntpDp->Column("pbarp_bc_Px", (Float_t) pbarpP4.Px(), -999.0f);
          ntpDp->Column("pbarp_bc_Py", (Float_t) pbarpP4.Py(), -999.0f);
          ntpDp->Column("pbarp_bc_Pz", (Float_t) pbarpP4.Pz(), -999.0f);
          ntpDp->Column("pbar_bc_P", (Float_t) pbarP, -999.0f);
          ntpDp->Column("pbar_bc_E", (Float_t) pbarE, -999.0f);
          ntpDp->Column("pbar_bc_M", (Float_t) pbarM, -999.0f);
          ntpDp->Column("proton_bc_P", (Float_t) protonP, -999.0f);
          ntpDp->Column("proton_bc_E", (Float_t) protonE, -999.0f);
          ntpDp->Column("proton_bc_M", (Float_t) protonM, -999.0f);
          ntpDp->Column("pbarp_bc_x", (Float_t) pbarpP3.X(), -999.0f);
          ntpDp->Column("pbarp_bc_y", (Float_t) pbarpP3.Y(), -999.0f);
          ntpDp->Column("pbarp_bc_z", (Float_t) pbarpP3.Z(), -999.0f);
          ntpDp->Column("pbar_bc_x", (Float_t) pbarP3.X(), -999.0f);
          ntpDp->Column("pbar_bc_y", (Float_t) pbarP3.Y(), -999.0f);
          ntpDp->Column("pbar_bc_z", (Float_t) pbarP3.Z(), -999.0f);
          ntpDp->Column("proton_bc_x", (Float_t) protonP3.X(), -999.0f);
          ntpDp->Column("proton_bc_y", (Float_t) protonP3.Y(), -999.0f);
          ntpDp->Column("proton_bc_z", (Float_t) protonP3.Z(), -999.0f);
          ntpDp->Column("pbarp_bc_theta_p3", (Float_t) pbarpP3.Theta(), -999.0f);
          ntpDp->Column("pbarp_bc_theta_p4", (Float_t) pbarpP4.Theta(), -999.0f);
          ntpDp->Column("pbar_bc_theta_p3", (Float_t) pbarP3.Theta(), -999.0f);
          ntpDp->Column("pbar_bc_theta_p4", (Float_t) pbarP4.Theta(), -999.0f);
          ntpDp->Column("proton_bc_theta_p3", (Float_t) protonP3.Theta(), -999.0f);
          ntpDp->Column("proton_bc_theta_p4", (Float_t) protonP4.Theta(), -999.0f);
          ntpDp->Column("proton_bc_phi", (Float_t) protonP4.Phi(), -999.0f);
          ntpDp->Column("pbar_bc_phi", (Float_t) pbarP4.Phi(), -999.0f);
          ntpDp->Column("pbarp_bc_phi", (Float_t) pbarpP4.Phi(), -999.0f);
          //ntpDp->Column("pbarp_bc_theta_p4_mc", (Float_t) pbarpP4_mc.Theta(), -999.0f);
          //ntpDp->Column("pbar_bc_theta_p4_mc", (Float_t) pbarP4_mc.Theta(), -999.0f);
          //ntpDp->Column("proton_bc_theta_p4_mc", (Float_t) protonP4_mc.Theta(), -999.0f);
          //ntpDp->Column("proton_bc_phi_mc", (Float_t) protonP4_mc.Phi(), -999.0f);
          //ntpDp->Column("pbar_bc_phi_mc", (Float_t) pbarP4_mc.Phi(), -999.0f);
          //ntpDp->Column("pbarp_bc_phi_mc", (Float_t) pbarpP4_mc.Phi(), -999.0f);

          if(protonP4.Theta()<0.0523599||protonP4.Theta()>2.61799) continue;
          if(pbarP4.Theta()<0.0523599||pbarP4.Theta()>2.61799) continue;
                   
          ntpDp->DumpData();

          RhoKinVtxFitter vtxfitter(BestConb);        // instantiate the vertex fitter; input is the object to be fitted
          vtxfitter.Fit();                           // perform fit
          RhoCandidate *jfit = BestConb -> GetFit();    // get the fitted candidate
          TVector3 jVtx      = jfit->Pos();          // and the decay vertex position
          double chi2_vtx = vtxfitter.GetChi2();     // and the chi^2 of the fit
          TLorentzVector pos = jfit -> P4();
          
          ntpDp->Column("chisq_bc", (Float_t) chi2_vtx, -999.0f);
          ntpDp->Column("vtx_P", (Float_t) jfit->P(), -999.0f);
          ntpDp->Column("vtx_E", (Float_t) jfit->E(), -999.0f);
          ntpDp->Column("vtx_M", (Float_t) jfit->M(), -999.0f);
          ntpDp->Column("pvx_bc", (Float_t) jVtx.X(), -999.0f);
          ntpDp->Column("pvy_bc", (Float_t) jVtx.Y(), -999.0f);
          ntpDp->Column("pvz_bc", (Float_t) jVtx.Z(), -999.0f);
          ntpDp->Column("ptheta_bc_p3", (Float_t) jVtx.Theta(), -999.0f);
          ntpDp->Column("ptheta_bc_p4", (Float_t) pos.Theta(), -999.0f);
          ntpDp->Column("pbarp_bc_phi", (Float_t) pos.Phi(), -999.0f);

          ntpDp->DumpData();
          
          RhoKinFitter fit4c(BestConb);
          fit4c.Add4MomConstraint(ini);
          fit4c.Fit();
          double chi2_4c = fit4c.GetChi2();
          RhoCandidate *cfit  = BestConb -> GetFit();    // get the fitted candidate
          TVector3 cVtx       = cfit->Pos();          // and the decay vertex position
          TLorentzVector cpos = cfit -> P4();

          ntpDp->Column("chisq_4c_bc", (Float_t) chi2_4c, -999.0f);
          ntpDp->Column("pvx_4c_bc", (Float_t) cVtx.X(), -999.0f);
          ntpDp->Column("pvy_4c_bc", (Float_t) cVtx.Y(), -999.0f);
          ntpDp->Column("pvz_4c_bc", (Float_t) cVtx.Z(), -999.0f);
          ntpDp->Column("ptheta_4c_bc_p3", (Float_t) cVtx.Theta(), -999.0f);
          ntpDp->Column("ptheta_4c_bc_p4", (Float_t) cpos.Theta(), -999.0f);

          ntpDp->DumpData();

       }

       pbarp.Combine(Proton,Pbar);
   
       for(n = 0; n < pbarp.GetLength(); ++n){
         RhoCandidate *p = pbarp[n];
         TVector3 ppos = p -> Pos();          // and the decay vertex position
         TLorentzVector angle = p -> P4();
         ntpDp->Column("pbarp_P", (Float_t) p -> P(), -999.0f);
         ntpDp->Column("pbarp_E", (Float_t) p -> E(), -999.0f);
         ntpDp->Column("pbarp_M", (Float_t) p -> M(), -999.0f);
         ntpDp->Column("pbarp_x", (Float_t) ppos.X(), -999.0f);
         ntpDp->Column("pbarp_y", (Float_t) ppos.Y(), -999.0f);
         ntpDp->Column("pbarp_z", (Float_t) ppos.Z(), -999.0f);
         ntpDp->Column("pbarp_theta", (Float_t) angle.Theta(), -999.0f);
         ntpDp->DumpData();
         
         RhoKinVtxFitter vtxfitter(pbarp[n]);        // instantiate the vertex fitter; input is the object to be fitted
         vtxfitter.Fit();                           // perform fit
   
         RhoCandidate *jfit = pbarp[n]->GetFit();    // get the fitted candidate
         TVector3 jVtx      = jfit->Pos();          // and the decay vertex position
         double chi2_vtx = vtxfitter.GetChi2();     // and the chi^2 of the fit
         TLorentzVector pos = jfit -> P4();
         
         ntpDp->Column("chisq", (Float_t) chi2_vtx, -999.0f);
         ntpDp->Column("pvx", (Float_t) jVtx.X(), -999.0f);
         ntpDp->Column("pvy", (Float_t) jVtx.Y(), -999.0f);
         ntpDp->Column("pvz", (Float_t) jVtx.Z(), -999.0f);
         ntpDp->Column("ptheta", (Float_t) pos.Theta(), -999.0f);
   
         ntpDp->DumpData();
       } 
       
       for(n = 0; n < pbarp.GetLength(); ++n){
         //Rho4CFitter fit4c(pbarp[n], ini);
         RhoKinFitter fit4c(pbarp[n]);
         fit4c.Add4MomConstraint(ini);
         //RhoDecayTreeFitter fit4c(pbarp[n], ini);
         fit4c.Fit();
         double chi2_4c = fit4c.GetChi2();
//         double prob_dt = fit4c.GetProb();
         RhoCandidate *cfit  = pbarp[n]->GetFit();    // get the fitted candidate
         //RhoCandidate *cfit  = pbarp[n]->GetFit();    // get the fitted candidate
         TVector3 cVtx       = cfit->Pos();          // and the decay vertex position
         TLorentzVector cpos = cfit -> P4();

//         ntpDp->Column("Chisq_4c", (Float_t) chi2_4c, -999.0f);
//         ntpDp->DumpData();
         
//         if (chi2_4c>0. && chi2_4c<50.) {
//         ntpDp->Column("prob_dt", (Float_t) prob_dt, -999.0f);
         ntpDp->Column("chisq_4c", (Float_t) chi2_4c, -999.0f);
         ntpDp->Column("pvx_4c", (Float_t) cVtx.X(), -999.0f);
         ntpDp->Column("pvy_4c", (Float_t) cVtx.Y(), -999.0f);
         ntpDp->Column("pvz_4c", (Float_t) cVtx.Z(), -999.0f);
         ntpDp->Column("ptheta_4c", (Float_t) cpos.Theta(), -999.0f);
   
         ntpDp->DumpData();
//         }
       
       }
   
       //pbarp_match.Combine(Proton_match, Pbar_match);

       for(l = 0; l < pbarp.GetLength(); ++l){
         if(!(flag1*flag2)) continue;

//         pbarp.SetType(88888);
//         if(!theAnalysis->McTruthMatch(pbarp[l])) continue;
       
//         RhoCandidate *pbarp_mc = pbarp[l]->GetMcTruth();
//         if(!pbarp_mc) continue;

         RhoCandidate *p = pbarp[l];
         TVector3 ppos = p -> Pos();          // and the decay vertex position
         TLorentzVector angle = p -> P4();
         ntpDp->Column("pbarp_match_P", (Float_t) p -> P(), -999.0f);
         ntpDp->Column("pbarp_match_E", (Float_t) p -> E(), -999.0f);
         ntpDp->Column("pbarp_match_M", (Float_t) p -> M(), -999.0f);
         ntpDp->Column("pbarp_match_x", (Float_t) ppos.X(), -999.0f);
         ntpDp->Column("pbarp_match_y", (Float_t) ppos.Y(), -999.0f);
         ntpDp->Column("pbarp_match_z", (Float_t) ppos.Z(), -999.0f);
         ntpDp->Column("pbarp_match_theta", (Float_t) angle.Theta(), -999.0f);
         ntpDp->DumpData();
         
         RhoKinVtxFitter vtxfitter(pbarp[l]);        // instantiate the vertex fitter; input is the object to be fitted
         vtxfitter.Fit();                           // perform fit
   
         RhoCandidate *jfit = pbarp[l]->GetFit();    // get the fitted candidate
         TVector3 jVtx      = jfit->Pos();          // and the decay vertex position
         double chi2_vtx = vtxfitter.GetChi2();     // and the chi^2 of the fit
         TLorentzVector pos = jfit -> P4();
        
         ntpDp->Column("match_chisq", (Float_t) chi2_vtx, -999.0f);
         ntpDp->Column("match_pvx", (Float_t) jVtx.X(), -999.0f);
         ntpDp->Column("match_pvy", (Float_t) jVtx.Y(), -999.0f);
         ntpDp->Column("match_pvz", (Float_t) jVtx.Z(), -999.0f);
         ntpDp->Column("match_ptheta", (Float_t) pos.Theta(), -999.0f);
   
         ntpDp->DumpData();

         pbarp[l]->SetPosition(jVtx);
/*       }
       
       for(l = 0; l < pbarp.GetLength(); ++l){
         if(!(flag1*flag2)) continue;
*/         //Rho4CFitter fit4c(pbarp[n], ini);
         RhoKinFitter fit4c(pbarp[l]);
         fit4c.Add4MomConstraint(ini);
         //RhoDecayTreeFitter fit4c(pbarp[n], ini);
         fit4c.Fit();
         double chi2_4c = fit4c.GetChi2();
//         double prob_dt = fit4c.GetProb();
         RhoCandidate *cfit  = pbarp[l]->Daughter(0)->GetFit();    // get the fitted candidate
         //RhoCandidate *cfit  = pbarp[n]->GetFit();    // get the fitted candidate
         TVector3 cVtx       = cfit->Pos();          // and the decay vertex position
         TLorentzVector cpos = cfit -> P4();
//         ntpDp->Column("match_Chisq_4c", (Float_t) chi2_4c, -999.0f);
//         ntpDp->DumpData();
         
//         if (chi2_4c>0. && chi2_4c<50.) {
//         ntpDp->Column("match_prob_dt", (Float_t) prob_dt, -999.0f);
         ntpDp->Column("match_chisq_4c", (Float_t) chi2_4c, -999.0f);
         ntpDp->Column("match_pvx_4c", (Float_t) cVtx.X(), -999.0f);
         ntpDp->Column("match_pvy_4c", (Float_t) cVtx.Y(), -999.0f);
         ntpDp->Column("match_pvz_4c", (Float_t) cVtx.Z(), -999.0f);
         ntpDp->Column("match_ptheta_4c", (Float_t) cpos.Theta(), -999.0f);
   
         ntpDp->DumpData();
//         }
       
       }
   

     }
  out->cd();
  ntpDp->GetInternalTree()->Write();
  out->Save();

  // --- START OF ANALYSIS SCRIPT ---
  TTree *tree = ntpDp->GetInternalTree();
  // Use "pvx" from the vertex fit as the variable to plot
  TCut valid_data = "pvx != -999.0";

  // Create a TCanvas for display
  TCanvas *c1 = new TCanvas("c1", "Vertex Verification", 1200, 400);
  c1->Divide(3, 1);

  // First pass: determine Z cut to apply to all histograms
  TCut z_cut = "";
  Int_t entries_z_3rms = 0;
  
  if (tree->GetEntries(valid_data) > 0) {
    // Create temporary Z histogram to get mean and RMS
    Double_t min_z_range = config_z - 1.0;
    Double_t max_z_range = config_z + 1.0;
    TH1F *htemp_z = new TH1F("htemp_z", "", 100, min_z_range, max_z_range);
    tree->Draw("pvz>>htemp_z", valid_data, "goff");
    
    Double_t mean_z = htemp_z->GetMean();
    Double_t rms_z = htemp_z->GetRMS();
    delete htemp_z;
    
    // Define Z cut: mean_z ± 3*RMS
    z_cut = Form("pvz > %f && pvz < %f", mean_z - 3*rms_z, mean_z + 3*rms_z);
    entries_z_3rms = tree->GetEntries(valid_data && z_cut);
    
    std::cout << "Z cut applied: " << mean_z - 3*rms_z << " < pvz < " << mean_z + 3*rms_z << std::endl;
    std::cout << "Events after Z cut: " << entries_z_3rms << std::endl;
  }
  
  // Combined cut: valid data AND Z cut
  TCut final_cut = valid_data && z_cut;

  // --- Histogram for X coordinate (with Z cut) ---
  c1->cd(1);
  TH1F *h_vtx_x = nullptr;
  if (tree->GetEntries(final_cut) > 0) {
    Double_t min_x_range = config_x - 0.5;
    Double_t max_x_range = config_x + 0.5;

    h_vtx_x = new TH1F("h_vtx_x", "Fitted Vertex X (Z cut);X (cm);Events", 100, min_x_range, max_x_range);
    tree->Draw("pvx>>h_vtx_x", final_cut, "");
  }

  // --- Histogram for Y coordinate (with Z cut) ---
  c1->cd(2);
  TH1F *h_vtx_y = nullptr;
  if (tree->GetEntries(final_cut) > 0) {
    Double_t min_y_range = config_y - 0.5;
    Double_t max_y_range = config_y + 0.5;

    h_vtx_y = new TH1F("h_vtx_y", "Fitted Vertex Y (Z cut);Y (cm);Events", 100, min_y_range, max_y_range);
    tree->Draw("pvy>>h_vtx_y", final_cut, "");
  }

  // --- Histogram for Z coordinate (with Z cut) ---
  c1->cd(3);
  TH1F *h_vtx_z = nullptr;
  if (tree->GetEntries(final_cut) > 0) {
    Double_t min_z_range = config_z - 1.0;
    Double_t max_z_range = config_z + 1.0;

    h_vtx_z = new TH1F("h_vtx_z", "Fitted Vertex Z (Z cut);Z (cm);Events", 100, min_z_range, max_z_range);
    tree->Draw("pvz>>h_vtx_z", final_cut, "");
    
    // Count entries in X, Y, Z histograms (all with Z cut applied)
    Int_t entries_x = (h_vtx_x != nullptr) ? h_vtx_x->GetEntries() : 0;
    Int_t entries_y = (h_vtx_y != nullptr) ? h_vtx_y->GetEntries() : 0;
    Int_t entries_z = h_vtx_z->GetEntries();
    
    // Write to text file (append mode for parallel jobs)
    // Output format: config_x config_y config_z entries_z_3rms
    TString stats_file = stats_output_dir + "/vtx_stats.txt";
    std::ofstream outfile(stats_file.Data(), std::ios::app);
    if (outfile.is_open()) {
      outfile << config_x << " " << config_y << " " << config_z << " " 
              << entries_z_3rms << std::endl;
      outfile.close();
      std::cout << "Vertex statistics appended to " << stats_file << std::endl;
      std::cout << "Config: X=" << config_x << " Y=" << config_y << " Z=" << config_z 
                << " | Entries after Z cut: X=" << entries_x << " Y=" << entries_y 
                << " Z=" << entries_z << " Total=" << entries_z_3rms << std::endl;
    } else {
      std::cerr << "Error: Could not open " << stats_file << " for writing!" << std::endl;
    }
  }

  c1->SaveAs(figure_path + "/" + figure_name + "_vtx_verification.png");
  std::cout << "Verification histograms saved to " << figure_path << "/" << figure_name << "_vtx_verification.png" << std::endl;
  // --- END OF ANALYSIS SCRIPT ---

  fRun->Finish();

}
