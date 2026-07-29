class RhoCandList;
class RhoCandidate;
class PndAnaPidSelector;
class PndAnaPidCombiner;
class PndAnalysis;

void ana_dpm(int nevts = 100000, TString prefix = "barrel", Bool_t use_mvd_hvmaps = false, TString figure_path = ".", TString figure_name = "figure", Double_t pbarmom = 4.06)
{
  //-----User Settings:------------------------------------------------------
  TString parAsciiFile = use_mvd_hvmaps ? "all_hvmaps.par" : "all.par";
  //TString input = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  TString output = "ana";
  //TString friend1 = "reco";
  TString friend2 = "pid";
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
   gStyle->SetFrameBorderMode(0);
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetPadBorderMode(0);
    // Set global legend borderless
    gStyle->SetLegendBorderSize(0);
    // (Optional) Set global legend background transparent (Manual transparency is recommended as Legend FillStyle support varies across ROOT versions)
    gStyle->SetLegendFillColor(0);

    // ---------------------------------------------------------
    // Key: Line width and marker size (Optimized for 5000px width)
    // ---------------------------------------------------------
    // Default line width is usually 1px, almost invisible on a 5000px plot. Recommend 3 to 5.
    gStyle->SetLineWidth(4);      
    gStyle->SetFrameLineWidth(4); // Frame line width
    gStyle->SetHistLineWidth(6);  // Histogram line width
    gStyle->SetFuncWidth(6);      // Function/fitting curve width
    gStyle->SetGridWidth(2);      // Grid line width (if enabled)

    // Marker size, default 1.0, recommend enlarge to 2.0 - 3.0
    gStyle->SetMarkerSize(2.5);
    gStyle->SetMarkerStyle(20);   // Solid circle marker, clearest on large plots

    // ---------------------------------------------------------
    // Font settings (ROOT font size is percentage of Pad height)
    // ---------------------------------------------------------
    // 5000x2500 is 2:1 wide plot.
    // 0.05 means 5% of height, i.e., 2500 * 0.05 = 125 pixels high (very clear).
    
    // Axis scale numerical labels
    gStyle->SetLabelSize(0.04, "XY"); 
    gStyle->SetLabelFont(42, "XY");   // Font 42 (Helvetica) is more standard than default 62

    // Axis title
    gStyle->SetTitleSize(0.06, "XY"); 
    gStyle->SetTitleFont(42, "XY");

    // Top chart title
    gStyle->SetTitleSize(0.06, "t");  
    gStyle->SetTitleFont(42, "t");

    // ---------------------------------------------------------
    // Layout fine-tuning
    // ---------------------------------------------------------
    // Adjust distance between title and axis. Decrease Offset since canvas is very wide
    gStyle->SetTitleOffset(0.95, "X");
    gStyle->SetTitleOffset(1.2, "Y"); // Y axis title closer looks better on wide canvas

    // Tick length (slightly longer for better texture)
    gStyle->SetTickLength(0.02, "XY");
    
    // StatBox - Adjust font and position if display is needed
    gStyle->SetStatFont(42);
    gStyle->SetStatFontSize(0.06); // Enlarge font
    gStyle->SetStatBorderSize(2); // Thicken border
    // Set drawing style
    // 1111: 1(Name)-1(Entries)-1(Mean)-1(Std Dev)
    // Option 1110 displays Entries, Mean, Std Dev
    gStyle->SetOptStat(1110); 
    gStyle->SetOptTitle(1);
    gStyle->SetPadBottomMargin(0.15); // Bottom margin for X axis title (default approx 0.1)
    gStyle->SetPadLeftMargin(0.15);   // Left margin for Y axis title (default approx 0.1)
    gStyle->SetPadRightMargin(0.05);  // Right side slightly compact
    gStyle->SetPadTopMargin(0.08);

  /*  gStyle->SetOptFit(11);

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

  TFile *out = TFile::Open(prefix + "_boost.root","RECREATE");

  RhoTuple *ntpDp = new RhoTuple("ntpDp","Dp Analysis");

  // One entry per input event, consumed by the second PID pass.
  TTree *eventPocaTree = new TTree("event_poca", "Event-level proton-antiproton POCA");
  Int_t eventPocaId = -1;
  Double_t eventPocaX = 0.;
  Double_t eventPocaY = 0.;
  Double_t eventPocaZ = 0.;
  Bool_t eventPocaValid = kFALSE;
  eventPocaTree->Branch("event_id", &eventPocaId, "event_id/I");
  eventPocaTree->Branch("x", &eventPocaX, "x/D");
  eventPocaTree->Branch("y", &eventPocaY, "y/D");
  eventPocaTree->Branch("z", &eventPocaZ, "z/D");
  eventPocaTree->Branch("valid", &eventPocaValid, "valid/O");

  PndAnalysis* theAnalysis = new PndAnalysis();
  if (nevts == 0)
  nevts = theAnalysis->GetEntries();

  double mp = 0.938272;
  TLorentzVector ini(0., 0., pbarmom,
                     sqrt(pbarmom * pbarmom + mp * mp) + mp);

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

       eventPocaId = i - 1;
       eventPocaX = eventPocaY = eventPocaZ = 0.;
       eventPocaValid = kFALSE;

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
       if (Proton.GetLength() > 0 && Pbar.GetLength() > 0) {
           RhoVtxPoca pocaProtonPbar;
           TVector3 vtxProtonPbar;
           Double_t docaProtonPbar = pocaProtonPbar.GetPocaVtx(vtxProtonPbar, ProtonAndPbar);

           eventPocaX = vtxProtonPbar.X();
           eventPocaY = vtxProtonPbar.Y();
           eventPocaZ = vtxProtonPbar.Z();
           eventPocaValid = std::isfinite(eventPocaX) && std::isfinite(eventPocaY) &&
                            std::isfinite(eventPocaZ) && std::isfinite(docaProtonPbar);

           ntpDp->Column("proton_pbar_vtx_x", (Float_t)vtxProtonPbar.X(), -999.0f);
           ntpDp->Column("proton_pbar_vtx_y", (Float_t)vtxProtonPbar.Y(), -999.0f);
           ntpDp->Column("proton_pbar_vtx_z", (Float_t)vtxProtonPbar.Z(), -999.0f);
           ntpDp->Column("proton_pbar_doca", (Float_t)docaProtonPbar, -999.0f);
           ntpDp->DumpData();
       }
       eventPocaTree->Fill();
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
         Proton[j]->Boost(-ini.BoostVector());

         RhoCandidate *proton_mc = Proton[j]->GetMcTruth();
         if(!proton_mc) continue;

         //proton_mc->Boost(0,0,-betacm);
         proton_mc->Boost(-ini.BoostVector());

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
         Pbar[k]->Boost(-ini.BoostVector());

         RhoCandidate *pbar_mc = Pbar[k]->GetMcTruth();
         if(!pbar_mc) continue;

         //pbar_mc->Boost(0,0,-betacm);
         pbar_mc->Boost(-ini.BoostVector());
         
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
  eventPocaTree->Write();
  out->Save();

  // --- START OF ANALYSIS SCRIPT ---
  TTree *tree = ntpDp->GetInternalTree();
  TCut valid_data = "proton_pbar_vtx_x != -999.0";

  // Create a TCanvas for display
  TCanvas *c1 = new TCanvas("c1", "Vertex Fitting", 6000, 1800);
  c1->Divide(3, 1);

  // --- Fit X coordinate (Iterative Robust Range) ---
  c1->cd(1);
  TH1F *h_vtx_x = nullptr;
  if (tree->GetEntries(valid_data) > 0) {
    // 1. Create a temporary histogram with a wide range to get statistics
    TH1F *htemp_x = new TH1F("htemp_x", "", 200, -50, 50); // Wide range
    tree->Draw("proton_pbar_vtx_x>>htemp_x", valid_data, "goff");

    // 2. Get robust statistics (mean and RMS)
    Double_t mean_x_stat = htemp_x->GetMean();
    Double_t rms_x_stat = htemp_x->GetRMS();
    delete htemp_x; // Clean up temporary histogram

    // 3. Define a robust range, e.g., mean +/- 3*RMS
    Double_t min_x_robust = mean_x_stat - 3 * rms_x_stat;
    Double_t max_x_robust = mean_x_stat + 3 * rms_x_stat;

    // 4. Create the final histogram using the robust range
    h_vtx_x = new TH1F("h_vtx_x", ";#font[132]{#it{x} (cm)};#font[132]{#Events}", 100, min_x_robust, max_x_robust);
    tree->Draw("proton_pbar_vtx_x>>h_vtx_x", valid_data, "goff");
    
    // 5. First fit (wider range)
    double mean_x_h = h_vtx_x->GetMean();
    double rms_x_h = h_vtx_x->GetRMS();
    TF1 *fit1_x = new TF1("fit1_x", "gaus", mean_x_h - 2 * rms_x_h, mean_x_h + 2 * rms_x_h);
    h_vtx_x->Fit(fit1_x, "RQ0"); // "Q" for quiet, "0" to not draw

    // 6. Second fit (with a polynomial background)
    double mean1_x = fit1_x->GetParameter(1);
    double sigma1_x = fit1_x->GetParameter(2);
    TF1 *fit2_x = new TF1("fit2_x", "gaus(0)+pol1(3)", mean1_x - 1 * sigma1_x, mean1_x + 1 * sigma1_x);
    fit2_x->SetParameters(fit1_x->GetParameter(0), mean1_x, sigma1_x, 1, 0, 0); // Initialize parameters
    h_vtx_x->Fit(fit2_x, "R"); // "R" to fit in range

    h_vtx_x->GetXaxis()->CenterTitle();
    h_vtx_x->GetYaxis()->CenterTitle();
    h_vtx_x->Draw();
    
    // Manually adjust stat box position to avoid overflowing under large fonts
    gPad->Update(); // Must call Update() first to locate TPaveStats
    TPaveStats *st_x = (TPaveStats*)h_vtx_x->FindObject("stats");
    if(st_x) {
       st_x->SetX1NDC(0.65);
       st_x->SetX2NDC(0.95);
       st_x->SetY1NDC(0.65);
       st_x->SetY2NDC(0.90);
    }
    
    fit2_x->Draw("same"); // Draw the final fit on the histogram

    TLatex *latex_x = new TLatex();
    latex_x->SetNDC();
    latex_x->SetTextSize(0.06);
    latex_x->DrawLatex(0.58, 0.60, Form("#font[132]{#it{v_{x}} = %.2g #pm %.2g}", fit2_x->GetParameter(1), fit2_x->GetParError(1)));

    if (fit2_x) {
      cout << "********* Vertex X Fit Result (Iterative) *********" << endl;
      cout << "Mean: " << fit2_x->GetParameter(1) << " +/- " << fit2_x->GetParError(1) << " cm" << endl;
      cout << "Sigma: " << fit2_x->GetParameter(2) << " +/- " << fit2_x->GetParError(2) << " cm" << endl;
      cout << "***************************************************" << endl;
    }
  }

  // --- Fit Y coordinate (Iterative Robust Range) ---
  c1->cd(2);
  TH1F *h_vtx_y = nullptr;
  if (tree->GetEntries(valid_data) > 0) {
    TH1F *htemp_y = new TH1F("htemp_y", "", 200, -50, 50);
    tree->Draw("proton_pbar_vtx_y>>htemp_y", valid_data, "goff");
    Double_t mean_y_stat = htemp_y->GetMean();
    Double_t rms_y_stat = htemp_y->GetRMS();
    delete htemp_y;
    Double_t min_y_robust = mean_y_stat - 3 * rms_y_stat;
    Double_t max_y_robust = mean_y_stat + 3 * rms_y_stat;

    h_vtx_y = new TH1F("h_vtx_y", ";#font[132]{#it{y} (cm)};#font[132]{#Events}", 100, min_y_robust, max_y_robust);
    tree->Draw("proton_pbar_vtx_y>>h_vtx_y", valid_data, "goff");
    
    double mean_y_h = h_vtx_y->GetMean();
    double rms_y_h = h_vtx_y->GetRMS();
    TF1 *fit1_y = new TF1("fit1_y", "gaus", mean_y_h - 2 * rms_y_h, mean_y_h + 2 * rms_y_h);
    h_vtx_y->Fit(fit1_y, "RQ0");

    // Second fit (with a polynomial background)
    double mean1_y = fit1_y->GetParameter(1);
    double sigma1_y = fit1_y->GetParameter(2);
    TF1 *fit2_y = new TF1("fit2_y", "gaus(0)+pol1(3)", mean1_y - 1 * sigma1_y, mean1_y + 1 * sigma1_y);
    fit2_y->SetParameters(fit1_y->GetParameter(0), mean1_y, sigma1_y, 1, 0, 0);
    h_vtx_y->Fit(fit2_y, "R");

    h_vtx_y->GetXaxis()->CenterTitle();
    h_vtx_y->GetYaxis()->CenterTitle();
    h_vtx_y->Draw();
    
    gPad->Update();
    TPaveStats *st_y = (TPaveStats*)h_vtx_y->FindObject("stats");
    if(st_y) {
       st_y->SetX1NDC(0.65);
       st_y->SetX2NDC(0.95);
       st_y->SetY1NDC(0.65);
       st_y->SetY2NDC(0.90);
    }
    
    fit2_y->Draw("same");

    TLatex *latex_y = new TLatex();
    latex_y->SetNDC();
    latex_y->SetTextSize(0.06);
    latex_y->DrawLatex(0.58, 0.60, Form("#font[132]{#it{v_{y}} = %.2g #pm %.1g}", fit2_y->GetParameter(1), fit2_y->GetParError(1)));

    if (fit2_y) {
      cout << "********* Vertex Y Fit Result (Iterative) *********" << endl;
      cout << "Mean: " << fit2_y->GetParameter(1) << " +/- " << fit2_y->GetParError(1) << " cm" << endl;
      cout << "Sigma: " << fit2_y->GetParameter(2) << " +/- " << fit2_y->GetParError(2) << " cm" << endl;
      cout << "***************************************************" << endl;
    }
  }

  // --- Fit Z coordinate (Iterative Robust Range) ---
  c1->cd(3);
  TH1F *h_vtx_z = nullptr;
  if (tree->GetEntries(valid_data) > 0) {
    TH1F *htemp_z = new TH1F("htemp_z", "", 200, -50, 50);
    tree->Draw("proton_pbar_vtx_z>>htemp_z", valid_data, "goff");
    Double_t mean_z_stat = htemp_z->GetMean();
    Double_t rms_z_stat = htemp_z->GetRMS();
    delete htemp_z;
    Double_t min_z_robust = mean_z_stat - 3 * rms_z_stat;
    Double_t max_z_robust = mean_z_stat + 3 * rms_z_stat;

    h_vtx_z = new TH1F("h_vtx_z", ";#font[132]{#it{z} (cm)};#font[132]{#Events}", 100, min_z_robust, max_z_robust);
    tree->Draw("proton_pbar_vtx_z>>h_vtx_z", valid_data, "goff");

    double mean_z_h = h_vtx_z->GetMean();
    double rms_z_h = h_vtx_z->GetRMS();
    TF1 *fit1_z = new TF1("fit1_z", "gaus", mean_z_h - 2 * rms_z_h, mean_z_h + 2 * rms_z_h);
    h_vtx_z->Fit(fit1_z, "RQ0");

    // Second fit (with a polynomial background)
    double mean1_z = fit1_z->GetParameter(1);
    double sigma1_z = fit1_z->GetParameter(2);
    TF1 *fit2_z = new TF1("fit2_z", "gaus(0)+pol1(3)", mean1_z - 1 * sigma1_z, mean1_z + 1 * sigma1_z);
    fit2_z->SetParameters(fit1_z->GetParameter(0), mean1_z, sigma1_z, 1, 0, 0);
    h_vtx_z->Fit(fit2_z, "R");

    h_vtx_z->GetXaxis()->CenterTitle();
    h_vtx_z->GetYaxis()->CenterTitle();
    h_vtx_z->Draw();
    
    gPad->Update();
    TPaveStats *st_z = (TPaveStats*)h_vtx_z->FindObject("stats");
    if(st_z) {
       st_z->SetX1NDC(0.65);
       st_z->SetX2NDC(0.95);
       st_z->SetY1NDC(0.65);
       st_z->SetY2NDC(0.90);
    }
    
    fit2_z->Draw("same");

    TLatex *latex_z = new TLatex();
    latex_z->SetNDC();
    latex_z->SetTextSize(0.06);
    latex_z->DrawLatex(0.58, 0.60, Form("#font[132]{#it{v_{z}} = %.3g #pm %.1g}", fit2_z->GetParameter(1), fit2_z->GetParError(1)));

    if (fit2_z) {
      cout << "********* Vertex Z Fit Result (Iterative) *********" << endl;
      cout << "Mean: " << fit2_z->GetParameter(1) << " +/- " << fit2_z->GetParError(1) << " cm" << endl;
      cout << "Sigma: " << fit2_z->GetParameter(2) << " +/- " << fit2_z->GetParError(2) << " cm" << endl;
      cout << "***************************************************" << endl;
    }
  }

  c1->SaveAs(figure_path + "/" + figure_name + "_vtx_fit.png");

  // --- Save fit results to JSON ---
  TString json_filename = prefix + "_vtx_fit.json";
  std::ofstream json_file(json_filename.Data());
  if (json_file.is_open()) {
    json_file << "{\n";
    bool first_entry = true;
    if (h_vtx_x && h_vtx_x->GetFunction("fit2_x")) {
      TF1 *fit2_x = h_vtx_x->GetFunction("fit2_x");
      if (!first_entry) json_file << ",\n";
      json_file << "  \"vertex_x\": {\n";
      json_file << "    \"mean\": " << fit2_x->GetParameter(1) << ",\n";
      json_file << "    \"sigma\": " << fit2_x->GetParameter(2) << "\n";
      json_file << "  }";
      first_entry = false;
    }
    if (h_vtx_y && h_vtx_y->GetFunction("fit2_y")) {
      TF1 *fit2_y = h_vtx_y->GetFunction("fit2_y");
      if (!first_entry) json_file << ",\n";
      json_file << "  \"vertex_y\": {\n";
      json_file << "    \"mean\": " << fit2_y->GetParameter(1) << ",\n";
      json_file << "    \"sigma\": " << fit2_y->GetParameter(2) << "\n";
      json_file << "  }";
      first_entry = false;
    }
    if (h_vtx_z && h_vtx_z->GetFunction("fit2_z")) {
      TF1 *fit2_z = h_vtx_z->GetFunction("fit2_z");
      if (!first_entry) json_file << ",\n";
      json_file << "  \"vertex_z\": {\n";
      json_file << "    \"mean\": " << fit2_z->GetParameter(1) << ",\n";
      json_file << "    \"sigma\": " << fit2_z->GetParameter(2) << "\n";
      json_file << "  }";
      first_entry = false;
    }
    json_file << "\n}\n";
    json_file.close();
    std::cout << "Fit results saved to " << json_filename.Data() << std::endl;
  } else {
    std::cerr << "Error: Could not open " << json_filename.Data() << " for writing." << std::endl;
  }

  // --- END OF ANALYSIS SCRIPT ---

  fRun->Finish();

}
