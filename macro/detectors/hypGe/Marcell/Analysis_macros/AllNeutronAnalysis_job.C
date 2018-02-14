#include"TH1D.h"
#include"TString.h"
#include"TFile.h"
#include"TTree.h"
#include"TStopwatch.h"
#include"TVector3.h"
#include"TROOT.h"
#include"TMath.h"
#include"TClonesArray.h"
#include<stdio.h">
#include<iostream>


using namespace std;
int AllNeutronAnalysis_job(TString Filename_ext)
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
	TString Filename = "$SIMDATADIR/Neutron/"+Filename_ext;
	//TString Filename = "$SIMDATADIR/TripleBall40Offset10_urqmd_pbarC_1_5000Evts.root";
	TString outfile= "$SIMDATADIR/Neutron/Ana/Ana" + Filename_ext;
	TFile* InputFile = new TFile(Filename);
	TFile* OutputFile = new TFile(outfile,"RECREATE");
	
	//getting simulation branches from input file
  TTree *b=(TTree *) InputFile->Get("cbmsim") ;
  TClonesArray* hit_bar=new TClonesArray("PndHypGePoint");
  b->SetBranchAddress("HypGePoint",&hit_bar);//Branch names
  TClonesArray* mc_bar=new TClonesArray("PndMCTrack");
  b->SetBranchAddress("MCTrack",&mc_bar);//Branch names

	//declaring histograms
		//histogram with polar angle of detector interactions of primary neutrons
	TH1D* hNHits = new TH1D("hNHits","Polar angle of primary neutrons interacting with the crystals",180, 90,180);
	hNHits->SetXTitle("#Theta [#circ]");
	hNHits->SetYTitle("Counts / 0.5 #circ");
	
	//histogram with polar angle of crystal ring 1 interactions of primary neutrons
	TH1D* hRing1 = new TH1D("hRing1","Polar angle of primary neutrons interacting with the crystals of ring 1",180, 90,180);
	hRing1->SetXTitle("#Theta [#circ]");
	hRing1->SetYTitle("Counts / 0.5 #circ");
	
	//histogram with polar angle of crystal ring 2 interactions of primary neutrons
	TH1D* hRing2 = new TH1D("hRing2","Polar angle of primary neutrons interacting with the crystals of ring 2",180, 90,180);
	hRing2->SetXTitle("#Theta [#circ]");
	hRing2->SetYTitle("Counts / 0.5 #circ");
	
	//histogram with polar angle of crystal ring 3 interactions of primary neutrons
	TH1D* hRing3 = new TH1D("hRing3","Polar angle of primary neutrons interacting with the crystals of ring 3",180, 90,180);
	hRing3->SetXTitle("#Theta [#circ]");
	hRing3->SetYTitle("Counts / 0.5 #circ");
	
	//histogram with polar angle of crystal ring 4 interactions of primary neutrons
	TH1D* hRing4 = new TH1D("hRing4","Polar angle of primary neutrons interacting with the crystals of ring 4",180, 90,180);
	hRing4->SetXTitle("#Theta [#circ]");
	hRing4->SetYTitle("Counts / 0.5 #circ");
	
	//histogram with polar angle of crystal ring 5 interactions of primary neutrons
	TH1D* hRing5 = new TH1D("hRing5","Polar angle of primary neutrons interacting with the crystals of ring 5",180, 90,180);
	hRing5->SetXTitle("#Theta [#circ]");
	hRing5->SetYTitle("Counts / 0.5 #circ");
	
	//histogram with polar angle of crystal ring 6 interactions of primary neutrons
	TH1D* hRing6 = new TH1D("hRing6","Polar angle of primary neutrons interacting with the crystals of ring 6",180, 90,180);
	hRing6->SetXTitle("#Theta [#circ]");
	hRing6->SetYTitle("Counts / 0.5 #circ");
	
	//histogram with polar angle of crystal ring 7 interactions of primary neutrons
	TH1D* hRing7 = new TH1D("hRing7","Polar angle of primary neutrons interacting with the crystals of ring 7",180, 90,180);
	hRing7->SetXTitle("#Theta [#circ]");
	hRing7->SetYTitle("Counts / 0.5 #circ");
	
	//histogram with polar angle of crystal ring 8 interactions of primary neutrons
	TH1D* hRing8 = new TH1D("hRing8","Polar angle of primary neutrons interacting with the crystals of ring 8",180, 90,180);
	hRing8->SetXTitle("#Theta [#circ]");
	hRing8->SetYTitle("Counts / 0.5 #circ");
	
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
	TH1D* hCrystalHit = new TH1D("hCrystalHit","Hits per Crystal",2100,1,2100);
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
	Int_t nEvents = b->GetEntriesFast();
	cout<< "Number of Simulated Events: "<<nEvents<<endl;

	Int_t Hits=0;
	Int_t DetID=-10;
	Int_t TrackID=-10;
	int iNeutrons = 0;
	//event loop
	
	TVector3 NHit;
	TVector3 NHit_p;
	TVector3 NMom;
	TVector3 NAllMom;
	TVector3 StartVertex;
	for (Int_t k=0; k<nEvents; k++)
	{ 
		b->GetEntry(k);
		if (!((k*100)% nEvents))	// mark every % of proceeded events
		{
			cout << "Event number :\t" << k << endl;
		}
	    //if(verbose) cout<<"Event No "<<j<<endl;
	  
	  DetID=-10;
	  TrackID=-10; 
		// loop over the hits of an event
		for (Int_t i=0; i<hit_bar->GetEntriesFast(); i++)
		{
														//if (i==0)
															//Hits++;
														//cout <<"Hit "<< i << endl;
			
			PndHypGePoint *hitgam=(PndHypGePoint*)hit_bar->At(i);
			if (!hitgam)
			{
				continue;
			}
			PndMCTrack *mcgam = (PndMCTrack*)mc_bar->At(hitgam->GetTrackID());
			if (!mcgam)
			{
				continue;
			}
			if(hitgam->GetpdgCode()==2112)
			{
			//NMom = mcgam->GetMomentum();
				StartVertex = mcgam->GetStartVertex();
				cout << "Sv" << StartVertex.Mag() << endl;
				if (StartVertex.Mag() <60)
				{
					if ( hitgam->GetpdgCode()==2112 && mcgam->GetMotherID() == -1 && !(DetID == hitgam->GetDetectorID() && TrackID==hitgam->GetTrackID())) // primary neutron, last part: not(same particle and crystal) -> against multi counting of a single neutron
					{
						DetID = hitgam->GetDetectorID();
						TrackID = hitgam->GetTrackID();
						//cout << "Event :\t" << k <<"\tHit : \t" << i << "\tDetector :\t" << DetID << "\tTrackID :\t"<< TrackID<<"\tMother :\t"<< mcgam->GetMotherID()<<endl;
						iNeutrons++;
						hCrystalHit->Fill(hitgam->GetDetectorID());				//fill histogram to see which detector is hit
						
						
						// fill vector with hit coordinates to get theta of the hit
						NHit.SetX(hitgam->GetX());
						NHit.SetY(hitgam->GetY());
						NHit.SetZ(hitgam->GetZ()+55);
						Double_t theta = 180/TMath::Pi()*NHit.Theta();
						hNHits->Fill(theta);				//fill histogram with polar angle of detector interactions of primary neutrons
						cout << DetID << endl;
						switch (DetID)
						{
							case 101:
								//iRing3+=Content;
								hRing3->Fill(theta);
								break;
							case 102:
								//iRing4+=Content;
								hRing4->Fill(theta);
								break;
							case 103:
								//iRing3+=Content;
								hRing3->Fill(theta);
								break;
							case 204:
								//iRing4+=Content;
								hRing4->Fill(theta);
								break;
							case 205:
								//iRing3+=Content;
								hRing3->Fill(theta);
								break;
							case 206:
								//iRing3+=Content;
								hRing3->Fill(theta);
								break;
							case 307:
								//iRing3+=Content;
								hRing3->Fill(theta);
								break;
							case 308:
								//iRing3+=Content;
								hRing3->Fill(theta);
								break;
							case 309:
								//iRing4+=Content;
								hRing4->Fill(theta);
								break;
							case 410:
								//iRing2+=Content;
								hRing2->Fill(theta);
								break;
							case 411:
								//iRing1+=Content;
								hRing1->Fill(theta);
								break;
							case 412:
								//iRing2+=Content;
								hRing2->Fill(theta);
								break;
							case 513:
								//iRing1+=Content;
								hRing1->Fill(theta);
								break;
							case 514:
								//iRing2+=Content;
								hRing2->Fill(theta);
								break;
							case 515:
								//iRing2+=Content;
								hRing2->Fill(theta);
								break;
							case 616:
								//iRing2+=Content;
								hRing2->Fill(theta);
								break;
							case 617:
								//iRing1+=Content;
								hRing1->Fill(theta);
								break;
							case 618:
								//iRing1+=Content;
								hRing1->Fill(theta);
								break;
							case 719:
								//iRing1+=Content;
								hRing1->Fill(theta);
								break;
							case 720:
								//iRing2+=Content;
								hRing2->Fill(theta);
								break;
							case 721:
								//iRing2+=Content;
								hRing2->Fill(theta);
								break;
							case 822:
								//iRing2+=Content;
								hRing2->Fill(theta);
								break;
							case 823:
								//iRing2+=Content;
								hRing2->Fill(theta);
								break;
							case 824:
								//iRing1+=Content;
								hRing1->Fill(theta);
								break;
							case 925:
								//iRing3+=Content;
								hRing3->Fill(theta);
								break;
							case 926:
								//iRing3+=Content;
								hRing3->Fill(theta);
								break;
							case 927:
								//iRing4+=Content;
								hRing4->Fill(theta);
								break;
							case 1028:
								//iRing4+=Content;
								hRing4->Fill(theta);
								break;
							case 1029:
								//iRing3+=Content;
								hRing3->Fill(theta);
								break;
							case 1030:
								//iRing3+=Content;
								hRing3->Fill(theta);
								break;
							case 1131:
								//iRing3+=Content;
								hRing3->Fill(theta);
								break;
							case 1132:
								//iRing4+=Content;
								hRing4->Fill(theta);
								break;
							case 1133:
								//iRing3+=Content;
								hRing3->Fill(theta);
								break;
							case 1234:
								//iRing2+=Content;
								hRing2->Fill(theta);
								break;
							case 1235:
								//iRing2+=Content;
								hRing2->Fill(theta);
								break;
							case 1236:
								//iRing1+=Content;
								hRing1->Fill(theta);
								break;
							case 1337:
								//iRing1+=Content;
								hRing1->Fill(theta);
								break;
							case 1338:
								//iRing2+=Content;
								hRing2->Fill(theta);
								break;
							case 1339:
								//iRing2+=Content;
								hRing2->Fill(theta);
								break;
							case 1440:
								//iRing2+=Content;
								hRing2->Fill(theta);
								break;
							case 1441:
								//iRing1+=Content;
								hRing1->Fill(theta);
								break;
							case 1442:
								//iRing1+=Content;
								hRing1->Fill(theta);
								break;
							case 1543:
								//iRing1+=Content;
								hRing1->Fill(theta);
								break;
							case 1544:
								//iRing2+=Content;
								hRing2->Fill(theta);
								break;
							case 1545:
								//iRing2+=Content;
								hRing2->Fill(theta);
								break;
							case 1646:
								//iRing2+=Content;
								hRing2->Fill(theta);
								break;
							case 1647:
								//iRing1+=Content;
								hRing1->Fill(theta);
								break;
							case 1648:
								//iRing2+=Content;
								hRing2->Fill(theta);
								break;
							default: 
								break;
						}
						

						// fill vector with hit momentum to get theta of the hit momentum
						//NHit_p.SetX(hitgam->GetPx());
						//NHit_p.SetY(hitgam->GetPy());
						//NHit_p.SetZ(hitgam->GetPz());
						//hNHits_p->Fill(180/TMath::Pi()*NHit_p.Theta());		//fill histogram with the polar momentum of primary neutrons interacting with the crystals
						
						hNeutronXYDistribution->Fill(hitgam->GetX(),hitgam->GetY());
						//hNMom->Fill(1./2.* NMom.Mag2()/0.939);				//fill histogram with E_kin of primary neutrons
						
					}

					//if (hitgam->GetpdgCode()==2112)							// neutron
					//{
						//hNeutronEnergyDeposit->Fill(hitgam->GetEnergyLoss());
					//}
				}
			}
		}
	
		// loop over all entries in mctrack
		//cout << "mcbar length :\t" <<mc_bar->GetEntriesFast() << endl;
		for (Int_t m = 0; m<mc_bar->GetEntriesFast();m++)
		{
			//cout << "mcbar length :\t" <<mc_bar->GetEntriesFast() << endl;
			PndMCTrack *mcgam = (PndMCTrack*)mc_bar->At(m);
			
			NAllMom = mcgam->GetMomentum();
			if (mcgam->GetPdgCode() == 2112 && mcgam->GetMotherID() == -1)
			{
				//cout << "event :\t"<<k<<"\tmcpdg :\t" << mcgam->GetPdgCode() << "\tmcmotherid :\t"<< mcgam->GetMotherID() <<endl;
				
				hNAll->Fill(180/TMath::Pi()*NAllMom.Theta());		//fill histogram with the polar momentum of primary neutrons in the solid angle of the crystal (not necessarily interacting with the crystal)
			}
			if (mcgam->GetPdgCode() == 2212 && mcgam->GetMotherID() == -1)
			{
				//cout << "event :\t"<<k<<"\tmcpdg :\t" << mcgam->GetPdgCode() << "\tmcmotherid :\t"<< mcgam->GetMotherID() <<endl;
				
				hPAll->Fill(180/TMath::Pi()*NAllMom.Theta());		//fill histogram with the polar momentum of primary neutrons in the solid angle of the crystal (not necessarily interacting with the crystal)
			}
		}
	}// end for k (events)
  
  TString PathFirstPart = getenv("SIMDATADIR");			//always needed for i/ofstream files, TFile can direcly use $SIMDATADIR
	TString TxTOutFilename =PathFirstPart+"/Neutron/Ana/ana" + Filename_ext + ".txt";
	ofstream TxTOutfile(TxTOutFilename.Data());
	Int_t CrystalNumber = 1;
	cout << "Bin\tCrystal\tCluster\tNeutron hits"<<endl;
	TxTOutfile << "Bin\tCrystal\tCluster\tNeutron hits"<<endl;
	for (Int_t iBin=0;iBin<2100;iBin++)
	{
		
		if(hCrystalHit->GetBinContent(iBin))
		{
			cout << iBin << "\t"<< CrystalNumber <<"\t"<< iBin/100 <<"\t"<< hCrystalHit->GetBinContent(iBin)<< endl;
			
			TxTOutfile << iBin << "\t"<< CrystalNumber <<"\t"<< iBin/100 <<"\t"<< hCrystalHit->GetBinContent(iBin)<< endl;
			CrystalNumber++;
			
		}
	}
	TxTOutfile.close();
	hNeutronXYDistribution->DrawCopy("colz");
	//hNAll->DrawCopy();
  //hNHits->SetLineColor(kRed);
  //hNHits->DrawCopy("same");
  
  //building the average over the crystals
  hRing1->Scale(12);
  hRing2->Scale(18);
  hRing3->Scale(12);
  hRing4->Scale(6);
  

  hNHits->Write();
  hRing1->Write();
  hRing2->Write();
  hRing3->Write();
  hRing4->Write();
  hNHits_p->Write();
	hNAll->Write();
	hPAll->Write();
	hNMom->Write();
	hCrystalHit->Write();
	hNeutronEnergyDeposit->Write();
	hNeutronXYDistribution->Write();

	
	
	//hNeutronEnergyDeposit->DrawCopy();
	
	OutputFile->Close();
	
	//hNeutronEnergyDeposit->Draw();
  //hNHits->Draw();


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
