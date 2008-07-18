#ifdef __CINT__
// ##################################################
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class TConsistency;
#pragma link C++ class TConsistencySet;
#pragma link C++ class TAsymGaussConsistency;
#pragma link C++ class TGaussConsistency;
#pragma link C++ class TChisqConsistency;
#pragma link C++ class TPoissonConsistency;
#pragma link C++ class TNumRecipes;
// TError
#pragma link C++ class TError;
#pragma link C++ function operator << ( ostream &, const TError & );
#pragma link C++ class TDoubleErr;
#pragma link C++ class TPointErr;
#pragma link C++ class TVectorErr;
#pragma link C++ class TLorentzVectorErr;

#endif
