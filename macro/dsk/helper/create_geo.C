/**

  helper/create_geo.C

  @brief Calculates the values for media file

  Creates geometry file for disk DIRC

  @author  Peter Koch
  @date    2008-04-06
  @version 2
  @since   2007-11-26


  Version 2:\n
  Now we will put our detector in a air-mother, so all mcp/mirror/glass will be surrounded by \n
  air and the total reflections wont need further implementations.\n
  Also the photons will now reach the mcp and can be detected.\n \n
  Version 1:\n
  This version was made of the glass coated with air-volumes. surrounded by uncoated mirrors. \n
  So we cant get total reflection in the mirrors and the photons wont ever reach the mcp.

**/

{
  // root
  gROOT->Reset();
  gSystem->Load("libMathCore");

  // control switches
  Bool_t fWithPip = true;

  // constants and controls
  const Double_t mm   = 1.;
  const Double_t cm   = 10.;
  const Double_t zero = 0.;
  const Double_t one  = 1.;
  const Int_t    prc  = 6;
  Int_t    pos = 0;                // needed to create copies


  // fFile, definitions for output
  Int_t    fFileGeoPrec = 6;       // precision used in geo-file
//   TString  fFileGeoName = "/opt/fairroot/pandaroot/geometry/dsk.geo";
  const TString  fFileGeoName = "/opt/panda/pandaroot/macro/dsk/dsk.geo";

  // globals
  const TString  fGlbName                        = "dsk";
  const TString  fGlbMother                      = "cave";
  const Int_t    fGlbEdges                       = 8;                          // a octagon; max: 36
  const Int_t    fGlbDetectorsPerEdge            = 12;
  const Int_t    fGlbDetectorTypes               = 3;
  const Double_t fGlbDistance                    = 180. *cm;                   // z-coordinate of first disk-plane
        Double_t fGlbThickness;                  // calculated
  // beam pipe
  const Double_t fPipRadOuter                    = 30. *mm;                    // outer radius
  const Double_t fPipRadInner                    = 20. *mm;                    // inner radius
  // local mother
        Double_t fLMoThickness;                  // calculated
        Double_t fLMoRadInner;                   // calculated
        Double_t fLMoRadOuter;                   // calculated
  // coating
  const Double_t fCotThickness                   = 1. *cm;
  // disk
  const Double_t fDskThickness                   = 2. *cm;                     // thickness of disk
        Double_t fDskRadInner;                   // calculated
        Double_t fDskRadOuter;                   // calculated
  // window
        Double_t fWinHeightHalf;                 // calculated
        Double_t fWinWidthHalf;                  // calculated
  // array
        Double_t fAryHeight;                     // calculated
        Double_t fAryWidth;                      // calculated
        Double_t fAryWidthHalf;                  // calculated
  // detector
        Double_t fDetWidth;                      // calculated
  const Double_t fDetGapBetween                  = 0. *mm;
  const Double_t fDetGapBorder                   = 0. *mm;

  // mirror
  const Double_t fMirHeight                      = 5. *mm;
  // mcp
  const Double_t fMcpHeight                      = 1.7 *cm;


  // the dskXX numbers
  TString fNmbrPip                         = "49";         // the Beampipe
  TString fNmbrLMo                         = "01";         // local mother
  TString fNmbrDsk                         = "02";         // glass polygon
  Int_t   fNmbrWin                         = 10;           // window inside 02; so will be 10, 11, 12, ..., 18
  TString fNmbrAry                         = "19";         // mother for all detecttors of an edge
  TString fNmbrDet                         = "2";          // detector: a mirror and an mcp
  TString fNmbrMir                         = "3";          // mirror
  TString fNmbrMcp                         = "4";          // mcp


  // the materials
  TString fMatPip                        = "DISKmirror0";
  TString fMatLMo                        = "DISKair";
  TString fMatDsk                        = "DISKglass";
  TString fMatWin                        = "vacuum";
  TString fMatAry                        = "DISKair";
  TString fMatDet                        = "DISKair";
  TString fMatMir                        = "DISKmirror";
  TString fMatMcp                        = "DISKmcp";


  // calculated values


  // angles
  // segment angle [rad]; 2\pi/fEdges
  const Double_t fAlphaRad = 360./fGlbEdges*TMath::DegToRad();
  // offset from x=0-line to first segment, angle [dgree]
  const Double_t fPhi   = 180./fGlbEdges;
  const Double_t fPhiRad = fPhi * TMath::DegToRad();


  fGlbThickness = fDskThickness + fCotThickness;

  fWinHeightHalf = fGlbDistance*TMath::Tan(5.*TMath::DegToRad());
  fWinWidthHalf  = fGlbDistance*TMath::Tan(10.*TMath::DegToRad());

  fDskRadOuter = fGlbDistance*TMath::Tan(22.*TMath::DegToRad());
  fDskRadInner = fWinHeightHalf;

  fAryHeight    = fMirHeight + fMcpHeight;
  fAryWidthHalf = fDskRadOuter*TMath::Tan(fAlphaRad/2.);
  fAryWidth     = 2.*fAryWidthHalf;

  fLMoThickness = fGlbThickness + fCotThickness;
  fLMoRadInner  = fPipRadOuter + 1.*cm;
  fLMoRadOuter  = fDskRadOuter + fAryHeight + fCotThickness;

  fDetWidth = (fAryWidth-2.*fDetGapBorder-(fGlbDetectorsPerEdge-1)*fDetGapBetween)/fGlbDetectorsPerEdge;


  // z-koordinate shifts
  const Double_t fLMoZ = -fCotThickness;



// -----------------------------------------------------------------------------
//    output to .geo - file
// -----------------------------------------------------------------------------


  FILE* fFile;
//   fFile = fopen(fFileGeoName.Data(), "w");
  fFile = fopen("/opt/panda/pandaroot/macro/dsk/dsk.geo", "w");

  if (fWithPip) {
  cout<<"-I- calculating beampipe (debug)"<<endl;

  fprintf(fFile, "//-----------------------------------------------------\n");
  fprintf(fFile, "//  The Beampipe (for debug)\n");
  fprintf(fFile, "//-----------------------------------------------------\n");

  // according to Technical Progress Report 2005 Page 79, the beampipe has a
  // diameter of 60mm
  fprintf(fFile, "%s%s\n", fGlbName.Data(), fNmbrPip.Data());
  fprintf(fFile, "%s\n",   fGlbMother.Data());
  fprintf(fFile, "TUBE\n");
  fprintf(fFile, "%s\n",   fMatPip.Data());
  // x,y,z coordinate of the center of the circle at the beginning of the tube
  fprintf(fFile, "%.*e %.*e %.*e\n", prc, zero, prc, zero, prc, zero);
  // inner radius RMIN of the tube, outer radius RMAX of the tube
  fprintf(fFile, "%.*e %.*e\n",      prc, fPipRadInner, prc, fPipRadOuter);
  // x,y,z coordinate of the center of the circle at the end of the tube
  fprintf(fFile, "%.*e %.*e %.*e\n", prc, zero, prc, zero, prc, fDskThickness);
  // coordinates of local (0,0,0)
  fprintf(fFile, "%.*e %.*e %.*e\n",
    prc, zero,
    prc, zero,
    prc, fGlbDistance);
  // rotation of local coordinate system
  fprintf(fFile, "%.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e\n",
    prc,  one, prc, zero, prc, zero,
    prc, zero, prc,  one, prc, zero,
    prc, zero, prc, zero, prc,  one);
  fprintf(fFile, "//*****************************************************\n");
  }





  cout<<"-I- calculating local mother"<<endl;

  fprintf(fFile, "//-----------------------------------------------------\n");
  fprintf(fFile, "//  The Local Mother (air, surrounding whole detector)\n");
  fprintf(fFile, "//-----------------------------------------------------\n");

  // this octagon will be the mother for all other detectors. It will be as close to beampipe as necessary
  // and will cover the disk and the surrounding detectorarray
  // local coordinate system: x,y: center; z: where disk will be

  fprintf(fFile, "%s%s\n", fGlbName.Data(), fNmbrLMo.Data());
  fprintf(fFile, "%s\n",   fGlbMother.Data());
  fprintf(fFile, "PGON\n");
  fprintf(fFile, "%s\n",   fMatLMo.Data());
  // NZ number of planes perpendicular to the z-axis where the section is given
  fprintf(fFile, "2\n");
  // azimuthal angle PHI1 at which the volume begins, opening angle DPHI of the volume,
  // number NPDV of sides of the cross section between the phi limits
  fprintf(fFile, "%.*e 360. %i\n", prc, fPhi, fGlbEdges);
  // (each plane:) z coordinate Z of the section, inner radius RMIN at position z, outer radius RMAX at position z
  fprintf(fFile, "%.*e %.*e %.*e\n",
    prc, fLMoZ,
    prc, fLMoRadInner,
    prc, fLMoRadOuter);
  fprintf(fFile, "%.*e %.*e %.*e\n",
    prc, fLMoZ + fLMoThickness,
    prc, fLMoRadInner,
    prc, fLMoRadOuter);
  // coordinates of local (0,0,0); needs to be shifted cause of rotation
  fprintf(fFile, "%.*e %.*e %.*e\n",
    prc, zero,
    prc, zero,
    prc, fGlbDistance);
  // rotation of local coordinate system so it has same orientation as global system
  fprintf(fFile, "%.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e\n",
    prc,  one, prc, zero, prc, zero,
    prc, zero, prc,  one, prc, zero,
    prc, zero, prc, zero, prc,  one);
  fprintf(fFile, "//*****************************************************\n");




  cout<<"-I- calculating radiator disk as PGON"<<endl;

  fprintf(fFile, "//-----------------------------------------------------\n");
  fprintf(fFile, "//  The Disk (fused silica)\n");
  fprintf(fFile, "//-----------------------------------------------------\n");

  // this polygon is our glass disk
  // it needs to be mother for some air, cause teh window is too small and not rectangular but also a pgon

  fprintf(fFile, "%s%s\n", fGlbName.Data(), fNmbrDsk.Data());
  fprintf(fFile, "%s%s\n", fGlbName.Data(), fNmbrLMo.Data());
  fprintf(fFile, "PGON\n");
  fprintf(fFile, "%s\n",   fMatDsk.Data());
  // NZ number of planes perpendicular to the z-axis where the section is given
  fprintf(fFile, "2\n");
  // azimuthal angle PHI1 at which the volume begins, opening angle DPHI of the volume,
  // number NPDV of sides of the cross section between the phi limits
  fprintf(fFile, "%.*e 360. %i\n", prc, fPhi, fGlbEdges);
  // (each plane:) z coordinate Z of the section, inner radius RMIN at position z, outer radius RMAX at position z
  fprintf(fFile, "%.*e %.*e %.*e\n",
    prc, zero,
    prc, fDskRadInner,
    prc, fDskRadOuter);
  fprintf(fFile, "%.*e %.*e %.*e\n",
    prc, fDskThickness,
    prc, fDskRadInner,
    prc, fDskRadOuter);
  // coordinates of local (0,0,0)
  fprintf(fFile, "%.*e %.*e %.*e\n",
    prc, zero,
    prc, zero,
    prc, zero);
  // rotation of local coordinate system
  fprintf(fFile, "%.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e\n",
    prc,  one, prc, zero, prc, zero,
    prc, zero, prc,  one, prc, zero,
    prc, zero, prc, zero, prc,  one);
  fprintf(fFile, "//*****************************************************\n");





  cout<<"-I- calculating fillers for radiator disks window"<<endl;

  fprintf(fFile, "//-----------------------------------------------------\n");
  fprintf(fFile, "//  The Fillers (vacuum)\n");
  fprintf(fFile, "//-----------------------------------------------------\n");

  // the polygon for the disk has the wrong window shape. Need to have
  // some volume made of air, so teh window will get the right size and shape

  // conditions for the disk layout:
  // - top has a edge, no corner
  // - symmetric in rotation of 90deg

  fprintf(fFile, "%s%i\n", fGlbName.Data(), fNmbrWin++);
  fprintf(fFile, "%s%s\n", fGlbName.Data(), fNmbrLMo.Data());
  fprintf(fFile, "PGON\n");
  fprintf(fFile, "%s\n",   fMatWin.Data());
  fprintf(fFile, "2\n");
  fprintf(fFile, "%.*e 360. %i\n", prc, fPhi, fGlbEdges);
  fprintf(fFile, "%.*e %.*e %.*e\n",
    prc, zero,
    prc, fLMoRadInner,
    prc, fDskRadInner);
  fprintf(fFile, "%.*e %.*e %.*e\n",
    prc, fDskThickness,
    prc, fLMoRadInner,
    prc, fDskRadInner);
  // coordinates of local (0,0,0)
  fprintf(fFile, "%.*e %.*e %.*e\n",
    prc, zero,
    prc, zero,
    prc, zero);
  // rotation of local coordinate system
  fprintf(fFile, "%.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e\n",
    prc,  one, prc, zero, prc, zero,
    prc, zero, prc,  one, prc, zero,
    prc, zero, prc, zero, prc,  one);
  fprintf(fFile, "//*****************************************************\n");

  // vector with coordinates of first corner
  TVector3 corner(-TMath::Tan(fPhiRad)*fWinHeightHalf,fWinHeightHalf,0.);
  Double_t x1,x2,y1,y2;

  /// in this for-loop is a bug.
  /// seems like the statement is too long for cint?
  /// if teh script is called wit .x there wont be any error message besides
  /// /macro/dsk/helper/create_geo.C:379:

  // now calc all but the last fillers
  for (Int_t i=0; i<(fGlbEdges-4)/4; i++) {
    x1 = corner.X(); y1 = corner.Y();
    corner.RotateZ(2.*fPhiRad);
    x2 = corner.X(); y2 = corner.Y();

    fprintf(fFile, "%s%i#1\n", fGlbName.Data(), fNmbrWin);
    fprintf(fFile, "%s%s\n",   fGlbName.Data(), fNmbrDsk.Data());
    fprintf(fFile, "TRAP\n");
    fprintf(fFile, "%s\n",     fMatWin.Data());
    fprintf(fFile, "%.*e %.*e %.*e\n", prc, x2,             prc, y2, prc, zero);
    fprintf(fFile, "%.*e %.*e %.*e\n", prc, x1,             prc, y1, prc, zero);
    fprintf(fFile, "%.*e %.*e %.*e\n", prc, -fWinWidthHalf, prc, y1, prc, zero);
    fprintf(fFile, "%.*e %.*e %.*e\n", prc, -fWinWidthHalf, prc, y2, prc, zero);
    fprintf(fFile, "%.*e %.*e %.*e\n", prc, x2,             prc, y2, prc, fDskThickness);
    fprintf(fFile, "%.*e %.*e %.*e\n", prc, x1,             prc, y1, prc, fDskThickness);
    fprintf(fFile, "%.*e %.*e %.*e\n", prc, -fWinWidthHalf, prc, y1, prc, fDskThickness);
    fprintf(fFile, "%.*e %.*e %.*e\n", prc, -fWinWidthHalf, prc, y2, prc, fDskThickness);
    // coordinates of local (0,0,0)
    fprintf(fFile, "%.*e %.*e %.*e\n",
      prc, zero,
      prc, zero,
      prc, zero);
    // rotation of local coordinate system
    fprintf(fFile, "%.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e\n",
      prc,  one, prc, zero, prc, zero,
      prc, zero, prc,  one, prc, zero,
      prc, zero, prc, zero, prc,  one);
    fprintf(fFile, "//*****************************************************\n");

    // calc for rotations
    fprintf(fFile, "%s%i#2\n", fGlbName.Data(), fNmbrWin);
    fprintf(fFile, "%s%s\n",   fGlbName.Data(), fNmbrDsk.Data());
    //coordinates of local (0,0,0)
    fprintf(fFile, "%.*e %.*e %.*e\n",
      prc, zero,
      prc, zero,
      prc, fDskThickness);
    // rotation of local coordinate system
    fprintf(fFile, "%.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e\n",
      prc,  one, prc, zero, prc, zero,
      prc, zero, prc, -one, prc, zero,
      prc, zero, prc, zero, prc, -one);
    fprintf(fFile, "//*****************************************************\n");

    // calc for rotations
    fprintf(fFile, "%s%i#3\n", fGlbName.Data(), fNmbrWin);
    fprintf(fFile, "%s%s\n",   fGlbName.Data(), fNmbrDsk.Data());
    //coordinates of local (0,0,0)
    fprintf(fFile, "%.*e %.*e %.*e\n",
      prc, zero,
      prc, zero,
      prc, zero);
    // rotation of local coordinate system
    fprintf(fFile, "%.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e\n",
      prc, -one, prc, zero, prc, zero,
      prc, zero, prc, -one, prc, zero,
      prc, zero, prc, zero, prc,  one);
    fprintf(fFile, "//*****************************************************\n");

    // calc for rotations
    fprintf(fFile, "%s%i#4\n", fGlbName.Data(), fNmbrWin++);
    fprintf(fFile, "%s%s\n",   fGlbName.Data(), fNmbrDsk.Data());
    //coordinates of local (0,0,0)
    fprintf(fFile, "%.*e %.*e %.*e\n",
      prc, zero,
      prc, zero,
      prc, fDskThickness);
    // rotation of local coordinate system
    fprintf(fFile, "%.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e\n",
      prc, -one, prc, zero, prc, zero,
      prc, zero, prc,  one, prc, zero,
      prc, zero, prc, zero, prc, -one);
    fprintf(fFile, "//*****************************************************\n");
  }

  fprintf(fFile, "%s%i#1\n", fGlbName.Data(), fNmbrWin);
  fprintf(fFile, "%s%s\n",   fGlbName.Data(), fNmbrDsk.Data());
  fprintf(fFile, "BOX\n");
  fprintf(fFile, "%s\n",     fMatWin.Data());
  fprintf(fFile, "%.*e %.*e %.*e\n", prc, x2,             prc,  y2, prc, zero);
  fprintf(fFile, "%.*e %.*e %.*e\n", prc, -fWinWidthHalf, prc,  y2, prc, zero);
  fprintf(fFile, "%.*e %.*e %.*e\n", prc, -fWinWidthHalf, prc, -y2, prc, zero);
  fprintf(fFile, "%.*e %.*e %.*e\n", prc, x2,             prc, -y2, prc, zero);
  fprintf(fFile, "%.*e %.*e %.*e\n", prc, x2,             prc,  y2, prc, fDskThickness);
  fprintf(fFile, "%.*e %.*e %.*e\n", prc, -fWinWidthHalf, prc,  y2, prc, fDskThickness);
  fprintf(fFile, "%.*e %.*e %.*e\n", prc, -fWinWidthHalf, prc, -y2, prc, fDskThickness);
  fprintf(fFile, "%.*e %.*e %.*e\n", prc, x2,             prc, -y2, prc, fDskThickness);
  // coordinates of local (0,0,0)
  fprintf(fFile, "%.*e %.*e %.*e\n",
    prc, zero,
    prc, zero,
    prc, zero);
  // rotation of local coordinate system
  fprintf(fFile, "%.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e\n",
    prc, one,  prc, zero, prc, zero,
    prc, zero, prc, one,  prc, zero,
    prc, zero, prc, zero, prc, one);
  fprintf(fFile, "//*****************************************************\n");

  // calc for rotations
  fprintf(fFile, "%s%i#2\n", fGlbName.Data(), fNmbrWin);
  fprintf(fFile, "%s%s\n",   fGlbName.Data(), fNmbrDsk.Data());
  //coordinates of local (0,0,0)
  fprintf(fFile, "%.*e %.*e %.*e\n",
    prc, zero,
    prc, zero,
    prc, zero);
  // rotation of local coordinate system
  fprintf(fFile, "%.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e\n",
    prc, -one, prc, zero, prc, zero,
    prc, zero, prc, -one, prc, zero,
    prc, zero, prc, zero, prc, one);
//     prc, TMath::Cos(TMath::Pi()), prc, -TMath::Sin(TMath::Pi()), prc, zero,
//     prc, TMath::Sin(TMath::Pi()), prc,  TMath::Cos(TMath::Pi()), prc, zero,
//     prc, zero, prc, zero, prc, one);
  fprintf(fFile, "//*****************************************************\n");





  cout<<"-I- calculating detector-array, surrounding the disk"<<endl;

  fprintf(fFile, "//-----------------------------------------------------\n");
  fprintf(fFile, "//  The Detector Array (hold mothers for mirror/mcp, air)\n");
  fprintf(fFile, "//-----------------------------------------------------\n");
  fprintf(fFile, "%s%s#1\n", fGlbName.Data(), fNmbrAry.Data());
  fprintf(fFile, "%s%s\n",   fGlbName.Data(), fNmbrLMo.Data());
  fprintf(fFile, "BOX\n");
  fprintf(fFile, "%s\n",     fMatAry.Data());
  fprintf(fFile, "%.*e %.*e %.*e\n",  prc,  fAryWidthHalf, prc,       zero, prc, zero);
  fprintf(fFile, "%.*e %.*e %.*e\n",  prc, -fAryWidthHalf, prc,       zero, prc, zero);
  fprintf(fFile, "%.*e %.*e %.*e\n",  prc, -fAryWidthHalf, prc, fAryHeight, prc, zero);
  fprintf(fFile, "%.*e %.*e %.*e\n",  prc,  fAryWidthHalf, prc, fAryHeight, prc, zero);
  fprintf(fFile, "%.*e %.*e %.*e\n",  prc,  fAryWidthHalf, prc,       zero, prc, fDskThickness);
  fprintf(fFile, "%.*e %.*e %.*e\n",  prc, -fAryWidthHalf, prc,       zero, prc, fDskThickness);
  fprintf(fFile, "%.*e %.*e %.*e\n",  prc, -fAryWidthHalf, prc, fAryHeight, prc, fDskThickness);
  fprintf(fFile, "%.*e %.*e %.*e\n",  prc,  fAryWidthHalf, prc, fAryHeight, prc, fDskThickness);
  // coordinates of local (0,0,0)
  fprintf(fFile, "%.*e %.*e %.*e\n",
    prc, zero,
    prc, fDskRadOuter,
    prc, zero);
  // rotation of local coordinate system
  fprintf(fFile, "%.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e\n",
    prc,  one, prc, zero, prc, zero,
    prc, zero, prc,  one, prc, zero,
    prc, zero, prc, zero, prc,  one);
  fprintf(fFile, "//*****************************************************\n");

  // copys
  for (Int_t i=1; i<fGlbEdges; i++) {
    fprintf(fFile, "%s%s#%i\n", fGlbName.Data(), fNmbrAry.Data(), i+1);
    fprintf(fFile, "%s%s\n",   fGlbName.Data(), fNmbrLMo.Data());
    // coordinates of local (0,0,0)
    fprintf(fFile, "%.*e %.*e %.*e\n",
      prc, -1.*TMath::Sin(i*fAlphaRad)*fDskRadOuter,
      prc,     TMath::Cos(i*fAlphaRad)*fDskRadOuter,
      prc, zero);
    // rotation of local coordinate system
    fprintf(fFile, "%.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e\n",
      prc,    TMath::Cos(i*fAlphaRad), prc, -1.*TMath::Sin(i*fAlphaRad), prc, zero,
      prc, 1.*TMath::Sin(i*fAlphaRad), prc,     TMath::Cos(i*fAlphaRad), prc, zero,
      prc,                    zero, prc,                     zero, prc,  one);
    fprintf(fFile, "//*****************************************************\n");
   }





  cout<<"-I- calculating detectors (mother for mirror/mcp)"<<endl;

  for (Int_t j=0; j<fGlbDetectorTypes; j++) {
    fprintf(fFile, "//-----------------------------------------------------\n");
    fprintf(fFile, "//  The Mother for Detectors, type %i (mirror/mcp, air)\n", j);
    fprintf(fFile, "//-----------------------------------------------------\n");
    for (Int_t i=0; i<fGlbDetectorsPerEdge/fGlbDetectorTypes; i++) {
      if (i == 0) {
        fprintf(fFile, "%s%s%i#1\n", fGlbName.Data(), fNmbrDet.Data(), j);
        fprintf(fFile, "%s%s#1\n",   fGlbName.Data(), fNmbrAry.Data());
        fprintf(fFile, "BOX\n");
        fprintf(fFile, "%s\n",       fMatDet.Data());
        fprintf(fFile, "%.*e %.*e %.*e\n",  prc,       zero, prc,       zero, prc, zero);
        fprintf(fFile, "%.*e %.*e %.*e\n",  prc,       zero, prc, fAryHeight, prc, zero);
        fprintf(fFile, "%.*e %.*e %.*e\n",  prc, -fDetWidth, prc, fAryHeight, prc, zero);
        fprintf(fFile, "%.*e %.*e %.*e\n",  prc, -fDetWidth, prc,       zero, prc, zero);
        fprintf(fFile, "%.*e %.*e %.*e\n",  prc,       zero, prc,       zero, prc, fDskThickness);
        fprintf(fFile, "%.*e %.*e %.*e\n",  prc,       zero, prc, fAryHeight, prc, fDskThickness);
        fprintf(fFile, "%.*e %.*e %.*e\n",  prc, -fDetWidth, prc, fAryHeight, prc, fDskThickness);
        fprintf(fFile, "%.*e %.*e %.*e\n",  prc, -fDetWidth, prc,       zero, prc, fDskThickness);
        // coordinates of local (0,0,0)
        fprintf(fFile, "%.*e %.*e %.*e\n",
          prc, fAryWidthHalf-fDetGapBorder-j*(fDetWidth+fDetGapBetween),
          prc, zero,
          prc, zero);
        // rotation of local coordinate system
        fprintf(fFile, "%.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e\n",
          prc,  one, prc, zero, prc, zero,
          prc, zero, prc,  one, prc, zero,
          prc, zero, prc, zero, prc,  one);
        fprintf(fFile, "//*****************************************************\n");
      } else {
        fprintf(fFile, "%s%s%i#%i\n", fGlbName.Data(), fNmbrDet.Data(), j, i+1);
        fprintf(fFile, "%s%s#1\n",   fGlbName.Data(), fNmbrAry.Data());
        // coordinates of local (0,0,0): always left/bottom for each detector
        fprintf(fFile, "%.*e %.*e %.*e\n",
          prc, fAryWidthHalf-fDetGapBorder-(i*fGlbDetectorTypes+j)*(fDetWidth+fDetGapBetween),
          prc, zero,
          prc, zero);
        // rotation of local coordinate system
        fprintf(fFile, "%.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e\n",
          prc,  one, prc, zero, prc, zero,
          prc, zero, prc,  one, prc, zero,
          prc, zero, prc, zero, prc,  one);
        fprintf(fFile, "//*****************************************************\n");
      }
    }
  }





  cout<<"-I- calculating mirrors"<<endl;

  for (Int_t j=0; j<fGlbDetectorTypes; j++) {
    fprintf(fFile, "//-----------------------------------------------------\n");
    fprintf(fFile, "//  The Mirror, type %i\n", j);
    fprintf(fFile, "//-----------------------------------------------------\n");
    fprintf(fFile, "%s%s%i\n",   fGlbName.Data(), fNmbrMir.Data(), j);
    fprintf(fFile, "%s%s%i#1\n", fGlbName.Data(), fNmbrDet.Data(), j);
    fprintf(fFile, "BOX\n");
    fprintf(fFile, "%s%i\n",     fMatMir.Data(), j);
    fprintf(fFile, "%.*e %.*e %.*e\n",  prc,       zero, prc,       zero, prc, zero);
    fprintf(fFile, "%.*e %.*e %.*e\n",  prc,       zero, prc, fMirHeight, prc, zero);
    fprintf(fFile, "%.*e %.*e %.*e\n",  prc, -fDetWidth, prc, fMirHeight, prc, zero);
    fprintf(fFile, "%.*e %.*e %.*e\n",  prc, -fDetWidth, prc,       zero, prc, zero);
    fprintf(fFile, "%.*e %.*e %.*e\n",  prc,       zero, prc,       zero, prc, fDskThickness);
    fprintf(fFile, "%.*e %.*e %.*e\n",  prc,       zero, prc, fMirHeight, prc, fDskThickness);
    fprintf(fFile, "%.*e %.*e %.*e\n",  prc, -fDetWidth, prc, fMirHeight, prc, fDskThickness);
    fprintf(fFile, "%.*e %.*e %.*e\n",  prc, -fDetWidth, prc,       zero, prc, fDskThickness);
    // coordinates of local (0,0,0)
    fprintf(fFile, "%.*e %.*e %.*e\n",
      prc, zero,
      prc, zero,
      prc, zero);
    // rotation of local coordinate system
    fprintf(fFile, "%.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e\n",
      prc,  one, prc, zero, prc, zero,
      prc, zero, prc,  one, prc, zero,
      prc, zero, prc, zero, prc, one);
    fprintf(fFile, "//*****************************************************\n");
  }



  cout<<"-I- calculating mcps"<<endl;

  for (Int_t j=0; j<fGlbDetectorTypes; j++) {
    fprintf(fFile, "//-----------------------------------------------------\n");
    fprintf(fFile, "//  The MCPs\n");
    fprintf(fFile, "//-----------------------------------------------------\n");
    fprintf(fFile, "%s%s%i\n",   fGlbName.Data(), fNmbrMcp.Data(), j);
    fprintf(fFile, "%s%s%i#1\n", fGlbName.Data(), fNmbrDet.Data(), j);
    fprintf(fFile, "BOX\n");
    fprintf(fFile, "%s\n",       fMatMcp.Data());
    fprintf(fFile, "%.*e %.*e %.*e\n",  prc,       zero, prc,       zero, prc, zero);
    fprintf(fFile, "%.*e %.*e %.*e\n",  prc,       zero, prc, fMcpHeight, prc, zero);
    fprintf(fFile, "%.*e %.*e %.*e\n",  prc, -fDetWidth, prc, fMcpHeight, prc, zero);
    fprintf(fFile, "%.*e %.*e %.*e\n",  prc, -fDetWidth, prc,       zero, prc, zero);
    fprintf(fFile, "%.*e %.*e %.*e\n",  prc,       zero, prc,       zero, prc, fDskThickness);
    fprintf(fFile, "%.*e %.*e %.*e\n",  prc,       zero, prc, fMcpHeight, prc, fDskThickness);
    fprintf(fFile, "%.*e %.*e %.*e\n",  prc, -fDetWidth, prc, fMcpHeight, prc, fDskThickness);
    fprintf(fFile, "%.*e %.*e %.*e\n",  prc, -fDetWidth, prc,       zero, prc, fDskThickness);
    // coordinates of local (0,0,0)
    fprintf(fFile, "%.*e %.*e %.*e\n",
      prc, zero,
      prc, fMirHeight,
      prc, zero);
    // rotation of local coordinate system
    fprintf(fFile, "%.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e %.*e\n",
      prc,  one, prc, zero, prc, zero,
      prc, zero, prc,  one, prc, zero,
      prc, zero, prc, zero, prc, one);
    fprintf(fFile, "//*****************************************************\n");
  }



  fclose(fFile);
}



