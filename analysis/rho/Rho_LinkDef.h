#ifdef __CINT__
// ##################################################

//################  RhoMath #########################
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

//#pragma link C++ class TConsistency;
//#pragma link C++ class TConsistencySet;
//#pragma link C++ class TAsymGaussConsistency;
//#pragma link C++ class TGaussConsistency;
//#pragma link C++ class TChisqConsistency;
//#pragma link C++ class TPoissonConsistency;
//#pragma link C++ class TNumRecipes;
// TError
#pragma link C++ class RhoError;
#pragma link C++ function operator << ( ostream &, const RhoError & );
#pragma link C++ class RhoDoubleErr;
//#pragma link C++ class RhoPointErr;
#pragma link C++ class RhoVector3Err;
#pragma link C++ class RhoLorentzVectorErr;

#pragma link C++ class RhoCalculationTools;
//#################### RhoBase #######################
//#pragma link C++ class TRho;
//#pragma link C++ function operator << ( ostream &, const TRho & );
// #pragma link C++ class TParm;
// #pragma link C++ class TParmBool;
// #pragma link C++ class TParmInt;
// #pragma link C++ class TParmDouble;
// #pragma link C++ class TParmString;

#pragma link C++ class RhoPdtLoader;

#pragma link C++ class RhoEventInfo;
#pragma link C++ function operator << ( ostream &, const RhoEventInfo & );
#pragma link C++ class RhoEventShape;
#pragma link C++ class RhoEventShapes;

#pragma link C++ class RhoFactory;
//#pragma link C++ class TSmartPointer;

#pragma link C++ class RhoCandidate;
#pragma link C++ function operator << ( ostream &, const RhoCandidate & );
//#pragma link C++ class TFitParams;
//#pragma link C++ class TConstraint;
#pragma link C++ class RhoCandList;
#pragma link C++ function operator << ( ostream &, const RhoCandList & );
#pragma link C++ class RhoCandListIterator;

#pragma link C++ class RhoConstraint;
#pragma link C++ class RhoMassConstraint;
#pragma link C++ class RhoPointingConstraint;
#pragma link C++ class RhoBeamConstraint;
//#pragma link C++ class RhoVertexConstraint;

// Interface section

//#pragma link C++ class VAbsTag;
//#pragma link C++ function operator << ( ostream &, const VAbsTag & );
//#pragma link C++ class VAbsMicroCandidate;
//#pragma link C++ function operator << ( ostream &, const VAbsMicroCandidate & );
//#pragma link C++ class VAbsPidInfo;


// Collection
//#pragma link C++ class VAbsCollection;
// Tag
//#pragma link C++ class VAbsBit;
//#pragma link C++ function operator << ( ostream &, const VAbsBit & );
//#pragma link C++ class VAbsBitBoolIterator;
//#pragma link C++ class VAbsBitIntIterator;
//#pragma link C++ class VAbsBitFloatIterator;
// MC
//#pragma link C++ class VAbsTruth;
//#pragma link C++ function operator << ( ostream &, const VAbsTruth & );
// IO
//#pragma link C++ class VAbsReader;
//#pragma link C++ class VAbsWriter;
// VAbsVertex
//#pragma link C++ class VAbsVertex;
//#pragma link C++ function operator << ( ostream &, const VAbsVertex & );
//#pragma link C++ class VAbsVertexer;
//#pragma link C++ class VAbsVertexAlgorithm;
//#pragma link C++ class TSimpleVertex;
// Selector
#pragma link C++ class RhoParticleSelectorBase;
#pragma link C++ class RhoEventSelectorBase;
#pragma link C++ class RhoVertexSelectorBase;
#pragma link C++ function operator << ( ostream &, const RhoVertexSelectorBase & );

#pragma link C++ class RhoFitterBase;

//#pragma link C++ class RhoKinVtxFitter;
//#pragma link C++ class RhoKinFitter;
//#pragma link C++ class RhoDecayTreeFitter;
//#pragma link C++ class RhoVtxPoca;
//#pragma link C++ class RhoKalmanVtxFitter;
//#pragma link C++ class Rho4CFitter;

//#################### RhoSelector #######################
#pragma link C++ class RhoChargedParticleSelector;
#pragma link C++ class RhoNeutralParticleSelector;
#pragma link C++ class RhoPlusParticleSelector;
#pragma link C++ class RhoMinusParticleSelector;
#pragma link C++ class RhoMassParticleSelector;
#pragma link C++ class RhoEnergyParticleSelector;
#pragma link C++ class RhoMomentumParticleSelector;
#pragma link C++ class RhoGoodTrackSelector;
#pragma link C++ class RhoGoodPhotonSelector;

#pragma link C++ class RhoSimpleVertexSelector;

#pragma link C++ class RhoSimpleElectronSelector;
#pragma link C++ class RhoSimpleMuonSelector;
#pragma link C++ class RhoSimplePionSelector;
#pragma link C++ class RhoSimpleKaonSelector;
#pragma link C++ class RhoSimpleProtonSelector;

//#################### RhoTools #######################
//#pragma link C++ class TAdd4Vertexer;
#pragma link C++ class RhoBooster;
//#pragma link C++ class RhoCandListSort;
//#pragma link C++ class TCandPtr;
//#pragma link C++ class TCandReader;
//#pragma link C++ class TConeMomFlow;
//#pragma link C++ class TCones;
//#pragma link C++ class TDecayMode;
//#pragma link C++ class TDecayTree;
//#pragma link C++ class TDiagVertex;
//#pragma link C++ class TDummyFitter;
//#pragma link C++ class TFastCombiner;
#pragma link C++ class RhoFindOmittedParticle;
//#pragma link C++ class TFoxWolfMom;
//#pragma link C++ class TFWMoments;
//#pragma link C++ class TLgndrMomFlow;
//#pragma link C++ class TMomTensor;
//#pragma link C++ class TOpAdd4;
//#pragma link C++ class TOpClone;
//#pragma link C++ class TOperatorBase;
//#pragma link C++ class TOperator;
//#pragma link C++ class TOpMakeTree;
//#pragma link C++ class TPrintTree;
//#pragma link C++ class TReadTree;
//#pragma link C++ class TSphericity;
//#pragma link C++ class TThrust;
//#pragma link C++ class TThrustVector;
#pragma link C++ class RhoTotalMomentum;
//#pragma link C++ class TTreeNavigator;
//#pragma link C++ class TUnaryOperator;


//#################### RhoHistogram #######################

#pragma link C++ class RhoTuple;
#pragma link C++ class RhoColumn;

#endif
