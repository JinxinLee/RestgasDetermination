#include"TVector3.h"
#include"TMatrixT.h"
#include"TFile.h"
#include"TTree.h"
#include"TRandom3.h"
#include"src/TCcluster.h"
#include"src/TCtrack.h"


TCcluster makeCluster(TCtrack *t,int id){
  TVector3 trans;
  TMatrixT<double> rot(3,3);
  double pitch,theta,d1,d2,res;
  TCalign* a = TCalign::getInstance("alignment/simAlign.txt");
  a->getConv(id,trans,rot,pitch,theta,d1,d2,res);
  double x=t->getAx()*trans.Z()+t->getBx();
  double y=t->getAy()*trans.Z()+t->getBy();
  TVector3 globPos(x,y,trans.Z());
  TVector3 pos = a->XYZtoUVW(id,globPos);
  TRandom3 rand2(0);
  double smear = rand2.Gaus(0.,res);
  pos.SetX(pos.X()+smear);
  TVector3 err(res,100.,100.);
  TCcluster c(pos,err,1.,id);
  c.setFit();
  return c;
}

void makeStripTree(){
  TCtrack *outTr = new TCtrack();
  TFile* rootOutfile = new TFile("out.root","RECREATE");
  TTree* outTree = new TTree("at","test code for strip detectors");
  outTree->Branch("track","TCtrack",&outTr,32000,99);

  

  int nTracks = 1;

  TRandom3 rand(0);

  for(int i=0;i<nTracks;++i){
    outTr->clear();

    TVector3 pos1,pos2;
    pos1.SetXYZ(0.,0.,0.);
    pos2.SetXYZ(1.,2.,100.);
    
    /* 
       pos1.SetX(rand.Uniform(-1.,1.));
       pos1.SetY(rand.Uniform(-1.,1.));
       pos1.SetZ(rand.Uniform(0.,0.1));
       pos2.SetX(rand.Uniform(-1.,1.));
       pos2.SetY(rand.Uniform(-1.,1.));
       pos2.SetZ(rand.Uniform(100.,100.1));
    */
    TVector3 mom=pos2-pos1;
    mom.SetMag(1.);
    double ax,bx,ay,by;

    ax=mom.X()/mom.Z();
    ay=mom.Y()/mom.Z();    
    double t=(0.-pos1.Z())/mom.Z();
    bx=pos1.X()+t*mom.X();
    by=pos1.Y()+t*mom.Y();
    
    outTr->setPar(ax,bx,ay,by);

    for(int i=1;i<=4;++i){
      TCcluster c = makeCluster(outTr,i);
      outTr->addCluster(c);
    }
    outTr->print();
    outTr->fit(1,2,3,4);
    outTr->print();
    outTree->Fill();    

  }
  rootOutfile->Write();
}
