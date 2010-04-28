void Hough(int i)
{ 
gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	gSystem->Load("libriemann.C");
	
	TString MCFile = "Mvd_Test.root";
	
	PndFileNameCreator creator(MCFile.Data());
	TString DigiFile = creator.GetDigiFileName(false).c_str();
	TString RecoFile = creator.GetRecoFileName(false).c_str();
	TString TrackFFile = creator.GetTrackFindingFileName(false).c_str();
	
	TH2D h2("h2","h2", 200,0,TMath::Pi(), 200,-TMath::Pi(), TMath::Pi());
	
	TFile* fReco = new TFile(RecoFile.Data());
	
	
	TTree* t = (TTree*)(fReco->Get("cbmsim"));
	
	t->StartViewer();
	
	TClonesArray* PixReco = new TClonesArray("PndMvdHit");
	TClonesArray* StripReco = new TClonesArray("PndMvdHit");
	
	t->SetBranchAddress("MVDHitsPixel", &PixReco);
	t->SetBranchAddress("MVDHitsStrip", &StripReco);
	
	t->GetEntry(i);

	for (int l = 0; l < PixReco->GetEntriesFast(); l++)
	{
		PndMvdHit* myPixHit = (PndMvdHit*)PixReco->At(l);
		TVector3 hit;
		myPixHit->Position(hit);
		h2.Fill(hit.Theta(), hit.Phi());
		std::cout << "Pixel " << l << " : ";
		//myPixHit->Print();
		std::cout << "Theta: " << hit.Theta() << " Phi: " << hit.Phi() << std::endl;
		
	}

	for (int l = 0; l < StripReco->GetEntriesFast(); l++)
	{
		PndMvdHit* myStripHit = (PndMvdHit*)StripReco->At(l);
		TVector3 striphit;
		myStripHit->Position(striphit);
		h2.Fill(striphit.Theta(), striphit.Phi());
		std::cout << "Strip " << l << " : ";
		//myStripHit->Print();
		std::cout << "Theta: " << striphit.Theta() << " Phi: " << striphit.Phi() << std::endl;
	}
	gStyle->SetPalette(1);
	h2.DrawCopy("colz");
}