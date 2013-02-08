/////////////////////////////////////////////////////////////
// PndGeoDrc
//
// Class for geometry of DRC (PandaRoot general case)
//
// Created 13/11/07 by A. Cecchi, Ferrara University 
// Modified by M.Patsyuk 2010-2013 m.patsyuk@gsi.de
//
/////////////////////////////////////////////////////////////

#include "PndGeoDrc.h"
#include "FairGeoNode.h"

ClassImp(PndGeoDrc)

// -----   Default constructor   -------------------------------------------
PndGeoDrc::PndGeoDrc() {
  // Constructor
  fName="drc";

  maxSectors=0;
  maxModules=20;
  
  // mean refraction index of fused silica (for reconstruction)
  fNquartz      = 1.474;//(Lambda = 366 nm for EV filled with FS)//1.47383;//1.47248;//1.47125;//1.47012;//1.46907;
  fNEV		= 1.474;//1.47383;//1.47248;//1.47125;//1.47012;// 1.46907;//1.467;
  // oil ondex of refraction
  fRoughness	= 0.001; // [nm] = 10A, roughness of the quartz bar surface
  fGlueLayer	= 0.0025; // [cm]
  fGreaseLayer  = 0.1; // [cm] 

  fRadius       = 47.6;//50.;	      // [cm] Barrel radius
  fBarHalfThick = 1.7/2.0;    // [cm] bar half thickness
  fBarBoxZDown  = 130.0;      // [cm] Downstream coordinate of radiator barrel
  fBarBoxZUp    =-120.0;      // [cm] Upstream coordinate of radiator barrel  
  fBarNum       = 5;          // Number of bars within barbox.
  fBoxGap	= 0.5;   // [cm] Gap between bar and bar box (only at the top and bottom of the bars)
  fBarGap	= 0.01;	      // [cm] Half gap between bars, =1 to have a realistic barbox thickness
  fBoxThick	= 0.05;	      // [cm] Thickness of bar box
      
  fBarBoxNum    = 16;
  fBarBoxGap    = 1.5; 	      // [cm] gap between neighboring bar boxes
  fPipehAngle   = 3.6;	      // [degrees] half of the angle taken by the target pipe
  
  // parameters of the EV:
  fEVlength     = 30.;        // [cm] Length of expansion volume
  fEVdrop	= 1.;	      // [cm] drop of the EV - inner radius (used only with prism)
  fEVoffset	= 0.;	      // [cm] offset of the EV - outer radius
  fEVangle	= 45.;	      // [degrees] opening angle of the EV
  fEVbackAngle  = 90.;	      // [degrees] angle between the PD surface and the Z axis, default = 90.
  
  // prism parameters:
  fPhLength	=  4.5;       // [cm] half length of the prism
  fPAngle	=  30.;       // [degrees] opening angle of the prism
  fPDrop	=  0.5;       // [cm] drop of the prism - inner side
  fPOffset	=  1.;	      // [cm] offset of the prism - outer side  
  
  fPi            =  TMath::Pi();
  fLside	= (180. - 2.*fPipehAngle - fBarBoxGap/fRadius*(fBarBoxNum/2. - 1.)/fPi*180.)/(fBarBoxNum/2.) * fRadius/ 180.*fPi;
  fBarWidth	=  fLside/fBarNum;
    
 }
// -------------------------------------------------------------------------

const char* PndGeoDrc::getModuleName(Int_t m) {
  // Returns the module name of drc number m
  // sprintf(modName,"drc0%i",m+1);
  if ( m < 9 ) sprintf(modName,"drc0%i",m+1);
  else sprintf(modName,"drc%i",m+1);
  return modName;
}

const char* PndGeoDrc::getEleName(Int_t m) {
  // // Returns the element name of drc number m
  //sprintf(eleName,"drc0%i",m+1);
  if ( m < 9 ) sprintf(eleName,"drc0%i",m+1);
  else sprintf(eleName,"drc%i",m+1);

  return eleName;
}
