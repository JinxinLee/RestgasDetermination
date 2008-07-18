// -----------------------------------------------------------------
// $Id: PndTpcGas.hh,v 1.10 2006/03/10 14:45:24 sneubert Exp $
//
// Description:
//      Data class which provides access to various gas-parameters 
//      for drift detectors
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert         TUM            Original Author
//      Cristoforo Simonetto      TUM
//
// -----------------------------------------------------------------

#ifndef TPCGAS_HH
#define TPCGAS_HH

#include "assert.h"
#include <ostream>
#include <vector>
#include <string>

enum gascomponents {Ne,Ar,CO2,CH4};

class PndTpcGas {
public:
  // constructors
  PndTpcGas();
  ~PndTpcGas();
  
  PndTpcGas(double const E,
	 double const B,
	 double const T,
	 double const p,
	 double const VDrift,
	 double const Dl,
	 double const Dt,
	 double const k,
	 double const W,
	 const std::vector<double>& CSD,
	 double const CSDEpol);

  PndTpcGas(const std::string& Filename,
	 double const E);

  // accessors
  double VDrift() const {return _VDrift;} 
  double Dl() const {return _Dl;}      
  double Dt() const {return _Dt;}  
  double VDrift(double const E, double const B) const {return _VDrift;} 
  double Dl(double const E, double const B) const {return _Dl;}      
  double Dt(double const E, double const B) const {return _Dt;}      
  double k() const {return _k;}       
  double W() const {return _W;}       
  double CSD(int i) const {return _CSD.at(i);}
  const std::vector<double>& CSD() const {return _CSD;}
  int nCSD() const {return _CSD.size();}       
  double CSDEpol() const {return _CSDEpol;} 

  double E() const {return _E;}
  double B() const {return _B;}
  double T() const {return _T;}       
  double p() const {return _p;}
    
  int GetRandomCS(double const r) const;

  void PrintAll(std::ostream& s) const {s<<*this;}

  void operator=(const PndTpcGas& GasToCopy);
  friend std::ostream& operator<< (std::ostream&, const PndTpcGas&);

  // modifiers
  void SetE(double const E){_E=E;}  // later this method should also retrieve
                                    // updated gas values for the new field!
  void SetB(double const B){_B=B;}
  void SetT(double const T){_T=T;}
  void Setp(double const p){_p=p;}
  void SetCSD(const std::vector<double>& CSD);
  void SetCSDEpol(double const CSDEpol){_CSDEpol=CSDEpol;}

private:
  double _E;        // electric field [V/cm](some gas parameters depend on it)
  double _B;        // B field [T] assumption: B || E !!! 
  double _T;        // Temperature [K]
  double _p;        // pressure [mbar]

  double _VDrift;   // electron Drift velocity [cm/ns]
  double _Dl;       // longitudinal diffusion coefficient [sqrt(cm)]
  double _Dt;       // transversal diffusion coefficient [sqrt(cm)]
  double _k;        // attachment coefficient[1/cm]
  double _W;        // effective ionisation energy [eV]
  std::vector<double> _CSD;     // Cluster size distribution
  double _CSDEpol;  // Constant used for the inverse quadratic extrapolation
                    // of Cluster Sizes bigger than _nCSD

  double LinExpolation(double const inTable, const double* const table,
		       int const nTable);
  int ReadGasBegin(std::ifstream* const pinfile);
  int ReadGasArrays(std::ifstream* const pinfile, int const noent,
		    double* const e, double* const vdrift, double* const dt,
		    double* const dl, double* const k); 
  double GetPositionOfE(int const noent, const double* const e);
 

};

#endif
