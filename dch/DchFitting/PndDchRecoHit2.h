// -------------------------------------------------------------------------
// -----                  PndDchRecoHit2 header file                   -----
// -----                Created 29.09.2008 A. Wronska                  -----
// -------------------------------------------------------------------------

/** \class PndDchRecoHit2
 *  \author A.Wronska 
 *  \date 29.09.2008
 *  \brief PndDchRecoHit2 is a representation of reconstructed dch hits used by genfit;
 **/

#ifndef PNDDCHRECOHIT2_H
#define PNDDCHRECOHIT2_H 1

// Pnd includes
#include "PndSttRecoHit.h"

// c++ headers
#include <ostream> 

class PndDchCylinderHit;

class PndDchRecoHit2 : public PndSttRecoHit {

public:

  /** All-by-hand Constructor  **/
  /* PndDchRecoHit2(double r, double wireposx, double angle, double z, */
/* 		 double sigr); */

  /** Default Constructor for dch **/
  PndDchRecoHit2(const PndDchCylinderHit* cylHit);
  
  /** Public method Print() **/
  virtual void  Print();

private:

  double _sangle;  ///<  sine of the angle of the wire
  double _cangle;  ///<  cosine of the angle of the wire
  double _wirepos; ///<  u coordinate of wire in detector plane

  ClassDef(PndDchRecoHit2,1)

};

#endif
