{

TH2D* ph=new TH2D("ph","p steps",100,0,100,1000,1.5,2.5);

TClonesArray* tracks=new TClonesArray("Track");
cbmsim->SetBranchAddress("Track",&tracks);

int n=cbmsim->GetEntriesFast();
for(int i=0;i<n;++i){
  cbmsim->GetEntry(i);
  int ntr=tracks->GetEntriesFast();
  for(int itr=0;itr<ntr;++itr){
    Track* trk=(Track*)tracks->At(itr);
    FitParams* param=trk->getCardinalRep()->getFitParams();
    int np=param->getN();
    for(int ip=0;ip<np;++ip){// loop over params
      TMatrixD state;
      param->getfStatePred(ip,state);
      ph->Fill(ip,state[0][0]);
    }
  }// end loop over tracks


}// end loop over events

ph->GetXaxis()->SetTitle("Kalman Step");
ph->GetYaxis()->SetTitle("q/p");
ph->Draw();


}
