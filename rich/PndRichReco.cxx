#include "PndRichReco.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"

#include "PndMCTrack.h"
#include "TLorentzVector.h"

#include "PndRichGeo.h"
#include "PndRichPhoton.h"
#include "PndRichPDHit.h"

#include "TH1F.h"
#include "TF1.h"
#include "TProfile.h"
#include <cmath>

#include "Math/GSLMinimizer.h"
#include "Math/Functor.h"

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TRandom2.h"
#include "TError.h"
#include <iostream>
#include <algorithm>

namespace {

 // Show usage with all the possible minimizers. 
 // Minimize the Rosenbrock function (a 2D -function)
 // This example is described also in 
 // http://root.cern.ch/drupal/content/numerical-minimization#multidim_minim
 // input : minimizer name + algorithm name
 // randomSeed: = <0 : fixed value: 0 random with seed 0; >0 random with given seed 
 //
 // Author: L. Moneta Dec 2010
 
      
//------------------------------------------------------------------
    
    std::vector<double> ti;
    std::vector<double> fi;
    double nopt_, beta_, nnz_;
    double dnopt_, dbeta_, dnnz_;
    double chi2_;
    
    double thc(const double phic,
               const double nopt,
               const double beta,
               const double nnz0)
    {
       Double_t cthc = 1.0/nopt/beta;
       Double_t nnz = nnz0;
       if (nnz>1) nnz = 1;
       if (cthc>1) cthc = 1;
       Double_t sthc = std::sqrt(1-cthc*cthc);
       Double_t nng = nnz*cthc+std::sqrt(1-nnz*nnz)*sthc*std::cos(phic);
       Double_t cdthc = nopt*(1-nng*nng);
       cdthc = cdthc + std::sqrt(std::fabs(1-nopt*cdthc))*nng;
       if (cdthc>1) cdthc = 1;
       return std::acos(cdthc)+std::acos(cthc);
    }
    
    double Chi2(const double *xx )
    {
       const double nopt = xx[0];
       const double beta = xx[1];
       const double nnz  = xx[2];
       double Chi2_ = 0;
       size_t n = fi.size();
       for(size_t i=0;i<n;i++) {
          double chi = (ti.at(i) - thc(fi.at(i),nopt,beta,nnz))/0.05;
          Chi2_ += chi*chi;
       }
       return Chi2_;
    }
    
    int Minimizer(const char * minName = "Minuit2",
                  const char *algoName = "" ,
                  int randomSeed = -1)
    {
       // create minimizer giving a name and a name (optionally) for the specific
       // algorithm
       // possible choices are: 
       //     minName                  algoName
       // Minuit /Minuit2             Migrad, Simplex,Combined,Scan  (default is Migrad)
       //  Minuit2                     Fumili2
       //  Fumili
       //  GSLMultiMin                ConjugateFR, ConjugatePR, BFGS, 
       //                              BFGS2, SteepestDescent
       //  GSLMultiFit
       //   GSLSimAn
       //   Genetic
       ROOT::Math::Minimizer* min = 
          ROOT::Math::Factory::CreateMinimizer(minName, algoName);
       
       // set tolerance , etc...
       min->SetMaxFunctionCalls(1000000); // for Minuit/Minuit2 
       min->SetMaxIterations(10000);  // for GSL 
       min->SetTolerance(1e-12);
       min->SetPrintLevel(0);
       
       // create funciton wrapper for minmizer
       // a IMultiGenFunction type 
       ROOT::Math::Functor f(&Chi2,3); 
       double step[3] = {0.00,0.001,0.00};
       
       // starting point
                                                                                                                                                                    
       double variable[3] = { nopt_, beta_, nnz_ };
       
       min->SetFunction(f);
       
       // Set the free variables to be minimized!
       min->SetVariable(0,"nopt",variable[0], step[0]);
       min->SetVariable(1,"beta",variable[1], step[1]);
       min->SetVariable(2,"nnz",variable[2], step[2]);
       
       // do the minimization
       min->Minimize(); 
       
       const double *xs = min->X();
       const double *dxs = min->Errors();
       
       nopt_ = xs[0];
       beta_ = xs[1];
       nnz_  = xs[2];
       dnopt_ = dxs[0];
       dbeta_ = dxs[1];
       dnnz_  = dxs[2];
       chi2_  = min->MinValue();
       
       return 0;
    }

    double Chi2_v1(const double *xx )
    {
       const double nopt = nopt_;
       const double beta = xx[0];
       const double nnz  = nnz_;
       double Chi2_ = 0;
       size_t n = fi.size();
       for(size_t i=0;i<n;i++) {
          double chi = (ti.at(i) - thc(fi.at(i),nopt,beta,nnz))/0.0025;
          Chi2_ += chi*chi;
       }
       return Chi2_;
    }
    
    int Minimizer_v1(const char * minName = "Minuit2",
                     const char *algoName = "" ,
                     int randomSeed = -1)
    {
       // create minimizer giving a name and a name (optionally) for the specific
       // algorithm
       // possible choices are: 
       //     minName                  algoName
       // Minuit /Minuit2             Migrad, Simplex,Combined,Scan  (default is Migrad)
       //  Minuit2                     Fumili2
       //  Fumili
       //  GSLMultiMin                ConjugateFR, ConjugatePR, BFGS, 
       //                              BFGS2, SteepestDescent
       //  GSLMultiFit
       //   GSLSimAn
       //   Genetic
       ROOT::Math::Minimizer* min = 
          ROOT::Math::Factory::CreateMinimizer(minName, algoName);
       
       // set tolerance , etc...
       min->SetMaxFunctionCalls(1000000); // for Minuit/Minuit2 
       min->SetMaxIterations(10000);  // for GSL 
       min->SetTolerance(1e-12);
       min->SetPrintLevel(0);
       
       // create funciton wrapper for minmizer
       // a IMultiGenFunction type 
       ROOT::Math::Functor f(&Chi2_v1,1);
       min->SetFunction(f);
       
       // Set the beta variable to be minimized!
       min->SetVariable(0, "beta", beta_, 0.001);
       
       // do the minimization
       min->Minimize(); 
       
       const double *xs = min->X();
       const double *dxs = min->Errors();
       
       beta_ = xs[0];
       dbeta_ = dxs[0];
       chi2_  = min->MinValue();
       
       return 0;
    }

//------------------------------------------------------------------
}

ClassImp(PndRichReco)

//___________________________________________________________
PndRichReco::~PndRichReco() 
{
  //
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}

// -----   Default constructor   -------------------------------------------
PndRichReco::PndRichReco()
  : fRichPDHit(0)
{
   fGeoVersion = 13;
   Init(); // init geometry parameters
   Register();
   
//-----------------------------------------------------   
}

PndRichReco::PndRichReco(UInt_t version = 0,
                         UInt_t pid = 0,
                         TVector3 position = TVector3(0,0,0),
                         TVector3 direction = TVector3(0,0,1))
  : fRichPDHit(0)
{
   fGeoVersion = version;
   fParticleID = pid;
   fTrackPosition = position;
   fTrackDirection = direction.Unit();
   Init(); // init geometry parameters
   Register();
   
//-----------------------------------------------------   
}

void PndRichReco::Init()
{
   FairRootManager *fManager = FairRootManager::Instance();
   fGeo = new PndRichGeo();
   fGeo->init(fGeoVersion);
   fRichPDHit = dynamic_cast<TClonesArray *> (fManager->GetObject("RichPDHit"));
   //
   TVector3 richOffset = fGeo->richOffset();
   TVector3 aerogelOffset = fGeo->aerogelOffset();
   TVector3 aerogelSize = fGeo->aerogelSize();
   Double_t zain = richOffset.Z() + aerogelOffset.Z();
   fZamid = zain + 0.5*aerogelSize.Z(); // midle position of the point of cherenkov photons emission
   //fZamid = zain + 0.69075*aerogelSize.Z(); // optimal position of the point of cherenkov photons emission
   //
   fPhDetAngle = fGeo->phDetAngle();
   std::vector<Double_t> flatMirrorY  = fGeo->flatMirrorYGlob();
   std::vector<Double_t> flatMirrorZ  = fGeo->flatMirrorZGlob();
   //
   fMirrorLength = fGeo->mirrorLength();
   fNumberOfFlatMirrorSegments = flatMirrorY.size()-1;
   for(UInt_t i=0; i<fNumberOfFlatMirrorSegments; i++) {
      // middle point on the mirror segment
      Double_t xm = 0;
      Double_t ym = (flatMirrorY[i+1]+flatMirrorY[i])/2;
      Double_t zm = (flatMirrorZ[i+1]+flatMirrorZ[i])/2;
      fMiddleFlatMirrorPoint.push_back(TVector3(xm,ym,zm));
      // size of the flat mirror segment 
      Double_t dxm = 3*fMirrorLength/2;
      Double_t dym = std::fabs(flatMirrorY[i+1]-flatMirrorY[i])/2;
      Double_t dzm = std::fabs(flatMirrorZ[i+1]-flatMirrorZ[i])/2;
      fSizeOfFlatMirror.push_back(TVector3(dxm,dym,dzm));
      // normal vector of the flat mirror segment
      TVector3 norm = (TVector3(-1,0,0).Cross(TVector3(0,
                                                       flatMirrorY[i+1]-flatMirrorY[i],
                                                       flatMirrorZ[i+1]-flatMirrorZ[i]))).Unit();
      fNormalOfFlatMirror.push_back(norm.Unit());
   }
   for(UInt_t i=0; i<fNumberOfFlatMirrorSegments; i++) {
      // middle point on the mirror segment
      Double_t xm = 0;
      Double_t ym = -(flatMirrorY[i+1]+flatMirrorY[i])/2;
      Double_t zm = (flatMirrorZ[i+1]+flatMirrorZ[i])/2;
      fMiddleFlatMirrorPoint.push_back(TVector3(xm,ym,zm));
      // size of the flat mirror segment 
      Double_t dxm = 3*fMirrorLength/2;
      Double_t dym = std::fabs(flatMirrorY[i+1]-flatMirrorY[i])/2;
      Double_t dzm = std::fabs(flatMirrorZ[i+1]-flatMirrorZ[i])/2;
      fSizeOfFlatMirror.push_back(TVector3(dxm,dym,dzm));
      // normal vector of the flat mirror segment
      TVector3 norm = (TVector3(-1,0,0).Cross(TVector3(0,
                                                       -flatMirrorY[i+1]+flatMirrorY[i],
                                                       flatMirrorZ[i+1]-flatMirrorZ[i]))).Unit();
      fNormalOfFlatMirror.push_back(norm);
   }
   fNumberOfFlatMirrorSegments *= 2;
   fEvent = 0;
}

//______________________________________________________
void PndRichReco::RichFullReconstruction(TVector3 pos0, TVector3 dir, Float_t ts,
                                         Float_t &chi2, Float_t &chTh, Float_t &dChTh, Int_t &nph ) {
   
   if ( fRichPDHit->GetEntriesFast()==0 ) return;

   // finding position of the track in middle depth of the aerogel bar
   Double_t tam = (fZamid-pos0.Z())/dir.Z();
   TVector3 pos = pos0 + tam*dir;

   // flat mirror
   if (fGeoVersion==13) {
      std::vector<PndRichPhoton> photons;
      beta_ = 1;
      dbeta_ = 0;
      photons = CherenkovPhotonListFlat(pos,dir,ts);
      if (photons.size()) {
         Double_t nnz = dir.Z();
         Double_t nopt = 1.05;
         // beta peak finding
         Double_t beta = BetaPeakFinding(photons,nopt,nnz);
         // hits selection
         HitSelection(fi,ti,photons,beta,nopt,nnz);
         //
         nopt_ = nopt;
         beta_ = beta;
         nnz_ = nnz;
         Minimizer_v1();
      }

      chi2 = chi2_;
      chTh = beta_;
      dChTh = dbeta_;
      nph = fi.size();
   }
   fEvent++;
}

std::vector<double> PndRichReco::GetDThetas() {
   size_t n = fi.size();
   std::vector<double> dth(n);
   for(size_t i=0;i<n;i++)
      dth.at(i) = ti.at(i) - thc(fi.at(i),nopt_,beta_,nnz_);
   return dth;
}

double PndRichReco::BetaPeakFinding(std::vector<PndRichPhoton> photons,
                                    Double_t nopt,
                                    Double_t nnz) {
   // beta peak finding
   Int_t nch = 60;
   Double_t bmin = 1.0/nopt;
   Double_t bmax = 1+(1-bmin)*0.2;
   Double_t dbeta = (bmax-bmin)/nch;
   Double_t beta = 1.0;
   Double_t thcmin = 0;
   Double_t thcmax = std::acos(bmin);
   Double_t bim = 0;
   Int_t ibm = -1;
   Double_t dtm = 0.5;
   int nph = 0;
   for(size_t j=0; j<2; j++) {
      std::vector<Double_t> bi(nch,0);
      bim = 0;
      ibm = -1;
      nph = 0;
      for(UInt_t i=0; i<photons.size(); i++) {
         Double_t thcc = photons.at(i).GetTheta();
         Double_t phcc = photons.at(i).GetPhi();
         Double_t thcm = thc(phcc,nopt,beta,nnz);
         Double_t b = bmin+thcc*(beta-bmin)/thcm;
         Int_t ib = (b-bmin)/(bmax-bmin)*nch;
         Double_t dt = photons.at(i).GetTime()-0.1;
         if ((ib>=0)&&(ib<nch)&&std::fabs(dt)<dtm) {
            nph++;
            bi.at(ib)++;
            if (bim<bi.at(ib)) {
               bim = bi.at(ib);
               ibm = ib;
            }
         }
      }
      beta = bmin+ibm*(bmax-bmin)/nch;
   }
   return beta;
}

void PndRichReco::HitSelection(std::vector<double> &ph, std::vector<double> &th,
                                 std::vector<PndRichPhoton> photons,
                                 Double_t beta,
                                 Double_t nopt,
                                 Double_t nnz) {
   //
   ph.resize(photons.size());
   th.resize(photons.size());
   Double_t hitx, hity = 10;
   Double_t thccc, phccc, dthccc = 10;
   Double_t dthc = 0.03;
   Double_t dt = 0;
   Int_t ind = 0;
   Double_t dtm = 0.5;
   for(UInt_t i=0; i<photons.size(); i++) {
      TVector3 hit = photons.at(i).GetHitPos();
      if (((hit.X()!=hitx)||(hit.Y()!=hity))&&(hity!=10)) {
         Double_t thcm = thc(phccc,nopt,beta,nnz);
         if (std::fabs(thccc-thcm)<dthc && std::fabs(dt)<dtm) {
            th.at(ind) = thccc;
            ph.at(ind) = phccc;
            ind++;
         }
         dthccc = 10;
      }
      Double_t thcc = photons.at(i).GetTheta();
      Double_t phcc = photons.at(i).GetPhi();
      Double_t thcm = thc(phcc,nopt,beta,nnz);
      Double_t dthccl = std::fabs(thcc-thcm);
      if (dthccl<dthccc) {
         dthccc = dthccl;
         thccc = thcc;
         phccc = phcc;
         dt = photons.at(i).GetTime()-0.1;
      }
      hitx = hit.X();
      hity = hit.Y();
   }      
   Double_t thcm = thc(phccc,nopt,beta,nnz);
   if (std::fabs(thccc-thcm)<dthc && std::fabs(dt)<dtm) {
      th.at(ind) = thccc;
      ph.at(ind) = phccc;
      ind++;
   }
   //
   /*
   ph.resize(photons.size());
   th.resize(photons.size());
   Double_t dthc = 0.02;
   Int_t ind = 0;
   for(UInt_t i=0; i<photons.size(); i++) {
      Double_t thcc = photons.at(i).GetTheta();
      Double_t phcc = photons.at(i).GetPhi();
      Double_t thcm = thc(phcc,nopt,beta,nnz);
      if (std::fabs(thcc-thcm)<dthc) {
         th.at(ind) = thcc;
         ph.at(ind) = phcc;
         ind++;
      }
   }*/
   ph.resize(ind);
   th.resize(ind);
}

// Чернова-Лисовского(?) алгоритм восстановления окружностей

std::vector<PndRichPhoton> PndRichReco::CherenkovPhotonListFlat( TVector3 pos, TVector3 dir, Double_t time ) {
   size_t nHits = fRichPDHit->GetEntriesFast();
   std::vector<PndRichPhoton> ph(nHits*fNumberOfFlatMirrorSegments);
   PndRichPDHit* richPDHit = NULL;
   TVector3 axis = TVector3(0,0,1);
   TVector3 axisTx = axis-dir*(dir*axis);
   if (axisTx.Mag())
      axisTx = axisTx.Unit();
   else
      axisTx = TVector3(1,0,0);
   TVector3 axisTy = (dir.Cross(axisTx)).Unit();
   size_t ind = 0;
   for(size_t ih=0; ih<nHits; ih++ ) {
      richPDHit = (PndRichPDHit*) fRichPDHit->At(ih);
      TVector3 hit = richPDHit->GetPosition();
      {
         std::vector<TVector3> pi = FlatMirrorReflections(pos,hit);
         for(size_t ir=0; ir<pi.size(); ir++ ) {
            if (pi[ir].Z()) {
               ph.at(ind).SetMirror(ir);
               ph.at(ind).SetLength((pos-pi[ir]).Mag()+(hit-pi[ir]).Mag());
               ph.at(ind).SetHitPos(hit);
               ph.at(ind).SetMirrRefPos(pi[ir]);
               ph.at(ind).SetTheta(0);
               ph.at(ind).SetPhi(0);
               TVector3 nf = (pi[ir]-pos).Unit();
               ph.at(ind).SetTheta(acos(dir*nf));
               TVector3 nfT = (nf-dir*(dir*nf)).Unit();
               ph.at(ind).SetPhi(atan2(axisTy*nfT,axisTx*nfT));
               double length = (hit-pi[ir]).Mag() + (pi[ir]-pos).Mag();
               ph.at(ind).SetTime( richPDHit->GetTime() - length/30 - time );
               ind++;
            }
         }
      }
/*      Double_t x_hit = hit.X();
      {
         // left mirror
         hit.SetX(fMirrorLength - x_hit);
         std::vector<TVector3> pi = FlatMirrorReflections(pos,hit);
         for(size_t ir=0; ir<pi.size(); ir++ ) {
            if (pi[ir].Z()) {
               ph.at(ind).SetMirror(ir);
               ph.at(ind).SetLength((pos-pi[ir]).Mag()+(hit-pi[ir]).Mag());
               ph.at(ind).SetHitPos(hit);
               ph.at(ind).SetMirrRefPos(pi[ir]);
               ph.at(ind).SetTheta(0);
               ph.at(ind).SetPhi(0);
               TVector3 nf = (pi[ir]-pos).Unit();
               ph.at(ind).SetTheta(acos(dir*nf));
               TVector3 nfT = (nf-dir*(dir*nf)).Unit();
               ph.at(ind).SetPhi(atan2(axisTy*nfT,axisTx*nfT));
               double length = (hit-pi[ir]).Mag() + (pi[ir]-pos).Mag();
               ph.at(ind).SetTime( richPDHit->GetTime() - length/30 - time );
               ind++;
            }
         }
      }
      {
         // right mirror
         hit.SetX(-fMirrorLength - x_hit);
         std::vector<TVector3> pi = FlatMirrorReflections(pos,hit);
         for(size_t ir=0; ir<pi.size(); ir++ ) {
            if (pi[ir].Z()) {
               ph.at(ind).SetMirror(ir);
               ph.at(ind).SetLength((pos-pi[ir]).Mag()+(hit-pi[ir]).Mag());
               ph.at(ind).SetHitPos(hit);
               ph.at(ind).SetMirrRefPos(pi[ir]);
               ph.at(ind).SetTheta(0);
               ph.at(ind).SetPhi(0);
               TVector3 nf = (pi[ir]-pos).Unit();
               ph.at(ind).SetTheta(acos(dir*nf));
               TVector3 nfT = (nf-dir*(dir*nf)).Unit();
               ph.at(ind).SetPhi(atan2(axisTy*nfT,axisTx*nfT));
               double length = (hit-pi[ir]).Mag() + (pi[ir]-pos).Mag();
               ph.at(ind).SetTime( richPDHit->GetTime() - length/30 - time );
               ind++;
            }
         }
      }*/
   }
   ph.resize(ind);
   return ph;
}
   
vector<TVector3> PndRichReco::FlatMirrorReflections( TVector3 point1, TVector3 point2z ) {
   vector<TVector3> pi;
   //photodetector: cell size
   Double_t sy = point2z.Y()>0 ? 1 : -1;
   TVector3 nl = TVector3( 0, sy*std::sin(fPhDetAngle), std::cos(fPhDetAngle) );
   TVector3 nx = TVector3( 1, 0, 0 );
   Double_t l  = point2z*nl;;
   Double_t dl = l - (int)(l/0.32)*0.32;
   Double_t x  = point2z.X();
   Double_t dx = x - (int)(x/0.38016)*0.38016;
   TVector3 point2 = point2z - dl*nl - dx*nx;
   point2 = point2z;
   //
   for(UInt_t i=0; i<fNumberOfFlatMirrorSegments; i++) {
      Double_t t1 = (point1-fMiddleFlatMirrorPoint[i])*fNormalOfFlatMirror[i];
      Double_t t2 = (point2-fMiddleFlatMirrorPoint[i])*fNormalOfFlatMirror[i];
      TVector3 p1 = point1-t1*fNormalOfFlatMirror[i];
      TVector3 p2 = point2-t2*fNormalOfFlatMirror[i];
      TVector3 p0 = (t2*p1+t1*p2)*(1.0/(t1+t2)); // point of reflection
      if ( std::fabs(p0.X()-fMiddleFlatMirrorPoint[i].X())<fSizeOfFlatMirror[i].X() &&
           std::fabs(p0.Y()-fMiddleFlatMirrorPoint[i].Y())<fSizeOfFlatMirror[i].Y() &&
           std::fabs(p0.Z()-fMiddleFlatMirrorPoint[i].Z())<fSizeOfFlatMirror[i].Z() )
         pi.push_back(p0); // point is within size of the segment
      else
         pi.push_back(TVector3(0,0,0));
   }
   return pi;
}

void PndRichReco::Register()
{

  /** This will create a branch in the output tree called
      PndRichPDPoint, setting the last parameter to kFALSE means:
      this collection will not be written to the file, it will exist
      only during the simulation.
  */

}


// -------------------------------------------------------------------------

