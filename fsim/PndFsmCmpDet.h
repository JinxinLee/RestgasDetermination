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

  bool AddDetector(std::string name, std::string params="");

private:

  bool setParameter(std::string &name, std::string &value);
  bool setParameter(std::string &name, double value);
  void readParameters();
  void initParameters();

  typedef std::list<PndFsmAbsDet*> FsmAbsDetList;
  FsmAbsDetList fDetList;
  PndFsmDetFactory fDetFact;

  TString _parFileName;
  bool _parFile;

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

  // estimates the variance of a weighted sum of gaussian 
  // distributed quantities that hold the same mean value
  class dX {
  public:
    dX();
    void operator+= (double variance);
    void operator = (double value);
    void operator*= (double value);
    operator double();
  private:
    char n;
    double b;
  };
};

#endif
