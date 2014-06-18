void lutmean(TString baseFile = "lut_all")
{
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gInterpreter->GenerateDictionary("vector<TVector3>","TVector3.h"); 
  
  TString inFile =baseFile+".root";
  TString outFile =baseFile+"_avr.root";

  TFile* f = new TFile(inFile);
  TTree *t=(TTree *) f->Get("dircsim") ;
  TClonesArray* fLut[5];
  for(Int_t l=0; l<5; l++){
    fLut[l] = new TClonesArray("PndDrcLutNode");
    t->SetBranchAddress(Form("LUT%d",l),&fLut[l]); 
  }
  t->GetEntry(0);

 
  TFile *fFileNew = TFile::Open(outFile, "RECREATE");
  TClonesArray *fLutNew[5];

  TTree *fTreeNew = new TTree("dircsim","Look-up table for DIRC. Averaged");
  for(Int_t l=0; l<5; l++){
    fLutNew[l] = new TClonesArray("PndDrcLutNode");
    fTreeNew->Branch(Form("LUT%d",l),&fLutNew[l],256000,0); 
  }

  Int_t Nnodes = 30000;
  for(Int_t l=0; l<5; l++){
    TClonesArray &fLutaNew = *fLutNew[l];
    for (Long64_t n=0; n<Nnodes; n++) {
      new((fLutaNew)[n]) PndDrcLutNode(-1);
    }
  }

  // TCanvas* c = new TCanvas("c","c",0,0,800,1200); c->Divide(1,2);
  // TH1F * histNode = new TH1F("LutNode","Node vs Multiplicity",30000,0,150000);
  // TH1F * hTime = new TH1F("hTime","Time",5000,0,10);
  // TH1F * hDir = new TH1F("hDir","X component",1000,-1,1);


  std::vector<TVector3> vArray[100];
  std::vector<Double_t> tArray[100];
  std::vector<Double_t> pArray;
  
  TVector3 dir, dir2, sum;
  Double_t angle, minangle,pathid,time,sumt;
  PndDrcLutNode *node;

  for(Int_t l=0; l<5; l++){
    for (Int_t inode=0; inode<fLut[l]->GetEntriesFast(); inode++){
      if(inode%1000==0) cout<<"Node # "<<inode<<endl;
      node= (PndDrcLutNode*) fLut[l]->At(inode);
      //histNode->Fill(node->GetNodeId(),node->Entries());
      Int_t size = node->Entries();
      if(size<1) continue;
      for(int i=0; i<size; i++){
	dir = node->GetEntry(i);
	time = node->GetTime(i);
	pathid = node->GetPathId(i);
      
	// hDir->Fill(dir.X());
	// hTime->Fill(time);

	bool newid = true;
	for(int j=0; j<pArray.size(); j++){
	  if(pathid == pArray[j]){
	    vArray[j].push_back(dir);
	    tArray[j].push_back(time);
	    newid= false;
	  }
	}
	if(newid) {
	  vArray[pArray.size()].push_back(dir);
	  tArray[pArray.size()].push_back(time);
	  pArray.push_back(pathid);
	}
      }
  
      for(int j=0; j<pArray.size(); j++){
	sum = TVector3(0,0,0);
	sumt=0;
	for(int v=0; v<vArray[j].size(); v++) {
	  sum += vArray[j][v]; 
	  sumt += tArray[j][v]; 

	  // hDir->Fill(vArray[j][v].X());
	  // hTime->Fill(tArray[j][v]);
	}
      
	// c->cd(1);
	// hTime->Draw();
	// c->cd(2);
	// hDir->Draw();
	// c->Update();  
	// c->WaitPrimitive();
	// hDir->Reset();
	// hTime->Reset();

	if(vArray[j].size()<5) continue;
	sum *= 1/(Double_t)vArray[j].size();
	sumt *= 1./(Double_t)tArray[j].size();
      
	((PndDrcLutNode*)(fLutNew[l]->At(inode)))->AddEntry(node->GetDetectorId(), sum,j,sumt, node->GetPos()); 
      }
      for(int i=0; i<100; i++) {vArray[i].clear();  tArray[i].clear();}
      pArray.clear();
    }
  }

  fTreeNew->Fill();
  fTreeNew->Write();
  fFileNew->Write();

}
