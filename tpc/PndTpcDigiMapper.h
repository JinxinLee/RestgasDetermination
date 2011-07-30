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

  static PndTpcDigiMapper *finstance;

  const PndTpcGas *fgas;
  const PndTpcGem *fgem;
  const PndTpcPadShapePool *fpadShapes;
  PndTpcPadPlane *fpadPlane;

  double fzGem;
  double ft0;
  double ftbin; // ns

 public:
  
  void init(PndTpcPadPlane* plane,
	    const PndTpcGem* gem,
	    const PndTpcGas* gas,
	    const PndTpcPadShapePool* padShapes,
	    double zgem,
	    double t0,
	    double samplFreqMhz); // does NOT take over Ownership!

  void padsize(const unsigned int id,
	       double& dx, double& dy);
  void map(const PndTpcDigi* const fdig, TVector3& fvec);
  
  double t_from_z(double z);
  double z_from_tick(double t,double vdr=-1.);
  double t_to_ticks(double t) const;
  double t_from_tick(double samples) const;

  PndTpcPadPlane* getPadPlane() {return fpadPlane;}
  const PndTpcPadShapePool * getPadShapePool() {return fpadShapes;}

  const PndTpcGas* const getGas() {return fgas;}
  const PndTpcGem* const getGem() {return fgem;}

  double zGem() const {return fzGem;}

  PndTpcPad* getPad(unsigned int id);

  inline static PndTpcDigiMapper* getInstance(bool autoinit=true) {
	if(finstance == NULL) {
	  finstance = new PndTpcDigiMapper(autoinit);
	}
	return finstance;
  }

  inline static void destruct() {
	if(finstance != NULL) {
	  delete finstance;
	  finstance = NULL;
	}
  }

};

#endif //TPCDIGIMAPPER_H
