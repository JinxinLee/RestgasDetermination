#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class std::pair<std::string,std::string>+;
#pragma link C++ class ModelFitResult+;
#pragma link C++ struct ModelStructs::minimization_parameter+;

#pragma link C++ class EstimatorOptions+;
#pragma link C++ struct DataStructs::DimensionRange+;
#pragma link C++ class LumiFit::PndLmdFitModelOptions;
#pragma link C++ class LumiFit::LmdDimensionOptions;
#pragma link C++ class LumiFit::LmdDimensionRange;
#pragma link C++ class LumiFit::LmdDimension;
#pragma link C++ enum LumiFit::LmdDimensionType;
#pragma link C++ class LumiFit::LmdSimIPParameters;
#pragma link C++ class PndLmdLumiFitResult;
#pragma link C++ class PndLmdFitStorage+;
#pragma link C++ class PndLmdLumiFitOptions;
#pragma link C++ class PndLmdAbstractData;
#pragma link C++ class PndLmdHistogramData;
#pragma link C++ class PndLmdAngularData;
#pragma link C++ class PndLmdVertexData;
#pragma link C++ class PndLmdAcceptance;

#pragma link C++ class std::vector < PndLmdAngularData >+;
#pragma link C++ class std::vector < PndLmdVertexData >+;
#pragma link C++ class std::vector < PndLmdAcceptance >+;
#pragma link C++ class std::vector < PndLmdHistogramData >+;

#pragma link C++ class PndLmdLumiHelper;
#pragma link C++ struct PndLmdLumiHelper::lmd_graph+;
#pragma link C++ struct PndLmdLumiHelper::lmd_graph_data_point+;
#pragma link C++ class std::vector < lmd_graph_data_point >+;
#pragma link C++ class std::set < LumiFit::LmdDimension >+;
#pragma link C++ class std::set < LumiFit::LmdDimensionType >+;
#pragma link C++ class std::vector < PndLmdLumiHelper::lmd_graph* >+;

#pragma link C++ class LumiFit::PndLmdPlotter;

#endif
