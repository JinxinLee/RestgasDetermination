// -------------------------------------------------------------------------
// -----                      PndLinTrack header file                  -----
// -----                       Created by M. Michel                    -----
// -------------------------------------------------------------------------


/** PndLinTrack.h
 *@author M.Michel <michel@kph.uni-mainz.de>
 * modified by Simone Bianco
 **
 ** A container for straight line tracks
 **/

#ifndef PNDLINTRACK_H
#define PNDLINTRACK_H


#include "TVector3.h"
#include "TString.h"

#include <stdio.h>
#include <iostream>
#include <vector>

class PndLinTrack : public TObject
{

 public:

  /** Default constructor (not for use) **/
  PndLinTrack();


  /** Standard constructor with all paramneters 
   *@param detName Detector Name
   *@param pX      Parameter of Fit [cm]
   *@param chi     ChiSquare of Fit
   *@param fEloss   Total energy loss along one track
   *@param fNsens   number of sensors with at least one hit
   *@param fSens   names of sensors included in the fit
   *@param cand    Id of TrackCandidate
   **/
  PndLinTrack(TString detName, Double_t p0, Double_t p1, Double_t p2, Double_t p3,
  	    Double_t chi, Double_t eloss, Int_t numSens, std::vector<TString> sens, Int_t cand);

  /** Destructor **/
  virtual ~PndLinTrack();


  /** Accessors **/
  
  void SetDetName(TString name)  { fDetName   = name;}
  
  TString 	GetDetName()		const { return fDetName;}
  void		GetPar(Double_t* par)	const { par[0]=fP0;par[1]=fP1;par[2]=fP2;par[3]=fP3;}
  TVector3	GetStartVec()		const { return TVector3(fP0,fP2,0.);}
  TVector3	GetDirectionVec()	const { return TVector3(fP1,fP3,1.);}
  Double_t 	GetChiSquare()		const { return fChiSq;}
  Double_t      GetEloss()              const { return fEloss;}
  Int_t         GetNsens()              const { return fNsens;}
  std::vector<TString> 	GetSens()		const { return fSens;}
  Int_t		GetTCandID()		const { return fCandId;}

 private:
  TString fDetName;  // Detector name
  Double_t fP0, fP1, fP2, fP3; // fit-parameter
  //start (P0, P2, 0), direction (P1, P3, 1) straight line
  Double_t fChiSq; // Chi-Square of Fit
  Double_t fEloss;
  Int_t fNsens; // number of sensors used for the fit
  std::vector<TString> fSens; // names of sensors included in the fit
  Int_t fCandId; //TrackCand id
  
  ClassDef(PndLinTrack,10);

};

#endif
