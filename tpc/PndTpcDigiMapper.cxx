#include "PndTpcDigiMapper.h"
#include "PndTpcPad.h"
#include "PndTpcAbsPadShape.h"

#ifdef TESTCHAMBER
#include "Calib.h"
#endif

PndTpcDigiMapper* PndTpcDigiMapper::finstance = NULL;

PndTpcDigiMapper::~PndTpcDigiMapper() {
  // do not delete the members! they might not belong to you!!!
}

PndTpcDigiMapper::PndTpcDigiMapper(bool autoinit) {

  if(autoinit){
    // objects instantiated here may be replace with the init method!
    fgas= new PndTpcGas("NEON-90_CO2-10_B2_PRES1013.asc",400);
    
    //TODO: Get these things from Database!!!
    fgem=new PndTpcGem(5000,           // Gain
		       0.02);          // Spread
    
    fzGem=0.;
    
    fpadShapes = new PndTpcPadShapePool("2mmPads.dat",
					*fgem,
					0.5, // lookup range
					0.02, // Lookup Step
					0.01); // LookupIntegrationStep
    
    fpadPlane= new PndTpcPadPlane("padplane.dat", fpadShapes);
    
  }
}

double PndTpcDigiMapper::z_from_tick(double t,double vdr){
  double v=vdr;
  if(v<0.){
    v=fgas->VDrift();
  }
  return (t*ftbin+ft0)*v+fzGem;
}

double PndTpcDigiMapper::t_to_ticks(double t) const {
  return (t-ft0)/ftbin;
}

void PndTpcDigiMapper::map(const PndTpcDigi* const fdig, TVector3& fvec) {
  double x,y,z;
#ifndef TESTCHAMBER
  z=z_from_tick(fdig->t());
#else
  z =z_from_tick(fdig->t(),Calib::getInstance()->get_vdr());
#endif
  fpadPlane->GetPadXY(fdig->padId(),x,y);
  fvec.SetXYZ(x,y,z);
}

void PndTpcDigiMapper::padsize(const unsigned int id,
			    double& dx, double& dy){
  fpadPlane->GetPad(id)->shape()->EvalBoundingRect(dx, dy,0);
}

PndTpcPad* 
PndTpcDigiMapper::getPad(unsigned int id){return fpadPlane->GetPad(id);}


void 
PndTpcDigiMapper::init(PndTpcPadPlane* plane,
		       const PndTpcGem* gem,
		       const PndTpcGas* gas,
		       const PndTpcPadShapePool*padShapes,
		       double zgem,
		       double  t0,
		       double sampleFreq) {
  fzGem=zgem;
  fgas=gas;
  fpadPlane=plane;
  fpadShapes=padShapes;
  fgem=gem;
  ft0=t0;
  ftbin=1./sampleFreq*1000; // 1/MHz -> ns
  
}

