/**

  helper/create_media.C

  @brief Calculates the values for media file

  Calculates refraction index and absorption lengths

  @author Peter Koch
  @date   2008-03-26
  @since  2007-12-22

**/

gROOT->Reset();
gSystem->Load("libMathCore");

// -------------------------------------------------------------------------------------------------
//   user defined variables
// -------------------------------------------------------------------------------------------------

/// where to save
TString  fFileName = "/opt/panda/pandaroot/macro/dsk/dsk_media.geo";

/// amount of mirror types
const Int_t    fNumOfMirrorTypes = 3;
/// where to make the splits. These values must be without fraction!
const Double_t fSplitLambda[fNumOfMirrorTypes+3] = {300., 350., 378., 400., 700., 800.};
// const Double_t fSplitLambda[fNumOfMirrorTypes+3] = {300., 320., 340., 350., 700., 800.};
// Double_t     fSplitLambda[fNumOfMirrorTypes+3] = {300., 350., 500., 700., 800.};
/// step size in nm for output. These values must be without fraction!
const Double_t fStepSize = 10.;


const Int_t fPrecision = 6;

// -------------------------------------------------------------------------------------------------
//   other variables and constants
// -------------------------------------------------------------------------------------------------

/// use Sellmeier equation to calculate RefracGlass
const Double_t B1 = 0.67071081;
const Double_t C1 = 0.00449192312e6; // [nm^2]
const Double_t B2 = 0.433322857;
const Double_t C2 = 0.0132812976e6; // [nm^2]
const Double_t B3 = 0.877379057;
const Double_t C3 = 95.8899878e6; // [nm^2]

const Double_t fConst_c = 299792458.;
const Double_t fConst_e = 1.602176487e-19;
const Double_t fConst_h = 6.6260693e-34;
const Double_t fConst_nm2eV = fConst_h*fConst_c/fConst_e*1.e9;

// -------------------------------------------------------------------------------------------------
//   Helper functions
// -------------------------------------------------------------------------------------------------


/// splits in eV
Double_t fSplit[fNumOfMirrorTypes+3];

/// vacuum
Double_t RefracVacuum(Double_t x) {
  return 1.0;
}


/// air, reflecting
Double_t AbsorpAir(Double_t x) { // x energy in [eV]
  return 0.;  // [cm]
}
Double_t RefracAir(Double_t x) {
  return 1.00000002;  // air - will result in 1.0 cause precision is to low!
}


/// mcp
Double_t AbsorpMcp(Double_t mom) { // x energy in [eV]
  return AbsorpGlass(mom); // [cm]
}
Double_t RefracMcp(Double_t lambda) {
  return RefracGlass(lambda);
}


/// glass
Double_t AbsorpGlass(Double_t x) { // x [eV]
  // non apsorption at the moment
  return 100000.0; // [cm]
}
Double_t RefracGlass(Double_t Lambda) { // vacuum wavelength in [nm]
  Double_t x = Lambda*Lambda;
  return TMath::Sqrt( 1. + B1*x/(x-C1)
                         + B2*x/(x-C2)
                         + B3*x/(x-C3) );
}


/// mirror, normal glas in interval, else reflecting
Double_t AbsorpMirror(Int_t nr, Double_t lambda, Double_t mom) {
  if ( ((nr<fNumOfMirrorTypes-1) && (mom<=fSplit[1+nr]) && (mom>fSplit[1+nr+1]))  ||
       ((nr==fNumOfMirrorTypes-1) && (mom<=fSplit[1+nr]) && (mom>=fSplit[1+nr+1]))    ) {
    // this is for transmission
    return AbsorpGlass(mom);
    // a test value:
//     return 0.0;
  }
  // this is for reflection
  return 0.0;
  // a test value:
//   return AbsorpGlass(mom);
}
Double_t EfficiencyMirror(Int_t nr, Double_t lambda) {
  if ( ((nr<fNumOfMirrorTypes-1) && (lambda>=fSplit[1+nr]) && (lambda<fSplit[1+nr+1]))  ||
       ((nr==fNumOfMirrorTypes-1) && (lambda>=fSplit[1+nr]) && (lambda<=fSplit[1+nr+1]))    ) {
    return 1.0.;
  }
  return 1.0;
}
Double_t RefracMirror(Int_t nr, Double_t lambda, Double_t mom) {
  if ( ((nr<fNumOfMirrorTypes-1) && (mom<=fSplit[1+nr]) && (mom>fSplit[1+nr+1]))  ||
       ((nr==fNumOfMirrorTypes-1) && (mom<=fSplit[1+nr]) && (mom>=fSplit[1+nr+1]))    ) {
    // this is for transmission
    return RefracGlass(lambda);
    // a test value
//     return 0.0;
  }
  // this is for reflection
  return 0.0;
  // a test value
//   return RefracGlass(lambda);
}



/// -----------------------------------------------------------------------------
///    create_media(void)
/// -----------------------------------------------------------------------------

void create_media(void)
{
  printf("Saveing in %s\n", fFileName.Data());

  // calc splits in eV
  for (Int_t i=0; i<fNumOfMirrorTypes+3; i++) {
    fSplit[i] = fConst_nm2eV / fSplitLambda[i];
  }

  // calculate the array size NCKOV for glass
  Int_t    fNCKOVGlass = 0;
  fNCKOVGlass = (fSplitLambda[fNumOfMirrorTypes+2]-fSplitLambda[0])/fStepSize +1;

  // calculate fNCKOVMirror ( for every edge -1+3=+2)
  Int_t    fNCKOVMirror = 0;
  fNCKOVMirror = fNCKOVGlass + 2*(fNumOfMirrorTypes+1);
  for (Int_t i = 0; i<fNumOfMirrorTypes+3; i++)
  {
    if (int(fSplitLambda[i]) % int(fStepSize) > 0)
      fNCKOVMirror++;
  }

  // used as counter between intervals
  Int_t    fNCKOV = 0;

  // variables
  Double_t fTmpStep;
  Double_t fTmpWlen;
  Int_t    fMirCnt;
  Double_t fMom      = 0.;


// -----------------------------------------------------------------------------
//    output to .geo - file
// -----------------------------------------------------------------------------

  FILE* fFile;
  fFile = fopen(fFileName.Data(), "w");



  /////////////////////////////////////////////////////////////////////////////
  // The vacuum
  // Its taken from the real media file
  // Just here so i wont have to copy n paste but have a whole media file
  /////////////////////////////////////////////////////////////////////////////
  cout<<"-I- generating vacuum"<<endl;

  fprintf(fFile, "vacuum  1  1.e-16  1.e-16  1.e-16\n");
  fprintf(fFile, "        0  1  30.  .001\n");
  fprintf(fFile, "        0\n\n\n");



  /////////////////////////////////////////////////////////////////////////////
  // The fused silica
  // Its glass our disk is made of
  // The values for absorptionlength are taken from DIRCgalss and
  // extrapolated to our wavelength interval.
  /////////////////////////////////////////////////////////////////////////////

  cout<<"-I- generating DISKglass"<<endl;

  // Amorph Silicium-Dioxid
  // name       name of the material
  // ncomp      number of components
  // aw[ncomp]  atomic weights
  // an[ncomp]  atomic numbers
  // dens       density in g cm(**-3)
  // wm[ncomp]  weight of each component
  fprintf(fFile, "DISKglass  -2  28.0855  15.9994  14.  8.  2.201  1.  2.\n");
  // sensflag   1 for active volume, 0 for passiv volume
  // fldflag    field
  // fld        maximum field value in kilogauss
  // epsil      boundray crossing precision
  fprintf(fFile, "           1  0  20.  .0001\n");
  // npckov     number of values used to define the optical properties
  fprintf(fFile, "           %d\n", fNCKOVGlass);
  // ppckov[npckov]   photon momentum in eV
  // absco[npckov]    absorption length (dielectric) / absorption propability (metal)
  // effic[npckov]    detection efficiency
  // rindex[npckov]   refraction index
  for (Int_t i=0; i<fNCKOVGlass; i++) {
    fTmpWlen = fSplitLambda[fNumOfMirrorTypes+2] - i*fStepSize;
    fMom = fConst_nm2eV / fTmpWlen;
    fprintf(fFile, "           %.*e  %.5e  1.0  %.*e\n",
            fPrecision, fMom, AbsorpGlass(fMom), fPrecision, RefracGlass(fTmpWlen) );
  }
  fprintf(fFile, "\n\n");



  /////////////////////////////////////////////////////////////////////////////
  // The air.
  // Its the thin layer around our disk.
  // The absorptionlength is infinit, the refraction index is the index of
  // normal air
  /////////////////////////////////////////////////////////////////////////////

  cout<<"-I- generating DISKair"<<endl;

  fprintf(fFile, "DISKair  3  14.01  16.  39.95  7.  8.  18.  1.205e-3  .78  .21  .01\n");
  fprintf(fFile, "         0  0  20.  .0001\n");
  fprintf(fFile, "         %d\n", fNCKOVGlass);

  for (Int_t i=0; i<fNCKOVGlass; i++) {
    fTmpWlen = fSplitLambda[fNumOfMirrorTypes+2] - i*fStepSize;
    fMom = fConst_nm2eV / fTmpWlen;
    fprintf(fFile, "         %.*e  %.5e  1.0  %.*e\n",
            fPrecision, fMom, AbsorpAir(fMom), fPrecision, RefracAir(fMom));
  }
  fprintf(fFile, "\n\n");




  /////////////////////////////////////////////////////////////////////////////
  // The air.
  // Its the thin layer around our disk.
  // The absorptionlength is infinit, the refraction index is the index of
  // normal air
  /////////////////////////////////////////////////////////////////////////////

  cout<<"-I- generating DISKmcp"<<endl;

  fprintf(fFile, "DISKmcp  -2  28.0855  15.9994  14.  8.  2.201  1.  2.\n");
  fprintf(fFile, "         1  0  20.  .0001\n");
  fprintf(fFile, "         %d\n", fNCKOVGlass);

  for (Int_t i=0; i<fNCKOVGlass; i++) {
    fTmpWlen = fSplitLambda[fNumOfMirrorTypes+2] - i*fStepSize;
    fMom = fConst_nm2eV / fTmpWlen;
    fprintf(fFile, "         %.*e  %.5e  1.0  %.*e\n",
            fPrecision, fMom, AbsorpMcp(fMom), fPrecision, RefracMcp(fTmpWlen));
  }
  fprintf(fFile, "\n\n");



  /////////////////////////////////////////////////////////////////////////////
  // The mirrors
  // Its fused silica coated with the mirror
  // so we use fused silica as in DISKglass, but change the refraction index
  /////////////////////////////////////////////////////////////////////////////

  for (Int_t i=0; i<fNumOfMirrorTypes; i++) {
    cout<<"-I- generating DISKmirror" << i << endl;
    fprintf(fFile, "DISKmirror%d  -2  28.0855  15.9994  14.  8.  2.201  1.  2.\n", i);
    fprintf(fFile, "             1  0  20.  .0001\n");
    fprintf(fFile, "             %d\n", fNCKOVMirror);

    fTmpStep = fSplitLambda[fNumOfMirrorTypes+2];
    fTmpWlen = fSplitLambda[fNumOfMirrorTypes+2];
    fMirCnt  = fNumOfMirrorTypes+1;
    while (fTmpStep >= fSplitLambda[0]) {
      fTmpWlen = fTmpStep;
      fMom = fConst_nm2eV / fTmpWlen;
      fprintf(fFile, "             %.*e  %.5e  %.1lf  %.*e\n",
              fPrecision, fMom, AbsorpMirror(i, fTmpWlen, fMom), EfficiencyMirror(i, fTmpWlen), fPrecision, RefracMirror(i, fTmpWlen, fMom) );

      if (fTmpStep - fStepSize <= fSplitLambda[fMirCnt]) {
        fTmpWlen = fSplitLambda[fMirCnt];

        // now around the split
        if (fMirCnt != 0) {
          fMom = fConst_nm2eV / fTmpWlen - TMath::Power(10, -fPrecision);
          fprintf(fFile, "             %.*e  %.5e  %.1lf  %.*e\n",
                  fPrecision, fMom, AbsorpMirror(i, fConst_nm2eV/fMom, fMom), EfficiencyMirror(i, fConst_nm2eV/fMom), fPrecision, RefracMirror(i, fConst_nm2eV/fMom, fMom) );
        }

        fMom = fConst_nm2eV / fTmpWlen;
        fprintf(fFile, "             %.*e  %.5e  %.1lf  %.*e\n",
                fPrecision, fMom, AbsorpMirror(i, fTmpWlen, fMom), EfficiencyMirror(i, fTmpWlen), fPrecision, RefracMirror(i, fTmpWlen, fMom) );

        if (fMirCnt != 0) {
          fMom = fConst_nm2eV / fTmpWlen + TMath::Power(10, -fPrecision);
          fprintf(fFile, "             %.*e  %.5e  %.1lf  %.*e\n",
                  fPrecision, fMom, AbsorpMirror(i, fConst_nm2eV/fMom, fMom), EfficiencyMirror(i, fConst_nm2eV/fMom), fPrecision, RefracMirror(i, fConst_nm2eV/fMom, fMom) );
        }

        if (fTmpStep -fStepSize == fSplitLambda[fMirCnt])
          fTmpStep -= fStepSize;
        fMirCnt--;
      }

      fTmpStep -= fStepSize;

    }

    fprintf(fFile, "\n\n");

  }

  fclose(fFile);
}
