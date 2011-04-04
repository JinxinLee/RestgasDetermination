#ifndef PNDTPCCLUSTERFITLIKELI_HH
#define PNDTPCCLUSTERFITLIKELI_HH 1

// Base Class Headers ----------------
#include "Math/IFunction.h"

#include <vector>
#include <TVector2.h>

class PndTpcPRLookupTable;

class PndTpcClusterFitLikeli : public ROOT::Math::IBaseFunctionMultiDim {
 public:
  
  // Constructors/Destructors ---------
  PndTpcClusterFitLikeli(){}
  PndTpcClusterFitLikeli(unsigned int nsigmaSteps,
  			 double sigmaMin, double sigmaMax,
  			 double irange, double step,
  			 double intStep);
  virtual ~PndTpcClusterFitLikeli(){}
  
  
  // Accessors -----------------------
  /// x,y,sig_x,sig_y, Amp of charge cloud
  virtual unsigned int NDim() const {return 5;}
  
  // Modifiers -----------------------
  void init(const std::vector<TVector2>* digis,
	    const std::vector<double>* amps){}// {fdigis=digis;famps=amps;}
  
  // Operations ----------------------
  virtual double DoEval(const double* par) const;
  
  virtual ROOT::Math::IBaseFunctionMultiDim* Clone() const {return new PndTpcClusterFitLikeli(*this);}
  
  
 private:
  

  const std::vector<TVector2>* fdigis;
  const std::vector<double>* famps;
  std::vector<std::vector<PndTpcPRLookupTable*>* > flookUp; // lookuptables
  std::vector<double> fSigmas; // 

  double fnSigmaSteps;
  double fsigMin;
  double fsigMax;

  double PRF(double x, double y,double sigma, unsigned int padShapeID);


};

#endif
