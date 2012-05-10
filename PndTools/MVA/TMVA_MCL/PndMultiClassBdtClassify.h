/* ***************************************
 * MultiClass MLP Classifier functions   *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */
/*
 * Note: This is just an interface to the original TMVA
 * implementation. To find out the available options, please read TMVA
 * manuals. In case of errors or wrong outputs produced by TMVA
 * classifiers, try to read their mailing list and send your questions
 * to the same list.
 ******* VERY IMORTANT ****
 * You NEED TMVA version > 4.1.X before this works.
 */
#pragma once
#ifndef PND_MULTICLASS_BDT_CLASSIFY_H
#define PND_MULTICLASS_BDT_CLASSIFY_H

//Local includes
#include "PndMvaClassifier.h"

// Root && TMVA
#include "TMVA/Reader.h"

// Interface definition for Multiclass MLP Classifier
class PndMultiClassBdtClassify : public PndMvaClassifier
{
  //______________ Public ____________________
 public:
  /**
   * Constructor.
   *@param InPut The file containig the weights.
   *@param classNames Labels of the classes to be used.
   *@param varNames  The name of the involved variables(features).
   */
  explicit PndMultiClassBdtClassify(std::string const& inputFile,
				    std::vector<std::string> const& classNames, 
				    std::vector<std::string> const& varNames);
  /**
   * Destructor.
   */
  virtual ~PndMultiClassBdtClassify();
  
  /**
   *@param EvtData The event to be classified.
   *@param result The output of the classifier is stored here.
   */
  void GetMvaValues( std::vector<float> EvtData,
		     std::map<std::string, float>& result );
  /**
   *@param EvtData The event to be classified.
   *@return The output of the classification.
   */
  std::string *Classify( std::vector<float> EvtData );
  
  /**
   * Initialize Classifier and data structures.
   */
  void Initialize();

  //______________________________________________
  //====== Getters and setters.  
  inline void SetReaderOptions(std::string const& opt);
  inline void SetWeightsFileName(std::string const& name);
  
  inline std::string const& GetReaderOptions() const;
  inline std::string const& GetWeightsFileName() const;
  
  //______________ Protected__________________
  //protected:
  //______________ Private ____________________
 private:
  // To avoid mistakes.
  PndMultiClassBdtClassify(PndMultiClassBdtClassify const& oth);
  PndMultiClassBdtClassify& operator=(PndMultiClassBdtClassify const& oth);
  
  // Set the TMVA reader options.
  void SetOptions();
  
  //======================
  TMVA::Reader* m_reader;//! TMVA reader.
  
  std::string m_weightsFile; /**< Weight file name. */
  std::string m_readerOptions;/**< Reader options. */
  std::vector<float> m_EventContainer;//! Container to bind the readers parameters.
};// End of classifier interface definition.

//=============== inline functions implementation. ========
//__________________________________________
inline void PndMultiClassBdtClassify::SetReaderOptions(std::string const& opt)
{
  this->m_readerOptions = opt;
};

inline std::string const& PndMultiClassBdtClassify::GetReaderOptions() const
{
  return m_readerOptions;
};

inline void PndMultiClassBdtClassify::SetWeightsFileName(std::string const& name)
{
  this->m_weightsFile = name;
};

inline std::string const& PndMultiClassBdtClassify::GetWeightsFileName() const
{
  return m_weightsFile;
};
#endif
