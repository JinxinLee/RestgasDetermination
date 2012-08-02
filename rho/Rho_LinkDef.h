#ifdef __CINT__
// ##################################################

//################  RhoMath #########################
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


//#################### RhoBase #######################
#pragma link C++ class TRho;
#pragma link C++ function operator << ( ostream &, const TRho & );
#pragma link C++ class TParm;
#pragma link C++ class TParmBool;
#pragma link C++ class TParmInt;
#pragma link C++ class TParmDouble;
#pragma link C++ class TParmString;

#pragma link C++ class TPdt;

#pragma link C++ class TEventInfo;
#pragma link C++ function operator << ( ostream &, const TEventInfo & );

#pragma link C++ class TFactory;
#pragma link C++ class TSmartPointer;

#pragma link C++ class TCandidate;
#pragma link C++ class TFitParams;
#pragma link C++ class TConstraint;
#pragma link C++ class TCandList;
#pragma link C++ function operator << ( ostream &, const TCandList & );
#pragma link C++ class TCandListIterator;
#pragma link C++ function operator << ( ostream &, const TCandidate & );

// Interface section

#pragma link C++ class VAbsTag;
#pragma link C++ function operator << ( ostream &, const VAbsTag & );
#pragma link C++ class VAbsMicroCandidate;
#pragma link C++ function operator << ( ostream &, const VAbsMicroCandidate & );
#pragma link C++ class VAbsPidInfo;


// Collection
#pragma link C++ class VAbsCollection;
// Tag
#pragma link C++ class VAbsBit;
#pragma link C++ function operator << ( ostream &, const VAbsBit & );
#pragma link C++ class VAbsBitBoolIterator;
#pragma link C++ class VAbsBitIntIterator;
#pragma link C++ class VAbsBitFloatIterator;
// MC
#pragma link C++ class VAbsTruth;
#pragma link C++ function operator << ( ostream &, const VAbsTruth & );
// IO
#pragma link C++ class VAbsReader;
#pragma link C++ class VAbsWriter;
// VAbsVertex
#pragma link C++ class VAbsVertex;
#pragma link C++ function operator << ( ostream &, const VAbsVertex & );
#pragma link C++ class VAbsVertexer;
#pragma link C++ class VAbsVertexAlgorithm;
#pragma link C++ class TSimpleVertex;
// Selector
#pragma link C++ class VAbsPidSelector;
#pragma link C++ class VAbsVertexSelector;
#pragma link C++ function operator << ( ostream &, const VAbsVertexSelector & );


//#################### RhoSelector #######################
#pragma link C++ class TPidChargedSelector;
#pragma link C++ class TPidNeutralSelector;
#pragma link C++ class TPidPlusSelector;
#pragma link C++ class TPidMinusSelector;
#pragma link C++ class TPidMassSelector;
#pragma link C++ class TPidEnergySelector;
#pragma link C++ class TPidMomentumSelector;
#pragma link C++ class TGoodPhotonSelector;

#pragma link C++ class TSimpleVertexSelector;

#pragma link C++ class TPidSimpleElectronSelector;
#pragma link C++ class TPidSimpleMuonSelector;
#pragma link C++ class TPidSimplePionSelector;
#pragma link C++ class TPidSimpleKaonSelector;
#pragma link C++ class TPidSimpleProtonSelector;

//#################### RhoTools #######################
#pragma link C++ class TAdd4Vertexer;
#pragma link C++ class TBooster;
//#pragma link C++ class TBVariables;
#pragma link C++ class TCandListSort;
#pragma link C++ class TCandPtr;
#pragma link C++ class TCandReader;
#pragma link C++ class TConeMomFlow;
#pragma link C++ class TCones;
#pragma link C++ class TDecayMode;
#pragma link C++ class TDecayTree;
#pragma link C++ class TDiagVertex;
#pragma link C++ class TDummyFitter;
#pragma link C++ class TEventShape;
#pragma link C++ class TFastCombiner;
#pragma link C++ class TFindOmittedParticle;
#pragma link C++ class TFoxWolfMom;
#pragma link C++ class TFWMoments;
#pragma link C++ class TLgndrMomFlow;
#pragma link C++ class TMomTensor;
#pragma link C++ class TOpAdd4;
#pragma link C++ class TOpClone;
#pragma link C++ class TOperatorBase;
#pragma link C++ class TOperator;
#pragma link C++ class TOpMakeTree;
#pragma link C++ class TPrintTree;
#pragma link C++ class TReadTree;
#pragma link C++ class TSphericity;
#pragma link C++ class TThrust;
#pragma link C++ class TThrustVector;
#pragma link C++ class TTotalMomentum;
#pragma link C++ class TTreeNavigator;
#pragma link C++ class TUnaryOperator;
#pragma link C++ class VAbsFitter;


//#################### RhoHistogram #######################

//#pragma link C++ class THistogram;
//#pragma link C++ class TEnergyHistogram;
//#pragma link C++ class TMassHistogram;
//#pragma link C++ class TMomentumHistogram;
//#pragma link C++ class TEoverPHistogram;
//#pragma link C++ class TMoverPHistogram;
//#pragma link C++ class TDalitzPlot;
#pragma link C++ class TTuple;
#pragma link C++ class TColumn;
//#pragma link C++ class THistID;

#endif
