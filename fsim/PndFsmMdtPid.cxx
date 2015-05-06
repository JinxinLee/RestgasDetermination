//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: PndFsmMdtPid.cxx $
//
// Description:
//      Class PndFsmMdtPid
//
//  Implementation of the MDT for the FastSim
//
//  This software was developed for the PANDA collaboration.  If you
//  use all or part of it, please give an appropriate acknowledgement.
//
// Author List:
//      Ralf Kliemt                     Original Author
//
// Copyright Information:
//      Copyright (C) 2014              GSI
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "PndFsmMdtPid.h"

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
#include "TH3F.h"
#include "TH1D.h"
#include "TFile.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

//----------------
// Constructors --
//----------------

PndFsmMdtPid::PndFsmMdtPid()
{
  initParameters();

  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;
  readParameters();
  //print(std::cout);
}

PndFsmMdtPid::PndFsmMdtPid(ArgList &par)
{
  initParameters();
  //set default parameter values and parses a parameter list
  //i.e. std::list<std::string> of the form
  //"a=1" "b=2" "c=3"
  parseParameterList(par);

  _thtMin=_thtMin*M_PI/180.0;
  _thtMax=_thtMax*M_PI/180.0;
  readParameters();
  //print(std::cout);
}

//--------------
// Destructor --
//--------------

PndFsmMdtPid::~PndFsmMdtPid()
{
}

//--------------
// Operations --
//--------------

PndFsmResponse*
PndFsmMdtPid::respond(PndFsmTrack *t)
{
  PndFsmResponse *result=new PndFsmResponse();

  result->setDetector(this);
  bool wasDetected=detected(t);
  result->setDetected(wasDetected);

  if (wasDetected)
  {
	  if (_useFlat)  // just use very simple likelihoods for muons and pions
	  {
			result->setLHElectron(0.);
			result->setLHMuon( 1-_misId );
			result->setLHPion( _misId );
			result->setLHKaon( 0. );
			result->setLHProton( 0. );
	  }
	  else   // use sophisticated methode with histogram lookup likelihoods
	  {
			int    type   = t->pdt();
			double p      = t->p4().Vect().Mag();
			double tht    = t->p4().Theta()*57.296;
			double charge = t->charge();
			
			// convert pdg code in type: 0=e+-, 1=mu+-, 2=pi+-, 3=K+-, 4=p, 5=p-bar
			int idx;
			if (abs(type)==11) idx=0;
			else if (abs(type)==13) idx=1;
			else if (abs(type)==211) idx=2;
			else if (abs(type)==321) idx=3;
			else if (type==2212) idx=4;
			else idx=5;
			
			// histogram max p = 8.0
			if (p>7.99) p=7.99;
			
			// get the bin corresponding to p
			int currbinT = _mdtPidPdf[0]->GetYaxis()->FindBin(tht);
			int currbinP = _mdtPidPdf[0]->GetZaxis()->FindBin(p);
					
			// get the slice containing the pdf emcecal(p) for true particle type
			TH1D *hpdf = _mdtPidPdf[idx]->ProjectionX("_tmppdf",currbinT, currbinT, currbinP, currbinP);
			
			// get a random emcecal value from the true distribution
			// if distribution is empty, choose 0
			double xsig = 0.; 
			if (hpdf->Integral()>0) xsig = hpdf->GetRandom();
			
			// store the value in the response object
			result->setMuoIron(xsig);
			
			// find the bin corresponding to the value
			int xsigbin = hpdf->FindBin(xsig);
			
			// get the probability values for the different particle types for this emcecal value; sum needed for normalization
			double P[6], Psum=0.;
			
			// sum up e ... K
			for (int k=0;k<6;++k)
			{
				P[k] = _mdtPidPdf[k]->GetBinContent(xsigbin,currbinT,currbinP);
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
  }
  
  return result;
}

bool
PndFsmMdtPid::detected(PndFsmTrack *t) const
{
    double theta = t->p4().Theta();
    double p=t->p4().Vect().Mag();
    double charge=t->charge();
    int type=abs(t->pdt());

	if (_useFlat) return ( charge!=0.0 && theta>=_thtMin && theta<=_thtMax && p>_pmin && ((type==13 && _rand->Rndm()<=_efficiency) || (type==211 && _rand->Rndm()<=_misId)) );
	
	return ( charge!=0.0 && theta>=_thtMin && theta<=_thtMax && p>_pmin );
}

void
PndFsmMdtPid::print(ostream &o)
{
  o <<"Parameters for detector <"<<detName()<<">"<<endl;
  o  <<"  _thtMin = "<<_thtMin<<endl;
  o  <<"  _thtMax = "<<_thtMax<<endl;
  o  <<"  _pmin = "<<_pmin<<endl;
  o  <<"  _misId = "<<_misId<<endl;
  o  <<"  _efficiency = "<<_efficiency<<endl;
}

void
PndFsmMdtPid::initParameters()
{
  _detName         = "MdtPid";
  _thtMin          = 5.;
  _thtMax          = 160.;
  _pmin            = 1.0;
  _misId	   	   = 0.01;
  _efficiency	   = 1.0;
  _parFileName     = "$VMCWORKDIR/fsim/MdtPidPdf3D.root"; 
  _useFlat		   = false;
}

bool
PndFsmMdtPid::setParameter(std::string &name, double value)
{
  // *****************
  // include here all parameters which should be settable via tcl
  // *****************

  bool knownName=true;

  if (name == "thtMin")
    _thtMin=value;
  else
  if (name == "thtMax")
    _thtMax=value;
  else
  if (name == "pmin")
	_pmin=value;
  else
  if (name == "misId")
        _misId=value;
  else
  if (name == "efficiency")
    _efficiency=value;
  else
    knownName=false;

  return knownName;
}

bool PndFsmMdtPid::readParameters()
{
  
  TFile *f=new TFile(_parFileName.c_str());
  
  for (int i=0;i<6;i++) 
  {
    _mdtPidPdf[i]=0;
  }
  
  if (f->IsZombie()) 
  {
    cout <<" -W-  (PndFsmMdtPid::readParameters) - file "<<_parFileName.c_str()
         <<" doesn't exist. Using simple likelihood."<<endl;
	_useFlat = true;
  }
  else 
  {
    _mdtPidPdf[0]=(TH3F*)f->Get("hpdf_e");
    _mdtPidPdf[1]=(TH3F*)f->Get("hpdf_mu");
    _mdtPidPdf[2]=(TH3F*)f->Get("hpdf_pi");
    _mdtPidPdf[3]=(TH3F*)f->Get("hpdf_k");
    _mdtPidPdf[4]=(TH3F*)f->Get("hpdf_p");
    _mdtPidPdf[5]=(TH3F*)f->Get("hpdf_pb");
    
	cout << _mdtPidPdf[0]<<endl;
	
    for (int i=0;i<6;i++) _mdtPidPdf[i]->SetDirectory(0);
      
    f->Close();
  }
  delete f;
  
  return true;
}
