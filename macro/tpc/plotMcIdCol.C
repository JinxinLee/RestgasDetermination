plotMcIdCol(TString name, double goodcut=0.8){

  TClonesArray* cl=new TClonesArray("PndTpcCluster");
  cbmsim->SetBranchAddress("PndTpcCluster",&cl);

  cbmsim->GetEntry(0);

  TH1D* hMc=new TH1D("hMC","Number of McIds in Cluster",10,0,10);
  TH1D* hMc2=new TH1D("hMC2","Number of McIds in Cluster with domWeight>0.95",10,0,10);
  TH2D* hMcWeight=new TH2D("hMcW","Number of McIds in Cluster vs Dominant Weight",10,0,10,200,0,1.1);

  unsigned int nbins=100;
  map<double,double> goodCl;
  for(unsigned int ib=0;ib<nbins;++ib){
    double frac=1./(double)nbins * ib;
    goodCl[frac]=0;
  }
  TGraph* gPE=new TGraph(nbins);

  McIdCollection globalCol;

  unsigned int ncl=cl->GetEntries();
  unsigned int good=0;
  for(unsigned int icl=0;icl<ncl;++icl){
    PndTpcCluster* c=(PndTpcCluster*)cl->At(icl);
    hMc->Fill(c->nMcIds()+0.5); 
    globalCol.AddIDCollection(c->mcId());
    map<double,double>::iterator it=goodCl.begin();
    while(it!=goodCl.end()){
      
      if(c->mcId().MaxRelWeight()>it->first){
	it->second=it->second+1;
      }
      ++it;
    }// end loop over bins

    if(c->nMcIds()>1)hMcWeight->Fill(c->nMcIds()+0.5,c->mcId().MaxRelWeight());
  }
  
  cout << "Found " << globalCol.nIDs() << " mcids in clusters" << endl;
  
  

  map<double,double>::iterator it=goodCl.begin();
  unsigned int i=0;
    while(it!=goodCl.end()){
      gPE->SetPoint(i,it->first,it->second/(double)ncl);
		    ++it;++i;
    }


  // hMc->Scale(1./(double)ncl);
  //hMc->Draw();
  //hMc2->Scale(1./(double)ncl);
  //hMc2->Draw("same");
  
  //std::cout << "total fraction of good clusters (Purity>"<<goodcut<<") : " << (double)good/(double)ncl << std::endl;

    gPE->Draw("APL");
    gPE->SaveAs(name);
    

  //hMcWeight->Draw("COLZ");
}
