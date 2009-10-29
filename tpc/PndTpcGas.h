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
	 double const xVDrift,
	 double const xDl,
	 double const xDt,
	 double const xk,
	 double const xW,
	 const std::vector<double>& xCSD,
	 double const xCSDEpol);

  PndTpcGas(const std::string& Filename,
	 double const E);

  // accessors
  double VDrift() const {return fVDrift;} 
  double Dl() const {return fDl;}      
  double Dt() const {return fDt;}  
  double VDrift(double const E, double const B) const {return fVDrift;} 
  double Dl(double const E, double const B) const {return fDl;}      
  double Dt(double const E, double const B) const {return fDt;}      
  double k() const {return fk;}       
  double W() const {return fW;}       
  double CSD(int i) const {return fCSD.at(i);}
  const std::vector<double>& CSD() const {return fCSD;}
  int nCSD() const {return fCSD.size();}       
  double CSDEpol() const {return fCSDEpol;} 

  double GetE() const {return fE;}
  double GetB() const {return fB;}
  double GetT() const {return fT;}       
  double Getp() const {return fp;}
    
  int GetRandomCS(double const r) const;

  void PrintAll(std::ostream& s) const {s<<*this;}

  void operator=(const PndTpcGas& GasToCopy);
  friend std::ostream& operator<< (std::ostream&, const PndTpcGas&);

  // modifiers
  void SetE(double const E){fE=E;}  // later this method should also retrieve
                                    // updated gas values for the new field!
  void SetB(double const B){fB=B;}
  void SetT(double const T){fT=T;}
  void Setp(double const p){fp=p;}
  void SetCSD(const std::vector<double>& csd);
  void SetCSDEpol(double const csdepol){fCSDEpol=csdepol;}

private:
  double fE;        // electric field [V/cm](some gas parameters depend on it)
  double fB;        // B field [T] assumption: B || E !!! 
  double fT;        // Temperature [K]
  double fp;        // pressure [mbar]

  double fVDrift;   // electron Drift velocity [cm/ns]
  double fDl;       // longitudinal diffusion coefficient [sqrt(cm)]
  double fDt;       // transversal diffusion coefficient [sqrt(cm)]
  double fk;        // attachment coefficient[1/cm]
  double fW;        // effective ionisation energy [eV]
  std::vector<double> fCSD;     // Cluster size distribution
  double fCSDEpol;  // Constant used for the inverse quadratic extrapolation
                    // of Cluster Sizes bigger than fnCSD

  double LinExpolation(double const inTable, const double* const table,
		       int const nTable);
  int ReadGasBegin(std::ifstream* const pinfile);
  int ReadGasArrays(std::ifstream* const pinfile, int const noent,
		    double* const e, double* const vdrift, double* const dt,
		    double* const dl, double* const k); 
  double GetPositionOfE(int const noent, const double* const e);
 

};

#endif
