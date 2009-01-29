#include"TCevent.h"
#include"TFile.h"
#include"TTree.h"


int main(){
  TCevent *e=NULL;
  e = new TCevent;

  TFile* outfile = TFile::Open("out.root","RECREATE");
  TTree *tree = new TTree("tree","");
  tree->Branch("event branch","TCevent",&e,32000,99);

  for(int i=0;i<20;++i){
    e->clear();
    std::vector<TCcluster*> clusters;
    TCcluster* cl;
    cl = new TCcluster; cl->x=1.5+i*10;cl->y=1.6+i*10;clusters.push_back(cl);
    cl = new TCcluster; cl->x=2.5+i*10;cl->y=2.6+i*10;clusters.push_back(cl);
    cl = new TCcluster; cl->x=3.+i*10;cl->y=3.+i*10;clusters.push_back(cl);
    cl = new TCcluster; cl->x=4.+i*10;cl->y=4.+i*10;clusters.push_back(cl);
    cl = new TCcluster; cl->x=5.+i*10;cl->y=5.+i*10;clusters.push_back(cl);
    cl = new TCcluster; cl->x=6.+i*10;cl->y=6.+i*10;clusters.push_back(cl);
    e->addClusters(clusters);
    TCtrack* tr;
    tr=new TCtrack;
    tr->index.push_back(0);
    tr->pointers.push_back(clusters.at(0));
    tr->index.push_back(2);
    tr->pointers.push_back(clusters.at(2));
    tr->index.push_back(4);
    tr->pointers.push_back(clusters.at(4));
    e->addTrack(tr);
    tr = new TCtrack;
    tr->index.push_back(1);
    tr->pointers.push_back(clusters.at(1));
    tr->index.push_back(3);
    tr->pointers.push_back(clusters.at(3));
    tr->index.push_back(5);
    tr->pointers.push_back(clusters.at(5));
    e->addTrack(tr);
    tree->Fill();
  }

  tree->Write();
  outfile->Write();
  outfile->Close();
}
