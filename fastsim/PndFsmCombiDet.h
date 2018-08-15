//--------------------------------------------------------------------------
// Description:
//      Class PndFsmCombiDet
//      
//  Detector class that combines responses from other detectors (same as PndFsmCombiDet)
//  Includes a simple parametrization for charged particles
//
//  This software was developed for the PANDA collaboration.  If you
//  use all or part of it, please give an appropriate acknowledgement.
//
// Author List:
//      Oscar Reinecke                   Original Author
//	Andreas Pitka			 Efficiency part
// 
// Copyright Information:
//      Copyright (C) 2008              GSI
//     
//	
//------------------------------------------------------------------------

#ifndef FSMCOMBIDET_HH
#define FSMCOMBIDET_HH

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating class Headers --
//-------------------------------
#include <iosfwd>
#include "ArgList.h"
#include "PndFsmAbsDet.h"
#include "PndFsmDetFactory.h"
#include <map>
#include "TFile.h"

//--------------------------------------------
// Collaborating class forward declarations --
// -------------------------------------------
class PndFsmTrack;
class PndFsmResponse;
class PndFsmAbsDet;
class TString;
class TSpline3;
class TF1;
class TF3;

template <class p> class TParameter;

class PndFsmCombiDet: public PndFsmAbsDet {
public:

  PndFsmCombiDet();
  PndFsmCombiDet(ArgList &par);
  virtual ~PndFsmCombiDet();

  virtual PndFsmResponse* respond(PndFsmTrack *t);

  // adds detector for pid contribution only 
  Bool_t AddDetector(std::string name, std::string params="");
  // this can be used to subtract a known detector
  // response from the parameterized track resolution
  Bool_t SubtractDetector(std::string name, std::string params="");

private:

  Bool_t setParameter(std::string &name, std::string &value);
  Bool_t setParameter(std::string &name, Double_t value);
  void readParameters();
  void initParameters();

  typedef std::list<PndFsmAbsDet*> FsmAbsDetList;
  FsmAbsDetList fDetList;
  FsmAbsDetList fSubtractDetList;
  PndFsmDetFactory fDetFact;

  TString _parFileName;
  TFile* fEffFile;
  
  Bool_t _parFile;
  Double_t _d0ResMulti;
  Double_t _z0ResMulti;
  Double_t _thtResMulti;
  Double_t _phiResMulti;
  Double_t _momResMulti;
  // this can be used do degrade the 
  // usually sharp likelihoods created 
  // by a typical fsim detector setup
  Double_t _pidLhMulti; 

  std::map<Int_t, TSpline3*> _d0;
  std::map<Int_t, TSpline3*> _z0;
  std::map<Int_t, TSpline3*> _tht;
  std::map<Int_t, TSpline3*> _phi;
  std::map<Int_t, TSpline3*> _mom;
  std::map<Int_t, TParameter<Double_t>*> _mom0;
  TParameter<Double_t>* _tht0;
  TParameter<Double_t>* _tht1;
  std::map<Int_t, TF1*> _d0Scale;
  std::map<Int_t, TF1*> _z0Scale;
  std::map<Int_t, TF1*> _thtScale;
  std::map<Int_t, TF1*> _phiScale;
  std::map<Int_t, TF1*> _momScale;

  TF3* fPipPara;
  TF3* fPimPara;
  TF3* fKpPara;
  TF3* fKmPara;
  TF3* fProtPara;
  TF3* fAntiProtPara;
  TF3* fepPara;
  TF3* femPara;
  TF3* fmupPara;
  TF3* fmumPara;
  
  Double_t eval(TSpline3* spline, Double_t theta);
};

#endif
