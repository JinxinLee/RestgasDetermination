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
  double getZGem()const {return _zGem;}
  double getZMax() const {return _zMax;}
  double getTDiff() const {return _tdiff;}
  double getTInt() const {return _tint;}
  double getTSig() const {return _tsig;}
  double getRMin() const {return _rmin;}
  double getRMax() const {return _rmax;}
  bool getAttach() const {return _attach>0;}
  bool getDiffuseL() const {return _diffuseL>0;}
  bool getDiffuseT() const {return _diffuseT>0;}
  double getGain() const {return _gain;}
  double getSupression() const {return _supression;}
  double getRate() const {return _rate;}
  double getMinSigAmp() const {return _minSignalAmp;}
  int getPSA() const {return _psa;}
  const PndTpcGas* getGas();
  const PndTpcGem* getGem();
  PndTpcPadPlane* getPadPlane();
  const PndTpcFrontend* getFrontend();

  bool getGaussianNoise() const {return _gaussianNoise != 0;}
  int getGaussianNoiseAmp() const {return _gaussianNoiseAmp;}

private :
  Int_t _tpcGasFile;
  TString _tpcGasFileName;
  Double_t _EField;
  Int_t _attach;
  Int_t _diffuseL;
  Int_t _diffuseT;
  Int_t _distort;
  Double_t _zGem; // get from Geom!
  Double_t _zMax; // get from Geom!

  Double_t _gain;
  Double_t _supression;
  Double_t _spread;
  Double_t _rate;

  Double_t _minSignalAmp; // cut on signal amplitude
  Double_t _rmin, _rmax; // cuts on the TPC volume in r should come from Geom!
  Int_t _padPlaneFile;
  TString _padPlaneFileName;
  Int_t _padShapes;
  TString _padShapesName;
  Double_t _shaperange; // range for padresponse lookuptable calculation
  Double_t _shapestep;  // LOT entry steps
  Double_t _intstep;    // integration step for LOT point calculation

  int _adcthres; // adc threshold
  int _adcmax; // adc dynamic range maximum
  int _adcbits; // adc # bits
  double _samplefreq; // sampling frequency
  double _t0; // t0 of time stamps
  int _timebits; // bits for timestamps
  int _psathres; // threshold for pulseshape analysis
  double _tdiff; // pulseshape: preamp differentiation time
  double _tint; // pulseshape: preamp integration time
  double _tsig; // pulseshape: signal risetime
  
  int _psa;	//0 -> Simple PSA, 1 -> TOTPSA

  int _gaussianNoise;
  int _gaussianNoiseAmp;

  // interface pointers (will be created on the fly):
  PndTpcGas* _gas; //!
  PndTpcGem* _gem; //!
  PndTpcPadPlane* _padplane; //!
  PndTpcFrontend* _frontend; //!
  PndTpcPadShapePool* _padshapepool; //!

  const PndTpcPadShapePool* getPadShapes();
  TString readString(int i);

  ClassDef(PndTpcDigiPar,1);
};

#endif
