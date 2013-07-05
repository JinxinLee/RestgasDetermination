#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class std::pair<std::string,std::string>+;
#pragma link C++ class ModelFitResult+;
#pragma link C++ struct ModelStructs::minimization_parameter+;
#pragma link C++ class std::pair<double, ModelStructs::minimization_parameter>+;
#pragma link C++ class std::vector<std::pair<double, ModelStructs::minimization_parameter> >+;
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
#pragma link C++ class std::map<std::string, std::vector<PndLmdResolution*>, ModelStructs::string_comp>+;
#pragma link C++ class std::vector<TFile*>+;
#pragma link C++ class std::vector < PndLmdLumiFitOptions* >+;
#pragma link C++ class std::vector<std::pair<PndLmdResolution*, PndLmdLumiFitResult*> >+;
#pragma link C++ class std::pair<PndLmdResolution*, PndLmdLumiFitResult*>+;
#pragma link C++ class PndLmdResultPlotter;
#pragma link C++ class std::map<PndLmdAcceptance*, std::vector<PndLmdLumiFitResult*> >+;
#pragma link C++ class std::map<PndLmdAcceptance*, std::vector<PndLmdLumiFitResult*> >::iterator;
#pragma link C++ class std::set<PndLmdLumiFitResult*>+;
#pragma link C++ class std::set<PndLmdLumiFitResult*>::iterator;
#pragma link C++ class std::vector < PndLmdResultPlotter::graph_bundle_1D >+;
#pragma link C++ class PndLmdResultPlotter::acceptance_bundle;
#pragma link C++ class PndLmdFit::lmd_dimension;
#pragma link C++ class PndLmdLumiHelper::lmd_graph;
#pragma link C++ class std::vector < PndLmdLumiHelper::lmd_graph* >;


#endif
