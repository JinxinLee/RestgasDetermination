//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Crosstalk correction for the Test Chamber TPC
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Maxence Vandenbroucke    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCTCCROSSTALK_HH
#define TPCTCCROSSTALK_HH

// Base Class Headers ----------------
#include "TVector3.h"
#include "PndTpcPadPlane.h"
#include "PndTpcDigi.h"
#include "PndTpcSample.h"

// Collaborating Class Headers -------
#include <ostream>

// Collaborating Class Declarations --
#include "PndTpcSample.h"

class QAPlotCollection;
class TClonesArray;
class FEC;
class TH1D;
class PndTpcTCcrossTalk {
 public:

  // Constructors/Destructors ---------

  PndTpcTCcrossTalk();
  ~PndTpcTCcrossTalk();

  void Init();

  //operator
  void  Process(std::vector<PndTpcSample*>* _sa);
  void  ProcessCut(std::vector<PndTpcSample*>* _sa);
  void CalcAmp(std::vector<PndTpcSample*>* _sa);
  void Fill();
  
  void Print();
  void SetQAPlotCol(QAPlotCollection* col){fqa=col;}
   


   void SetCrosstalkAmp(double * a) {for (int i = 0;i<10;i++)A[i]=a[i];}//amplitudes of crosstalk on the first 3 neighboring strips

 // Operations ----------------------

 private:
   QAPlotCollection* fqa;
  TH1D * cHist[10];
  double A[10];//crosstalk amplitude
  int AX[10];//normalisation in case of calculation
  // Private Data Members ------------

public:
  ClassDef(PndTpcTCcrossTalk,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
