void EventListing()
{ 
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	gSystem->Load("libriemann.C");
	
	TString MCFile = "Mvd_Test.root";
	TH1D* h1 = new TH1D("h1","PixError",100,-0.01,0.01);
	
	PndMvdFileNameCreator creator(MCFile.Data());
	TString DigiFile = creator.GetDigiFileName(false).c_str();
	TString RecoFile = creator.GetRecoFileName(false).c_str();
	//TString TrackFFile = creator.GetTrackFindingFileName(false).c_str();

	TFile* fMC = new TFile(MCFile.Data());
	TFile* fDigi = new TFile(DigiFile.Data());
	TFile* fReco = new TFile(RecoFile.Data());
	//TFile* fTrack = new TFile(TrackFFile.Data());
	
	TTree* t = (TTree*)(fMC->Get("cbmsim"));

	t->AddFriend("cbmsim", fDigi);
	t->AddFriend("cbmsim", fReco);
	//t->AddFriend("cbmsim", fTrack);
	
	t->StartViewer();
	
	TClonesArray* MCHits = new TClonesArray("PndMvdMCPoint");
	TClonesArray* PixDigis = new TClonesArray("PndMvdDigiPixel");
	TClonesArray* StripDigis = new TClonesArray("PndMvdDigiStrip");
	TClonesArray* PixReco = new TClonesArray("PndMvdHit");
	TClonesArray* StripReco = new TClonesArray("PndMvdHit");
	TClonesArray* PixCluster = new TClonesArray("PndMvdClusterPixel");
	TClonesArray* StripCluster = new TClonesArray("PndMvdClusterStrip");
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
		PndMvdMCPoint* myPoint = (PndMvdMCPoint*)(MCHits->At(i));
		std::cout << "<<<<<<<<<<< MCPoint >>>>>>>>>> " << std::endl;
		myPoint->Print();																		//write out MC info
		
		for (int j = 0; j < PixDigis->GetEntriesFast(); j++){									//get all Digis
			PndMvdDigiPixel* myPixDigi = (PndMvdDigiPixel*)PixDigis->At(j);
			bool dig = false;
			for (int ind = 0; ind < myPixDigi->GetNIndices(); ind++)							//test if digi belongs to MCHit
				if (myPixDigi->GetIndex(ind) == i) dig = true;
			if (dig){
				std::cout << "PixDigi: ";														//write out DigiInfo
				myPixDigi->Print();
				for (int k = 0; k < PixCluster->GetEntriesFast(); k++){							//get all clusters
					PndMvdClusterPixel* myPixCluster = (PndMvdClusterPixel*)PixCluster->At(k);
					if (myPixCluster->DigiBelongsToCluster(j)){									//test if digi belongs to cluster
						std::cout << "Digi " << j << " belongs to cluster: " << k << std::endl; //write out cluster info
						for (int l = 0; l < PixReco->GetEntriesFast(); l++){					//get all RecoHits
							PndMvdHit* myPixHit = (PndMvdHit*)PixReco->At(l);
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
			PndMvdDigiStrip* myStripDigi = (PndMvdDigiStrip*)StripDigis->At(j);
			dig = false;
			for (int ind = 0; ind < myStripDigi->GetNIndices(); ind++)
							if (myStripDigi->GetIndex(ind) == i) dig = true;
			if (dig){
				std::cout << "StripDigi: "; // << myStripDigi;
				myStripDigi->Print();
				for (int k = 0; k < StripCluster->GetEntriesFast(); k++){
					PndMvdClusterStrip* myStripCluster = (PndMvdClusterStrip*)StripCluster->At(k);
					if (myStripCluster->DigiBelongsToCluster(j)){
						std::cout << "Digi " << j << " belongs to cluster: " << k << std::endl;
						for (int l = 0; l < StripReco->GetEntriesFast(); l++){
							PndMvdHit* myStripHit = (PndMvdHit*)StripReco->At(l);
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
	
}