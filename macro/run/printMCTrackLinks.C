int printMCTrackLinks()
{
	TFile* f = new TFile("reco_complete.root");
	TTree* t = (TTree*)f->Get("cbmsim");
	TClonesArray* tracks = new TClonesArray("PndTrack");
	t->SetBranchAddress("SttMvdGemGenTrack", &tracks);
	for (int event = 0; event < t->GetEntriesFast(); event++){
		t->GetEntry(event);
		for (int index = 0; index < tracks->GetEntriesFast(); index++){
			PndTrack* myTrack = (PndTrack*)tracks->At(index);
			//if (linkData.GetNLinks() > 0) cout << "*** 2 MCTracks found ***" << endl;
			cout << event << "/" << index << " : " << *myTrack << endl;
		}
	}
	return 0;
}
