#include "PndTpcClusterFitter.h"

using namespace std;

PndTpcClusterFitter::PndTpcClusterFitter()
 : fLikeli(20,0.005,0.5,1,0.02,0.01)
{
}



PndTpcCluster*
PndTpcClusterFitter::doFit(const vector<PndTpcDigi*>& digis){
  // do mapping

  // McId dummyID(1,1);
  // McIdCollection dummyColl;
  // dummyColl.AddID(dummyID);
  
  // //this block is to define the z jitter
  // TVector3 zDiff1,zDiff2;
  // PndTpcDigi zDiffDigi1(1,1,1,dummyColl),zDiffDigi2(1,2,1,dummyColl);
  // PndTpcDigiMapper::getInstance()->map(&zDiffDigi1,zDiff1);
  // PndTpcDigiMapper::getInstance()->map(&zDiffDigi2,zDiff2);
  // double zDiff = zDiff2.z() - zDiff1.z();
  //end of z jitter

  // TVector3 pos(0,0,0);
  // double amp=0;
  // unsigned int id=0;
  // McIdCollection mcid;
  // unsigned int ndigis=digis.ize();
  // map<unsigned int,unsigned int> padmap; // padmap[1]=how often pad 1 has been hit
  // double dx;
  // double dy;
  // for(;id<ndigis;++id){
  //   PndTpcDigi* adigi=(*digis)[id];
  //     double a=(double)adigi->amp();
  //     padmap[adigi->padId()]+=1;
  //     mcid.AddIDCollection(adigi->mcId(),a);
  //     TVector3 thispos;
  //     PndTpcDigiMapper::getInstance()->map(adigi,thispos);
  //     if(DEBUG) {
  // 	std::cout<<"PndTpcSectorProcessor: Digi position: ";
  // 	thispos.Print();
  //     }
  //     PndTpcDigiMapper::getInstance()->padsize(adigi->padId(),dx,dy);
  //     if(DEBUG) {
  // 	std::cout<<"PndTpcSectorProcessor: Got PadSize dx: "
  // 		 <<dx<<", dy: "<<dy<<std::endl;
  //     }
  //      pos+=a*thispos;
  //     amp+=a;
  //   } // end loop over digis in cluster
  //   pos*=1./amp;

  //   // calculate errors: ------------------------------------------
  //   double Dl = PndTpcDigiMapper::getInstance()->getGas()->Dl();
  //   double Dt = PndTpcDigiMapper::getInstance()->getGas()->Dt();
  //   double driftl=pos.z()-PndTpcDigiMapper::getInstance()->zGem();
     
  //   if(DEBUG) {
  //     std::cout<<"PndTpcSectorProcessor: Gas DiffL: "<<Dl 
  // 	       <<", Gas DiffT: "<<Dt<<std::endl;
  //   }
  //   if(DEBUG) {
  //     std::cout<<"PndTpcSectorProcessor: zGem is "
  // 	       <<PndTpcDigiMapper::getInstance()->zGem()<<std::endl
  // 	       <<", drift length: "<<driftl<<std::endl;
  //   }
    
  //   double absdriftl=fabs(driftl);
  //   double diffSigmaL = Dl * Dl * absdriftl;
  //   double diffSigmaT = Dt * Dt * absdriftl;
  //   TVector3 sig(0,0,0);
   
    
  //   for(id=0;id<ndigis;++id){
  //     PndTpcDigi* adigi=(*digis)[id];
  //     double a=(double)adigi->amp();
  //      TVector3 thispos;
  //     PndTpcDigiMapper::getInstance()->map(adigi,thispos);
  //     TVector3 df=thispos-pos;

  //     double sigmaX_sq = a*df.X()*df.X();
  //     double sigmaY_sq = a*df.Y()*df.Y();
  //     double sigmaZ_sq = a*df.Z()*df.Z();
    
  //   TVector3 thissig(sigmaX_sq,sigmaY_sq,sigmaZ_sq);
  //   sig+=thissig;
    
  //   } // end second loop over digis

  //   //if(ndigis==1 && sig.Mag()>1E-5)sig.Print();

  //   if(sig.X()<1E-5)sig.SetX(sqrt(dx*dx/12+diffSigmaT));
  //   else sig.SetX(sqrt((sig.X()+fG*diffSigmaT)/amp)*fC/amp);
  //   if(sig.Y()<1E-5)sig.SetY(sqrt(dy*dy/12+diffSigmaT));
  //   else sig.SetY(sqrt((sig.Y()+fG*diffSigmaT)/amp)*fC/amp);
  //   if(sig.Z()<1E-5)sig.SetZ(sqrt(zDiff*zDiff/12+diffSigmaL));
  //   else sig.SetZ(sqrt((sig.Z()+fG*diffSigmaL)/amp)*fC/amp);
  //   if(DEBUG && ndigis==1){
  //     sig.Print();
  //   }
  //   PndTpcCluster* cl=new PndTpcCluster(pos,sig,amp,ic,ndigis);
  //   mcid.Renormalize();
  //   cl->SetMcId(mcid);
  //   cl->nPad(padmap.size());
  //   //set link (temporary solution)
  //   //if(!fDataMode)
  //   //cl->SetLink(FairLink("MCTrack", mcid.DominantID().mctrackID()));

  //   // loop again over the digis to calculate 2nd moment
  //   TMatrixD cov(3,3);
  //   for(id=0;id<ndigis;++id){
  //     PndTpcDigi* adigi=(*digis)[id];
  //     TVector3 thispos;
  //     PndTpcDigiMapper::getInstance()->map(adigi,thispos);
  //     TMatrixD c(3,1);
  //     for(int i=0; i<3; ++i)c[i][0]=thispos[i]-pos[i];
  //     TMatrixD c_t(TMatrixD::kTransposed,c);
  //     TMatrixD acov(c,TMatrixD::kMult,c_t);
  //     cov+=acov;
      
  //     cl->addDigi(adigi);
  //   }
  //   cov*=1./(double)ndigis;
  //   cl->SetCov(cov);

  //   foutput_buffer->push_back(cl);
  return 0;

}
