int anaDMesonsCharged(TString filename="data/dpdm", int nevts=0)
{
  TStopwatch timer;
  timer.Start();
  int verbose=0;
  int canvasplot=1; //whether to plot into a ps, too.
  int canvasdraw=1;
	gSystem->Load("libEGPythia6");  // needed for TDatabasePDG
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	gSystem->Load("libRho");
	gSystem->Load("libPid");
	gSystem->Load("libAnalysisTools");
  
  gROOT->LoadMacro("plothistos.C");
  //gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Tools.C");
  //LoadPandaStyle();
    
  int bins=100;
  double min=0,max=2.5; // GeV/c
  double min2=0.13,max2=0.15;
  double min3=1.,max3=1.5;
  double max4=0.3;//cm
  double maxchi = 50;
  
  TList* histolist = new TList();
  
  //TH1F *hChisquare=new TH1F("chisq","#Chi^{2}",bins,0,maxchi);
  ////  TProfile *hChisquareMom=new TProfile("chisqmom",";p / GeV/c;#Chi^{2}",bins/5,min,max,0,maxchi);
  ////  TProfile *hChisquareTheta=new TProfile("chisqthe",";#theta / rad;#Chi^{2}",bins/5,0,3.5,0,maxchi);
  //TH2F *hChisquareMom=new TH2F("chisqmom",";p / GeV/c;#Chi^{2}",bins,min,max,bins,0,maxchi);
  //TH2F *hChisquareTheta=new TH2F("chisqthe",";#theta / rad;#Chi^{2}",bins,0,3.5,bins,0,maxchi);
  //TH2F *hMomMass=new TH2F("mommass","Mom vs mass;M*Q / e*GeV/c^{2};p / GeV/c",bins,-max2,max2,bins,min,max);
  
  TH1F *hMompip=new TH1F("momppip","P(#pi+);p / GeV/c",bins,min,max);
  TH1F *hMompim=new TH1F("momppim","P(#pi-);p / GeV/c",bins,min,max);
  TH1F *hMomKp=new TH1F("mompkp","P(K+);p / GeV/c",bins,min,max);
  TH1F *hMomKm=new TH1F("mompkm","P(K-);p / GeV/c",bins,min,max);
  TH1I *hTFStat=new TH1I("tfstat","Track Fit Status Flag",10,-5,5);
  
  TH1F *hDpMass=new TH1F("dpmass","Mass D+;m / (GeV/c^{2})",bins,0,5);
  TH1F *hDmMass=new TH1F("dmmass","Mass D-;m / (GeV/c^{2})",bins,0,5);
  TH1F *hBeamMass=new TH1F("beammass","Mass Beam;m / (GeV/c^{2})",bins,0,5);
  
  TH1F *hVtxX=new TH1F("hvtxx","Vertex after fit (D+ and D-);x/cm",bins,-0.2,0.2);
  TH1F *hVtxY=new TH1F("hvtxy","Vertex after fit (D+ and D-);y/cm",bins,-0.2,0.2);
  TH1F *hVtxZ=new TH1F("hvtxz","Vertex after fit (D+ and D-);z/cm",bins,-0.2,0.2);
  TH1F *hVtxR=new TH1F("hvtxr","Vertex after fit (D+ and D-);|r|/cm",bins,0,20);
  TH1F *hVtxChi2=new TH1F("hvtxchi2","Vertex Fit #Chi^{2};#Chi^{2}",bins,0,10);
  
  // Add histogram pointers to a list for iterative saving & plotting
  //---newpage
  histolist->Add(hMompip);
  histolist->Add(hMompim);
  histolist->Add(hMomKp);
  histolist->Add(hMomKm);
  histolist->Add(hDpMass);
  histolist->Add(hDmMass);
  histolist->Add(hBeamMass);
  histolist->Add(hTFStat);
  histolist->Add(hVtxX);
  histolist->Add(hVtxY);
  histolist->Add(hVtxZ);
  histolist->Add(hVtxR);
  histolist->Add(hVtxChi2);
  
  // Set up the Analysis
  TString histoFile = InitRun(filename);
  PndAnalysis* theAnalysis = new PndAnalysis();
  
  // the candidates lists we need
  TCandList pionp, pionm, kaonp, kaonm, dplus, dminus ,mctracks, beam;
  TCandidate *tmpcand=0, *recocand=0, *mccand=0;
  
  int evts = theAnalysis->GetEntries();
  if (nevts>0 && nevts<evts) evts=nevts; 
  
  double tfcut=-1; // trackfit status cut
  
  TLorentzVector tmpMom;
  TVector3 p1,p2,backboost,vertex;
  double mass=0,momentum=0,theta=0,chi2,energy=0;
  int ievt=0,j=0,mcidx=-1,fitstat=0;
  bool tester=false;
  
  TDatabasePDG *pdgDatabase = TRho::Instance()->GetPDG();	// Access particle DB
  //TParticlePDG* pTypePi0=pdgDatabase->GetParticle(111); // Pi0 PDG entry
  // some mass selectors
  //TPidMassSelector *piMassSel=new TPidMassSelector("pi0",pTypePi0->Mass(),0.03);
  
  // *************
  // this is the loop through the events ... as simple as this...
  // ****************
  std::cout<<"   *****   Start the eventloop with "<<evts<<" events.   *****"<<std::endl;
  while (theAnalysis->GetEvent() && ievt++<evts)
  {    
    theAnalysis->FillList(mctracks,"McTruth");
    theAnalysis->FillList(pionp,"PionLoosePlus");
    theAnalysis->FillList(pionm,"PionLooseMinus");
    theAnalysis->FillList(kaonp,"KaonLoosePlus");
    theAnalysis->FillList(kaonm,"KaonLooseMinus");
    if(verbose>0) printf("Event %i: TCandList sizes: pionp:%i, pionm:%i, kaonp:%i, kaonm:%i, mctracks:%i\n",ievt,pionp.GetLength(),pionm.GetLength(),kaonp.GetLength(),kaonm.GetLength(),mctracks.GetLength());
    
    for (j=0;j<pionp.GetLength();++j) {
      recocand=pionp.Get(j);
      if(!recocand) {if(verbose>0) cout<<"############## pionp\n#############"<<endl;continue;}
      if(verbose>1) std::cout<<" Pion+: "<<*recocand<<std::endl;
      fitstat=recocand->GetMicroCandidate().GetFitStatus();
      hTFStat->Fill(fitstat);
      if(fitstat<tfcut) {
        pionm.Remove(*recocand);
        continue;
      }
      tmpMom=recocand->P4();
      momentum=tmpMom.P();
      hMompip->Fill(momentum);
    }
    for (j=0;j<pionm.GetLength();++j) {
      recocand=pionm.Get(j);
      if(!recocand) {if(verbose>0) cout<<"############## pionm\n#############"<<endl;continue;}
      if(verbose>1) std::cout<<" Pion-: "<<*recocand<<std::endl;
      fitstat=recocand->GetMicroCandidate().GetFitStatus();
      hTFStat->Fill(fitstat);
      if(fitstat<tfcut) {
        pionm.Remove(*recocand);
        continue;
      }
      tmpMom=recocand->P4();
      momentum=tmpMom.P();
      hMompim->Fill(momentum);
    }
    for (j=0;j<kaonp.GetLength();++j) {
      recocand=kaonp.Get(j);
      if(!recocand) {if(verbose>0) cout<<"############## kaonp\n#############"<<endl;continue;}
      if(verbose>1) std::cout<<" K+: "<<*recocand<<std::endl;
      fitstat=recocand->GetMicroCandidate().GetFitStatus();
      hTFStat->Fill(fitstat);
      if(fitstat<tfcut) {
        kaonp.Remove(*recocand);
        continue;
      }
      tmpMom=recocand->P4();
      momentum=tmpMom.P();
      hMomKp->Fill(momentum);
    }
    for (j=0;j<kaonm.GetLength();++j) {
      recocand=kaonm.Get(j);
      if(!recocand) {if(verbose>0) cout<<"############## kaonm\n#############"<<endl;continue;}
      if(verbose>1) std::cout<<" K-: "<<*recocand<<std::endl;
      fitstat=recocand->GetMicroCandidate().GetFitStatus();
      hTFStat->Fill(fitstat);
      if(fitstat<tfcut) {
        kaonm.Remove(*recocand);
        continue;
      }
      tmpMom=recocand->P4();
      momentum=tmpMom.P();
      hMomKm->Fill(momentum);
    }
    
    //    for (j=0;j<pionp.GetLength();++j) {
    //      recocand = pionp.Get(j);
    //      if(!recocand) {if(verbose>0) cout<<"############## pi+\n#############"<<endl;continue;}
    //      chi2=recocand->GetMicroCandidate().GetChiSquared();
    //      if(0==chi2) continue;
    //      if(verbose>1) std::cout<<" Pion+: "<<*recocand<<std::endl;
    //      if(verbose>1) std::cout<<" recocand->Uid()="<<recocand->Uid()<<std::endl;
    //      tmpMom=recocand->P4(); 
    //      mass=tmpMom.M(); momentum=tmpMom.P(); theta=tmpMom.Theta();
    //      mcidx=recocand->GetMicroCandidate().GetMcIndex();
    //      mccand=mctracks.Get(mcidx);
    //      if(verbose>2) printf("Trying mccand no %i at position %p from pi+ candidate %i at %p.\n",mcidx,mccand,j,recocand);
    //      if(!mccand || mcidx<0) {if(verbose>0) printf("did not find mccand for pi+, mcid=%i\n",mcidx); continue;}
    //    }
    
    dplus.Combine(kaonm,pionp,pionp);
    //Vertex Fit. This changes daughters, too!
    for (j=0;j<dplus.GetLength();++j) {
      recocand=dplus.Get(j);
      if(!recocand) {if(verbose>0) cout<<"############## Combine dplus\n#############"<<endl;continue;}
      if(verbose>1) std::cout<<" recocand->Uid()="<<recocand->Uid()<<std::endl;
      if(verbose>1) recocand->Print();
      //PndVtxFitter fitter(*recocand); //D. Mishra 2008
      //PndChiVtxFitter fitter(*recocand); //V. Jha 2010 defunc-gives zeros?
      PndKinVtxFitter fitter(*recocand); //V. Jha 2010 to be tested
      //PndKinFitter fitter(*recocand); //V. Jha 2010
      //fitter.SetVerbose();
      fitter.Fit();
      recocand=fitter.FittedCand(*recocand);
      if(!recocand) {if(verbose>0) cout<<"############## Fit dplus\n#############"<<endl;continue;}
      if(verbose>1) std::cout<<" recocand->Uid()="<<recocand->Uid()<<std::endl;
      if(verbose>1) recocand->Print();
      // plot
      tmpMom=recocand->P4();
      mass=tmpMom.M();
      hDpMass->Fill(mass);
      vertex=recocand->GetPosition();
      if(verbose>2) std::cout<<"Vertex of the fit cand is:"<<std::endl;
      if(verbose>2) vertex.Print();
      hVtxX->Fill(vertex.X());
      hVtxY->Fill(vertex.Y());
      hVtxZ->Fill(vertex.Z());
      hVtxR->Fill(vertex.Mag());
      hVtxChi2->Fill(fitter.GlobalChi2());
    }
    dminus.Combine(kaonp,pionm,pionm);
    //Vertex Fit. This changes daughters, too!
    for (j=0;j<dminus.GetLength();++j) {
      recocand=dminus.Get(j);
      if(!recocand) {if(verbose>0) cout<<"############## Combine dminus\n#############"<<endl;continue;}
      if(verbose>1) std::cout<<" recocand->Uid()="<<recocand->Uid()<<std::endl;
      if(verbose>1) recocand->Print();
      //PndVtxFitter fitter(*recocand); //D. Mishra 2008
      //PndChiVtxFitter fitter(*recocand); //V. Jha 2010 defunc-gives zeros?
      PndKinVtxFitter fitter(*recocand); //V. Jha 2010 to be tested
      //PndKinFitter fitter(*recocand); //V. Jha 2010
      //fitter.SetVerbose();
      fitter.Fit();
      recocand=fitter.FittedCand(*recocand);
      if(!recocand) {if(verbose>0) cout<<"############## Fit dminus\n#############"<<endl;continue;}
      if(verbose>1) std::cout<<" recocand->Uid()="<<recocand->Uid()<<std::endl;
      if(verbose>1) recocand->Print();
      // plot
      tmpMom=recocand->P4();
      mass=tmpMom.M();
      hDmMass->Fill(mass);
      vertex=recocand->GetPosition();
      if(verbose>2) std::cout<<"Vertex of the fit cand is:"<<std::endl;
      if(verbose>2) vertex.Print();
      hVtxX->Fill(vertex.X());
      hVtxY->Fill(vertex.Y());
      hVtxZ->Fill(vertex.Z());
      hVtxR->Fill(vertex.Mag());
      hVtxChi2->Fill(fitter.GlobalChi2());
    }
    
    beam.Combine(dplus,dminus);
    for (j=0;j<beam.GetLength();++j) {
      recocand=beam.Get(j);
      if(!recocand) {if(verbose>0) cout<<"############## beam\n#############"<<endl;continue;}
      if(verbose>1) std::cout<<" beam: "<<*recocand<<std::endl;
      tmpMom=recocand->P4();
      mass=tmpMom.M(); momentum=tmpMom.P(); theta=tmpMom.Theta();
      hBeamMass->Fill(mass);
    }
    if(verbose>0) cout << " ----- event done -----"<<endl;
  } // end eventloop
  
  // Save all histograms to a file
  TFile* histofile = new TFile(histoFile.Data(),"RECREATE");
  histofile->cd();
  
  TListIter iter(histolist);
  TH1* tmph=NULL;
  while( tmph=(TH1*)iter() ) tmph->Write();
  histofile->Close();
  
  if(canvasplot){
    plothistos(histoFile);
  }
  
  if(canvasdraw){
    TCanvas* can = new TCanvas();
    //can->Divide(2,2);
    //can->cd(1);
    gPad->SetLogy();
    hVtxR->Draw();
    hVtxR->Fit("exp");
    //can->cd(2);
    //can->cd(3);
    //can->cd(4);
  }
  
  // now take the time...
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  return 0;
}


TString InitRun(TString filename)
{
  FairRunAna* fRun = new FairRunAna();
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  
  filename.ReplaceAll("_sim.root",".root");
  PndFileNameCreator namecreator(filename.Data());
  TString simFile = namecreator.GetSimFileName();
  TString parFile = namecreator.GetParFileName();
  TString recoFile = namecreator.GetRecoFileName();
  TString tracksFile = namecreator.GetCustomFileName("pidtracks");
  TString histoFile = namecreator.GetCustomFileName("histos");
  TString evrdummy = namecreator.GetCustomFileName("evrdummy");
  cout<<"simFile="<<simFile.Data()<<endl;
  cout<<"parFile="<<parFile.Data()<<endl;
  cout<<"recoFile="<<recoFile.Data()<<endl;
  cout<<"trkFile="<<tracksFile.Data()<<endl;
  fRun->SetInputFile(simFile);
  fRun->AddFriend(recoFile);//,"rec");
  fRun->AddFriend(tracksFile);//,"trk");
  FairParRootFileIo* parIO = new FairParRootFileIo();
  parIO->open(parFile.Data());
  rtdb->setFirstInput(parIO);
  rtdb->setOutput(parIO);  
  
  TString outFile = evrdummy;
  TString sysFile = gSystem->Getenv("VMCWORKDIR");  
  
  fRun->SetOutputFile(outFile.Data());
  FairGeane* geane = new FairGeane();
  fRun->AddTask(geane);
  fRun->Init();  
  return histoFile;
}



