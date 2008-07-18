{
int n=cbmsim->GetEntries();

TClonesArray *twoe=new TClonesArray("TLorentzVector");
cbmsim->SetBranchAddress("V0Mom",&twoe);

TClonesArray *twopi=new TClonesArray("TLorentzVector");
cbmsim->SetBranchAddress("TwoPionMom",&twopi);

TClonesArray *twopivert=new TClonesArray("TVector3");
cbmsim->SetBranchAddress("TwoPionVertex",&twopivert);

TClonesArray* twoevert=new TClonesArray("TVector3");
cbmsim->SetBranchAddress("V0Vertex",&twoevert);


TH1D* h=new TH1D("h","4 particle mass",30,3,4);
TH1D* hpz=new TH1D("hpz","4 particle pz",40,-0.1,0.1);

for(int i=0;i<n;++i){
  cbmsim->GetEntry(i);
  int n2e=twoe->GetEntriesFast();
  int n2pi=twopi->GetEntriesFast();
  for(int i2e=0;i2e<n2e;++i2e){
    TLorentzVector* twoemom=(TLorentzVector*)twoe->At(i2e);
    TVector3* twoevertex=(TVector3*)twoevert->At(i2e);
    if(fabs(twoevertex->Mag())>2)continue;
    if(fabs(twoemom.M()-3.0969)>0.2)continue;
    for(int i2pi=0;i2pi<n2pi;++i2pi){
      TVector3* twopivertex=(TVector3*)twopivert->At(i2pi);
      if(fabs(twopivertex->Mag())>2)continue;
      if(fabs(twoevertex->Perp()-twopivertex->Perp())>0.5)continue;
      TLorentzVector* twopimom=(TLorentzVector*)twopi->At(i2pi);
      TLorentzVector p4part=*twoemom+*twopimom;
      hpz->Fill(p4part.Z());
      if(fabs(p4part.Z())>0.05)continue;
      std::cout<<p4part.M()<<std::endl;
      h->Fill(p4part.M());
    }
  }
}

h->Draw();
}
