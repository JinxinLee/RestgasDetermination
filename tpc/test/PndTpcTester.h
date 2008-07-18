//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Testerclass for tpc package
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCTESTER_HH
#define TPCTESTER_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <vector>
#include "TMatrixD.h"

// Collaborating Class Declarations --
class PndTpcCluster;
class TGraph;
class PndTpcSignal;
class PndTpcAbsPulseshape;
class PndTpcFrontend;


class PndTpcTester : public TObject {
public:

  // Constructors/Destructors ---------
  PndTpcTester(){}
  virtual ~PndTpcTester(){}

  // Operations ----------------------
  void testDebugStream();
  void testDebugStreamFunctionBlock();
  TMatrixD testCOG();
  
  void testSpline();
  
  void SetUpTestSampling();
  void SetUpTestSampling(unsigned int adcthreshold, double amp1=1000, double amp2=1000, double t1=20, double t2=100);  
  void testSampling_Signal1(TGraph *pGraph);
  void testSampling_Signal2(TGraph *pGraph);
  void testSampling_Signal12(TGraph *pGraph); 
  void testSampling_Signal123(TGraph *pGraph); 
  
  void testMcId();

private:
  void testSampling(std::vector<PndTpcSignal*> *pv, TGraph *pGraph);
  PndTpcSignal *pSignal1;
  PndTpcSignal *pSignal2;
  PndTpcSignal *pSignal3;
  PndTpcAbsPulseshape* pPulseshape; 
  PndTpcFrontend* pFrontend;
  
  // Private Data Members ------------


  // Private Methods -----------------

public:
  ClassDef(PndTpcTester,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
