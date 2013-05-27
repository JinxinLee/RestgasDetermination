#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class ModelFitResult+;
#pragma link C++ struct ModelFitResult::fit_parameter+;
#pragma link C++ class PndLmdLumiFitResult;
#pragma link C++ class PndLmdLumiFitOptions;
#pragma link C++ class PndLmdDataInterface;
#pragma link C++ class PndLmdDataBase;
#pragma link C++ class PndLmdData;
#pragma link C++ class PndLmdAcceptance;
#pragma link C++ class PndLmdResolution;
#pragma link C++ class PndLmdLumiHelper;
#pragma link C++ class std::vector < PndLmdData* >+;
#pragma link C++ class std::vector < PndLmdAcceptance* >+;
#pragma link C++ class std::vector < PndLmdResolution* >+;
#pragma link C++ class std::vector < PndLmdLumiFitOptions* >+;
#pragma link C++ class PndLmdResultPlotter;
#pragma link C++ class std::map<PndLmdAcceptance*, std::vector<PndLmdLumiFitResult*> >+;
#pragma link C++ class std::map<PndLmdAcceptance*, std::vector<PndLmdLumiFitResult*> >::iterator;
#pragma link C++ class std::vector < PndLmdResultPlotter::graph_bundle_1D >+;
#pragma link C++ struct PndLmdResultPlotter::acceptance_bundle;
#pragma link C++ struct PndLmdFit::lmd_dimension+;

#endif
