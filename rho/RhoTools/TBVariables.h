//==========================================================================
// File and Version Information:
// 	$Id: TBVariables.h,v 1.3 2001-05-15 07:17:46 marcel Exp $
//
//--------------------------------------------------------------------------
// Description:

// Class TBVariables takes a 4-momentum of a fully-reconstructed B
// candidate, and calculates the B analysis variables Delta E, 
// mhat, and m_ES, defined below.  See BaBar Note 497 and B.A.D. 53 for 
// details. 
// 
//
// Delta E = E - fEBeam,
// where E is the B candidate energy's in the center-of-mass (CM) frame,
// and fEBeam is the beam energy in that frame (ie., half the total CM energy).
//
// mhat = sqrt(Ehat^2 - phat^2),
// where Ehat and phat are the energy and absolute momentum of the B
// candidate in the CM frame, obtained with a kinematic fit. The fit employs 
// the non-Lagrangian "constraint" 
//    E = fEBeam +/- fEBeamErr, 
// where fEBeamErr is the beam energy spread, narrowed by the
// Upsilon(4S) resonance width (see Eq. B1 in BaBar Note 497).
//
// m_ES is the beam energy substituted mass, calculated from the lab four
// momentum of the beam system and the lab three momentum of the candidate.
// (originially proposed by W. Dunwoodie, 
// http://www.slac.stanford.edu/~wmd/bbkinematics/kinematics.note; see
// also B.A.D. 53.)
//
// (The "traditional" energy-substituted mass calculation performed with
// CM four-momenta is available from energySubstitutedMass(); this function
// is deprecated.)
//--------------------------------------------------------------------------
// Simple User Code:
//
// Instantiate a TBVariables, giving it the 4-momentum
// (TLorentzVectorErr, so that measurement errors are specified) of
// your B candidate the the lab, then ntuple the analysis variables
// and their pulls:
//
//   TBVariables bVars(myBCandLabP4); 
//   if (bVars.Status() == TBVariables::GOOD){
//     ntpule->Column("mHat", bVars.MHat());
//     ntpule->Column("m_ES", bVars.M_ES());
//     ntpule->Column("deltaE", bVars.DeltaE());
//     ntpule->Column("mHatPull", (bVars.MHat() - 5.279) / bVars.mHatErr());
//     ntpule->Column("deltaEPull", bVars.DeltaE() / bVars.deltaEErr());
//  }
//
// See additional member functions for other things you can do. 
// Additional subtlties are described below:
//
//--------------------------------------------------------------------------
// Fine-Tuning the Fit:
//
// Since CLEO results indicate that the PDG value of the Upsilon(4S)
// width (10 +/- 4 MeV) is too small, you can change its value using 
// the function
//     void SetUpsilonWidth(Double_t upsWidth);
// The pdt.table value is obtained using 
//     void SetDefaultUpsilonWidth();
// 
//--------------------------------------------------------------------------
// Compiler Default Member Functions:
//      Copy constructor and assignment operator
//
//--------------------------------------------------------------------------
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
//--------------------------------------------------------------------------
// Author List:
//	Abi Soffer              (Original author)
//      Bill Ford        (Added m_ES)
//
//--------------------------------------------------------------------------
// Copyright Information:
//	Copyright (C) 1999	Colorado State University
//
// ROOT Version by Marcel Kunze, RUB
//==========================================================================

#ifndef TBVARIABLES_H
#define TBVARIABLES_H

#include "TObject.h"

//-------------
// C Headers --
//-------------

//---------------
// C++ Headers --
//---------------

//-----------------
// BaBar Headers --
//-----------------

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "RhoMath/TLorentzVectorErr.h"

//--------------------------------------------
// Collaborating Class Forward Declarations --
//--------------------------------------------

//		---------------------
// 		-- Class Interface --
//		---------------------

class TBVariables : public TObject {
public:
  // enums:
  enum Status {GOOD = 0, 
	       NO_BCAND = 1, 
	       NO_BEAMS_INFO = 2, 
	       BAD_BCAND = 3, 
	       SQRT_NEGATIVE = 4
  };

  // Constructors
  // Default constructor: All data initialized to 0:
  TBVariables();

  // Specify the p4() and p4Err() of a B candidate (in the lab frame),
  // conduct the fit and calculate analysis variables:
  TBVariables(const TLorentzVectorErr & p4Lab,
		Bool_t quiet=kFALSE);

  // Destructor
  virtual ~TBVariables();  

  // Accessors (const)
  // The analysis variables:
  Double_t MHat() const;
  Double_t M_ES() const;
  Double_t DeltaE() const;

  // errors of mHat, m_ES, and deltaE:
  Double_t MHatErr() const;
  Double_t M_ESErr() const;
  Double_t DeltaEErr() const;

  // Additional info:
  const TLorentzVectorErr & P4() const;    // original p4 in CM frame
  const TLorentzVectorErr & P4Hat() const; // p4 after the fit

  Double_t EHat() const;                       // energy after the fit
  Double_t PHat() const;                              // momentum after the fit
  Double_t EnergySubstitutedMass() const;             // energy-substituted mass

  // eHat, pHat and energySubstitutedMass with errors:
  Double_t EHatErr() const;
  Double_t PHatErr() const;
  Double_t EnergySubstitutedMassErr() const;

  // The beam parameters used in the fit:
  Double_t EBeam() const;
  Double_t EBeamErr() const;

  // Particle parameters used in the fit:
  Double_t UpsilonWidth() const;

  // Status of the last fit (done by a call to setP4Lab(..),
  // setP4CM(..), setPeLab(..) or setPeCM(..)):
  Status FitStatus() const {return fStatus;}

  // Modifiers:
  // Specify the 4-momentum (in the lab frame), conuct the
  // fit and calculate analysis variables. Returns status():
  Status SetP4Lab(const TLorentzVectorErr & theP4Lab);

  // Identical to setP4Lab, but the 4-moomentum is given in the
  // center-of-mass frame:
  Status SetP4CM(const TLorentzVectorErr & theP4CM);

  // Allow the user to change the Upsilon(4S) width, which seems to be
  // underestimated in the PDG. In order to ensure consistency, if
  // setP4Lab(..), setP4CM(..), setPeLab(..) or setPeCM(..) was
  // already called before, and the new Upsilon width is different
  // from the previous one, then the fit is redone and accessors will
  // return different values
  void SetUpsilonWidth(Double_t upsWidth);

  // Set the Upsilon(4S) from the pdt.table. Calls setUpsilonWidth(..), so
  // read the comments for setUpsilonWidth(..):
  void SetDefaultUpsilonWidth();


protected:
  // Helpers:
  Status ReFit();
  Double_t CheckSqrt(Double_t square, const char * file, int line) const;

  // A utility function to give the error on the absolute value
  // of the 3 momentum, given a 4-vector and its error:
  Double_t MomErr(const TLorentzVectorErr & p4) const;


private:
  // Data members
  TLorentzVectorErr fP4Lab;   
  TLorentzVectorErr fP4;  
  TLorentzVectorErr fP4Hat;

  Double_t fMHat;
  Double_t fM_ES;
  Double_t fDeltaE;

  Double_t fMHatErr;
  Double_t fM_ESErr;
  Double_t fDeltaEErr;

  Status fStatus;

  Double_t fEBeam;
  Double_t fEBeamErr;

  Double_t fUpsilonWidth;
  Double_t fBMass;

  Bool_t fQuietMode;

  //----------------
  // Static Members:
  //----------------
public:
  // Static holder of the B mass:
  static Double_t BMass();

public:
    ClassDef(TBVariables,1) // Calculate the B analysis variables
};

#endif
