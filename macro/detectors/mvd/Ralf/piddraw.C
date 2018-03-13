{
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  gStyle->SetHistFillColor(9);
  gStyle->SetCanvasColor(0);
  gStyle->SetLabelSize(0.025,"X");
  gStyle->SetLabelSize(0.025,"Y");
  gStyle->SetTitleSize(0.03,"Y");
  gStyle->SetBarOffset(10);
  gStyle->SetTitleOffset(1,"X");
  gStyle->SetTitleOffset(1.55,"Y");
  gStyle->SetTitleSize(0.03,"X");
  gStyle->SetPalette(1,0);
  gStyle->SetOptFit(1111);

 	//opening tree
  TFile* inFile = new TFile("../data/MvdMC_Pid_10k.root","READ");
  TTree* tree = (TTree *)inFile->Get("pndsim");
  TClonesArray* pointlist=new TClonesArray("PndSdsMCPoint");
  tree->SetBranchAddress("MVDPoint",&pointlist);

  //Creating canvas and diagramms
  TH2D * hist10 = new TH2D("specEnergyLossVsP","dE/dx(p)",200,0.0,1,200,0,0.0005);
  hist10->SetYTitle("(dE/dx)/(GeV/cm)");
  hist10->SetXTitle("p/(GeV/c)");

 	//variable
  TVector3 vecFront,vecBack,vecP;
  Int_t nEvents = 5000; //1000;
  Double_t dx,dE,p,dEdX;


  for(int j=0;j<nEvents && j<tree->GetEntriesFast();j++)
	{
		tree->GetEntry(j);
		for(int i=0;i<pointlist->GetEntriesFast();i++)
		{
		  PndSdsMCPoint* point=(PndSdsMCPoint*)pointlist->At(i);

		  vecFront.SetXYZ(point->GetX(),point->GetY(),point->GetZ());
		  vecBack.SetXYZ(point->GetXOut(),point->GetYOut(),point->GetZOut());
		  vecP.SetXYZ(point->GetPxOut(),point->GetPyOut(),point->GetPzOut());

    	dx=(vecBack-vecFront).Mag();
      dE=point->GetEnergyLoss();
      p=vecP.Mag();

      if(dx!=0)
      {
        dEdX=(dE/dx);
        hist10->Fill(p,dE);
      }
    }
  }

  TCanvas* can1 = new TCanvas("test","Energy in MVD",20,20,800,600);

  hist10->DrawCopy("col");
 // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
}
