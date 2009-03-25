/*
 *  S.Vanniarajan  V.Suyam.Jothi@kvi.nl
 *
 *  This is the Mutivariate Event classification tool
 *  designed for PANDAROOT Analysis package for 
 *  PANDA Experiments.
 *       TMVA(Toolkit for MultiVariate Analysis) is a 
 *  two class classifier(signal and background). 
 *  available with ROOT package. mainly used for 
 *  Event Classification in High Energy Physics Experiments.
 *         
 *       This tool here is Designed from TMVA, for Multiclass 
 * Classification purpose. 
 *
 *                  S.Vanniarajan  01-08-08
 * Modified:
 * M. Babai
 */

#pragma once
#ifndef PND_PID_CAND_H
#define PND_PID_CAND_H

//C++ headers
#include <map>
#include <string>
#include <vector>

//Root & PandaRoot headers
#include "TCandidate.h"

using namespace std;

typedef std::map<std::string, float> Param;

/*
 * Any pointer data member here has to be deleted by destroy func
*/
class PndPidCand  : public TCandidate{

public:
  PndPidCand();
  ~PndPidCand();

  void Set(const std::string key, const float value);

  float Get(const std::string key);
  void GetVarName(std::vector<std::string>& varList);
  
 private:

  void destroy();
  Param fParam;

  ClassDef(PndPidCand,1);
};
#endif
/*
  inline Double_t GetTpcDeDx()		const { return fTpcDeDx;}
  inline Double_t GetMvdDeDx()		const { return fMvdDeDx;}
  inline Double_t GetP()			const { return fP;}
  inline Double_t GetEmcE()		const { return fEmcE;}
  inline Double_t GetTof()		const { return fTof;}
  inline Double_t GetSpeed()		const { return fSpeed;}
  inline Double_t GetLen()		const { return fLen;}
  inline Double_t GetNPhotons()		const { return fNPhotons;}
  inline Double_t GetThetaC()		const { return fThetaC;}
  inline void SetTpcDeDx(Double_t f) 	{ fTpcDeDx = f;}
  inline void SetMvdDeDx(Double_t f) 	{ fMvdDeDx = f;}
  inline void SetP(Double_t f) 		{ fP = f;}
  inline void SetEmcE(Double_t f) 	{ fEmcE = f;}
  inline void SetTof(Double_t f) 		{ fTof = f;}
  inline void SetSpeed(Double_t f) 	{ fSpeed = f;}
  inline void SetLen(Double_t f) 		{ fLen = f;}
  inline void SetNPhotons(Double_t f) 		{ fNPhotons = f;}
  inline void SetThetaC(Double_t f) 		{ fThetaC = f;}
*/
/*
  Double_t fTpcDeDx;
  Double_t fMvdDeDx;
  Double_t fP;
  Double_t fEmcE;
  Double_t fTof;
  Double_t fSpeed;
  Double_t fLen;
  Double_t fNPhotons;
  Double_t fThetaC;
*/
