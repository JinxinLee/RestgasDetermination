#include "PndTpcDigiMapper.h"
#include "PndTpcPad.h"
#include "PndTpcAbsPadShape.h"

#ifdef TESTCHAMBER
#include "Calib.h"
#endif

PndTpcDigiMapper* PndTpcDigiMapper::_instance = NULL;

PndTpcDigiMapper::~PndTpcDigiMapper() {
  // do not delete the members! they might not belong to you!!!
}

PndTpcDigiMapper::PndTpcDigiMapper(bool autoinit) {

  if(autoinit){
    // objects instantiated here may be replace with the init method!
    _gas= new PndTpcGas("NEON-90_CO2-10_B2_PRES1013.asc",400);
    
    //TODO: Get these things from Database!!!
    _gem=new PndTpcGem(5000,           // Gain
		       0.02);          // Spread
    
    _zGem=0.;
    
    _padShapes = new PndTpcPadShapePool("2mmPads.dat",
					*_gem,
					0.5, // lookup range
					0.02, // Lookup Step
					0.01); // LookupIntegrationStep
    
    _padPlane= new PndTpcPadPlane("padplane.dat", _padShapes);
    
  }
}

double PndTpcDigiMapper::z_from_tick(double t,double vdr){
  double v=vdr;
  if(v<0.){
    v=_gas->VDrift();
  }
  return (t*_tbin+_t0)*v+_zGem;
}



void PndTpcDigiMapper::map(const PndTpcDigi* const _dig, TVector3& _vec) {
  double x,y,z;
#ifndef TESTCHAMBER
  z=z_from_tick(_dig->t());
#else
  z =z_from_tick(_dig->t(),Calib::getInstance()->get_vdr());
#endif
  _padPlane->GetPadXY(_dig->padId(),x,y);
  _vec.SetXYZ(x,y,z);
}

void PndTpcDigiMapper::padsize(const unsigned int id,
			    double& dx, double& dy){
  _padPlane->GetPad(id)->shape()->EvalBoundingRect(dx, dy,0);
}

PndTpcPad* 
PndTpcDigiMapper::getPad(unsigned int id){return _padPlane->GetPad(id);}


void 
PndTpcDigiMapper::init(PndTpcPadPlane* plane,
		    const PndTpcGem* gem,
		    const PndTpcGas* gas,
		    double zGem,
		    double  t0,
		    double sampleFreq) {
  _zGem=zGem;
  _gas=gas;
  _padPlane=plane;
  _gem=gem;
  _t0=t0;
  _tbin=1./sampleFreq*1000; // 1/MHz -> ns
  
}

