//----------------------------------------------------//
//            PndLepLepGenerator header file            //
//----------------------------------------------------//

/**
 * @author Manuel Zambrana <zambrana@kph.uni-mainz.de> and Dmitry Khaneftd <khaneftd@kph.uni-mainz.de>
 *
 * Mainz, May 2011
**/

#ifndef PND_LEPLEPGENERATOR_H
#define PND_LEPLEPGENERATOR_H

// c/c++
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <time.h>

//ranlux
#include "ranlxs.h"
#include "ranlxd.h"

void ranlxs(float r[], int n);
void ranlxd(double r[], int n);

void rlxs_init(int level, int seed);
void rlxd_init(int level, int seed);

//mz (general)
#include "mzparameters.h"
#include "mzfunctions.h"

//mz for pp_to_pipi_vandewi
#include "mzfunctions_pp_to_leplep_vandewi.h"

#include "FairGenerator.h"

using namespace std;

class FairPrimaryGenerator;

class PndLepLepGenerator : public FairGenerator
{
public:
  /** Default constructor. **/
  PndLepLepGenerator();

  /** Destructor **/
  virtual ~PndLepLepGenerator();

  /** Set parameters of event generator**/
  void SetBeamMom(Double_t in_P)                 {P = in_P;};				// set pbar momentum
  void SetSeed(Int_t in_seed)                    {seed = in_seed;};			// set seed
  void SetParticleID(Int_t in_particleID)        {particleID = in_particleID;};		// particle ID (0 - electron, 1 - muon, 2 - tau)
  void SetGeGmRatio(Int_t in_gegm)               {gegm = in_gegm;};			// GE/GM ratio (|GE|/|GM| = 0,1,3)
  void SetCosThetaMin(Double_t in_cos_theta_min) {cos_theta_min = in_cos_theta_min;};	// set cos(theta*) min in the pbarp CM frame
  void SetCosThetaMax(Double_t in_cos_theta_max) {cos_theta_max = in_cos_theta_max;};	// set cos(theta*) max in the pbarp CM frame

  /** Pass event to the PANDAroot**/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen); 

  /** Initializer **/
  Bool_t Init();

  ClassDef(PndLepLepGenerator,1);

  Double_t P;               // pbar momentum
  Int_t seed;               // seed
  Int_t particleID;         // particle ID (0 - electron, 1 - muon, 2 - tau)
  Int_t PDGParticleID;      // PDG particle ID
  Int_t gegm;               // GE/GM ratio (|GE|/|GM| = 0,1,3)
  Double_t cos_theta_min;   // cos(theta*) min in the pbarp CM frame
  Double_t cos_theta_max;   // cos(theta*) max in the pbarp CM frame
  Double_t lepplus_p[4];    // lab frame: 4-momentum of a lepton
  Double_t lepminus_p[4];   // lab frame: 4-momentum of a lepton
};

#endif
