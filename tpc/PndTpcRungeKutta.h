#include "TVector3.h"

#ifndef TPCRUNGEKUTTA_H
#define TPCRUNGEKUTTA_H

class PndTpcEFieldCyl;  //E-Field
class PndMultiField;   //B-Field
class PndFieldMap;


class PndTpcRungeKutta {
public:
  
  PndTpcRungeKutta(const double timeStep,
		   const double maxRelError,
		   const double maxRelToSpeedError,
		   const double maxAbsoluteError,
		   const double specificCharge,
		   PndTpcEFieldCyl* eField,
		   PndMultiField* bField,
		   const double friction);
  
  PndTpcRungeKutta(const double timeStep,
		   const double maxRelError,
		   const double maxRelToSpeedError,
		   const double maxAbsoluteError,
		   const double specificCharge,
		   const char* eFieldFile,
		   const char* bFieldFile,
		   const double friction);
  
  ~PndTpcRungeKutta();

  void setTimeStep(const double newTimeStep) {fdt=newTimeStep;}
  double getTimeStep() const {return(fdt);}

  /*Easy way to set constant fields without changing input files */
  void setConstE(TVector3);
  void setConstB(TVector3);
  
  // advances the point by the global dt and returns an error estimate
  bool stepForwards(double* Point);
  

 private:
  
  double fdt;  		//time step
  double fepsilon; 	//the error may be fepsilon* the old position  +
  double fepsilonDeriv; // fepsilonDeriv * the velocity at the old position +
  double fepsilonAbs; 	// + fepsilonAbs
  double fsc;  		//specific charge
  double ffriction;
   
  PndTpcEFieldCyl* feField; 	//e-field generated from DOLFIN output
  PndMultiField* fbField;       //b-field generated from combined field maps
  PndTpcEFieldCyl* fbFieldCyl;
  double fdglmat[6][6];   // this matrix and this 
  double fdgladd[6];      // vector contain the differential equation


  //constants used for Runge Kutta fifth order by Cash and Carp (see W.H Pres)
  double frk5b[6][5];
  double frk5c1[6];
  double frk5c2[6];

  bool fconstE_flag;
  bool fconstB_flag;

  TVector3 fconstE;
  TVector3 fconstB;
  
  void getNextPoint(double* Point, double* errorEstimate);
  void derivates(const double*  const point, double*derivate);

  void init();
};

#endif
