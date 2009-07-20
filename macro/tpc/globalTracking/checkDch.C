void checkDch(TString filename="mc.reco.root"){

  TFile::Open(filename);

  TTree* t = (TTree*)gROOT->FindObject("cbmsim");
  t->Print();

  TClonesArray* dchTrack = 0;
  TClonesArray* dchTrackMatch = 0;
  TClonesArray* trackCand = 0;
  TClonesArray* mvdTrackCand = 0;
  TClonesArray* gemTrackCand = 0;

  t->SetBranchAddress("PndDchTrack",&dchTrack);
  t->SetBranchAddress("PndDchTrackMatch",&dchTrackMatch);
  t->SetBranchAddress("DchPndTrackCand",&trackCand);
  t->SetBranchAddress("MVDIdealTrackCand",&mvdTrackCand);
  t->SetBranchAddress("GemPndTrackCand",&gemTrackCand);

  int nentr = t->GetEntriesFast();
  std::cout << "ROOT tree with nentr=" << nentr << std::endl;
  for(int ientr=0;ientr<nentr;++ientr){
    t->GetEntry(ientr);
    int nTrack = dchTrack->GetEntriesFast();
    int nTrackMatch = dchTrackMatch->GetEntriesFast();
    int nTrackCand = trackCand->GetEntriesFast();
    int nMvdTrackCand = mvdTrackCand->GetEntriesFast();
    int nGemTrackCand = gemTrackCand->GetEntriesFast();
    std::cout << "========= event #" << ientr << " with nTrack|nTrackMatch|nTrackCand|nMvdTrackCand|nGemTrackCand="
	      << nTrack << "|" << nTrackMatch
	      << "|" << nTrackCand 
	      << "|" << nMvdTrackCand << "|" 
	      << nGemTrackCand << std::endl; 
    for(unsigned int itr=0;itr<nTrack;++itr){
      //std::cout << "Track #" << itr << std::endl;
      //((PndDchTrack*)dchTrack->At(itr))->Print();
    }
    for(unsigned int itrm=0;itrm<nTrackMatch;++itrm){
      //std::cout << "TrackMatch #" << itrm << std::endl;
      //((PndDchTrackMatch*)dchTrackMatch->At(itrm))->Print();
    }


    for(unsigned int itrc=0;itrc<nTrackCand;++itrc){
      std::cout << "DCHTrackCand #" << itrc << std::endl;
      ((PndTrackCand*)trackCand->At(itrc))->Print();
    }
    for(unsigned int itrc=0;itrc<nMvdTrackCand;++itrc){
      std::cout << "MVDTrackCand #" << itrc << std::endl;
      ((TrackCand*)mvdTrackCand->At(itrc))->print();
    }
    for(unsigned int itrc=0;itrc<nGemTrackCand;++itrc){
      std::cout << "GEMTrackCand #" << itrc << std::endl;
      ((PndTrackCand*)gemTrackCand->At(itrc))->Print();
    }
 }

}
