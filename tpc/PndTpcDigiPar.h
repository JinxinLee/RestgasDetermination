#ifndef TPCDIGIPAR_H
#define TPCDIGIPAR_H

#include "FairParGenericSet.h"
#include "FairParamList.h"
#include "TString.h"

// interfaces to the PndTpcCode:
class PndTpcGas;
class PndTpcGem;
class PndTpcPadPlane;
class PndTpcFrontend;
class PndTpcPadShapePool;

class PndTpcDigiPar : public FairParGenericSet
{
public :
  
  PndTpcDigiPar (const char* name="PndTpcPars",
	      const char* title="PndTpc digi parameter",
	      const char* context="TestDefaultContext");

  ~PndTpcDigiPar();
  void clear(){};
  void putParams(FairParamList* list);
  Bool_t getParams(FairParamList* list);

  // Accessors (partially catched!)
  double getZGem()const {return fzGem;}
  double getZMax() const {return fzMax;}
  double getTDiff() const {return ftdiff;}
  double getTInt() const {return ftint;}
  double getTSig() const {return ftsig;}
  double getRMin() const {return frmin;}
  double getRMax() const {return frmax;}
  bool getAttach() const {return fattach>0;}
  bool getDiffuseL() const {return fdiffuseL>0;}
  bool getDiffuseT() const {return fdiffuseT>0;}
  double getGain() const {return fgain;}
  double getSupression() const {return fsupression;}
  double getRate() const {return frate;}
  double getMinSigAmp() const {return fminSignalAmp;}
  int getPSA() const {return fpsa;}
  const PndTpcGas* getGas();
  const PndTpcGem* getGem();
  PndTpcPadPlane* getPadPlane();
  const PndTpcFrontend* getFrontend();

  bool getGaussianNoise() const {return fgaussianNoise != 0;}
  int getGaussianNoiseAmp() const {return fgaussianNoiseAmp;}

private :
  Int_t ftpcGasFile;
  TString ftpcGasFileName;
  Double_t fEField;
  Int_t fattach;
  Int_t fdiffuseL;
  Int_t fdiffuseT;
  Int_t fdistort;
  Double_t fzGem; // get from Geom!
  Double_t fzMax; // get from Geom!

  Double_t fgain;
  Double_t fsupression;
  Double_t fspread;
  Double_t frate;

  Double_t fminSignalAmp; // cut on signal amplitude
  Double_t frmin, frmax; // cuts on the TPC volume in r should come from Geom!
  Int_t fpadPlaneFile;
  TString fpadPlaneFileName;
  Int_t fpadShapes;
  TString fpadShapesName;
  Double_t fshaperange; // range for padresponse lookuptable calculation
  Double_t fshapestep;  // LOT entry steps
  Double_t fintstep;    // integration step for LOT point calculation

  int fadcthres; // adc threshold
  int fadcmax; // adc dynamic range maximum
  int fadcbits; // adc # bits
  double fsamplefreq; // sampling frequency
  double ft0; // t0 of time stamps
  int ftimebits; // bits for timestamps
  int fpsathres; // threshold for pulseshape analysis
  double ftdiff; // pulseshape: preamp differentiation time
  double ftint; // pulseshape: preamp integration time
  double ftsig; // pulseshape: signal risetime
  
  int fpsa;	//0 -> Simple PSA, 1 -> TOTPSA

  int fgaussianNoise;
  int fgaussianNoiseAmp;

  // interface pointers (will be created on the fly):
  PndTpcGas* fgas; //!
  PndTpcGem* fgem; //!
  PndTpcPadPlane* fpadplane; //!
  PndTpcFrontend* ffrontend; //!
  PndTpcPadShapePool* fpadshapepool; //!

  const PndTpcPadShapePool* getPadShapes();
  TString readString(int i);

  ClassDef(PndTpcDigiPar,1);
};

#endif
