// -------------------------------------------------------------------------
// -----                     PndGeoDrc header file                     -----
// -----               Created 11/10/06  by A. Cecchi                  -----
// -------------------------------------------------------------------------

#ifndef PNDGEODRC_H
#define PNDGEODRC_H

#include "FairGeoSet.h"

class  PndGeoDrc : public FairGeoSet {

  private:
  double fRadius;                           //!< Barrel radius [cm] (middle of radiator)
  double fBarHalfThick;                     //!< Radiator bars half thickness [cm].
  double fBarBoxZDown;                      //!< Bar box down stream coordinate [cm].
  double fBarBoxZUp;                        //!< Bar box up   stream coordinate [cm].
  
  protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
  public:
  PndGeoDrc();
  ~PndGeoDrc() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  

  /*! The DRC barrel radius.
    \return The radius [cm]. */
  Double_t  radius() {return fRadius;}

  /*! The DRC barrel bar half thickness.
    \return Half thickness [cm]. */
  Double_t  barHalfThick() {return fBarHalfThick;}

  /*! The DRC barrel bar box downstream coordinate.
    \return Coordinate [cm]. */
  Double_t  barBoxZDown() {return fBarBoxZDown;}

  /*! The DRC barrel bar box upstream coordinate.
    \return Coordinate [cm]. */
  Double_t  barBoxZUp() {return fBarBoxZUp;}

  ClassDef(PndGeoDrc,0) // Class for Drc


};


#endif  /* !PNDGEODRC_H */



