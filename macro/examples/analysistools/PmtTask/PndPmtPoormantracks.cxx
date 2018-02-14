
#include "TMath.h"
#include "TRandom3.h"
#include "TDatabasePDG.h"
//#include "FairRootManager.h"
//#include "FairRunAna.h"
//#include "FairRuntimeDb.h"

#include "RhoCandidate.h"
#include "RhoCalculationTools.h"

#include <string>
#include <iostream>

#include "PndPmtPoormantracks.h"


using std::cout;
using std::flush;
using std::endl;


PndPmtPoormantracks::PndPmtPoormantracks()
{
  fSigVx=0.005; //[cm]
  fSigVy=0.005; //[cm]
  fSigVz=0.005; //[cm]
  fSigPx=0.01; //[GeV/c]
  fSigPy=0.01; //[GeV/c]
  fSigPz=0.01; //[GeV/c]
  fPtMin=0.9;//[GeV/c]
  fPtMax=1.1;//[GeV/c]
  fCharge=1.;
  fDtheta = 1.; //0.3 [pi]
  fPID=211;// 211 pion, 22 muon, 2212 proton, 321 kaon
  fPDG = 0;
  fNumTrk=2;
  fSeed=936650;//
  fVerbose=0;
  fMcCands = new TClonesArray("RhoCandidate");
  fCands = new TClonesArray("RhoCandidate");
}

PndPmtPoormantracks::~PndPmtPoormantracks()
{
}




TVector3 PndPmtPoormantracks::RollVertexBox(double widx,double widy, double widz)
{
  double rnd1=widx*2*(0.5-gRandom->Rndm());
  double rnd2=widy*2*(0.5-gRandom->Rndm());
  double rnd3=widz*2*(0.5-gRandom->Rndm());
  TVector3 vertex(rnd1,rnd2,rnd3);
  if(fVerbose>2) cout<<"RollVertexBox: ("<<vertex.x()<<","<<vertex.y()<<","<<vertex.z()<<")"<<endl;
  return vertex;
}

void PndPmtPoormantracks::SmearVertex(TVector3& vertex)
{
  double rnd1=gRandom->Gaus(vertex.x(),fSigVx);
  double rnd2=gRandom->Gaus(vertex.y(),fSigVy);
  double rnd3=gRandom->Gaus(vertex.z(),fSigVz);
  if(fVerbose>2) cout<<"SmearVertex (B): ("<<vertex.x()<<","<<vertex.y()<<","<<vertex.z()<<")"<<endl;
  vertex.SetXYZ(rnd1,rnd2,rnd3);
  if(fVerbose>2) cout<<"SmearVertex (A): ("<<vertex.x()<<","<<vertex.y()<<","<<vertex.z()<<")"<<endl;
  return;
}

TVector3 PndPmtPoormantracks::RollMomentumBox(const TVector3& vtx, double dtheta, double ptmin, double ptmax)
{
  // allow whole phi range
  double phi=TMath::TwoPi()*gRandom->Rndm();
  // allow theta at vertex pointing theta +- dtheta
  double theta = vtx.Theta() + dtheta*2*(0.5-gRandom->Rndm());
  // allow transverse momentum in [ptmin,ptmax]
  double pt = ptmin + (ptmax-ptmin)*gRandom->Rndm();
  TVector3 momentum;
  momentum.SetPtThetaPhi(pt,theta,phi);
  if(fVerbose>2) cout<<"RollMomentumBox: ("<<momentum.x()<<","<<momentum.y()<<","<<momentum.z()<<")"<<endl;
  return momentum;
}

void PndPmtPoormantracks::SmearMomentum(TVector3& momentum)
{
  double rnd1=gRandom->Gaus(momentum.Px(),fSigPx);
  double rnd2=gRandom->Gaus(momentum.Py(),fSigPy);
  double rnd3=gRandom->Gaus(momentum.Pz(),fSigPz);
  momentum.SetXYZ(rnd1,rnd2,rnd3);
  return;
}

void PndPmtPoormantracks::PoorManTracks()//TVector3& vtx=fVertex
{
  if(fVerbose>1) cout<<" --------------------------------------------------- "<<endl;
  if(fVerbose>0) cout<<"Poor man tracks."<<endl;
  fMcCands->Clear("C");
  fCands->Clear("C");

  RhoError covPos(3), covP4(4);
  covPos[0][0]=fSigVx*fSigVx;
  covPos[1][1]=fSigVy*fSigVy;
  covPos[2][2]=fSigVz*fSigVz;
  covP4[0][0]=fSigPx*fSigPx;
  covP4[1][1]=fSigPy*fSigPy;
  covP4[2][2]=fSigPz*fSigPz;

  for (int iTr=0; iTr<fNumTrk; iTr++)
  {
    fPDG = TDatabasePDG::Instance()->GetParticle(fPID);
    TVector3 momentum = RollMomentumBox(fVertex,fDtheta*TMath::Pi(),fPtMin,fPtMax);

    int sizeMc = fMcCands->GetEntriesFast();
    RhoCandidate *pmc=new ((*fMcCands)[sizeMc]) RhoCandidate(momentum,fPDG);
    pmc->SetPos(fVertex);

    // Smeared "reco"
    int size = fCands->GetEntriesFast();
    TVector3 vertexMeasure = fVertex;
    SmearVertex(vertexMeasure);
    TVector3 momMeasure = momentum;
    SmearMomentum(momMeasure);


    RhoCandidate *p=new ((*fCands)[size]) RhoCandidate(momMeasure,fPDG);//TODO charge
    p->SetMcTruth(pmc);
    p->SetPos(vertexMeasure);
    EnergyCorrelations(covP4,p->P4());
    p->SetCov7(covPos,covP4);

    if(fVerbose>1) cout<<" ---------\n pmc, p, pmcpos, ppos, fVertex"<<endl;
    if(fVerbose>1) std::cout<<*pmc<<std::endl;
    if(fVerbose>1) std::cout<<*p<<std::endl;
    if(fVerbose>1) p->P3Cov().Print();
    if(fVerbose>1) pmc->GetPosition().Print();
    if(fVerbose>1) p->GetPosition().Print();
    if(fVerbose>1) fVertex.Print();
    // print helix parameters and cov together with RhoCandidate V-P4 and cov7

    // alternate particle type
    fCharge*=-1;
    fPID*=-1;
  }
}
void PndPmtPoormantracks::EnergyCorrelations(RhoError& covP4, TLorentzVector p4)
{
  double invE=1/p4.E();
    covP4[3][3] = (p4.X()*p4.X()*covP4[0][0]+p4.Y()*p4.Y()*covP4[1][1]+p4.Z()*p4.Z()*covP4[2][2])*invE*invE;
    covP4[0][3] = covP4[3][0] = p4.X()*covP4[0][0]*invE;
    covP4[1][3] = covP4[3][1] = p4.Y()*covP4[1][1]*invE;
    covP4[2][3] = covP4[3][2] = p4.Z()*covP4[2][2]*invE;
//    covP4[0][3] = covP4[3][0] = (p4.X()*covP4[0][0]+p4.Y()*covP4[0][1]+p4.Z()*covP4[0][2])*invE;
//    covP4[1][3] = covP4[3][1] = (p4.X()*covP4[1][0]+p4.Y()*covP4[1][1]+p4.Z()*covP4[1][2])*invE;
//    covP4[2][3] = covP4[3][2] = (p4.X()*covP4[2][0]+p4.Y()*covP4[2][1]+p4.Z()*covP4[2][2])*invE;

}








