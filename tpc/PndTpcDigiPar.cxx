// /////////////////////////////////////////////////////////////
// // PndTpcDigiPar
// //
// // Container class for Digitisation parameters
// // class is inherited from CbmParGenericSet
// //
// /////////////////////////////////////////////////////////////

#include "PndTpcDigiPar.h"
#include <iostream>
#include <fstream>
#include <exception>
#include "TSystem.h"

#include "PndTpcGas.h"
#include "PndTpcGem.h"
#include "PndTpcPadShapePool.h"
#include "PndTpcPadPlane.h"
#include "PndTpcFrontend.h"

ClassImp(PndTpcDigiPar)

PndTpcDigiPar::PndTpcDigiPar(const char* name,const char* title,
			     const char* context)
  : CbmParGenericSet(name,title,context),
    _gas(NULL), _gem(NULL), _padplane(NULL), _frontend(NULL), _padshapepool(NULL)
{
	clear();
}

PndTpcDigiPar::~PndTpcDigiPar()
{
  if(_gas!=NULL)delete _gas;
  if(_gem!=NULL)delete _gem;
  if(_padplane!=NULL)delete _padplane;
  if(_padshapepool!=NULL)delete _padshapepool;
  if(_frontend!=NULL)delete _frontend;

}


void PndTpcDigiPar::putParams(CbmParamList* list)
{
	if(!list) return;
	list->add("PndTpcGasFile",_tpcGasFile);
	list->add("EField",_EField);
	list->add("AttachFlag",_attach);
	list->add("LogitudinaldiffusionFlag",_diffuseL);
	list->add("TransversediffusionFlag",_diffuseT);
	list->add("DriftDistortionFlag",_distort);
	list->add("zGem",_zGem);
	list->add("Gain",_gain);
	list->add("Spread",_spread);
	list->add("MinSignalAmp",_minSignalAmp);
	list->add("rMin",_rmin);
	list->add("rMax",_rmax);
	list->add("PadPlaneFile",_padPlaneFile);
	list->add("PadShapeFile",_padShapes);
	list->add("PadShapeRange",_shaperange);
	list->add("PadShapeStep",_shapestep);
	list->add("PadShapeIntStep",_intstep);
	list->add("ADCThreshold",_adcthres);
	list->add("ADCMax",_adcmax);
	list->add("ADCBits",_adcbits);
	list->add("SamplingFreq",_samplefreq);
	list->add("T0_wallclock",_t0);
	list->add("TimeBits",_timebits);
	list->add("PSAThreshold",_psathres);
	list->add("Shaper_tDiff",_tdiff);
	list->add("Shaper_tInt",_tint);
	list->add("Shaper_tSig",_tsig);
	list->add("TOTPSA",_psa);
	list->add("GAUSSIANNOISE",_gaussianNoise);
	list->add("GAUSSIANNOISEAMP",_gaussianNoiseAmp);
}

Bool_t PndTpcDigiPar::getParams(CbmParamList* list)
{
 	if (!list) {std::cout<<"list not found"<<std::endl; return kFALSE;}
	if(!list->fill("EField",&_EField)) 
	  {std::cout<<"par: EField not found"<<std::endl; return kFALSE;}
	if(!list->fill("PndTpcGasFile",&_tpcGasFile)) 
	  {std::cout<<"par: PndTpcGasFile not found"<<std::endl; return kFALSE;}
	if(!list->fill("AttachFlag",&_attach)) 
	  {std::cout<<"par: AttachFlag not found"<<std::endl; return kFALSE;}
	if(!list->fill("LogitudinaldiffusionFlag",&_diffuseL)) 
	  {std::cout<<"par: LogitudinaldiffusionFlag not found"<<std::endl; return kFALSE;}
	if(!list->fill("TransversediffusionFlag",&_diffuseT)) 
	  {std::cout<<"par: TransversediffusionFlag not found"<<std::endl; return kFALSE;}
	if(!list->fill("DriftDistortionFlag",&_distort)) 
	  {std::cout<<"par: DriftDistortionFlag not found"<<std::endl; return kFALSE;}
	if(!list->fill("zGem",&_zGem)) 
	  {std::cout<<"par: zGem not found"<<std::endl; return kFALSE;}
	if(!list->fill("Gain",&_gain)) 
	  {std::cout<<"par: Gain not found"<<std::endl; return kFALSE;}
	if(!list->fill("Spread",&_spread)) 
	  {std::cout<<"par: Spread not found"<<std::endl; return kFALSE;}
	if(!list->fill("MinSignalAmp",&_minSignalAmp)) 
	  {std::cout<<"par: MinSignalAmp not found"<<std::endl; return kFALSE;}
	if(!list->fill("rMin",&_rmin)) 
	  {std::cout<<"par: rMin not found"<<std::endl; return kFALSE;}
	if(!list->fill("rMax",&_rmax)) 
	  {std::cout<<"par: rMax not found"<<std::endl; return kFALSE;}
	if(!list->fill("PadPlaneFile",&_padPlaneFile)) 
	  {std::cout<<"par: PadPlaneFile not found"<<std::endl; return kFALSE;}
	if(!list->fill("PadShapeFile",&_padShapes)) 
	  {std::cout<<"par: PadShapeFile not found"<<std::endl; return kFALSE;}
	if(!list->fill("PadShapeRange",&_shaperange)) 
	  {std::cout<<"par: PadShapeRange not found"<<std::endl; return kFALSE;}
	if(!list->fill("PadShapeStep",&_shapestep)) 
	  {std::cout<<"par: PadShapeStep not found"<<std::endl; return kFALSE;}
	if(!list->fill("PadShapeIntStep",&_intstep)) 
	  {std::cout<<"par: PadShapeIntStep not found"<<std::endl; return kFALSE;}
	if(!list->fill("ADCThreshold",&_adcthres)) 
	  {std::cout<<"par: ADCThreshold not found"<<std::endl; return kFALSE;}
	if(!list->fill("ADCMax",&_adcmax)) 
	  {std::cout<<"par: ADCMax not found"<<std::endl; return kFALSE;}
	if(!list->fill("ADCBits",&_adcbits)) 
	  {std::cout<<"par: ADCBits not found"<<std::endl; return kFALSE;}
	if(!list->fill("SamplingFreq",&_samplefreq)) 
	  {std::cout<<"par: SamplingFreq not found"<<std::endl; return kFALSE;}
	if(!list->fill("T0_wallclock",&_t0)) 
	  {std::cout<<"par: T0_wallclock not found"<<std::endl; return kFALSE;}
	if(!list->fill("TimeBits",&_timebits)) 
	  {std::cout<<"par: TimeBits not found"<<std::endl; return kFALSE;}
	if(!list->fill("PSAThreshold",&_psathres)) 
	  {std::cout<<"par: PSAThreshold not found"<<std::endl; return kFALSE;}
	if(!list->fill("Shaper_tDiff",&_tdiff)) 
	  {std::cout<<"par: Shaper_tDiff not found"<<std::endl; return kFALSE;}
	if(!list->fill("Shaper_tInt",&_tint)) 
	  {std::cout<<"par: Shaper_tInt not found"<<std::endl; return kFALSE;}
	if(!list->fill("Shaper_tSig",&_tsig)) 
	  {std::cout<<"par: Shaper_tSig not found"<<std::endl; return kFALSE;}
	if(!list->fill("TOTPSA",&_psa)) 
	  {std::cout<<"par: TOTPSA not found"<<std::endl; return kFALSE;}
	if(!list->fill("GAUSSIANNOISE",&_gaussianNoise)) 
	  {std::cout<<"par: GAUSSIANNOISE not found"<<std::endl; return kFALSE;}
	if(!list->fill("GAUSSIANNOISEAMP",&_gaussianNoiseAmp)) 
	  {std::cout<<"par: GAUSSIANNOISEAMP not found"<<std::endl; return kFALSE;}
	  
	// read strings
	_tpcGasFileName=readString(_tpcGasFile);
	_padPlaneFileName=readString(_padPlaneFile);
	_padShapesName=readString(_padShapes);
	

	// update interfaces!
	// Todo: implement chaching!
	try{
	if(_gas!=NULL){
	  delete _gas;
	  getGas();
	}
	if(_gem!=NULL){
	  delete _gem;
	  getGem();
	}
	
	if(_padplane!=NULL){
	  delete _padplane;
	  if(_padshapepool!=NULL){
	    delete _padshapepool;
	    // padshapes should be reloaded by padplane
	  }
	  getPadPlane();
	}

	if(_frontend!=NULL){
	  delete _frontend;
	  getFrontend();
	}
	}
	catch (std::exception& ex){
	  std::cout<<ex.what()<<std::endl;
	  throw ex;
	}

	return kTRUE;
}

const PndTpcGas* 
PndTpcDigiPar::getGas(){
  if(_gas==NULL){
    // load gasfile
    _gas=new PndTpcGas(_tpcGasFileName.Data(),_EField);
  }
  return _gas;
}

const PndTpcGem* 
PndTpcDigiPar::getGem(){
  if(_gem==NULL){
    _gem=new PndTpcGem(_gain,_spread);
  }
  return _gem;
}

PndTpcPadPlane* 
PndTpcDigiPar::getPadPlane(){
  if(_padplane==NULL){
    _padplane=new PndTpcPadPlane(_padPlaneFileName.Data(),getPadShapes());
  }
  return _padplane;
}

const PndTpcPadShapePool* 
PndTpcDigiPar::getPadShapes(){
  if(_padshapepool==NULL){
    _padshapepool=new PndTpcPadShapePool(_padShapesName.Data(),*getGem(),
				   _shaperange,_shapestep,_intstep);
  }
  return _padshapepool;
}

const PndTpcFrontend* 
PndTpcDigiPar::getFrontend(){
  if(_frontend==NULL){
    _frontend=new PndTpcFrontend(_tint,_tdiff,_tsig,
			      _adcthres,_adcmax,_adcbits,
			      _samplefreq,_t0,_timebits,
			      _psathres);
  }
  return _frontend;
}

TString
PndTpcDigiPar::readString(int i){
  ifstream infile;
  TString sysFile = gSystem->Getenv("VMCWORKDIR");
  TString parFile = sysFile+"/tpc/tpc.files.par";
  infile.open(parFile.Data());
  //  infile.open("tpc/tpc.files.par");
  if(!infile){
    std::cout<<"File " << parFile.Data() << " not found!"<<std::endl;
    throw;
  }
  char buffer[256];
  for(int j=0;j<i+1;++j){
    if(infile.eof()){
      std::cout<<"Did not find string#"<<i<<"in file " << parFile.Data() <<std::endl;
      throw;
    }
    infile.getline(buffer,256);
  }
  infile.close();
  return TString(sysFile+"/"+buffer);
}
