//--------------------------------------------------------------------------
// Description:
//      Class PndFsmCmpDet
//      
//  Detector class that combines responses from oher detectors
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

#ifndef FsmCmpDet_hh
#define FsmCmpDet_hh

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

#include "TParameter.h"

//--------------------------------------------
// Collaborating class forward declarations --
// -------------------------------------------
class PndFsmTrack;
class PndFsmResponse;
class PndFsmAbsDet;
class TString;
class TSpline3;
class TF1;
template <class p> class TParameter;

class PndFsmCmpDet: public PndFsmAbsDet {
public:

  PndFsmCmpDet();
  PndFsmCmpDet(ArgList &par);
  virtual ~PndFsmCmpDet();

  virtual PndFsmResponse* respond(PndFsmTrack *t);

  // adds detector for pid contribution only 
  bool AddDetector(std::string name, std::string params="");
  // this can be used to subtract a known detector
  // response from the parameterized track resolution
  bool SubtractDetector(std::string name, std::string params="");

private:

  bool setParameter(std::string &name, std::string &value);
  bool setParameter(std::string &name, double value);
  void readParameters();
  void initParameters();

  typedef std::list<PndFsmAbsDet*> FsmAbsDetList;
  FsmAbsDetList fDetList;
  FsmAbsDetList fSubtractDetList;
  PndFsmDetFactory fDetFact;

  TString _parFileName;
  bool _parFile;
  double _d0ResMulti;
  double _z0ResMulti;
  double _thtResMulti;
  double _phiResMulti;
  double _momResMulti;
  // this can be used do degrade the 
  // usually sharp likelihoods created 
  // by a typical fsim detector setup
  double _pidLhMulti; 

  std::map<int, TSpline3*> _d0;
  std::map<int, TSpline3*> _z0;
  std::map<int, TSpline3*> _tht;
  std::map<int, TSpline3*> _phi;
  std::map<int, TSpline3*> _mom;
  std::map<int, TParameter<double>*> _mom0;
  TParameter<double>* _tht0;
  TParameter<double>* _tht1;
  std::map<int, TF1*> _d0Scale;
  std::map<int, TF1*> _z0Scale;
  std::map<int, TF1*> _thtScale;
  std::map<int, TF1*> _phiScale;
  std::map<int, TF1*> _momScale;

  double eval(TSpline3* spline, double theta);
};

#endif
