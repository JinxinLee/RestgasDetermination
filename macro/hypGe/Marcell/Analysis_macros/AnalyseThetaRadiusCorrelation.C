// root macro to analyze the simulation output
//void convertMCPoints()

#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TTree.h"
#include <fstream>
#include <string>
#include "TVector3.h"

int AnalyseThetaRadiusCorrelation()
{

  // -----  Load libraries   ------------------------------------------------
//``gSystem->Load("fstream.h");
   gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  gSystem->Load("libHyp");
   gSystem->Load("libHypGe");
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
	TString CompleteFilename = "/data/work/kpha1/steinen/CrystalsOnly/TripleBall40Offset20STTCrystalsOnly_0.01MeV_10000000Evts.root";
	TFile* g = new TFile(CompleteFilename);
	
//Output Files
	TString Path = getenv("SIMDATADIR");
	TString outfile= Path+"/CrystalsOnly/Ana/AnaTripleBall40Offset20STTCrystalsOnly_0.01MeV_10000000Evts";
	
	TString txtfileName =  outfile;
	outfile +=".root";
	txtfileName += ".txt";
	TFile* fi = new TFile(outfile,"RECREATE");

	ofstream txtfile;
	txtfile.open(txtfileName);
	txtfile << "File read:" << CompleteFilename << endl;
   //photons from hyp electromag. decay
  TTree *b=(TTree *) g->Get("cbmsim") ;
  TClonesArray* hit_bar=new TClonesArray("PndHypGePoint");
  b->SetBranchAddress("HypGePoint",&hit_bar);//Branch names
  TClonesArray* mc_bar=new TClonesArray("PndMCTrack");
  b->SetBranchAddress("MCTrack",&mc_bar);//Branch names


	TString Name = "#Theta detector distance correlation;#Theta [#circ];Distance [cm]";
	TH2D* hThetaR = new TH2D("hThetaR",Name.Data(),180,90,180,250,15,40);
	
	TH2D* hRing1 = new TH2D("hRing1","#Theta detector ring 1 distance correlation;#Theta [#circ];Distance [cm]",180,90,180,250,15,40);
	TH2D* hRing2 = new TH2D("hRing2","#Theta detector ring 2 distance correlation;#Theta [#circ];Distance [cm]",180,90,180,250,15,40);
	TH2D* hRing3 = new TH2D("hRing3","#Theta detector ring 3 distance correlation;#Theta [#circ];Distance [cm]",180,90,180,250,15,40);
	TH2D* hRing4 = new TH2D("hRing4","#Theta detector ring 4 distance correlation;#Theta [#circ];Distance [cm]",180,90,180,250,15,40);
	TH2D* hRing5 = new TH2D("hRing5","#Theta detector ring 5 distance correlation;#Theta [#circ];Distance [cm]",180,90,180,250,15,40);
	TH2D* hRing6 = new TH2D("hRing6","#Theta detector ring 6 distance correlation;#Theta [#circ];Distance [cm]",180,90,180,250,15,40);
	TH2D* hRing7 = new TH2D("hRing7","#Theta detector ring 7 distance correlation;#Theta [#circ];Distance [cm]",180,90,180,250,15,40);
	TH2D* hRing8 = new TH2D("hRing8","#Theta detector ring 8 distance correlation;#Theta [#circ];Distance [cm]",180,90,180,250,15,40);
	
	TH1D* hRing1Theta = new TH1D("hRing1Theta","#Theta distribution of ring 1 ;#Theta [#circ];Counts",180,90,180);
	TH1D* hRing2Theta = new TH1D("hRing2Theta","#Theta distribution of ring 2 ;#Theta [#circ];Counts",180,90,180);
	TH1D* hRing3Theta = new TH1D("hRing3Theta","#Theta distribution of ring 3 ;#Theta [#circ];Counts",180,90,180);
	TH1D* hRing4Theta = new TH1D("hRing4Theta","#Theta distribution of ring 4 ;#Theta [#circ];Counts",180,90,180);
	TH1D* hRing5Theta = new TH1D("hRing5Theta","#Theta distribution of ring 5 ;#Theta [#circ];Counts",180,90,180);
	TH1D* hRing6Theta = new TH1D("hRing6Theta","#Theta distribution of ring 6 ;#Theta [#circ];Counts",180,90,180);
	TH1D* hRing7Theta = new TH1D("hRing7Theta","#Theta distribution of ring 7 ;#Theta [#circ];Counts",180,90,180);
	TH1D* hRing8Theta = new TH1D("hRing8Theta","#Theta distribution of ring 8 ;#Theta [#circ];Counts",180,90,180);
	
	TH1D* hRing1Distance = new TH1D("hRing1Distance","Crystal distance of ring 1 ;#Theta [#circ];Counts",250,15,40);
	TH1D* hRing2Distance = new TH1D("hRing2Distance","Crystal distance of ring 2 ;#Theta [#circ];Counts",250,15,40);
	TH1D* hRing3Distance = new TH1D("hRing3Distance","Crystal distance of ring 3 ;#Theta [#circ];Counts",250,15,40);
	TH1D* hRing4Distance = new TH1D("hRing4Distance","Crystal distance of ring 4 ;#Theta [#circ];Counts",250,15,40);
	TH1D* hRing5Distance = new TH1D("hRing5Distance","Crystal distance of ring 5 ;#Theta [#circ];Counts",250,15,40);
	TH1D* hRing6Distance = new TH1D("hRing6Distance","Crystal distance of ring 6 ;#Theta [#circ];Counts",250,15,40);
	TH1D* hRing7Distance = new TH1D("hRing7Distance","Crystal distance of ring 7 ;#Theta [#circ];Counts",250,15,40);
	TH1D* hRing8Distance = new TH1D("hRing8Distance","Crystal distance of ring 8 ;#Theta [#circ];Counts",250,15,40);
	
	bool verbose = false;
	Int_t MotherId,Motherpdg;
	
	
	TVector3 vecs , pos;
	int mcpdg = -1,ev;
	Double_t mult,En,Eng,Enth;
	
	//vector<int> event;
	int count;
	set<int> SetOfCrystalHit;
	set<int>::iterator it;
	
	Int_t nEvents = b->GetEntriesFast()/100;
	cout<< "Number of Simulated Events: "<<nEvents<<endl;
	txtfile<< "Number of Simulated Events: "<<nEvents<<endl;
	TVector3 Hit;
	
	for (Int_t k=0; k<nEvents; k++)
	{ 
		b->GetEntry(k);
		if (!((k*1000)% nEvents))
		{
			cout << k << endl;
		}
	    
		for (Int_t i=0; i<hit_bar->GetEntriesFast(); i++)
		{ 
			PndHypGePoint *hitgam=(PndHypGePoint*)hit_bar->At(i);
			PndMCTrack *mcgam = (PndMCTrack*)mc_bar->At(hitgam->GetTrackID());
			
			Hit.SetX(hitgam->GetX());
			Hit.SetY(hitgam->GetY());
			Hit.SetZ(hitgam->GetZ()+55);					
			Double_t theta = 180/TMath::Pi()*Hit.Theta();
			//cout << 180/TMath::Pi()*Hit.Theta()<< ", " << Hit.Mag() << endl;
			hThetaR->Fill(theta, Hit.Mag());
			
			switch (hitgam->GetDetectorID())
					{
						case 101:
							//iRing6+=Content;
							hRing6->Fill(theta, Hit.Mag()); hRing6Theta->Fill(theta); hRing6Distance->Fill(Hit.Mag());
							break;
						case 102:
							//iRing7+=Content;
							hRing7->Fill(theta, Hit.Mag()); hRing7Theta->Fill(theta); hRing7Distance->Fill(Hit.Mag());
							break;
						case 103:
							//iRing6+=Content;
							hRing6->Fill(theta, Hit.Mag()); hRing6Theta->Fill(theta); hRing6Distance->Fill(Hit.Mag());
							break;
						case 204:
							//iRing7+=Content;
							hRing7->Fill(theta, Hit.Mag()); hRing7Theta->Fill(theta); hRing7Distance->Fill(Hit.Mag());
							break;
						case 205:
							//iRing6+=Content;
							hRing6->Fill(theta, Hit.Mag()); hRing6Theta->Fill(theta); hRing6Distance->Fill(Hit.Mag());
							break;
						case 206:
							//iRing6+=Content;
							hRing6->Fill(theta, Hit.Mag()); hRing6Theta->Fill(theta); hRing6Distance->Fill(Hit.Mag());
							break;
						case 307:
							//iRing6+=Content;
							hRing6->Fill(theta, Hit.Mag()); hRing6Theta->Fill(theta); hRing6Distance->Fill(Hit.Mag());
							break;
						case 308:
							//iRing6+=Content;
							hRing6->Fill(theta, Hit.Mag()); hRing6Theta->Fill(theta); hRing6Distance->Fill(Hit.Mag());
							break;
						case 309:
							//iRing7+=Content;
							hRing7->Fill(theta, Hit.Mag()); hRing7Theta->Fill(theta); hRing7Distance->Fill(Hit.Mag());
							break;
						case 410:
							//iRing4+=Content;
							hRing4->Fill(theta, Hit.Mag()); hRing4Theta->Fill(theta); hRing4Distance->Fill(Hit.Mag());
							break;
						case 411:
							//iRing1+=Content;
							hRing1->Fill(theta, Hit.Mag()); hRing1Theta->Fill(theta); hRing1Distance->Fill(Hit.Mag()); 
							break;
						case 412:
							//iRing5+=Content;
							hRing5->Fill(theta, Hit.Mag()); hRing5Theta->Fill(theta); hRing5Distance->Fill(Hit.Mag());
							break;
						case 513:
							//iRing1+=Content;
							hRing1->Fill(theta, Hit.Mag()); hRing1Theta->Fill(theta); hRing1Distance->Fill(Hit.Mag());
							break;
						case 514:
							//iRing5+=Content;
							hRing5->Fill(theta, Hit.Mag()); hRing5Theta->Fill(theta); hRing5Distance->Fill(Hit.Mag());
							break;
						case 515:
							//iRing3+=Content;
							hRing3->Fill(theta, Hit.Mag()); hRing3Theta->Fill(theta); hRing3Distance->Fill(Hit.Mag());
							break;
						case 616:
							//iRing5+=Content;
							hRing5->Fill(theta, Hit.Mag()); hRing5Theta->Fill(theta); hRing5Distance->Fill(Hit.Mag());
							break;
						case 617:
							//iRing2+=Content;
							hRing2->Fill(theta, Hit.Mag()); hRing2Theta->Fill(theta); hRing2Distance->Fill(Hit.Mag());
							break;
						case 618:
							//iRing2+=Content;
							hRing2->Fill(theta, Hit.Mag()); hRing2Theta->Fill(theta); hRing2Distance->Fill(Hit.Mag());
							break;
						case 719:
							//iRing1+=Content;
							hRing1->Fill(theta, Hit.Mag()); hRing1Theta->Fill(theta); hRing1Distance->Fill(Hit.Mag());
							break;
						case 720:
							//iRing3+=Content;
							hRing3->Fill(theta, Hit.Mag()); hRing3Theta->Fill(theta); hRing3Distance->Fill(Hit.Mag());
							break;
						case 721:
							//iRing5+=Content;
							hRing5->Fill(theta, Hit.Mag()); hRing5Theta->Fill(theta); hRing5Distance->Fill(Hit.Mag());
							break;
						case 822:
							//iRing4+=Content;
							hRing4->Fill(theta, Hit.Mag()); hRing4Theta->Fill(theta); hRing4Distance->Fill(Hit.Mag());
							break;
						case 823:
							//iRing5+=Content;
							hRing5->Fill(theta, Hit.Mag()); hRing5Theta->Fill(theta); hRing5Distance->Fill(Hit.Mag());
							break;
						case 824:
							//iRing1+=Content;
							hRing1->Fill(theta, Hit.Mag()); hRing1Theta->Fill(theta); hRing1Distance->Fill(Hit.Mag());
							break;
						case 925:
							//iRing6+=Content;
							hRing6->Fill(theta, Hit.Mag()); hRing6Theta->Fill(theta); hRing6Distance->Fill(Hit.Mag());
							break;
						case 926:
							//iRing6+=Content;
							hRing6->Fill(theta, Hit.Mag()); hRing6Theta->Fill(theta); hRing6Distance->Fill(Hit.Mag());
							break;
						case 927:
							//iRing7+=Content;
							hRing7->Fill(theta, Hit.Mag()); hRing7Theta->Fill(theta); hRing7Distance->Fill(Hit.Mag());
							break;
						case 1028:
							//iRing7+=Content;
							hRing7->Fill(theta, Hit.Mag()); hRing7Theta->Fill(theta); hRing7Distance->Fill(Hit.Mag()); 
							break;
						case 1029:
							//iRing6+=Content;
							hRing6->Fill(theta, Hit.Mag()); hRing6Theta->Fill(theta); hRing6Distance->Fill(Hit.Mag());
							break;
						case 1030:
							//iRing6+=Content;
							hRing6->Fill(theta, Hit.Mag()); hRing6Theta->Fill(theta); hRing6Distance->Fill(Hit.Mag());
							break;
						case 1131:
							//iRing6+=Content;
							hRing6->Fill(theta, Hit.Mag()); hRing6Theta->Fill(theta); hRing6Distance->Fill(Hit.Mag());
							break;
						case 1132:
							//iRing7+=Content;
							hRing7->Fill(theta, Hit.Mag()); hRing7Theta->Fill(theta); hRing7Distance->Fill(Hit.Mag());
							break;
						case 1133:
							//iRing6+=Content;
							hRing6->Fill(theta, Hit.Mag()); hRing6Theta->Fill(theta); hRing6Distance->Fill(Hit.Mag());
							break;
						case 1234:
							//iRing4+=Content;
							hRing4->Fill(theta, Hit.Mag()); hRing4Theta->Fill(theta); hRing4Distance->Fill(Hit.Mag());
							break;
						case 1235:
							//iRing5+=Content;
							hRing5->Fill(theta, Hit.Mag()); hRing5Theta->Fill(theta); hRing5Distance->Fill(Hit.Mag());
							break;
						case 1236:
							//iRing1+=Content;
							hRing1->Fill(theta, Hit.Mag()); hRing1Theta->Fill(theta); hRing1Distance->Fill(Hit.Mag());
							break;
						case 1337:
							//iRing1+=Content;
							hRing1->Fill(theta, Hit.Mag()); hRing1Theta->Fill(theta); hRing1Distance->Fill(Hit.Mag());
							break;
						case 1338:
							//iRing3+=Content;
							hRing3->Fill(theta, Hit.Mag()); hRing3Theta->Fill(theta); hRing3Distance->Fill(Hit.Mag());
							break;
						case 1339:
							//iRing5+=Content;
							hRing5->Fill(theta, Hit.Mag()); hRing5Theta->Fill(theta); hRing5Distance->Fill(Hit.Mag());
							break;
						case 1440:
							//iRing5+=Content;
							hRing5->Fill(theta, Hit.Mag()); hRing5Theta->Fill(theta); hRing5Distance->Fill(Hit.Mag());
							break;
						case 1441:
							//iRing2+=Content;
							hRing2->Fill(theta, Hit.Mag()); hRing2Theta->Fill(theta); hRing2Distance->Fill(Hit.Mag());
							break;
						case 1442:
							//iRing2+=Content;
							hRing2->Fill(theta, Hit.Mag()); hRing2Theta->Fill(theta); hRing2Distance->Fill(Hit.Mag());
							break;
						case 1543:
							//iRing1+=Content;
							hRing1->Fill(theta, Hit.Mag()); hRing1Theta->Fill(theta); hRing1Distance->Fill(Hit.Mag());
							break;
						case 1544:
							//iRing5+=Content;
							hRing5->Fill(theta, Hit.Mag()); hRing5Theta->Fill(theta); hRing5Distance->Fill(Hit.Mag());
							break;
						case 1545:
							//iRing3+=Content;
							hRing3->Fill(theta, Hit.Mag()); hRing3Theta->Fill(theta); hRing3Distance->Fill(Hit.Mag());
							break;
						case 1646:
							//iRing4+=Content;
							hRing4->Fill(theta, Hit.Mag()); hRing4Theta->Fill(theta); hRing4Distance->Fill(Hit.Mag());
							break;
						case 1647:
							//iRing1+=Content;
							hRing1->Fill(theta, Hit.Mag()); hRing1Theta->Fill(theta); hRing1Distance->Fill(Hit.Mag());
							break;
						case 1648:
							//iRing5+=Content;
							hRing5->Fill(theta, Hit.Mag()); hRing5Theta->Fill(theta); hRing5Distance->Fill(Hit.Mag());
							break;
						default: 
							break;
					}
			
		}//end for i (points in event)
		
	}// end for j (events)
  cout << "event loop finished"<< endl;

  
	//TCanvas* can3 = new TCanvas("can3","germanium detector",0,0,1000,1000);
	//can3->cd();
 
  hThetaR->Draw("");
  
  hThetaR->Write();
  
  hRing1->Write();
  hRing2->Write();
  hRing3->Write();
  hRing4->Write();
  hRing5->Write();
  hRing6->Write();
  hRing7->Write();
  hRing8->Write();

	hRing1Theta->Write();
	hRing2Theta->Write();
	hRing3Theta->Write();
	hRing4Theta->Write();
	hRing5Theta->Write();
	hRing6Theta->Write();
	hRing7Theta->Write();
	hRing8Theta->Write();
	
	hRing1Distance->Write();
	hRing2Distance->Write();
	hRing3Distance->Write();
	hRing4Distance->Write();
	hRing5Distance->Write();
	hRing6Distance->Write();
	hRing7Distance->Write();
	hRing8Distance->Write();

	fi->Close();
	txtfile.close();



	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is "    << outfile << endl;
	cout << "Parameter file is " << txtfileName << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
	
  return 0;
}
