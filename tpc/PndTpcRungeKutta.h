

#ifndef TPCRUNGEKUTTA_H
#define TPCRUNGEKUTTA_H

class PndTpcEFieldCyl;  //E-Field
class PndMultiField;   //B-Field
class PndFieldMap;

class PndTpcRungeKutta
{
public:
  PndTpcRungeKutta(const double timeStep,
		const double maxRelError,
		const double maxRelToSpeedError,
		const double maxAbsoluteError,
		const double specificCharge,
		const char* eFieldFile,
		const char* bFieldFile,
		const double friction);
  
  ~PndTpcRungeKutta();
  void setTimeStep(const double newTimeStep) {_dt=newTimeStep;}
  double getTimeStep() const {return(_dt);}
  
  // advances the point by the global dt and returns an error estimate
  bool stepForwards(double* Point);

 private:
  
  double _dt;  		//time step
  double _epsilon; 	//the error may be _epsilon* the old position  +
  double _epsilonDeriv; // _epsilonDeriv * the velocity at the old position +
  double _epsilonAbs; 	// + _epsilonAbs
  double _sc;  		//specific charge
  
  PndTpcEFieldCyl* _eField; 	//e-field generated from DOLFIN output
  //PndMultiField* _bField;       //b-field generated from combined field maps
   PndTpcEFieldCyl* _bField;
  double _dglmat[6][6];   // this matrix and this 
  double _dgladd[6];      // vector contain the differential equation


  //constants used for Runge Kutta fifth order by Cash and Carp (see W.H Pres)
  double _rk5b[6][5];
  double _rk5c1[6];
  double _rk5c2[6];
  
  void getNextPoint(double* Point, double* errorEstimate);
  void derivates(const double*  const point, double*derivate);
};

#endif
