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

typedef std::map<string,double> Param;
//typedef std::map<string,double>::iterator Iter;


/*
any pointer data member here has to be deleted by destroy func
*/
class PndPidCand  : public TCandidate{

public:
  PndPidCand();
  ~PndPidCand();
  void Set(const string key, const double value);
  double Get(const string key);
  vector<string> GetVarName();
  
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
 private:
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
  Param fParam;         
  std::vector<string> fVarName;     
  void destroy();
  ClassDef(PndPidCand,1);
};
#endif
