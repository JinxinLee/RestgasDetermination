
int drawTracks(int eventNr, TTree* t, int track = -1)
{
	TClonesArray* tracks = new TClonesArray("TGeoTrack");
	TClonesArray* hits = new TClonesArray("PndSdsMCPoint");
	t->SetBranchAddress("MVDPoint",&hits);
	t->SetBranchAddress("GeoTracks", &tracks);
	
	t->GetEntry(eventNr);
	map<int,int> trackIDCount;
	
	for (int i = 0; i < hits->GetEntriesFast(); i++){
		PndSdsMCPoint* myPoint = (PndSdsMCPoint*)(hits->At(i));
		trackIDCount[myPoint->GetTrackID()]++;
		//std::cout << "Point: " <<  << " " << myPoint->GetTrackID() << std::endl;
		myPoint->Print();
	}
	if (track == -1){
		for (map<int,int>::const_iterator ci = trackIDCount.begin(); ci!= trackIDCount.end(); ci++){
			TGeoTrack* myTrack = (TGeoTrack*)(tracks->At(ci->first));
			std::cout << "Track " << ci->first << " : " << ci->second << std::endl;
			myTrack->Draw("");
		}
	}
	else{
		map<int,int>::const_iterator c2 = trackIDCount.begin();
		for (int j = 0; j < track; j++){
			c2++;
		}
		TGeoTrack* myTrack = (TGeoTrack*)(tracks->At(c2->first));
		std::cout << "Track " << c2->first << " : " << c2->second << std::endl;
		myTrack->Draw("");
	}
  return 0;
}
