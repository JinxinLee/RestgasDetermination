#ifndef TPCDIGIMAPPER_H
#define TPCDIGIMAPPER_H

#include<iostream>

#include "TVector3.h"

#include "PndTpcGas.h"
#include "PndTpcDigi.h"
#include "PndTpcGem.h"
#include "PndTpcPadShapePool.h"
#include "PndTpcPadPlane.h"
#include "PndTpcPad.h"


//singleton that can give x,y,z from PndTpcDigi s
class PndTpcDigiMapper {
 private:
  virtual ~PndTpcDigiMapper();
  PndTpcDigiMapper(bool autoinit);

  static PndTpcDigiMapper *_instance;

  const PndTpcGas *_gas;
  const PndTpcGem *_gem;
  const PndTpcPadShapePool *_padShapes;
  PndTpcPadPlane *_padPlane;

  double _zGem;
  double _t0;
  double _tbin; // ns

 public:
  
  void init(PndTpcPadPlane* plane,
	    const PndTpcGem* gem,
	    const PndTpcGas* gas,
	    double zGem,
	    double t0,
	    double samplFreqMhz); // does NOT take over Ownership!

  void padsize(const unsigned int id,
	       double& dx, double& dy);
  void map(const PndTpcDigi* const _dig, TVector3& _vec);
  
  double t_from_z(double z);
  double z_from_tick(double t);

  PndTpcPadPlane* getPadPlane() {return _padPlane;}

  const PndTpcGas* const getGas() {return _gas;}

  double zGem() const {return _zGem;}

  PndTpcPad* getPad(unsigned int id);

  inline static PndTpcDigiMapper* getInstance(bool autoinit=true) {
	if(_instance == NULL) {
	  _instance = new PndTpcDigiMapper(autoinit);
	}
	return _instance;
  }

  inline static void destruct() {
	if(_instance != NULL) {
	  delete _instance;
	  _instance = NULL;
	}
  }

};

#endif //TPCDIGIMAPPER_H
