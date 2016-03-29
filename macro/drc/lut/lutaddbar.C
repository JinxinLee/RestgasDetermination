TClonesArray *fLutSum[5];

void lutaddbar(TString inFile = "lut_one_*.root", TString outFile = "lut_all.root"){

  TTree *fTreeNew = new TTree("dircsim","Look-up table for DIRC");
  for(Int_t l=0; l<5; l++){
    fLutSum[l] = new TClonesArray("PndDrcLutNode");
    fTreeNew->Branch(Form("LUT%d",l),&fLutSum[l],256000,0); 
  }

  Int_t Nnodes = 30000;
  for(Int_t l=0; l<5; l++){
    TClonesArray &fLutaSum = *fLutSum[l];
    for (Long64_t n=0; n<Nnodes; n++) {
      new((fLutaSum)[n]) PndDrcLutNode(-1);
    }
  }

  if(inFile.Contains("*")){
    TString tname = inFile;
    TString tname1 = inFile;
    TString tname2 = inFile;
    TString end = tname1.Remove(0,inFile.Last('*')+1);
    TString start = tname2.Remove(inFile.Last('*'));

    TString dirname= tname.Remove(inFile.Last('/')) + "/";

    const char *ext=".root";
    TSystemDirectory dir(dirname, dirname);
    TList *files = dir.GetListOfFiles();
    if (files) {
      TSystemFile *file;
      TString fname;
      TIter next(files);
      while ((file=(TSystemFile*)next())) {
	fname = file->GetName();
	if (!file->IsDirectory() && fname.EndsWith(ext)) {
	  TString path = dirname+fname;
	  TString substr = path.SubString(start);
	  if( substr.Length()>0 && path.EndsWith(end)){
	    adddirs(path);
	  }
	}
      }
    }
  }else{
    adddirs(inFile);
  }
 
  TFile *fFileNew = TFile::Open(outFile,"RECREATE");
  fTreeNew->Fill();
  fTreeNew->Write();
  fFileNew->Write();
  std::cout<<"File  "<<outFile<<" was created." <<std::endl;
  
}


void adddirs(TString filename){
  TFile* f = new TFile(filename);
  TTree *t=(TTree *) f->Get("dircsim") ;
  TClonesArray* fLut[5];
  for(Int_t l=0; l<5; l++){
    fLut[l] = new TClonesArray("PndDrcLutNode");
    t->SetBranchAddress(Form("LUT%d",l),&fLut[l]); 
  }
  t->GetEntry(0);
  std::cout<<filename<<" has "<<fLut[0]->GetEntriesFast()<< " entries" <<std::endl;
  for(Int_t l=0; l<5; l++){
    for (Int_t inode=0; inode<fLut[l]->GetEntriesFast(); inode++){
      if(inode%10000==0) std::cout<<"Entry # "<< inode <<std::endl;
      PndDrcLutNode *node= (PndDrcLutNode*) fLut[l]->At(inode);
      for(int i=0; i< node->Entries(); i++){
	((PndDrcLutNode*)(fLutSum[l]->At(inode)))->AddEntry(node->GetDetectorId(), node->GetEntry(i),node->GetPath(i),node->GetPathId(i),node->GetTime(i),node->GetPos());
      }
    }
 }

  f->Close();
}
