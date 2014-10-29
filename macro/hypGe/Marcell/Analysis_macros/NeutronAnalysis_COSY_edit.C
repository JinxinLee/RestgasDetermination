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
int NeutronAnalysis_COSY_edit(TString Filename_ext = "hypGeCOSYBeamTime2014_protons_mom2.7799999999999998GeV_1000000Evts_GausianSmearedBeam_9.root", Int_t StartEvent=0, Int_t NoOfEvents=100000)
{
	bool verbose = false;
  // -----  Load libraries   ------------------------------------------------
   gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
   gSystem->Load("libHypGe");
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
	
	TGeoManager *fgeom;
	//Files
							//TString Path = getenv("SIMDATADIR");
	TString Filename = "$SIMDATADIR/COSY/"+Filename_ext;
	TString ParfileName = "$SIMDATADIR/COSY/"+Filename_ext;
	ParfileName.ReplaceAll(".root","__Simparams.root");
	//TString Filename = "$SIMDATADIR/TripleBall40Offset10_urqmd_pbarC_1_5000Evts.root";
	TString outfile= "$SIMDATADIR/COSY/Ana/ToCombine/" + Filename_ext;
	outfile += "/Ana";
	outfile += Filename_ext;
	outfile.ReplaceAll(".root","");
	outfile += "_Start_";
	outfile += StartEvent;
	outfile += "_nEvents_";
	outfile += NoOfEvents;
	outfile += ".root";
	cout << outfile << endl;
	TFile* InputFile = new TFile(Filename);
	TFile* Parfile = new TFile(ParfileName);
		Parfile->Get("FairBaseParSet"); 
	TFile* OutputFile = new TFile(outfile,"RECREATE");
	
	//getting simulation branches from input file
  TTree *b=(TTree *) InputFile->Get("cbmsim") ;
  TClonesArray* fHypGe=new TClonesArray("PndHypGePoint");
  b->SetBranchAddress("HypGePoint",&fHypGe);//Branch names
  TClonesArray* fMcTr=new TClonesArray("PndMCTrack");
  b->SetBranchAddress("MCTrack",&fMcTr);//Branch names


			//declaring variables from PndHypGeCOSYBackgroundAna.h (macro updated for 2014 beam time changes, file not yet, 20.08.14 steinen)
	TGeoManager *fgeom;
	TH1D*											hNHitsAngleGe;		// angular distribution of neutrons in the germanium
	TH1D* 										hNeutronOriginGe;	// neutrons and their origin in the germanium
	TH1D* 										hNHitsGeKermitBall;	// neutrons in ge, active and passive neutron detector
	
	TH1D*											hAllParticlesGe;		// all particles hitting the germanium
	TH2D*											hAllParticlesEkinGe;	// 2D all E_kin distribution of particles hitting the germanium
	TH1D*											hAllParticlesPiezo;		// all particles hitting the first pieo
	TH1D*											hAllParticlesPiezo2;	// all particles hitting the germanium
	TH1D*											hNEkinGe;							// E_kin of neutrons entering the germanium
	TH1D*											hNEkinKermit;							// E_kin of neutrons entering the active neutron detector
	TH1D*											hNEkinBall;							// E_kin of neutrons entering the passive neutron detector
	TH1D*											hNEnergyLossGe;				// Energy loss of neutrons in the germanium
	TH1D*											hNEnergyLossKermit;				// Energy loss of neutrons in the active neutron detector
	TH1D*											hNEnergyLossBall;				// Energy loss of neutrons in the passive neutron detector
	TH1D*											hPEkinGe;							// E_kin of protons entering the germanium
	TH1D*											hPEnergyLossGe;				// Energy loss of protons in the germanium
	TH1D*											hGammaSpecGe;							// spectrum of gammas in the germanium

	
	TCanvas*									cNHitsGe;									//canvas for hNHitsAngleGe and hNeutronOriginGe
	TCanvas*									cNHitsGeKermitBall;				//canvas for hNHitsGeKermitBall
	TCanvas*									cAllParticlesGe;					//canvas for hAllParticlesGe and hAllParticlesEkinGe
	TCanvas*									cAllParticlesPiezo;				//canvas for hAllParticlesPiezo and hAllParticlesPiezo2
	TCanvas*									cNeutronEkinGe;						//canvas for hNEkinGe and hNEnergyLossGe
	TCanvas*									cNeutronEkinKermit;				//canvas for hNEkinKermit and hNEnergyLossKermit
	TCanvas*									cNeutronEkinBall;					//canvas for hNEkinBall and hNEnergyLossBall
	TCanvas*									cProtonEkinGe;						//canvas for hPEkinGe and hNEnergyLossGe
	TCanvas*									cGammaEnergyGe;						//canvas for hGammaSpecGe
	

	TString										VertexVolumeName;
	TVector3									NeutronMomentum;
	Double_t									NeutronEkin;
	TVector3									ProtonMomentum;
	Double_t									ProtonEkin;
	TVector3									ParticleMomentum;
	Double_t									ParticleEkin;

	//----------Get GeoManager---------------------------------------------	
	fgeom = (TGeoManager*)gROOT->FindObject("FAIRGeom");
	//----------create histograms------------------------------------------
	
			//histogram with polar angle of detector interactions of primary neutrons
	hNHitsAngleGe = new TH1D("hNHitsAngleGe","Polar angle of primary neutrons interacting with the crystals",180, 90,180);
		hNHitsAngleGe->SetXTitle("#Theta [#circ]");
		hNHitsAngleGe->SetYTitle("Counts / 0.5 #circ");
	
			//histogram to see where neutron is coming from
	hNeutronOriginGe = new TH1D("hNeutronOriginGe","Origin of Neutrons",13,0,13);
		hNeutronOriginGe->SetXTitle("Neutron Origin");
		hNeutronOriginGe->SetYTitle("Counts");
		hNeutronOriginGe->GetXaxis()->SetBinLabel(1,"Target");										// neutrons created in the carbon target
		hNeutronOriginGe->GetXaxis()->SetBinLabel(2,"Target Support");						// neutrons created in the goniometer, the brackets
		hNeutronOriginGe->GetXaxis()->SetBinLabel(3,"Al base plate");									// neutrons created in the al support plate
		hNeutronOriginGe->GetXaxis()->SetBinLabel(4,"Plastic blocks");						// neutrons created in the plastic blocks under the al plate
		hNeutronOriginGe->GetXaxis()->SetBinLabel(5,"Maytec frame");							// neutrons created in the maytec frame supporting the act. neutron det.
		hNeutronOriginGe->GetXaxis()->SetBinLabel(6,"Active neutron detector");		// neutrons created in the active neutron detector
		hNeutronOriginGe->GetXaxis()->SetBinLabel(7,"Passive neutron detector");	// neutrons created in the passive neutron detector
		hNeutronOriginGe->GetXaxis()->SetBinLabel(8,"Crystal");										// neutrons created in the germanium crystal
		hNeutronOriginGe->GetXaxis()->SetBinLabel(9,"Cryostat");										// neutrons created in the germanium detector cryostat
		hNeutronOriginGe->GetXaxis()->SetBinLabel(10,"Lumi glue test");						// neutrons created in the lumi glue test stuff
		hNeutronOriginGe->GetXaxis()->SetBinLabel(11,"Lumi electronics test");		// neutrons created in the lumi electronics test stuff
		hNeutronOriginGe->GetXaxis()->SetBinLabel(12,"Beam dump");								// neutrons created in the beam dump
		hNeutronOriginGe->GetXaxis()->SetBinLabel(13,"Cave");											// neutrons created in the experimental area cave
		
			//histogram to see which detector is hit
	hNHitsGeKermitBall = new TH1D("hNHitsGeKermitBall","Total number of neutron hits on the 3 detectors",3,0,1);
		hNHitsGeKermitBall->SetXTitle("Crystal number");
		hNHitsGeKermitBall->SetYTitle("Counts per crystal");
		hNHitsGeKermitBall->GetXaxis()->SetBinLabel(1,"Germanium crystal");				//Crystal_101
		hNHitsGeKermitBall->GetXaxis()->SetBinLabel(2,"Active neutron detector");				//Kermit
		hNHitsGeKermitBall->GetXaxis()->SetBinLabel(3,"Passive neutron detector");				//Ball

			//histogram with all particles in the germanium
	hAllParticlesGe = new TH1D("hAllParticlesGe","Particles interaction in the crystals;PDG Code of particle; Counts", 2e4,-1e4,1e4);

			// histogram for correlation of E_kin and particle PDG Code for the germanium
	hAllParticlesEkinGe = new TH2D("hAllParticlesEkinGe","E_{kin} - Particle - Correlation;E_{kin} of particles [MeV]; particle",3000,0,3000,9,1,10);
		hAllParticlesEkinGe->GetYaxis()->SetBinLabel(1,"#pi^{-}");
		hAllParticlesEkinGe->GetYaxis()->SetBinLabel(2,"#mu^{+}");
		hAllParticlesEkinGe->GetYaxis()->SetBinLabel(3,"e^{+}");
		hAllParticlesEkinGe->GetYaxis()->SetBinLabel(4,"e^{-}");
		hAllParticlesEkinGe->GetYaxis()->SetBinLabel(5,"#mu^{-}");
		hAllParticlesEkinGe->GetYaxis()->SetBinLabel(6,"#gamma");
		hAllParticlesEkinGe->GetYaxis()->SetBinLabel(7,"#pi^{+}");
		hAllParticlesEkinGe->GetYaxis()->SetBinLabel(8,"neutron");
		hAllParticlesEkinGe->GetYaxis()->SetBinLabel(9,"proton");

			//histogram with all particles in piezo created outside of the crystal
	hAllParticlesPiezo = new TH1D("hAllParticlesPiezo","Particles interaction in the first piezo;PDG Code of particle; Counts", 2e4,-1e4,1e4);
			//histogram with all particles in piezo created outside of the crystal
	hAllParticlesPiezo2 = new TH1D("hAllParticlesPiezo2","Particles interaction in the second piezo;PDG Code of particle; Counts", 2e4,-1e4,1e4);

		
			//histogram for kin. energy of neutrons in the germanium
	hNEkinGe = new TH1D("hNEkinGe","E_{kin} of neutrons in the germanium;E_{kin} of neutrons  [MeV]; Counts", 2000,0,200);
			//histogram for kin. energy of neutrons in the active neutron detector
	hNEkinKermit = new TH1D("hNEkinKermit","E_{kin} of neutrons in the active neutron detector;E_{kin} of neutrons [MeV]; Counts", 2000,0,200);
			//histogram for kin. energy of neutrons in the passive neutron detector
	hNEkinBall = new TH1D("hNEkinBall","E_{kin} of neutrons in the passive neutron detector;E_{kin} of neutrons [MeV]; Counts", 2000,0,200);

			//histogram with energy deposited by neutrons in the germanium
	hNEnergyLossGe = new TH1D("hNEnergyLossGe","Energy loss of neutrons inside the germanium;Energy loss of neutrons [MeV]; Counts", 2000,0,2);
			//histogram with energy deposited by neutrons in the active neutron detector
	hNEnergyLossKermit = new TH1D("hNEnergyLossKermit","Energy loss of neutrons inside the active neutron detector;Energy loss of neutrons [MeV]; Counts", 2000,0,2);
			//histogram with energy deposited by neutrons in the passive neutron detector
	hNEnergyLossBall = new TH1D("hNEnergyLossBall","Energy loss of neutrons inside the passive neutron detecor;Energy loss of neutrons [MeV]; Counts", 2000,0,2);
	
			//histogram for kin. energy of protons in the germanium
	hPEkinGe = new TH1D("hPEkinGe","E_{kin} of protons in the germanium;E_{kin} of protons  [MeV]; Counts", 2000,0,200);
			//histogram with energy deposited by protons in the germanium
	hPEnergyLossGe = new TH1D("hPEnergyLossGe","Energy loss of protons inside the germanium;Energy loss of protons [MeV]; Counts", 20000,0,200);
	
			//histogram of gamma spectrum in the germanium
	hGammaSpecGe = new TH1D("hGammaSpecGe","Energy loss of gammas inside the germanium;Energy loss of gammas [MeV]; Counts/1 keV", 10000,0,10);
	
		
		
	
	Int_t *ActualTrackID ;
	//Int_t nEvents = b->GetEntriesFast();
	//cout<< "Number of Simulated Events: "<<nEvents<<endl;

	Int_t Hits=0;
	Int_t DetID=-10;
	Int_t TrackID=-10;
	Int_t iNeutrons = 0;


	
	TVector3 NHit;
	TVector3 NMom;
	TVector3 NAllMom;
	TVector3 StartVertex;
	TVector3 PHit;
	TVector3 PMom;
	
	Int_t EndEvent;	
	if (NoOfEvents ==0)
		EndEvent = b->GetEntriesFast();
	else
		EndEvent = StartEvent + NoOfEvents;
		
	Double_t NeutronEnergyLossArrayGe[50];				// array to store the energy loss of all neutrons in the germanium during one event
	Double_t NeutronEnergyLossArrayKermit[50];				// array to store the energy loss of all neutrons in the active neutron detector during one event
	Double_t NeutronEnergyLossArrayBall[50];				// array to store the energy loss of all neutrons in the passive neutron detector during one event
	Double_t ProtonEnergyLossArrayGe[50];				// array to store the energy loss of all protons in the germanium during one event
	Double_t GammaEnergyLossArrayGe[50];				// array to store the energy loss of all gammas in the germanium during one event

		//event loop
	for (Int_t k=StartEvent; k< EndEvent; k++)
	//for (Int_t k=569494; k<569498; k++)
	{ 
		
		for(Int_t i = 0; i < 50; i++)	//init of array
		{
			NeutronEnergyLossArrayGe[i]=0;
			NeutronEnergyLossArrayKermit[i]=0;
			NeutronEnergyLossArrayBall[i]=0;
			ProtonEnergyLossArrayGe[i] =0;
			GammaEnergyLossArrayGe[i] = 0;
		}
		Int_t nNeutronsGe = -1;
		Int_t NeutronTrackIDGe =-1;
		Bool_t newNeutronGe = 1;
		Int_t nNeutronsKermit = -1;
		Int_t NeutronTrackIDKermit =-1;
		Bool_t newNeutronKermit = 1;
		Int_t nNeutronsBall = -1;
		Int_t NeutronTrackIDBall =-1;
		Bool_t newNeutronBall = 1;
		Int_t nProtonsGe = -1;
		Int_t ProtonTrackIDGe =-1;
		Bool_t newProton = 1;

		Int_t nGammasGe = -1;
		Int_t GammaTrackIDGe =-1;

		b->GetEntry(k);
		if (!((k*100)% NoOfEvents))	// mark every % of proceeded events
		{
			cout << "Event number :\t" << k << endl;
		}
	    //if(verbose) cout<<"Event No "<<j<<endl;
	  
	  DetID=-10;
	  TrackID=-10;

		// loop over the hits of the event
		for (Int_t i=0; i < fHypGe->GetEntriesFast(); i++)
		{
			PndHypGePoint *hitgam=(PndHypGePoint*)fHypGe->At(i);
			PndMCTrack *mcgam = (PndMCTrack*)fMcTr->At(hitgam->GetTrackID());

			StartVertex = mcgam->GetStartVertex();
			VertexVolumeName =gGeoManager->FindNode(StartVertex.X(),StartVertex.Y(),StartVertex.Z())->GetName();
			//only Neutrons
			if (hitgam->GetpdgCode() == 2112)
			{
				//cout <<"Event " <<EvtCount<< " \t\tStartVertex Radius: " <<StartVertex.Mag()<<endl;
				//Fill hNeutronOriginGe		
				
				//cout << VertexVolumeName << endl;
				
				//calculate E_kin of neutron
				NeutronMomentum.SetX(hitgam->GetPx());
				NeutronMomentum.SetY(hitgam->GetPy());
				NeutronMomentum.SetZ(hitgam->GetPz());
				NeutronEkin = 1./2.*NeutronMomentum.Mag2()/0.939565*1000; // in MeV
				//cout << NeutronEkin << "\t\t\t" << NeutronMomentum.Mag2()<<endl;



				if(hitgam->GetDetectorID()==101)
				{

					//cut on neutrons created outside of the germanium detecto
					if (!VertexVolumeName.Contains("Crystal_101") && ! VertexVolumeName.Contains ("Capsule") && !VertexVolumeName.Contains ("ColdFinger"))
					{
						if (newNeutronGe)
						{
							// fill Ekin histogram
							hNHitsGeKermitBall->Fill("Germanium crystal",1);
							hNEkinGe->Fill(NeutronEkin);
							newNeutronGe = 0;
						}
						// calculate energy loss of neutron in germanium
						if (NeutronTrackIDGe !=hitgam->GetTrackID()) // new neutron (different TrackID)
						{
							NeutronTrackIDGe = hitgam->GetTrackID();
							nNeutronsGe++;
							newNeutronGe = 1;
							//cout << nNeutronsGe << endl;
						}
						NeutronEnergyLossArrayGe[nNeutronsGe] += hitgam->GetEnergyLoss()*1000;
						//cout << hitgam->GetEnergyLoss()*1000<< endl;

						// calculate theta of neutron
						NHit.SetX(hitgam->GetX());
						NHit.SetY(hitgam->GetY());
						NHit.SetZ(hitgam->GetZ());					// no shift like in PANDA!!!!
						hNHitsAngleGe->Fill(180/TMath::Pi()*NHit.Theta());				//fill histogram with polar angle of detector interactions of primary neutrons
						// fill Ekin histogram
					}

					if( VertexVolumeName.Contains("Target") || VertexVolumeName.Contains("Piezo"))
						hNeutronOriginGe->Fill("Target",1);
					if( VertexVolumeName.Contains("ScrewHead") || VertexVolumeName.Contains("Goniometer")|| VertexVolumeName.Contains("Bracket")|| VertexVolumeName.Contains("AlPlate"))
						hNeutronOriginGe->Fill("Target Support",1);
					if( VertexVolumeName.Contains("AlBasePlate"))
						hNeutronOriginGe->Fill("Al base plate",1);
					if( VertexVolumeName.Contains("PlasticBlock"))
						hNeutronOriginGe->Fill("Plastic blocks",1);
					if( VertexVolumeName.Contains("Beam") || VertexVolumeName.Contains("Pillar"))
						hNeutronOriginGe->Fill("Maytec frame",1);
					if( VertexVolumeName.Contains("KermitCrystal"))
						hNeutronOriginGe->Fill("Active neutron detector",1);
					if( VertexVolumeName.Contains("KermitCrystal"))
						hNeutronOriginGe->Fill("Active neutron detector",1);
					if( VertexVolumeName.Contains("NeutronBallCrystal"))
							hNeutronOriginGe->Fill("Passive neutron detector",1);
					if (VertexVolumeName.Contains("Crystal_101"))
						hNeutronOriginGe->Fill("Crystal",1);
					if (VertexVolumeName.Contains ("Capsule") || VertexVolumeName.Contains ("ColdFinger") || VertexVolumeName.Contains ("Cryostat"))
						hNeutronOriginGe->Fill("Cryostat",1);
					if (VertexVolumeName.Contains("Glue"))
						hNeutronOriginGe->Fill("Lumi glue test",1);
					if (VertexVolumeName.Contains("LumiElectronics"))
						hNeutronOriginGe->Fill("Lumi electronics test",1);
					if (VertexVolumeName.Contains("BeamDump"))
						hNeutronOriginGe->Fill("Beam dump",1);
					if (VertexVolumeName.Contains("cave"))
						hNeutronOriginGe->Fill("Cave",1);

				}
				if(hitgam->GetDetectorID()==2000)
				{

					//cut on neutrons created outside of the active neutron detector
					if (!VertexVolumeName.Contains("Kermit") )
					{
						if (newNeutronKermit)
						{
							// fill Ekin histogram
							hNHitsGeKermitBall->Fill("Active neutron detector",1);
							hNEkinKermit->Fill(NeutronEkin);
							newNeutronKermit = 0;
						}
						// calculate energy loss of neutron in active neutron detektor
						if (NeutronTrackIDKermit !=hitgam->GetTrackID()) // new neutron (different TrackID)
						{
							NeutronTrackIDKermit = hitgam->GetTrackID();
							nNeutronsKermit++;
							newNeutronKermit = 1;
							//cout << nNeutronsKermit << endl;
						}
						NeutronEnergyLossArrayKermit[nNeutronsKermit] += hitgam->GetEnergyLoss()*1000;
						//cout << hitgam->GetEnergyLoss()*1000<< endl;

						hNHitsGeKermitBall->Fill("Active neutron detector",1);
						// fill histogram with Ekin of neutrons in the active neutron detector
						hNEkinKermit->Fill(NeutronEkin);
					}


				}
				if(hitgam->GetDetectorID()==2001)
				{

					// cut on neutrons created outside of the passive neutron detector
					if (!VertexVolumeName.Contains("NeutronBall") )
					{
						if (newNeutronBall)
						{
							// fill Ekin histogram
							hNHitsGeKermitBall->Fill("Passive neutron detector",1);
							hNEkinBall->Fill(NeutronEkin);
							newNeutronBall = 0;
						}
						// calculate energy loss of a neutron in passive neutron detector
						if (NeutronTrackIDBall !=hitgam->GetTrackID()) // new neutron (different TrackID)
						{
							NeutronTrackIDBall = hitgam->GetTrackID();
							nNeutronsBall++;
							newNeutronBall = 1;
							//cout << nNeutronsBall << endl;
						}
						NeutronEnergyLossArrayBall[nNeutronsBall] += hitgam->GetEnergyLoss()*1000;
						//cout << hitgam->GetEnergyLoss()*1000<< endl;


					}


				}
			}		// end of only neutrons
			// only protons
			/*if (hitgam->GetpdgCode() == 2212)
			{
				if(hitgam->GetDetectorID()==101)
				{
				//calculate E_kin of proton
					ProtonMomentum.SetX(hitgam->GetPx());
					ProtonMomentum.SetY(hitgam->GetPy());
					ProtonMomentum.SetZ(hitgam->GetPz());
					ProtonEkin = 1./2.*ProtonMomentum.Mag2()/0.938272*1000; // in MeV
					//cout << ProtonEkin << "\t\t\t" << ProtonMomentum.Mag2()<<endl;


					//cut on protons created outside of the germanium detector
					if (!VertexVolumeName.Contains("Crystal_101") && ! VertexVolumeName.Contains ("Capsule") && ! VertexVolumeName.Contains ("ColdFinger"))
					{
						// if first occurence of the proton in the detecor fill the E_kin
						if (newProton)
						{
							// fill Ekin histogram
							hPEkinGe->Fill(ProtonEkin);
							newProton = 0;
						}
						// calculate energy loss of proton in germanium
						if (ProtonTrackIDGe !=hitgam->GetTrackID()) // new proton (different TrackID)
						{
							ProtonTrackIDGe = hitgam->GetTrackID();
							nProtonsGe++;
							newProton = 1;
							//cout << nProtonsGe << endl;
						}
						ProtonEnergyLossArrayGe[nProtonsGe] += hitgam->GetEnergyLoss()*1000;
						cout << hitgam->GetEnergyLoss()*1000<< endl;

					}
				}
			}		// end of only protons

			// only gammas
			if (hitgam->GetpdgCode() == 22  )
						{
							if(hitgam->GetDetectorID()==101)
							{
									// calculate energy loss of gamma in the germanium
									if (GammaTrackIDGe !=hitgam->GetTrackID()) // new gamma (different TrackID)
									{
										GammaTrackIDGe = hitgam->GetTrackID();
										nGammasGe++;
									}
									GammaEnergyLossArrayGe[nGammasGe] += hitgam->GetEnergyLoss()*1000;
									cout << nGammasGe << "\t" << GammaEnergyLossArrayGe[nGammasGe]<< "\t" << hitgam->GetEnergyLoss()*1000 << endl;
							}
						}		// end of only gammas

			//all particles in Crystal
			if (!VertexVolumeName.Contains("Crystal_101") && ! VertexVolumeName.Contains ("Capsule") && !VertexVolumeName.Contains ("ColdFinger"))	//created outside of the detector
			{
				if(hitgam->GetDetectorID()==101 || hitgam->GetDetectorID()==102)
					hAllParticlesGe->Fill(hitgam->GetpdgCode());

				ParticleMomentum.SetX(hitgam->GetPx());
				ParticleMomentum.SetY(hitgam->GetPy());
				ParticleMomentum.SetZ(hitgam->GetPz());
				Double_t ParticleMass = 0;
				TString ParticleName = "";
				switch ((int) hitgam->GetpdgCode())
				{
					case -211: ParticleMass = 0.13957;		ParticleName ="#pi^{-}";		break;		//pi-
					case -13: ParticleMass = 0.10561;		ParticleName ="#mu^{+}";		break;			//mu+
					case -11: ParticleMass = 0.000511;		ParticleName ="e^{+}";		break;			//e+
					case  11: ParticleMass = 0.000511;		ParticleName ="e^{-}";		break;			//e-
					case  13: ParticleMass = 0.10561;		ParticleName ="#mu^{-}";		break;			//mu-
					case  22: ParticleMass = 0;					ParticleName ="#gamma";		break;			//gamma
					case 211: ParticleMass = 0.13957;		ParticleName ="#pi^{+}";		break;			//pi+
					case 2112: ParticleMass = 0.939565;	ParticleName ="neutron";		break;			//n
					case 2212: ParticleMass = 0.93827;		ParticleName ="proton";	break;			//p
					default: ParticleMass = -1;	std::cout.precision(10);	cout << "Found new particle:\t" << hitgam->GetpdgCode() <<"\t" << VertexVolumeName << endl; 	break;
				}
				if(ParticleMass != -1)
				{
					ParticleEkin = (sqrt(ParticleMass*ParticleMass+ParticleMomentum.Mag2())-ParticleMass)*1000;//Ekin in MeV
					//all particles (but gammas) in Crystal - E_{kin} - correlation
					if (!VertexVolumeName.Contains("Crystal_101") && ! VertexVolumeName.Contains ("Capsule"))	//created outside of the detector
					{
						if(hitgam->GetDetectorID()==101)
						hAllParticlesEkinGe->Fill( ParticleEkin , ParticleName.Data(),1);
					}
				}
				
			}		// end of all particles part
			//piezos
			if(hitgam->GetDetectorID()==1000)
				hAllParticlesPiezo ->Fill(hitgam->GetpdgCode());
			if(hitgam->GetDetectorID()==1001)
				hAllParticlesPiezo2 ->Fill(hitgam->GetpdgCode());*/
		}			// end of for loop over the hits

		//fill Neutron Engergy loss histograms
			//germanium
		if (nNeutronsGe>-1)
		{
			//		cout << nNeutronsGe+1 << endl;
			for (Int_t i = 0; i <= nNeutronsGe; i++)
			{
				//		cout << i << "\t\t" << NeutronEnergyLossArrayGe[i] << endl;
				hNEnergyLossGe->Fill(NeutronEnergyLossArrayGe[i]);
			}
		}
			//active neutron detector
		if (nNeutronsKermit>-1)
		{
			//		cout << nNeutronsKermit+1 << endl;
			for (Int_t i = 0; i <= nNeutronsKermit; i++)
			{
				//		cout << i << "\t\t" << NeutronEnergyLossArrayKermit[i] << endl;
				hNEnergyLossKermit->Fill(NeutronEnergyLossArrayKermit[i]);
			}
		}
			//passive neutron detector
		if (nNeutronsBall>-1)
		{
			//		cout << nNeutronsBall+1 << endl;
			for (Int_t i = 0; i <= nNeutronsBall; i++)
			{
				//		cout << i << "\t\t" << NeutronEnergyLossArrayBall[i] << endl;
				hNEnergyLossBall->Fill(NeutronEnergyLossArrayBall[i]);
			}
		}
		if (nProtonsGe>-1)
		{
			//		cout << nProtonsGe+1 << endl;
			for (Int_t i = 0; i <= nProtonsGe; i++)
			{
				//		cout << i << "\t\t" << ProtonEnergyLossArrayGe[i] << endl;
				hPEnergyLossGe->Fill(ProtonEnergyLossArrayGe[i]);
			}
		}
		if (nGammasGe>-1)
				{
					//		cout << nGammasGe+1 << endl;
					for (Int_t i = 0; i <= nGammasGe; i++)
					{
						//		cout << i << "\t\t" << GammaEnergyLossArrayGe[i] << endl;
						hGammaSpecGe->Fill(GammaEnergyLossArrayGe[i]);
					}
				}
	}// end for k (events)
  



	cNHitsGe = new TCanvas("cNHitsGe","Neutron Hits",1600,600);
		cNHitsGe->Divide(2,1);

	cNHitsGeKermitBall = new TCanvas("cNHitsGeKermitBall","Neutron Hits on all 3 detectors",800,600);

	cAllParticlesGe = new TCanvas("cAllParticles","All particles interacting in the germanium crystal", 1600, 600);
		cAllParticlesGe->Divide(2,1);

	cAllParticlesPiezo = new TCanvas("cAllParticlesPiazo","All particles interacting in the piezo", 1600, 600);
		cAllParticlesPiezo->Divide(2,1);

	cNeutronEkinGe = new TCanvas("cNeutronEkinGe","E_{kin} of neutrons", 1600, 600);
		cNeutronEkinGe->Divide(2,1);

	cNeutronEkinKermit = new TCanvas("cNeutronEkinKermit","E_{kin} of neutrons", 1600, 600);
		cNeutronEkinKermit->Divide(2,1);

	cNeutronEkinBall = new TCanvas("cNeutronEkinBall","E_{kin} of neutrons", 1600, 600);
		cNeutronEkinBall->Divide(2,1);

	cProtonEkinGe = new TCanvas("cProtonEkinGe","E_{kin} of protons", 1600, 600);
		cProtonEkinGe->Divide(2,1);
	
	cGammaEnergyGe= new TCanvas("cGammaEnergyGe","#gamma spectrum", 800, 600);
	
	cNHitsGe->cd(1);
	hNHitsAngleGe->DrawCopy();	// angular distribution of neutrons in the germanium
	cNHitsGe->cd(2);
	hNeutronOriginGe->DrawCopy();// neutrons and their origin in the germanium
	
	cNHitsGeKermitBall->cd();
	hNHitsGeKermitBall->DrawCopy();// neutrons in ge, active and passive neutron detector
	
	cAllParticlesGe->cd(1);
	hAllParticlesGe->DrawCopy();	// all particles hitting the germanium
	cAllParticlesGe->cd(2);
	hAllParticlesEkinGe->DrawCopy("colz");// 2D all E_kin distribution of particles hitting the germanium
	
	cAllParticlesPiezo->cd(1);
	hAllParticlesPiezo->DrawCopy();	// all particles hitting the first pieo
	cAllParticlesPiezo->cd(2);
	hAllParticlesPiezo2->DrawCopy();// all particles hitting the germanium
	
	cNeutronEkinGe->cd(1);
	hNEkinGe->DrawCopy();						// E_kin of neutrons entering the germanium
	cNeutronEkinKermit->cd(1);
	hNEkinKermit->DrawCopy();						// E_kin of neutrons entering the active neutron detector
	cNeutronEkinBall->cd(1);
	hNEkinBall->DrawCopy();						// E_kin of neutrons entering the passive neutron detector

	cNeutronEkinGe->cd(2);
	hNEnergyLossGe->DrawCopy();			// Energy loss of neutrons in the germanium
	cNeutronEkinKermit->cd(2);
	hNEnergyLossKermit->DrawCopy();			// Energy loss of neutrons in the active neutron detector
	cNeutronEkinBall->cd(2);
	hNEnergyLossBall->DrawCopy();			// Energy loss of neutrons in the passive neutron detector

	cProtonEkinGe->cd(1);
	hPEkinGe->DrawCopy();						// E_kin of protons entering the germanium
	cProtonEkinGe->cd(2);
	hPEnergyLossGe->DrawCopy();			// Energy loss of protons in the germanium

	cGammaEnergyGe->cd();
	hGammaSpecGe->DrawCopy();						// spectrum of gammas in the germanium

	
	
	hNHitsAngleGe->Write();	// angular distribution of neutrons in the germanium
	hNeutronOriginGe->Write();// neutrons and their origin in the germanium
	hNHitsGeKermitBall->Write();// neutrons in ge, active and passive neutron detector

//	hAllParticlesGe->Write();	// all particles hitting the germanium
//	hAllParticlesEkinGe->Write();// 2D all E_kin distribution of particles hitting the germanium
//
//	hAllParticlesPiezo->Write();	// all particles hitting the first pieo
//	hAllParticlesPiezo2->Write();// all particles hitting the germanium
//
//
	hNEkinGe->Write();						// E_kin of neutrons entering the germanium
	hNEkinKermit->Write();						// E_kin of neutrons entering the active neutron detector
	hNEkinBall->Write();						// E_kin of neutrons entering the passive neutron detector
//
//	hNEnergyLossGe->Write();			// Energy loss of neutrons in the germanium
//	hNEnergyLossKermit->Write();			// Energy loss of neutrons in the active neutron detector
//	hNEnergyLossBall->Write();			// Energy loss of neutrons in the passive neutron detector
//
//	hPEkinGe->Write();						// E_kin of protons entering the germanium
//	hPEnergyLossGe->Write();			// Energy loss of protons in the germanium
//
//	hGammaSpecGe->Write();						// spectrum of gammas in the germanium


	for(int i = 0;i<hAllParticlesGe->GetNbinsX(); i++)
	{
		if (hAllParticlesGe->GetBinContent(i) != 0)
		{
			cout << "PDG code:\t" << hAllParticlesGe->GetBinCenter(i)-0.5 << "\tCount:\t"<<hAllParticlesGe->GetBinContent(i) << endl;
		}
	}
		
	cout << "HypGe COSYBackgroundSim Ana:\tAnalysis finished succesfully" << endl;

	
	OutputFile->Close();
	


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
