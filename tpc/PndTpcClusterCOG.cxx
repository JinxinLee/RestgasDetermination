//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcClusterCOG
//      see PndTpcClusterCOG.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcClusterCOG.h"

// C/C++ Headers ----------------------
#include "PndTpcCluster.h"
#include "TVector3.h"
#include "TMatrixD.h"

// Collaborating Class Headers --------
#include "PndTpcDigi.h"
#include "McIdCollection.h"
#include "McId.h"
#include "PndTpcDigiMapper.h"

// Class Member definitions -----------



PndTpcCluster*
PndTpcClusterCOG::cog(std::vector<PndTpcDigi*>* digis){
  
  TVector3 pos(0,0,0);
  double amp=0;
  TVector3 sig(0,0,0);
  unsigned int id=0;
  McIdCollection mcid;
  unsigned int ndigis=digis->size();
  for(;id<ndigis;++id){ // Loop over digis
    PndTpcDigi* adigi=(*digis)[id];
    mcid.AddIDCollection(adigi->mcId());
    double a=(double)adigi->amp();
    TVector3 thispos;
    PndTpcDigiMapper::getInstance()->map(adigi,thispos);
    double dx;
    double dy;
    PndTpcDigiMapper::getInstance()->padsize(adigi->padId(),dx,dy);
    
    McId dummyID(1,1,0);
    McIdCollection dummyColl;
    dummyColl.AddID(dummyID);
    
    //this block is to define the z jitter
    TVector3 zDiff1,zDiff2;
    PndTpcDigi zDiffDigi1(1,1,1,dummyColl),zDiffDigi2(1,2,1,dummyColl);
    PndTpcDigiMapper::getInstance()->map(&zDiffDigi1,zDiff1);
    PndTpcDigiMapper::getInstance()->map(&zDiffDigi2,zDiff2);
    double zDiff = zDiff2.z() - zDiff1.z();
    //end of z jitter
    
    double Dl = PndTpcDigiMapper::getInstance()->getGas()->Dl();
    double Dt = PndTpcDigiMapper::getInstance()->getGas()->Dt();
    
    double driftl=thispos.z()-PndTpcDigiMapper::getInstance()->zGem();
    assert(driftl>=0);
    
    double diffSigmaL = Dl * sqrt(driftl);
    double diffSigmaT = Dt * sqrt(driftl);
    double sigmaX_sq = dx*dx/12. + diffSigmaT*diffSigmaT;
    double sigmaY_sq = dy*dy/12. + diffSigmaT*diffSigmaT;
    double sigmaZ_sq = zDiff*zDiff/12. + diffSigmaL*diffSigmaL;
    
    TVector3 thissig(sigmaX_sq,sigmaY_sq,sigmaZ_sq);
    sig+=a*a*thissig;
    
    
    pos+=a*thispos;
    amp+=a;
  }
  pos*=1./amp;
  sig.SetX(sqrt(sig.X())/amp);
  sig.SetY(sqrt(sig.Y())/amp);
  sig.SetZ(sqrt(sig.Z())/amp);
  
  PndTpcCluster* cl=new PndTpcCluster(pos,sig,amp,id,ndigis);
  cl->SetMcId(mcid);

  // loop again over the digis to calculate 2nd moment
  TMatrixD cov(3,3);
  for(int iid=0;iid<ndigis;++iid){
    PndTpcDigi* adigi=(*digis)[iid];
    TVector3 thispos;
    PndTpcDigiMapper::getInstance()->map(adigi,thispos);
    TMatrixD c(3,1);
    for(int i=0; i<3; ++i)c[i][0]=thispos[i]-pos[i];
    TMatrixD c_t(TMatrixD::kTransposed,c);
    TMatrixD acov(c,TMatrixD::kMult,c_t);
    cov+=acov;
  }
  cov*=1./(double)ndigis;
  cov.Print();
  cl->SetCov(cov);

  return cl;
}
