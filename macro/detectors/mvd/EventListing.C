int EventListing()
{
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	gSystem->Load("libriemann.C");

	TString MCFile = "Mvd_Test.root";
	TH1D* h1 = new TH1D("h1","PixError",100,-0.01,0.01);

	PndFileNameCreator creator(MCFile.Data());
	TString DigiFile = creator.GetDigiFileName(false).c_str();
	TString RecoFile = creator.GetRecoFileName(false).c_str();
	//TString TrackFFile = creator.GetTrackFindingFileName(false).c_str();

	TFile* fMC = new TFile(MCFile.Data());
	TFile* fDigi = new TFile(DigiFile.Data());
	TFile* fReco = new TFile(RecoFile.Data());
	//TFile* fTrack = new TFile(TrackFFile.Data());

	TTree* t = (TTree*)(fMC->Get("pndsim"));

	t->AddFriend("pndsim", fDigi);
	t->AddFriend("pndsim", fReco);
	//t->AddFriend("pndsim", fTrack);

	t->StartViewer();

	TClonesArray* MCHits = new TClonesArray("PndSdsMCPoint");
	TClonesArray* PixDigis = new TClonesArray("PndSdsDigiPixel");
	TClonesArray* StripDigis = new TClonesArray("PndSdsDigiStrip");
	TClonesArray* PixReco = new TClonesArray("PndSdsHit");
	TClonesArray* StripReco = new TClonesArray("PndSdsHit");
	TClonesArray* PixCluster = new TClonesArray("PndSdsClusterPixel");
	TClonesArray* StripCluster = new TClonesArray("PndSdsClusterStrip");
	//TClonesArray* TrackCand = new TClonesArray("TrackCand");

	t->SetBranchAddress("MVDPoint", &MCHits);
	t->SetBranchAddress("MVDPixelDigis", &PixDigis);
	t->SetBranchAddress("MVDStripDigis", &StripDigis);
	t->SetBranchAddress("MVDHitsPixel", &PixReco);
	t->SetBranchAddress("MVDHitsStrip", &StripReco);
	t->SetBranchAddress("MVDPixelClusterCand", &PixCluster);
	t->SetBranchAddress("MVDStripClusterCand", &StripCluster);

	t->GetEntry(0);

	for (int i = 0; i < MCHits->GetEntriesFast(); i++){											//get all MC Hits
		PndSdsMCPoint* myPoint = (PndSdsMCPoint*)(MCHits->At(i));
		std::cout << "<<<<<<<<<<< MCPoint >>>>>>>>>> " << std::endl;
		myPoint->Print();																		//write out MC info

		for (int j = 0; j < PixDigis->GetEntriesFast(); j++){									//get all Digis
			PndSdsDigiPixel* myPixDigi = (PndSdsDigiPixel*)PixDigis->At(j);
			bool dig = false;
			for (int ind = 0; ind < myPixDigi->GetNIndices(); ind++)							//test if digi belongs to MCHit
				if (myPixDigi->GetIndex(ind) == i) dig = true;
			if (dig){
				std::cout << "PixDigi: ";														//write out DigiInfo
				myPixDigi->Print();
				for (int k = 0; k < PixCluster->GetEntriesFast(); k++){							//get all clusters
					PndSdsClusterPixel* myPixCluster = (PndSdsClusterPixel*)PixCluster->At(k);
					if (myPixCluster->DigiBelongsToCluster(j)){									//test if digi belongs to cluster
						std::cout << "Digi " << j << " belongs to cluster: " << k << std::endl; //write out cluster info
						for (int l = 0; l < PixReco->GetEntriesFast(); l++){					//get all RecoHits
							PndSdsHit* myPixHit = (PndSdsHit*)PixReco->At(l);
							if (myPixHit->GetRefIndex() == k){									//test if RecoHit belongs to cluster
								std::cout << "PixHit: " << l << std::endl;						//write out RecoHit
								myPixHit->Print();
							}
						}
					}
				}
			}
		}

		for (int j = 0; j < StripDigis->GetEntriesFast(); j++){
			PndSdsDigiStrip* myStripDigi = (PndSdsDigiStrip*)StripDigis->At(j);
			dig = false;
			for (int ind = 0; ind < myStripDigi->GetNIndices(); ind++)
							if (myStripDigi->GetIndex(ind) == i) dig = true;
			if (dig){
				std::cout << "StripDigi: "; // << myStripDigi;
				myStripDigi->Print();
				for (int k = 0; k < StripCluster->GetEntriesFast(); k++){
					PndSdsClusterStrip* myStripCluster = (PndSdsClusterStrip*)StripCluster->At(k);
					if (myStripCluster->DigiBelongsToCluster(j)){
						std::cout << "Digi " << j << " belongs to cluster: " << k << std::endl;
						for (int l = 0; l < StripReco->GetEntriesFast(); l++){
							PndSdsHit* myStripHit = (PndSdsHit*)StripReco->At(l);
							if (myStripHit->GetRefIndex() == k){
								std::cout << "StripHit: " << l << std::endl;
								myStripHit->Print();
							}
						}
					}
				}
			}
		}
	}
  return 0;

}
