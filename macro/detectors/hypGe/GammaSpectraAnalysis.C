// root macro to analyze the simulation output
//void convertMCPoints()
{

  // -----  Load libraries   ------------------------------------------------
//``gSystem->Load("fstream.h");
   gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");


   gSystem->Load("libHypGe");

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
#include <vector>



  //photons from hyp electromag. decay

  TFile* g = new TFile("sim_hypGe_BgUrqmd2.root");
  // the sim file you want to analyse
  TTree *b=(TTree *) g->Get("pndsim") ;
  TClonesArray* hit_bar=new TClonesArray("PndHypGePoint");
  b->SetBranchAddress("HypGePoint",&hit_bar);//Branch names
  TClonesArray* mc_bar=new TClonesArray("PndMCTrack");
  b->SetBranchAddress("MCTrack",&mc_bar);//Branch names


  //**** Energy deposited from particle backgroun


  //****photons from hyp elect. decay
      TH1D* gamTde = new TH1D("gamTde","total gam energy deposit  ",500,0.006,0.0012);


	bool verbose = false;
	Int_t MotherId,Motherpdg;



	TVector3 vecs,pos;
	int mcpdg = -1,ev;
	Double_t mult,En,Eng,Enth;

	//vector<int> event;
	int count;
	cout<<b->GetEntriesFast()<<endl;


	for (Int_t k=0; k<b->GetEntriesFast(); k++)
	  {
	    Eng=0.;
	    b->GetEntry(k);
	    //if(verbose) cout<<"Event No "<<j<<endl;
	    for (Int_t i=0; i<hit_bar->GetEntriesFast(); i++)
	      {
		PndHypGePoint *hitgam=(PndHypGePoint*)hit_bar->At(i);

		PndMCTrack *mcgam = (PndMCTrack*)mc_bar->At(hitgam->GetTrackID());

		Double_t rande;
		//rande= gRandom->Gaus(hitgam->GetEnergyLoss(),0.000003);
		//if (sci->Getpdgcode())



		//if (sci)cout<<sci->GetEnergyLoss()<<endl;
		//En +=gRandom->Gaus(0,0.000003);
		//randy= gRandom->Gaus(0,1);




		Eng =Eng + (hitgam->GetEnergyLoss());
		//Eng =Eng + (rande);

	      }//end for i (points in event)
	    //count =0;
	    if(Eng>0)gamTde->Fill(Eng);



	  }// end for j (events)



  TCanvas* can3 = new TCanvas("can3","germanium detector",0,0,1000,1000);

  gamTde->Draw();





	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	//cout << "Output file is "    << outFile << endl;
	//cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------

}
