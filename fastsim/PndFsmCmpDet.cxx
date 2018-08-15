//--------------------------------------------------------------------------
// Description:
//      Class PndFsmCmpDet
//
//  Detector class that combines responses from other detectors
//
//  This software was developed for the PANDA collaboration.  If you
//  use all or part of it, please give an appropriate acknowledgement.
//
// Author List:
//      Oscar Reinecke                   Original Author
//
// Copyright Information:
//      Copyright (C) 2008              GSI
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "PndFsmCmpDet.h"

//-------------
// C Headers --
//-------------

//---------------
// C++ Headers --
//---------------
#include <math.h>
#include <iostream>

using std::cout;
using std::endl;
using std::ostream;
using std::string;

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

#include "ArgList.h"
#include "PndFsmResponse.h"
#include "PndFsmTrack.h"
#include "PndFsmAbsDet.h"
#include "PndFsmDetFactory.h"

#include "TFile.h"
#include "TString.h"
#include "TSpline.h"
#include "TParameter.h"
#include "TF1.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

//----------------
// Constructors --
//----------------

PndFsmCmpDet::PndFsmCmpDet(ArgList& par) {
  initParameters();
  parseParameterList(par);
  std::cout<<" -I- (PndFsmCmpDet::PndFsmCmpDet) - Using parameter file '"<<_parFileName<<"'"<<endl;
  readParameters();

  // when PndFsmCmpDet has been created by the detector
  // factory, some arbitrary detector set is added

  std::cout<<" -I- (PndFsmCmpDet::PndFsmCmpDet) - Creating arbitrary detector set"<<endl;

  //EM Calorimeters w/ default parameters
  AddDetector("EmcBarrel");
  AddDetector("EmcFwCap");
  AddDetector("EmcBwCap");
  AddDetector("EmcFS");

  //Tracking, Vertexing, dE/dx
  AddDetector("Mvd2");
  AddDetector("Stt");
  AddDetector("MdcTS");
  AddDetector("MdcFS");
//AddDetector("Tpc");

  //PID detectors
  AddDetector("DrcBarrel");
  AddDetector("DrcDisc");
  AddDetector("Rich");
  AddDetector("Tof");
}

PndFsmCmpDet::PndFsmCmpDet() {
  initParameters();
  readParameters();
}

//--------------
// Destructor --
//--------------

PndFsmCmpDet::~PndFsmCmpDet() {
}

//--------------
// Operations --
//--------------

PndFsmResponse*
PndFsmCmpDet::respond(PndFsmTrack *t) {
  PndFsmResponse *result=new PndFsmResponse();

  result->setDetector(this);

  bool detected=false;

  double dE=0.0;
  double dp=0.0;
  double dtheta=0.0;
  double dphi=0.0;
  double dt=0.0;
  double dm=0.0;

  double m2=0;
  double MvddEdx=0;
  double TpcdEdx=0;
  double SttdEdx=0;
  double DrcDiscThtc=0;
  double DrcBarrelThtc=0;
  double RichThtc=0;

  double m2Err=0;
  double MvddEdxErr=0;
  double TpcdEdxErr=0;
  double SttdEdxErr=0;
  double DrcDiscThtcErr=0;
  double DrcBarrelThtcErr=0;
  double RichThtcErr=0;

  double dVx=0;
  double dVy=0;
  double dVz=0;

  double LH_e=1.0;
  double LH_mu=1.0;
  double LH_pi=1.0;
  double LH_K=1.0;
  double LH_p=1.0;

  double val=0.0;

  for (FsmAbsDetList::iterator iter=fDetList.begin();iter!=fDetList.end(); iter++) {
    PndFsmResponse* resp=(*iter)->respond(t);

    detected |= resp->detected();

    if (resp->detected()) {
      if (fabs(val = resp->dE()) > 1e-8)    dE+=1/(val*val);
      if (fabs(val = resp->dt()) > 1e-8)     dt += val*val;
      if (fabs(val = resp->dm()) > 1e-8)     dm +=val;
      if (fabs (val = resp->m2()) > 1e-11)    m2+=val;
      if (fabs (val = resp->MvddEdx()) > 1e-11)    MvddEdx+=val;
      if (fabs (val = resp->TpcdEdx()) > 1e-11)    TpcdEdx+=val;
      if (fabs (val = resp->SttdEdx()) > 1e-11)    SttdEdx+=val;
      if (fabs (val = resp->DrcDiscThtc()) > 1e-11)   DrcDiscThtc+=val;
      if (fabs (val = resp->DrcBarrelThtc()) > 1e-11)    DrcBarrelThtc+=val;
      if (fabs (val = resp->RichThtc()) > 1e-11)    RichThtc+=val;

      if (fabs (val = resp->m2Err()) > 1e-11)    m2Err+=val;
      if (fabs (val = resp->MvddEdxErr()) > 1e-11)    MvddEdxErr+=val;
      if (fabs (val = resp->TpcdEdxErr()) > 1e-11)    TpcdEdxErr+=val;
      if (fabs (val = resp->SttdEdxErr()) > 1e-11)    SttdEdxErr+=val;
      if (fabs (val = resp->DrcDiscThtcErr()) > 1e-11)   DrcDiscThtcErr+=val;
      if (fabs (val = resp->DrcBarrelThtcErr()) > 1e-11)    DrcBarrelThtcErr+=val;
      if (fabs (val = resp->RichThtcErr()) > 1e-11)    RichThtcErr+=val;

      double rawLHe  = resp->LHElectron();
      double rawLHmu = resp->LHMuon();
      double rawLHpi = resp->LHPion();
      double rawLHK  = resp->LHKaon();
      double rawLHp  = resp->LHProton();

      double sumRaw = rawLHe+rawLHmu+rawLHpi+rawLHK+rawLHp;

      if (sumRaw>0) {
        rawLHe  /= sumRaw;
        rawLHmu /= sumRaw;
        rawLHpi /= sumRaw;
        rawLHK  /= sumRaw;
        rawLHp  /= sumRaw;
        LH_e  *= rawLHe;
        LH_mu *= rawLHmu;
        LH_pi *= rawLHpi;
        LH_K  *= rawLHK;
        LH_p  *= rawLHp;
      } else {
        LH_e  *= 0.2;
        LH_mu *= 0.2;
        LH_pi *= 0.2;
        LH_K  *= 0.2;
        LH_p  *= 0.2;
      }
    }
  }

  // invoke parameterised vertex/momentum resolution
  // (this will overwrite dp, dtheta, dphi and dV)
    if ( _parFile ) {
        // haven't done neutral particles yet
        if ( fabs(t->charge())>1e-8 ) {
            int pid=abs(t->pdt());
            if ( pid==11 || pid==13 || pid==211 || pid==321 || pid==2212) {
                double p=t->p4().Vect().Mag();
                double theta=t->p4().Vect().Theta()*180/M_PI;
                // cut off slow and out of theta range particles
                // (this avoids floating point exceptions)
                detected &= ( p > _mom0[pid]->GetVal() );
                detected &= ( theta >= _tht0->GetVal() );
                detected &= ( theta <= _tht1->GetVal() );

                if (detected) {
                    dtheta = eval(_tht[pid], theta) * _thtScale[pid]->Eval(p) * _thtResMulti * 3.1416/180;
                    dphi = eval(_phi[pid], theta) * _phiScale[pid]->Eval(p) * _phiResMulti * 3.1416/180;
                    dp = eval(_mom[pid], theta) * _momScale[pid]->Eval(p) * _momResMulti * p;

                    dVx = eval(_d0[pid], theta) * _d0Scale[pid]->Eval(p) * _d0ResMulti;
                    dVy = eval(_d0[pid], theta) * _d0Scale[pid]->Eval(p) * _d0ResMulti;
                    dVz = eval(_z0[pid], theta) * _z0Scale[pid]->Eval(p) * _z0ResMulti;

                    dtheta=1/(dtheta*dtheta);
                    dphi=1/(dphi*dphi);
                    dp=1/(dp*dp);
                }
            }
        }
    }

  for (FsmAbsDetList::iterator iter=fSubtractDetList.begin();iter!=fSubtractDetList.end(); iter++) {
    PndFsmResponse* resp=(*iter)->respond(t);

    if (resp->detected()) {
      if (fabs(val = resp->dE()) > 1e-8)    dE-=1/(val*val);
      if (fabs(val = resp->dp()) > 1e-8)     dp-=1/(val*val);
      if (fabs(val = resp->dtheta())> 1e-8)    dtheta-=1/(val*val);
      if (fabs(val = resp->dphi()) > 1e-8)  dphi-=1/(val*val);
      if (fabs(val = resp->dt()) > 1e-8)     dt -= val*val;
      if (fabs(val = resp->dm()) > 1e-8)     dm -=val;
      if (fabs (val = resp->m2()) > 1e-11)    m2-=val;
      if (fabs (val = resp->MvddEdx()) > 1e-11)    MvddEdx-=val;
      if (fabs (val = resp->TpcdEdx()) > 1e-11)    TpcdEdx-=val;
      if (fabs (val = resp->SttdEdx()) > 1e-11)    SttdEdx-=val;
      if (fabs (val = resp->DrcDiscThtc()) > 1e-11)   DrcDiscThtc-=val;
      if (fabs (val = resp->DrcBarrelThtc()) > 1e-11)    DrcBarrelThtc-=val;
      if (fabs (val = resp->RichThtc()) > 1e-11)    RichThtc-=val;

      if (fabs (val = resp->m2Err()) > 1e-11)    m2Err-=val;
      if (fabs (val = resp->MvddEdxErr()) > 1e-11)    MvddEdxErr-=val;
      if (fabs (val = resp->TpcdEdxErr()) > 1e-11)    TpcdEdxErr-=val;
      if (fabs (val = resp->SttdEdxErr()) > 1e-11)    SttdEdxErr-=val;
      if (fabs (val = resp->DrcDiscThtcErr()) > 1e-11)   DrcDiscThtcErr-=val;
      if (fabs (val = resp->DrcBarrelThtcErr()) > 1e-11)    DrcBarrelThtcErr-=val;
      if (fabs (val = resp->RichThtcErr()) > 1e-11)    RichThtcErr-=val;

      if (fabs (val = resp->dV().X()) > 1e-11) dVx-=1/(val*val);
      if (fabs (val = resp->dV().Y()) > 1e-11) dVy-=1/(val*val);
      if (fabs (val = resp->dV().Z()) > 1e-11) dVz-=1/(val*val);

      double rawLHe  = resp->LHElectron();
      double rawLHmu = resp->LHMuon();
      double rawLHpi = resp->LHPion();
      double rawLHK  = resp->LHKaon();
      double rawLHp  = resp->LHProton();

      double sumRaw = rawLHe+rawLHmu+rawLHpi+rawLHK+rawLHp;

      if (sumRaw>0) {
        rawLHe  /= sumRaw;
        rawLHmu /= sumRaw;
        rawLHpi /= sumRaw;
        rawLHK  /= sumRaw;
        rawLHp  /= sumRaw;
        LH_e  /= rawLHe;
        LH_mu /= rawLHmu;
        LH_pi /= rawLHpi;
        LH_K  /= rawLHK;
        LH_p  /= rawLHp;
      }
    }
  }

  double sumLH = LH_e + LH_mu + LH_pi + LH_K + LH_p;

  if (sumLH>0) {
    LH_e  /= sumLH;
    LH_mu /= sumLH;
    LH_pi /= sumLH;
    LH_K  /= sumLH;
    LH_p  /= sumLH;
  } else {
    LH_e  = 0.2;
    LH_mu = 0.2;
    LH_pi = 0.2;
    LH_K  = 0.2;
    LH_p  = 0.2;
  }

  // this shifts all likelihoods linearly to the state
  // of no pid information at all i. e. all lhs are 0.2
  LH_e  = 0.2*(1-_pidLhMulti) + LH_e*_pidLhMulti;
  LH_mu = 0.2*(1-_pidLhMulti) + LH_mu*_pidLhMulti;
  LH_pi = 0.2*(1-_pidLhMulti) + LH_pi*_pidLhMulti;
  LH_K  = 0.2*(1-_pidLhMulti) + LH_K*_pidLhMulti;
  LH_p  = 0.2*(1-_pidLhMulti) + LH_p*_pidLhMulti;

  result->setdE( dE>0. ? 1/sqrt(dE) : 0.0 );
  result->setdp( dp>0. ? 1/sqrt(dp) : 0.0 );
  result->setdtheta( dtheta>0. ? 1/sqrt(dtheta) : 0.0 );
  result->setdphi( dphi>0. ? 1/sqrt(dphi) : 0.0 );
  result->setdt( sqrt(dt) );
  result->setdm(dm);

  result->setm2(m2, m2Err);
  result->setMvddEdx(MvddEdx,MvddEdxErr);
  result->setTpcdEdx(TpcdEdx,TpcdEdxErr);
  result->setSttdEdx(SttdEdx,SttdEdxErr);

  result->setDrcDiscThtc(DrcDiscThtc,DrcDiscThtcErr);
  result->setDrcBarrelThtc(DrcBarrelThtc,DrcBarrelThtcErr);
  result->setRichThtc(RichThtc,RichThtcErr);

  if (dVx > 0.) dVx=1./sqrt(dVx); else dVx = 0.0;
  if (dVy > 0.) dVy=1./sqrt(dVy); else dVy = 0.0;
  if (dVz > 0.) dVz=1./sqrt(dVz); else dVz = 0.0;

  result->setdV( dVx , dVy , dVz );

  result->setLHElectron(LH_e);
  result->setLHMuon(LH_mu);
  result->setLHPion(LH_pi);
  result->setLHKaon(LH_K);
  result->setLHProton(LH_p);

  result->setDetected(detected);

  return result;
}


bool PndFsmCmpDet::AddDetector(string name, string params) {
  PndFsmAbsDet *det=fDetFact.create(name,params);
  if (det)
    fDetList.push_back(det);
  return det;
}

bool PndFsmCmpDet::SubtractDetector(string name, string params) {
  PndFsmAbsDet *det=fDetFact.create(name,params);
  if (det)
    fSubtractDetList.push_back(det);
  return det;
}

bool PndFsmCmpDet::setParameter(string &name, string &value) {
  // *****************
  // include here all string parameters which should be settable
  // *****************

  bool knownName=true;

  if (name == "parFileName")
    _parFileName=value;
  else
    knownName=false;

  return knownName;
}

bool PndFsmCmpDet::setParameter(std::string &name, double value) {
  // *****************
  // include here all float parameters which should be settable
  // *****************

  bool knownName=true;

  if (name == "d0ResMulti")
    _d0ResMulti=value;
  else
  if (name == "z0ResMulti")
    _z0ResMulti=value;
  else
  if (name == "thtResMulti")
    _thtResMulti=value;
  else
  if (name == "phiResMulti")
    _phiResMulti=value;
  else
  if (name == "momResMulti")
    _momResMulti=value;
  else
  if (name == "thtMin")
    _tht0 = new TParameter<double>("tht0", value);
  else
  if (name == "thtMax")
    _tht1 = new TParameter<double>("tht1", value);
  else
  if (name == "pidLhMulti")
    _pidLhMulti=value;
  else
    knownName=false;

  return knownName;
}

void PndFsmCmpDet::readParameters() {

  TFile f(_parFileName);

  if (f.IsZombie()) {
    cout <<" -W-  (PndFsmCmpDet::readParameters) - file "<<_parFileName
         <<" doesn't exist. Using constant vertex reso"<<endl;
  } else {
    // electrons
    _d0[11]=(TSpline3*)f.Get("d0E-");
    _z0[11]=(TSpline3*)f.Get("z0E-");
    _tht[11]=(TSpline3*)f.Get("thtE-");
    _phi[11]=(TSpline3*)f.Get("phiE-");
    _mom[11]=(TSpline3*)f.Get("momE-");
    if (!_mom0[11]) _mom0[11]=(TParameter<double>*)f.Get("mom0E-");
    _d0Scale[11]=(TF1*)f.Get("d0ScaleE-");
    _z0Scale[11]=(TF1*)f.Get("z0ScaleE-");
    _thtScale[11]=(TF1*)f.Get("thtScaleE-");
    _phiScale[11]=(TF1*)f.Get("phiScaleE-");
    _momScale[11]=(TF1*)f.Get("momScaleE-");
    // muons
    _d0[13]=(TSpline3*)f.Get("d0Mu+");
    _z0[13]=(TSpline3*)f.Get("z0Mu+");
    _tht[13]=(TSpline3*)f.Get("thtMu+");
    _phi[13]=(TSpline3*)f.Get("phiMu+");
    if (!_mom0[13]) _mom[13]=(TSpline3*)f.Get("momMu+");
    _mom0[13]=(TParameter<double>*)f.Get("mom0Mu+");
    _d0Scale[13]=(TF1*)f.Get("d0ScaleMu+");
    _z0Scale[13]=(TF1*)f.Get("z0ScaleMu+");
    _thtScale[13]=(TF1*)f.Get("thtScaleMu+");
    _phiScale[13]=(TF1*)f.Get("phiScaleMu+");
    _momScale[13]=(TF1*)f.Get("momScaleMu+");
    // pions
    _d0[211]=(TSpline3*)f.Get("d0Pi-");
    _z0[211]=(TSpline3*)f.Get("z0Pi-");
    _tht[211]=(TSpline3*)f.Get("thtPi-");
    _phi[211]=(TSpline3*)f.Get("phiPi-");
    _mom[211]=(TSpline3*)f.Get("momPi-");
    if (!_mom0[211]) _mom0[211]=(TParameter<double>*)f.Get("mom0Pi-");
    _d0Scale[211]=(TF1*)f.Get("d0ScalePi-");
    _z0Scale[211]=(TF1*)f.Get("z0ScalePi-");
    _thtScale[211]=(TF1*)f.Get("thtScalePi-");
    _phiScale[211]=(TF1*)f.Get("phiScalePi-");
    _momScale[211]=(TF1*)f.Get("momScalePi-");
    // kaons
    _d0[321]=(TSpline3*)f.Get("d0K-");
    _z0[321]=(TSpline3*)f.Get("z0K-");
    _tht[321]=(TSpline3*)f.Get("thtK-");
    _phi[321]=(TSpline3*)f.Get("phiK-");
    _mom[321]=(TSpline3*)f.Get("momK-");
    if (!_mom0[321]) _mom0[321]=(TParameter<double>*)f.Get("mom0K-");
    _d0Scale[321]=(TF1*)f.Get("d0ScaleK-");
    _z0Scale[321]=(TF1*)f.Get("z0ScaleK-");
    _thtScale[321]=(TF1*)f.Get("thtScaleK-");
    _phiScale[321]=(TF1*)f.Get("phiScaleK-");
    _momScale[321]=(TF1*)f.Get("momScaleK-");
    // protons
    _d0[2212]=(TSpline3*)f.Get("d0P+");
    _z0[2212]=(TSpline3*)f.Get("z0P+");
    _tht[2212]=(TSpline3*)f.Get("thtP+");
    _phi[2212]=(TSpline3*)f.Get("phiP+");
    _mom[2212]=(TSpline3*)f.Get("momP+");
    if (!_mom0[2212]) _mom0[2212]=(TParameter<double>*)f.Get("mom0P+");
    _d0Scale[2212]=(TF1*)f.Get("d0ScaleP+");
    _z0Scale[2212]=(TF1*)f.Get("z0ScaleP+");
    _thtScale[2212]=(TF1*)f.Get("thtScaleP+");
    _phiScale[2212]=(TF1*)f.Get("phiScaleP+");
    _momScale[2212]=(TF1*)f.Get("momScaleP+");

    if (!_tht0) _tht0=(TParameter<double>*)f.Get("tht0");
    if (!_tht1) _tht1=(TParameter<double>*)f.Get("tht1");

    f.Close();
    // now check that all parameters have been loaded correctly
    _parFile=true;
    std::map<int, TSpline3*>::iterator i;
    std::map<int, TParameter<double>*>::iterator j;
    std::map<int, TF1*>::iterator k;

    for (i=_d0.begin(); i!=_d0.end(); i++) _parFile = _parFile && i->second;
    for (i=_z0.begin(); i!=_z0.end(); i++) _parFile = _parFile && i->second;
    for (i=_tht.begin(); i!=_tht.end(); i++) _parFile = _parFile && i->second;
    for (i=_phi.begin(); i!=_phi.end(); i++) _parFile = _parFile && i->second;
    for (i=_mom.begin(); i!=_mom.end(); i++) _parFile = _parFile && i->second;

    for (j=_mom0.begin(); j!=_mom0.end(); j++) _parFile = _parFile && j->second;
    _parFile = _parFile && _tht0 && _tht1;

    for (k=_d0Scale.begin(); k!=_d0Scale.end(); k++) _parFile = _parFile && k->second;
    for (k=_z0Scale.begin(); k!=_z0Scale.end(); k++) _parFile = _parFile && k->second;
    for (k=_thtScale.begin(); k!=_thtScale.end(); k++) _parFile = _parFile && k->second;
    for (k=_phiScale.begin(); k!=_phiScale.end(); k++) _parFile = _parFile && k->second;
    for (k=_momScale.begin(); k!=_momScale.end(); k++) _parFile = _parFile && k->second;

    if (!_parFile)
      cout <<" -W-  (PndFsmCmpDet::readParameters) - file "<<_parFileName
           <<" is defective. Using constant vertex reso"<<endl;
  }
}

void PndFsmCmpDet::initParameters() {
  _detName = "CmpDet";
  _parFileName = "$VMCWORKDIR/fastsim/cmpdetparams.root";
  _parFile=false;
  _d0ResMulti=1.0;
  _z0ResMulti=1.0;
  _thtResMulti=1.0;
  _phiResMulti=1.0;
  _momResMulti=1.0;
  _pidLhMulti=1.0;
  _mom0[11]=0;
  _mom0[13]=0;
  _mom0[211]=0;
  _mom0[321]=0;
  _mom0[2212]=0;
  _tht0=0;
  _tht1=0;
 }

// TSpline3::Eval returns bogus values when outside the
// interval, so the spline has to be continued somehow
double PndFsmCmpDet::eval(TSpline3* s, double x) {
  double xmin=s->GetXmin();
  double xmax=s->GetXmax();
  if (x<xmin)
    return s->Eval(xmin)+(x-xmin)*s->Derivative(xmin);
  else if (x>xmax)
    return s->Eval(xmax)+(x-xmax)*s->Derivative(xmax);
  else
    return s->Eval(x);
}

