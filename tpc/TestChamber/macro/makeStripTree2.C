#include"TVector3.h"
#include"TMatrixT.h"
#include"TFile.h"
#include"TTree.h"
#include"TRandom3.h"
#include"src/TCcluster.h"
#include"src/TCtrack.h"
#include"src/TCevent.h"
#include"src/StdoutKiller.h"


TCcluster makeCluster(TCtrack *t,int id){
  TVector3 trans;
  TMatrixT<double> rot(3,3);
  double pitch,theta,d1,d2,res;
  TCalign* a = TCalign::getInstance("alignment/simRealAlign1.txt");
  a->clear();
  a->read("alignment/simRealAlign1.txt");
  a->getConv(id,trans,rot,pitch,theta,d1,d2,res);
  double x=t->getAx()*trans.Z()+t->getBx();
  double y=t->getAy()*trans.Z()+t->getBy();

  TVector3 globPos(x,y,trans.Z());
  TVector3 pos = a->XYZtoUVW(id,globPos);
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

void makeStripTree(string trackfile,int nTracks, int nEv){
  TCevent* outEv = new TCevent();

  TFile* rootOutfile = new TFile(trackfile.c_str(),"RECREATE");
  TTree* outTree = new TTree("at2","test code for strip detectors");
  outTree->Branch("event","TCevent",&outEv,32000,99);

  TRandom3 rand(0);
  for(int j=0;j<nEv;++j){
    outEv->clear();
    for(int i=0;i<nTracks;++i){
      TCtrack* outTr = new TCtrack();
      
      TVector3 pos1,pos2;
      
      pos1.SetX(rand.Uniform(13.890,16.716));
      pos1.SetY(rand.Uniform(21.079,21.618));
      pos1.SetZ(rand.Uniform(28.14,28.15));
      
      pos2.SetX(rand.Uniform(13.688,16.506));
      pos2.SetY(rand.Uniform(21.079,21.618));
      pos2.SetZ(rand.Uniform(166.6,166.5));
      
      TVector3 mom=pos2-pos1;
      mom.SetMag(1.);
      double ax,bx,ay,by;
      
      ax=mom.X()/mom.Z();
      ay=mom.Y()/mom.Z();    
      bx=pos1.X()-pos1.Z()*ax;
      by=pos1.Y()-pos1.Z()*ay;
      cout<<"ax "<<ax<<" bx "<<bx<<" ay "<<ay<<" by "<<by<<endl;
      TCalign* a = TCalign::getInstance("alignment/AlignmentFiles/simRealAlign1.txt");
      a->clear();
      a->read("alignment/AlignmentFiles/simRealAlign1.txt");
  
      
      outTr->setPar(ax,bx,ay,by);
      for(int k=1;k<=8;++k){
        TCcluster c = makeCluster(outTr,k);
        outEv->addCluster(c);
        outTr->addCluster(c);
        double res=a->getRes(k);
        if(i>2&&i<7){
            TVector3 pos(rand.Uniform(0,2),0,0);
            TVector3 err(res,100,100);
            TCcluster c(pos,err,1.,k);
            outEv->addCluster(c);
        }else if(i<3||i>6){
            TVector3 pos(rand.Uniform(0,10),0,0);
            TVector3 err(res,100,100);
            TCcluster c(pos,err,1.,k);
            outEv->addCluster(c);
        }
        
      }

      outEv->addTrack(outTr);
      
      if(j%100==0){
        cout<<j<<endl;
      }
     
    }
    outTree->Fill();    
  }
    rootOutfile->Write();
}
