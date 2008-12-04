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
  TCalign* a = TCalign::getInstance("alignment/simRealAlign.txt");
  a->clear();
  a->read("alignment/simRealAlign.txt");
  a->getConv(id,trans,rot,pitch,theta,d1,d2,res);
  //cout<<"x "<<trans[0]<<" y: "<<trans[1]<<" z: "<<trans[2]<<endl;
  double x=t->getAx()*trans.Z()+t->getBx();
  double y=t->getAy()*trans.Z()+t->getBy();

  TVector3 globPos(x,y,trans.Z());
  TVector3 pos = a->XYZtoUVW(id,globPos);
  // cout<<"x glob:"<<globPos[0]<<" y glob: "<<globPos[1]<<" z glob: "<<globPos[2]<<endl;
  //cout<<"u "<<pos[0]<<" v: "<<pos[1]<<" w: "<<pos[2]<<endl;
  TRandom3 rand2(0);
  double smear = rand2.Gaus(0.,res);
  pos.SetX(pos.X()+smear);
  pos.SetY(0.);
  pos.SetZ(0.);
  TVector3 err(res,100.,100.);
  TCcluster c(pos,err,1.,id);
  c.setFit();
  return c;
}

void makeStripTree(){
  TCtrack *outTr = new TCtrack();
  TFile* rootOutfile = new TFile("out2.root","RECREATE");
  TTree* outTree = new TTree("at","test code for strip detectors");
  outTree->Branch("track","TCtrack",&outTr,32000,99);

  

  int nTracks = 10000;

  TRandom3 rand(0);

  for(int i=0;i<nTracks;++i){
    outTr->clear();

    TVector3 pos1,pos2;
    //pos1.SetXYZ(0.,0.,0.);
    //pos2.SetXYZ(1.,2.,100.);
    
    pos1.SetX(rand.Uniform(15.161,15.443));
    pos1.SetY(rand.Uniform(21.079,21.362));
    pos1.SetZ(rand.Uniform(28.64,28.66));
    pos2.SetX(rand.Uniform(21.079,21.362));
    pos2.SetY(rand.Uniform(28.64,28.66));
    pos2.SetZ(rand.Uniform(171.0,171.1));
       
    TVector3 mom=pos2-pos1;
    mom.SetMag(1.);
    double ax,bx,ay,by;

    ax=mom.X()/mom.Z();
    ay=mom.Y()/mom.Z();    
    double t=(0.-pos1.Z())/mom.Z();
    bx=pos1.X()+t*ax;
    by=pos1.Y()+t*ay;
    
    outTr->setPar(ax,bx,ay,by);
    //cout<<"hei"<<endl;
    for(int j=1;j<=8;++j){
      TCcluster c = makeCluster(outTr,j);
      outTr->addCluster(c);
    }
    //"************* ---------------------------------------- *************************"<<endl;
    //    outTr->print();

    TCalign* a = TCalign::getInstance("alignment/simRealAlign.txt");
    a->clear();
    a->read("alignment/AlignmentOut.txt");
    outTr->fit(1,2,3,4,5,6,7,8);
    //outTr->print();
    cout<<endl<<endl;
    outTree->Fill();    
    
  }
  rootOutfile->Write();
}
