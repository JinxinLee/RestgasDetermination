// -------------------------------------------------------------------------
// -----                      PndLinTrack header file                  -----
// -----                       Created by M. Michel                    -----
// -------------------------------------------------------------------------


/** PndLinTrack.h
 *@author M.Michel <michel@kph.uni-mainz.de>
 **
 ** A container for straight line tracks
 **/

#ifndef PNDLINTRACK_H
#define PNDLINTRACK_H


#include "TVector3.h"
#include "TString.h"
#include "TMatrixTSym.h"

#include <stdio.h>
#include <iostream>

class PndLinTrack : public TObject
{

 public:

  /** Default constructor (not for use) **/
  PndLinTrack();


  /** Standard constructor with all paramneters 
   *@param detName Detector Name
   *@param pX      Parameter of Fit [cm]
   *@param chi     ChiSquare of Fit
   *@param first   Id of first Hit
   *@param last    Id of last Hit
   *@param cand    Id of TrackCandidate
   **/

  PndLinTrack(TString detName, Double_t z0, Double_t p0, Double_t p1, Double_t p2, Double_t p3, 
	      Double_t p0err, Double_t p1err, Double_t p2err, Double_t p3err, 
	      Double_t chi, Int_t first, Int_t last, Int_t cand);

  /** Destructor **/
  virtual ~PndLinTrack();


  /** Accessors **/
  
  void SetDetName(TString name)  { fDetName   = name;}
  
  TString 	GetDetName()		const { return fDetName;}
  void		GetPar(Double_t* par)	const { par[0]=fP0;par[1]=fP1;par[2]=fP2;par[3]=fP3;}
  /* TVector3	GetStartVec()		const { return TVector3(fP0,fP2,0.);} */
  TVector3	GetStartVec()		const { return TVector3(fP0,fP2,fz0);}
  TVector3	GetDirectionVec()	const { return TVector3(fP1,fP3,1.);}
  TVector3      GetParErr(Double_t* errpar) 
    const { errpar[0]=fP0err;errpar[1]=fP1err;errpar[2]=fP2err;errpar[3]=fP3err;}
  Double_t 	GetChiSquare()		const { return fChiSq;}
  Int_t		GetFirstHit()		const { return fFirst;}
  Int_t		GetLastHit()		const { return fLast;}
  Int_t		GetTCandID()		const { return fCandId;}

 private:
  TString fDetName;  // Detector name
  Double_t fz0;// z-position of first LMD plane
  Double_t fP0, fP1, fP2, fP3; // fit-parameter
  //start (P0, P2, 0), direction (P1, P3, 1) straight line

  Double_t fP0err, fP1err, fP2err, fP3err; // errors of parameters
  Double_t fChiSq; // Chi-Square of Fit
  Int_t fFirst, fLast;
  Int_t fCandId; //TrackCand id

  ClassDef(PndLinTrack,10);

};

#endif
