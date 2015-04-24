// -------------------------------------------------------------------------
// -----                 PndRestGas header file                  -----
// -----               Created 14/12/09 by P.Buehler              -----
// -------------------------------------------------------------------------

/** PndRestGas.h
 ** @author P.Buehler <paul.buehler@oeaw.ac.at>

 The PndRestGas provides tools to simulate the distribution of the rest gas
 (DD: Density Distribution) in the beam tube.
**/

#ifndef PND_RESTGAS_H
#define PND_RESTGAS_H

#include "TF1.h"
#include "TGraph.h"

class PndRestGas
{

// ------------------------------------------------------------------------
 public: 

  /** Default constructor (should not be used) **/
  PndRestGas();

  /** Standard constructor
   ** DDFname: a file name
	 ** zmin/zmax: minimum/maximum extension of gas distribution along z-axis
   **/
  PndRestGas(char DDFname[80]);
  PndRestGas(char DDFname[80], Double_t zmin, Double_t zmax);

  /** Destructor **/
  virtual ~PndRestGas();
	
	/** methods
	 ** LoadDD:    read data from file and create DD function
	 ** SetzRange: set range of zvalues
	 ** GetDDFun:  get the DD function
	 **/
	void LoadDD(char DDfname[80]);
	void SetzRange(Double_t zmin, Double_t zmax) {
	  fzmin = zmin;
		fzmax = zmax;
	}
	TF1*  GetDDFun() {
	  return fDensityFun;
	}

// ------------------------------------------------------------------------
 private:

  /** methods
	 ** ReadDD        : read DD values from file
   **/
	Float_t** ReadDD(char DDFname[80], Int_t* npoints,
    Double_t* zmin, Double_t* zmax);
	
  /** variables
	 ** fzmin, fzmax:
	 ** fDDvals:
	 ** fDensityFun: function representing DD
   **/
  Double_t  fzmin, fzmax;
  Float_t** fDDvals;
	TF1*      fDensityFun;
	
// ------------------------------------------------------------------------

  ClassDef(PndRestGas,1);

};

#endif
