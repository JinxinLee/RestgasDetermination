//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmEmcBarrel.cc,v 1.11 2007/05/24 08:07:40 klausg Exp $
//
// Description:
//      Class FsmEmcPid
//
//  Implementation of the PID info for EMCs
//
//  This software was developed for the PANDA collaboration.  If you
//  use all or part of it, please give an appropriate acknowledgement.
//
// Author List:
//      Klaus Goetzen                    Original Author
//
// Copyright Information:
//      Copyright (C) 2014              GSI
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "PndFsmEmcPid.h"

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
#include "TFile.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

//----------------
// Constructors --
//----------------

PndFsmEmcPid::PndFsmEmcPid()
{
  initParameters();

  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;
  _phiMin=_phiMin*M_PI/180.0;
  _phiMax=_phiMax*M_PI/180.0;
  readParameters();

  //print(std::cout);
}

PndFsmEmcPid::PndFsmEmcPid(ArgList &par)
{
  initParameters();
  //set default parameter values and parses a parameter list
  //i.e. std::list<std::string> of the form
  //"a=1" "b=2" "c=3"
  parseParameterList(par);

  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;
  _phiMin=_phiMin*M_PI/180.0;
  _phiMax=_phiMax*M_PI/180.0;
  readParameters();

  //print(std::cout);
}




//--------------
// Destructor --
//--------------

PndFsmEmcPid::~PndFsmEmcPid()
{
}

//--------------
// Operations --
//--------------

PndFsmResponse*
PndFsmEmcPid::respond(PndFsmTrack *t)
{
  PndFsmResponse *result=new PndFsmResponse();

  result->setDetector(this);
  bool wasDetected=detected(t);
  result->setDetected(wasDetected);

  if (wasDetected)
  {
    int    type   = t->pdt();
    double p      = t->p4().Vect().Mag();
    double charge = t->charge();

	// convert pdg code in type: 0=e+-, 1=mu+-, 2=pi+-, 3=K+-, 4=p, 5=p-bar
	int idx;
	if (abs(type)==11) idx=0;
	else if (abs(type)==13) idx=1;
	else if (abs(type)==211) idx=2;
	else if (abs(type)==321) idx=3;
	else if (type==2212) idx=4;
	else idx=5;

	// histogram max p = 10.0
	double hpmax = _emcPidPdf[0]->GetXaxis()->GetXmax();
	if (p>hpmax) p=hpmax;

	// get the bin corresponding to p
	int currbin = _emcPidPdf[0]->GetXaxis()->FindBin(p);

	// get the slice containing the pdf emcecal(p) for true particle type
	TH1D *hpdf = _emcPidPdf[idx]->ProjectionY("_tmppdf",currbin, currbin);

	// get a random emcecal value from the true distribution
	// if distribution is empty, choose 0
	double xsig = 0.;
	if (hpdf->Integral()>0) xsig = hpdf->GetRandom();

	// store the value in the response object
	result->setEmcEcal(xsig);

	// find the bin corresponding to the value
	int xsigbin = hpdf->FindBin(xsig);

	// get the probability values for the different particle types for this emcecal value; sum needed for normalization
	double P[6], Psum=0.;
	// find P up e ... pbar and sum
	for (int k=0;k<6;++k)
	{
		P[k] = _emcPidPdf[k]->GetBinContent(currbin,xsigbin);
		if (k<4) Psum += P[k];
	}

	// add P_p or P_pbar depending on charge of particle
	if (charge>0) Psum+=P[4];
	else Psum+=P[5];

	if (Psum<=0.) Psum=1.;

    result->setLHElectron(P[0]/Psum);
    result->setLHMuon(P[1]/Psum);
    result->setLHPion(P[2]/Psum);
    result->setLHKaon(P[3]/Psum);
	if (charge>0) result->setLHProton(P[4]/Psum);
	else result->setLHProton(P[5]/Psum);
  }

  return result;
}

bool
PndFsmEmcPid::detected(PndFsmTrack *t) const
{
    double theta = t->p4().Theta();
    double phi = t->p4().Phi();
    double p=t->p4().Vect().Mag();
    double pt=t->p4().Pt();
    double charge=t->charge();
    int type=abs(t->pdt());

	return ( fabs(charge)>1e-6 && theta>=_thtMin && theta<=_thtMax && phi>=_phiMin && phi<=_phiMax && p>_pmin && pt> _ptmin && _rand->Rndm()<=_efficiency);
}


void
PndFsmEmcPid::print(ostream &o)
{
  o <<"Detector <"<<_detName<<">"<<endl;
  o  <<"  _pmin = "<<_pmin<<endl;
  o  <<"  _ptmin = "<<_ptmin<<endl;
  o  <<"  _thtMin = "<<_thtMin<<endl;
  o  <<"  _thtMax = "<<_thtMax<<endl;
  o  <<"  _phiMin = "<<_phiMin<<endl;
  o  <<"  _phiMax = "<<_phiMax<<endl;
  o  <<"  _efficiency = "<<_efficiency<<endl;
  o  <<"  _parFileName     = "<<_parFileName<<endl;
}

void
PndFsmEmcPid::initParameters()
{
  _detName = "EmcPid";
  _pmin   = 0.00;
  _ptmin  = 0.00;
  _thtMin = 22.0;
  _thtMax = 140.0;
  _phiMin=-360.;
  _phiMax=360.;
  _efficiency=1.0;
  _parFileName     = "$VMCWORKDIR/fsim/EmcPidPdf.root";
}

bool
PndFsmEmcPid::setParameter(std::string &name, double value)
{
  // *****************
  // include here all parameters which should be settable via tcl
  // *****************

  bool knownName=true;

  if (name == "pmin")
    _pmin=value;
  else
  if (name == "ptmin")
    _ptmin=value;
  else
  if (name == "thtMin")
    _thtMin=value;
  else
  if (name == "thtMax")
    _thtMax=value;
  else
  if (name == "phiMin")
    _phiMin=value;
  else
  if (name == "phiMax")
    _phiMax=value;
  else
  if (name == "efficiency")
    _efficiency=value;
  else
    knownName=false;

  return knownName;
}

bool PndFsmEmcPid::readParameters()
{

  TFile *f=new TFile(_parFileName.c_str());

  for (int i=0;i<6;i++)
  {
    _emcPidPdf[i]=0;
  }

  if (f->IsZombie())
  {
    cout <<" -W-  (PndFsmEmcPid::readParameters) - file "<<_parFileName.c_str()
         <<" doesn't exist."<<endl;
	exit(0);
  }
  else
  {
    _emcPidPdf[0]=(TH2F*)f->Get("hpdf_e");
    _emcPidPdf[1]=(TH2F*)f->Get("hpdf_mu");
    _emcPidPdf[2]=(TH2F*)f->Get("hpdf_pi");
    _emcPidPdf[3]=(TH2F*)f->Get("hpdf_k");
    _emcPidPdf[4]=(TH2F*)f->Get("hpdf_p");
    _emcPidPdf[5]=(TH2F*)f->Get("hpdf_pb");

    for (int i=0;i<6;i++) _emcPidPdf[i]->SetDirectory(0);

    f->Close();
  }
  delete f;

  return true;
}
