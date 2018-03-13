#include "TH1D.h"
#include "TH2D.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TStopwatch.h"
#include "TVector3.h"
#include "TROOT.h"
#include "TMath.h"
#include "TClonesArray.h"
#include "TSystem.h"
#include "TStyle.h"
//#include "/home/steinen/work/pandaroot/hypGe/PndHypGePoint.h"

#include <stdio.h>
#include <iostream>


using namespace std;
int NeutronAnalysis_COSY_CrossSec(TString Filename_ext, Int_t StartEvent=0, Int_t NoOfEvents=0)
{
	bool verbose = false;
  // -----  Load libraries   ------------------------------------------------
   gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
   gSystem->Load("libHypGe");
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


	//Files
							//TString Path = getenv("SIMDATADIR");
	TString Filename = "$SIMDATADIR/COSY/"+Filename_ext;
	//TString Filename = "$SIMDATADIR/TripleBall40Offset10_urqmd_pbarC_1_5000Evts.root";
	TString outfile= "$SIMDATADIR/COSY/Ana/ToCombine/" + Filename_ext;
	outfile += "/Ana";
	outfile += Filename_ext;
	outfile.ReplaceAll(".root","");
	outfile += "_Start_";
	outfile += StartEvent;
	outfile += "_nEventssssssssssss_";
	outfile += NoOfEvents;
	outfile += ".root";
	cout << outfile << endl;
	TFile* InputFile = new TFile(Filename);
	//TFile* OutputFile = new TFile(outfile,"RECREATE");

	//getting simulation branches from input file
  TTree *b=(TTree *) InputFile->Get("pndsim") ;
  TClonesArray* hit_bar=new TClonesArray("PndHypGePoint");
  b->SetBranchAddress("HypGePoint",&hit_bar);//Branch names
  TClonesArray* mc_bar=new TClonesArray("PndMCTrack");
  b->SetBranchAddress("MCTrack",&mc_bar);//Branch names

	//declaring histograms
		//histogram with polar angle of detector interactions of primary neutrons
	TH1D* hNHits = new TH1D("hNHits","Polar angle of primary neutrons interacting with the crystals",180, 90,180);
	hNHits->SetXTitle("#Theta [#circ]");
	hNHits->SetYTitle("Counts / 0.5 #circ");
		//histogram with the polar momentum of primary neutrons interacting with the crystals
	TH1D* hNHits_p = new TH1D("hNHits_p","Direction of polar momentum of primary neutrons interacting with the crystals",180, 90,180);
	hNHits_p->SetXTitle("#Theta [#circ]");
	hNHits_p->SetYTitle("Counts / 0.5 #circ");
		//histogram with the polar momentum of primary neutrons in the solid angle of the crystal (not necessarily interacting with the crystal)
	TH1D* hNAll = new TH1D("hNAll","All Neutrons",360, 0,180);
	hNAll->SetXTitle("#Theta [#circ]");
	hNAll->SetYTitle("Counts / 0.5 #circ");

		//histogram with the polar momentum of primary neutrons in the solid angle of the crystal (not necessarily interacting with the crystal)
	TH1D* hPAll = new TH1D("hPAll","All Protons",360, 0,180);
	hPAll->SetXTitle("#Theta [#circ]");
	hPAll->SetYTitle("Counts / 0.5 #circ");

	//histogram with E_kin of primary neutrons
	TH1D* hNMom = new TH1D("hNMom","E_{kin} of neutrons",1000, 0,0.5);
	hNMom->SetXTitle("E_{kin} of neutrons [GeV]");
	hNMom->SetYTitle("Counts / 0.5 MeV");
		//histogram to see which detector is hit
	TH1D* hCrystalHit = new TH1D("hCrystalHit","Hits per Crystal",1700,1,1700);
	hCrystalHit->SetXTitle("Crystal number");
	hCrystalHit->SetYTitle("Counts per crystal");
 		//histogram to see the energy deposited by the neutron
	TH1D* hNeutronEnergyDeposit = new TH1D("hNeutronEnergyDeposit","Energy deposited by Neutrons per Hit",500,0,0.005);
	hNeutronEnergyDeposit->SetXTitle("Energy [GeV]");
	hNeutronEnergyDeposit->SetYTitle("Counts / 10 keV");

		//histogram to visualize the x-y-distribution of neutrons
	TH2D* hNeutronXYDistribution = new TH2D("hNeutronXYDistribution","Distribution of Neutrons",40, -40,40,40,-40,40);
	hNeutronXYDistribution->SetXTitle("X-postion [cm]");
	hNeutronXYDistribution->SetYTitle("Y-postion [cm]");
	gStyle->SetPalette(1);



	Int_t *ActualTrackID ;
	//Int_t nEvents = b->GetEntriesFast();
	//cout<< "Number of Simulated Events: "<<nEvents<<endl;

	Int_t Hits=0;
	Int_t DetID=-10;
	Int_t TrackID=-10;
	Int_t iNeutrons = 0;
	Int_t NuclearInteractionCounter=0;



	TVector3 NHit;
	TVector3 NMom;
	TVector3 NAllMom;
	TVector3 StartVertex;


	Int_t EndEvent;
	if (NoOfEvents ==0)
	{
		EndEvent = b->GetEntriesFast();
		NoOfEvents = EndEvent - StartEvent;
	}
	else
		EndEvent = StartEvent + NoOfEvents;

		//event loop
	for (Int_t k=StartEvent; k< EndEvent; k++)
	//for (Int_t k=569494; k<569498; k++)
	{
		b->GetEntry(k);
		if (!((k*100)% NoOfEvents))	// mark every % of proceeded events
		{
			cout << "Event number :\t" << k << endl;
		}
	    //if(verbose) cout<<"Event No "<<j<<endl;

	  DetID=-10;
	  TrackID=-10;

		// loop over the hits of an event
		//for (Int_t i=0; i<hit_bar->GetEntriesFast(); i++)
		//{
														////if (i==0)
															////Hits++;
														////cout <<"Hit "<< i << endl;

			//PndHypGePoint *hitgam=(PndHypGePoint*)hit_bar->At(i);
			//PndMCTrack *mcgam = (PndMCTrack*)mc_bar->At(hitgam->GetTrackID());

			////NMom = mcgam->GetMomentum();
			//if (hitgam->GetpdgCode() == 2112)
			//{
				//StartVertex = mcgam->GetStartVertex();
				//cout <<"Event " <<k<< " \t\tStartVertex Radius: " <<StartVertex.Mag()<< endl;

				//if (StartVertex.Mag() <20)	// prevent counting of neutrons created by nuclear reactions inside the detector	20 (cm) is random value, must be smaller than the distance from (0,0,0) to detector (here 30 cm)
				//{
					//if (!(DetID == hitgam->GetDetectorID() && TrackID==hitgam->GetTrackID())) // used to prevent counting of neutrons that reenter a crystal (neutrons left crystal through the central hole skewly and reentered the same crystal afterwards) and of neutrons that enter multiple crystals by scattering
					//{
						//DetID = hitgam->GetDetectorID();
						//TrackID = hitgam->GetTrackID();
						////cout << "Event :\t" << k <<"\tHit : \t" << i << "\tDetector :\t" << DetID << "\tTrackID :\t"<< TrackID<<"\tMother :\t"<< mcgam->GetMotherID()<<endl;
						//iNeutrons++;
						//hCrystalHit->Fill(hitgam->GetDetectorID());				//fill histogram to see which detector is hit


						//// fill vector with hit coordinates to get theta of the hit
						//NHit.SetX(hitgam->GetX());
						//NHit.SetY(hitgam->GetY());
						//NHit.SetZ(hitgam->GetZ());					// no shift like in PANDA!!!!
						//hNHits->Fill(180/TMath::Pi()*NHit.Theta());				//fill histogram with polar angle of detector interactions of primary neutrons
						//cout << "Event " << k <<" Theta " <<180/TMath::Pi()*NHit.Theta()<<endl;

						//TVector3 NHit_p;
						//// fill vector with hit momentum to get theta of the hit momentum
						//NHit_p.SetX(hitgam->GetPx());
						//NHit_p.SetY(hitgam->GetPy());
						//NHit_p.SetZ(hitgam->GetPz());
						//hNHits_p->Fill(180/TMath::Pi()*NHit_p.Theta());		//fill histogram with the polar momentum of primary neutrons interacting with the crystals

						//hNeutronXYDistribution->Fill(hitgam->GetX(),hitgam->GetY());
						////hNMom->Fill(1./2.* NMom.Mag2()/0.939);				//fill histogram with E_kin of primary neutrons
						//hNeutronEnergyDeposit->Fill(hitgam->GetEnergyLoss());

					//}
				//}
			//}
		//}

		// loop over all entries in mctrack
		//cout << "mcbar length :\t" <<mc_bar->GetEntriesFast() << endl;
		int verboseMC = 0;
		if (verboseMC) cout << "Event " << k << "\tNumber of particles: " <<mc_bar->GetEntriesFast() ;
		if(mc_bar->GetEntriesFast()>2)
		{
			//NuclearInteractionCounter++;
			for (Int_t m = 0; m<mc_bar->GetEntriesFast();m++)
			{
				//cout << "mcbar length :\t" <<mc_bar->GetEntriesFast() << endl;
				PndMCTrack *mcgam = (PndMCTrack*)mc_bar->At(m);

				if (mcgam->GetPdgCode() != 2212 && mcgam->GetPdgCode() != 11 && mcgam->GetPdgCode() !=-2212)
				{
					if (verboseMC)cout << "\t Found Particle with PDG code: " << mcgam->GetPdgCode() << endl;
					NuclearInteractionCounter++;
					break;
				}
				else
					if ( m == mc_bar->GetEntriesFast()-1)
						if (verboseMC)cout << endl;
			}
		}
		else
			if (verboseMC)cout << endl;


	}// end for k (events)


	//hNeutronXYDistribution->DrawCopy("colz");
	////hNAll->DrawCopy();
  ////hNHits->SetLineColor(kRed);
  ////hNHits->DrawCopy("same");

  ////Analysis of spectrum
  //hNHits->Write();
  //hNHits_p->Write();
	//hNAll->Write();
	//hPAll->Write();
	//hNMom->Write();
	//hCrystalHit->Write();
	//hNeutronEnergyDeposit->Write();
	//hNeutronXYDistribution->Write();



	//hNeutronEnergyDeposit->DrawCopy();

	//OutputFile->Close();

	//hNeutronEnergyDeposit->Draw();
  //hNHits->Draw();

	cout << "Number of nuclear interactions: " <<NuclearInteractionCounter << endl;
	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;

	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;

	cout << "Hits\t" << Hits << endl;
	// ------------------------------------------------------------------------
	return iNeutrons;
}
