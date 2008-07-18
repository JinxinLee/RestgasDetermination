#ifdef __CINT__
// ##################################################
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

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

// Beta Stuff
//#pragma link C++ class BtaMcTruth;
//#pragma link C++ class BtaConstraint;
//#pragma link C++ class BtaSimpleVertex;
//#pragma link C++ class BtaAbsVertex;
//#pragma link C++ class BtaFitParams;
//#pragma link C++ class BtaCandList;
//#pragma link C++ class BtaEventInfo;
//#pragma link C++ class BtaCandListIterator;
//#pragma link C++ class BtaCandidate;
//#pragma link C++ class BtaCandidateFactory-;

#endif
