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
  double fBarGap;			    //!< Half gap between bars [cm].
  double fBarNum;                           //!< Number of bars per barbox.
  double fBoxGap;			    //!< Gap between bar and bar box [cm].
  double fBoxThick;			    //!< Bar box thickness [cm].
  
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
  
  /*! The  half gap between bars.
    \return The BarGap [cm]. */
  Double_t  barGap() {return fBarGap;}
  
  /*! The gap between bars and the bar box.
    \return The Gap [cm]. */
  Double_t  boxGap() {return fBoxGap;}
  
  /*! The thickness of the bar box.
    \return The BoxThickness [cm]. */
  Double_t  boxThick() {return fBoxThick;}

  /*! Number of bars per  bar box.
    \return The number */
  Double_t  barNum() {return fBarNum;}




  ClassDef(PndGeoDrc,1) // Class for Drc


};


#endif  /* !PNDGEODRC_H */



