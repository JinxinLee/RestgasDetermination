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
  fNEV		= 1.47;//1.474;//1.47383;//1.47248;//1.47125;//1.47012;// 1.46907;//1.467;
  // oil ondex of refraction
  fRoughness	= 0.001; // [nm] = 10A, roughness of the quartz bar surface
  fGlueLayer	= 0.0005;//25; // [cm]
  fGreaseLayer  = 0.05; // [cm] grease layer between the MCP and the EV

  fRadius       = 47.6;//50.;	      // [cm] Barrel radius
  fBarHalfThick = 1.75/2.0;    // [cm] bar half thickness
  fBarWidth	= 3.2;
  fBarBoxZDown  = 130.0;      // [cm] Downstream coordinate of radiator barrel
  fBarBoxZUp    =-120.0;      // [cm] Upstream coordinate of radiator barrel  
  fBarNum       = 5;          // Number of bars within barbox.
  fBoxGap	= 0.425;   // [cm] Gap between bar and bar box (only at the top and bottom of the bars)
  fBarhGap	= 0.01;	      // [cm] Half gap between bars, =1 to have a realistic barbox thickness
  fBoxThick	= 0.2;	      // [cm] Thickness of bar box
      
  fBarBoxNum    = 16;
  fBarBoxGap    = 2.48; 	      // [cm] gap between neighboring bar boxes
  fPipehAngle   = 3.4;	      // [degrees] half of the angle taken by the target pipe
  fBarBoxAngle  = 21.65;
  
  // parameters of the EV:
  fEVlength     = 30.;        // [cm] Length of expansion volume
  fEVdrop	= 1.;//0.7;	      // [cm] drop of the EV - inner radius (used only with prism)
  fEVoffset	= 0.;	      // [cm] offset of the EV - outer radius
  fEVangle	= 45.;	      // [degrees] opening angle of the EV
  fEVbackAngle  = 90.;	      // [degrees] angle between the PD surface and the Z axis, default = 90.
  
  // prism parameters:
  fPhLength	=  4.5;       // [cm] half length of the prism
  fPAngle	=  30.;       // [degrees] opening angle of the prism
  fPDrop	=  0.5;       // [cm] drop of the prism - inner side
  fPOffset	=  1.;	      // [cm] offset of the prism - outer side 
  
  // MCP parameters:
  fMCPsize	= 5.78;	      // [cm] for geometry Mcp2 and Mcp1, = 5.78 for geometry Mcp2a
  fMCPactiveArea= 5.3;	      // [cm] 
  fMCPgap	= 0.1;        // [cm] gap between MCPs
  fNpix		= 8;	      // [cm] number of pixel in one column/row for one MCP (8 x 8)
  fPixelSize  	= 0.65;	      // [cm] 
  fSigmaCharge  = 0.03;	      // [cm] = 1 mm, sigma of the charge sharing
  fDeadTime	= 0.0  ;      // [ns] dead time of the photodetector
  
  fPi            =  TMath::Pi();
  fLside	= 2.*fRadius*TMath::Sin((fBarBoxAngle-fBarBoxGap)/180.*fPi/2.)+2.*fBarhGap;
    
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
