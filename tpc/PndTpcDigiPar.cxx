// /////////////////////////////////////////////////////////////
// // PndTpcDigiPar
// //
// // Container class for Digitisation parameters
// // class is inherited from FairParGenericSet
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
  : FairParGenericSet(name,title,context),
    fgas(NULL), fgem(NULL), fpadplane(NULL), ffrontend(NULL), fpadshapepool(NULL)
{
	clear();
}

PndTpcDigiPar::~PndTpcDigiPar()
{
  if(fgas!=NULL)delete fgas;
  if(fgem!=NULL)delete fgem;
  if(fpadplane!=NULL)delete fpadplane;
  if(fpadshapepool!=NULL)delete fpadshapepool;
  if(ffrontend!=NULL)delete ffrontend;

}


void PndTpcDigiPar::putParams(FairParamList* list)
{
	if(!list) return;
	list->add("PndTpcGasFile",ftpcGasFile);
	list->add("EField",fEField);
	list->add("AttachFlag",fattach);
	list->add("LogitudinaldiffusionFlag",fdiffuseL);
	list->add("TransversediffusionFlag",fdiffuseT);
	list->add("DriftDistortionFlag",fdistort);
	list->add("zGem",fzGem);
	list->add("zMax",fzMax);
	list->add("Gain",fgain);
	list->add("Supression", fsupression);
	list->add("Spread",fspread);
	list->add("MinSignalAmp",fminSignalAmp);
	list->add("rMin",frmin);
	list->add("rMax",frmax);
	list->add("PadPlaneFile",fpadPlaneFile);
	list->add("PadShapeFile",fpadShapes);
	list->add("PadShapeRange",fshaperange);
	list->add("PadShapeStep",fshapestep);
	list->add("PadShapeIntStep",fintstep);
	list->add("ADCThreshold",fadcthres);
	list->add("ADCMax",fadcmax);
	list->add("ADCBits",fadcbits);
	list->add("SamplingFreq",fsamplefreq);
	list->add("T0_wallclock",ft0);
	list->add("TimeBits",ftimebits);
	list->add("PSAThreshold",fpsathres);
	list->add("Shaper_tDiff",ftdiff);
	list->add("Shaper_tInt",ftint);
	list->add("Shaper_tSig",ftsig);
	list->add("TOTPSA",fpsa);
	list->add("GAUSSIANNOISE",fgaussianNoise);
	list->add("GAUSSIANNOISEAMP",fgaussianNoiseAmp);
	list->add("Rate",frate);
}

Bool_t PndTpcDigiPar::getParams(FairParamList* list)
{
 	if (!list) {std::cout<<"list not found"<<std::endl; return kFALSE;}
	if(!list->fill("EField",&fEField)) 
	  {std::cout<<"par: EField not found"<<std::endl; return kFALSE;}
	if(!list->fill("PndTpcGasFile",&ftpcGasFile)) 
	  {std::cout<<"par: PndTpcGasFile not found"<<std::endl; return kFALSE;}
	if(!list->fill("AttachFlag",&fattach)) 
	  {std::cout<<"par: AttachFlag not found"<<std::endl; return kFALSE;}
	if(!list->fill("LogitudinaldiffusionFlag",&fdiffuseL)) 
	  {std::cout<<"par: LogitudinaldiffusionFlag not found"<<std::endl; return kFALSE;}
	if(!list->fill("TransversediffusionFlag",&fdiffuseT)) 
	  {std::cout<<"par: TransversediffusionFlag not found"<<std::endl; return kFALSE;}
	if(!list->fill("DriftDistortionFlag",&fdistort)) 
	  {std::cout<<"par: DriftDistortionFlag not found"<<std::endl; return kFALSE;}
	if(!list->fill("zGem",&fzGem)) 
	  {std::cout<<"par: zGem not found"<<std::endl; return kFALSE;}
	if(!list->fill("zMax",&fzMax)) 
	  {std::cout<<"par: zMax not found"<<std::endl; return kFALSE;}
	if(!list->fill("Gain",&fgain)) 
	  {std::cout<<"par: Gain not found"<<std::endl; return kFALSE;}
	if(!list->fill("Supression",&fsupression)) 
	  {std::cout<<"par: Supression not found"<<std::endl; return kFALSE;}
	if(!list->fill("Spread",&fspread)) 
	  {std::cout<<"par: Spread not found"<<std::endl; return kFALSE;}
	if(!list->fill("MinSignalAmp",&fminSignalAmp)) 
	  {std::cout<<"par: MinSignalAmp not found"<<std::endl; return kFALSE;}
	if(!list->fill("rMin",&frmin)) 
	  {std::cout<<"par: rMin not found"<<std::endl; return kFALSE;}
	if(!list->fill("rMax",&frmax)) 
	  {std::cout<<"par: rMax not found"<<std::endl; return kFALSE;}
	if(!list->fill("PadPlaneFile",&fpadPlaneFile)) 
	  {std::cout<<"par: PadPlaneFile not found"<<std::endl; return kFALSE;}
	if(!list->fill("PadShapeFile",&fpadShapes)) 
	  {std::cout<<"par: PadShapeFile not found"<<std::endl; return kFALSE;}
	if(!list->fill("PadShapeRange",&fshaperange)) 
	  {std::cout<<"par: PadShapeRange not found"<<std::endl; return kFALSE;}
	if(!list->fill("PadShapeStep",&fshapestep)) 
	  {std::cout<<"par: PadShapeStep not found"<<std::endl; return kFALSE;}
	if(!list->fill("PadShapeIntStep",&fintstep)) 
	  {std::cout<<"par: PadShapeIntStep not found"<<std::endl; return kFALSE;}
	if(!list->fill("ADCThreshold",&fadcthres)) 
	  {std::cout<<"par: ADCThreshold not found"<<std::endl; return kFALSE;}
	if(!list->fill("ADCMax",&fadcmax)) 
	  {std::cout<<"par: ADCMax not found"<<std::endl; return kFALSE;}
	if(!list->fill("ADCBits",&fadcbits)) 
	  {std::cout<<"par: ADCBits not found"<<std::endl; return kFALSE;}
	if(!list->fill("SamplingFreq",&fsamplefreq)) 
	  {std::cout<<"par: SamplingFreq not found"<<std::endl; return kFALSE;}
	if(!list->fill("T0_wallclock",&ft0)) 
	  {std::cout<<"par: T0_wallclock not found"<<std::endl; return kFALSE;}
	if(!list->fill("TimeBits",&ftimebits)) 
	  {std::cout<<"par: TimeBits not found"<<std::endl; return kFALSE;}
	if(!list->fill("PSAThreshold",&fpsathres)) 
	  {std::cout<<"par: PSAThreshold not found"<<std::endl; return kFALSE;}
	if(!list->fill("Shaper_tDiff",&ftdiff)) 
	  {std::cout<<"par: Shaper_tDiff not found"<<std::endl; return kFALSE;}
	if(!list->fill("Shaper_tInt",&ftint)) 
	  {std::cout<<"par: Shaper_tInt not found"<<std::endl; return kFALSE;}
	if(!list->fill("Shaper_tSig",&ftsig)) 
	  {std::cout<<"par: Shaper_tSig not found"<<std::endl; return kFALSE;}
	if(!list->fill("TOTPSA",&fpsa)) 
	  {std::cout<<"par: TOTPSA not found"<<std::endl; return kFALSE;}
	if(!list->fill("GAUSSIANNOISE",&fgaussianNoise)) 
	  {std::cout<<"par: GAUSSIANNOISE not found"<<std::endl; return kFALSE;}
	if(!list->fill("GAUSSIANNOISEAMP",&fgaussianNoiseAmp)) 
	  {std::cout<<"par: GAUSSIANNOISEAMP not found"<<std::endl; return kFALSE;}
	if(!list->fill("Rate",&frate)) 
	  {std::cout<<"par: Rate not found"<<std::endl; return kFALSE;}  
	
	// read strings
	ftpcGasFileName=readString(ftpcGasFile);
	fpadPlaneFileName=readString(fpadPlaneFile);
	fpadShapesName=readString(fpadShapes);
	

	// update interfaces!
	// Todo: implement chaching!
	try{
	if(fgas!=NULL){
	  delete fgas;
	  getGas();
	}
	if(fgem!=NULL){
	  delete fgem;
	  getGem();
	}
	
	if(fpadplane!=NULL){
	  delete fpadplane;
	  if(fpadshapepool!=NULL){
	    delete fpadshapepool;
	    // padshapes should be reloaded by padplane
	  }
	  getPadPlane();
	}

	if(ffrontend!=NULL){
	  delete ffrontend;
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
  if(fgas==NULL){
    // load gasfile
    fgas=new PndTpcGas(ftpcGasFileName.Data(),fEField);
  }
  return fgas;
}

const PndTpcGem* 
PndTpcDigiPar::getGem(){
  if(fgem==NULL){
    fgem=new PndTpcGem(fgain,fspread);
  }
  return fgem;
}

PndTpcPadPlane* 
PndTpcDigiPar::getPadPlane(){
  if(fpadplane==NULL){
    fpadplane=new PndTpcPadPlane(fpadPlaneFileName.Data(),getPadShapes());
  }
  return fpadplane;
}

const PndTpcPadShapePool* 
PndTpcDigiPar::getPadShapes(){
  if(fpadshapepool==NULL){
    fpadshapepool=new PndTpcPadShapePool(fpadShapesName.Data(),*getGem(),
				   fshaperange,fshapestep,fintstep);
  }
  return fpadshapepool;
}

const PndTpcFrontend* 
PndTpcDigiPar::getFrontend(){
  if(ffrontend==NULL){
    ffrontend=new PndTpcFrontend(ftint,ftdiff,ftsig,
			      fadcthres,fadcmax,fadcbits,
			      fsamplefreq,ft0,ftimebits,
			      fpsathres);
  }
  return ffrontend;
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
