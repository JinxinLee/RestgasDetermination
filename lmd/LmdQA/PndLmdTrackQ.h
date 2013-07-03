// -------------------------------------------------------------------------
// -----               PndLmdTrackQ header file                  -----
// -----       Created 18/06/2013 by A.Karavdina          -----
// -------------------------------------------------------------------------


/** PndLmdTrackQ.h
 *@author A.Karavdina <karavdin@kph.uni-mainz.de>
 **
 ** Qualified reconstructed track in Luminosity detector
 **/


#ifndef PNDLMDTRACKQ_H
#define PNDLMDTRACKQ_H


#include "TVector3.h"
#include "TString.h"
#include "TObject.h"
#include "TMatrixTSym.h"
#include "TMatrixDSym.h"

#include <stdio.h>
#include <iostream>

//#include "PndTrack.h"

class PndLmdTrackQ: public TObject
{
public:
  /** Default constructor **/
  PndLmdTrackQ();

  PndLmdTrackQ(double Plab);

  /** Destructor **/
  ~PndLmdTrackQ();


  /** Accessors **/

  void SetTrkRecStatus(int st){fTrkRecStatus = st;}
  int GetTrkRecStatus() const{return fTrkRecStatus;}
  void SetPDGcode(int pdg){fPDGcode = pdg;}
  int GetPDGcode() const{return fPDGcode;}

  void SetLMDpoint(double x, double y, double z){fXrecLMD=x;fYrecLMD=y;fZrecLMD=z;}
  void GetLMDpoint(TVector3 lmdp){lmdp = TVector3(fXrecLMD,fYrecLMD,fZrecLMD);}
  void SetLMDdir(double theta, double phi){fThetarecLMD = theta; fPhirecLMD = phi;}
  double GetLMDtheta() const{return fThetarecLMD;}
  double GetLMDphi() const{return fPhirecLMD;}
  void SetLMDchi2(double chi2){fchi2LMD = chi2;}
  double GetLMDchi2() const{return fchi2LMD;}
  void SetIPpoint(double x, double y, double z){fXrec=x;fYrec=y;fZrec=z;}
  void GetIPpoint(TVector3 lmdp){lmdp = TVector3(fXrec,fYrec,fZrec);}
  void SetIPmom(double theta, double phi,double mom){fThetarec = theta; fPhirec = phi; fMomrec = mom;}
  double GetIPtheta() const{return fThetarec;}
  double GetIPphi() const{return fPhirec;}
  double GetIPmom() const{return fMomrec;}

  void SetIPerrpoint(double errx, double erry, double errz){ferrXrec=errx;ferrYrec=erry;ferrZrec=errz;}
  void GetIPerrpoint(TVector3 lmdp){lmdp = TVector3(ferrXrec,ferrYrec,ferrZrec);}

  void SetIPerrmom(double errtheta, double errphi, double errmom){ferrThetarec = errtheta; ferrPhirec = errphi; ferrMomrec = errmom;}
  void GetIPerrmom(TVector3 lmerrdp){lmerrdp = TVector3(ferrXrec,ferrYrec,ferrZrec);}

  void SetMCpoint(double x, double y, double z){fXmc=x;fYmc=y;fZmc=z;}
  void GetMCpoint(TVector3 lmdp){lmdp = TVector3(fXmc,fYmc,fZmc);}
  void SetMCmom(double theta, double phi,double mom){fThetamc = theta; fPhimc = phi; fMommc = mom;}
  double GetMCtheta() const{return fThetamc;}
  double GetMCphi() const{return fPhimc;}
  double GetMCmom() const{return fMommc;}
  void GetMCpointLMD(TVector3 lmdp){lmdp = TVector3(fXmcLMD,fYmcLMD,fZmcLMD);}
  void SetMCpointLMD(double x, double y, double z){fXmcLMD=x;fYmcLMD=y;fZmcLMD=z;}
  void SetMCmomLMD(double theta, double phi,double mom){fThetamcLMD = theta; fPhimcLMD = phi; fMommcLMD = mom;}
  double GetMCthetaLMD() const{return fThetamcLMD;}
  double GetMCphiLMD() const{return fPhimcLMD;}
  double GetMCmomLMD() const{return fMommcLMD;}
  void SetSecondary(int sec){fSecondary = sec;}
  int GetSecondary() const{return fSecondary;}
  void SetNumMChits(int num){fnumMChits;}
  int GetNumMChits() const{return fnumMChits;}
  void SetNumDoubleMChits(int num){fnumMCdoublehits;}
  int GetNumDoubleMChits() const{return fnumMCdoublehits;}
 private:
  Double_t fXrecLMD,fYrecLMD,fZrecLMD,fThetarecLMD,fPhirecLMD;
  Double_t fXrec,fYrec,fZrec,fThetarec,fPhirec, fMomrec;
  Double_t ferrXrec,ferrYrec,ferrZrec,ferrThetarec,ferrPhirec, ferrMomrec;
  Double_t fXmc,fYmc,fZmc,fThetamc,fPhimc, fMommc;
  Double_t fXmcLMD,fYmcLMD,fZmcLMD,fThetamcLMD,fPhimcLMD, fMommcLMD;
  Double_t fchi2LMD;
  int fnumMChits;//number of MC hits for MC trk
  int fnumMCdoublehits;//number of doubled [diff side of one plane] MC hits for MC trk 
  int fTrkRecStatus;// 0 = good,
                              // >0 = ghost: 1 = hit mixture, 2 = doubled trk
                              // <0 = missed: -1=trk-search losses,-2=little amount of hits,-3=no MC hits, -10=trk wasn't back-propag
  int fSecondary;//0=primary, 1=secondary
  int fPDGcode;// PDG code particle made the main contribution (65 % of hits) to the rec.track, -9999=n/a


  ClassDef(PndLmdTrackQ,10);
};

#endif
