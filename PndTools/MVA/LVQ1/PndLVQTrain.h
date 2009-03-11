/*! **************************************
 * LVQ  Training functions               *
 * Author: M.Babai@rug.nl                *
 * LICENSE:                              *
 * Version: 1.0 beta1.                   *
 * ***************************************
 */
#pragma once
#ifndef PndLVQTrain_H
#define PndLVQTrain_H

// Standard C++ libraries
#include <iostream>
#include <algorithm>
#include <limits>
#include <vector>
#include <map>

// Root includes
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"

/**
 * Class to hold the computed Euclidean distances between the current
 * example and the available LVQ protoTypes (codeBook) in LVQ2.1
 * implementation.
*/
class PndLVQDistObj{
 public:

  //! Constructor
 PndLVQDistObj():m_idx(-1),m_dist(0.0),m_cls("UNKNOWN"){};
  
  //! Operator < implementation
  inline bool operator < (const PndLVQDistObj &other)const{
    return (m_dist < other.m_dist);
  };
  
  //! Operator > implementation
  inline bool operator > (const PndLVQDistObj &other)const{
    return (m_dist > other.m_dist);
  };
  
  int m_idx;/**< Index of the prototype. */
  float m_dist;/**< Distance to the current example. */
  std::string m_cls;/**< Class name of the prototype. */
};


class PndLVQTrain{
 public:
  /**
   * Constructor:
   * @param InPut, Input file name.
   * @param ClassNames, class names.
   * @param VarNames, variable names of the features.
   */
  PndLVQTrain(const char *InPut,
	      const std::vector<std::string>& ClassNames, 
	      const std::vector<std::string>& VarNames);
  /**
   * Destructor
   */
  virtual ~PndLVQTrain();
  
  /**
   * Train the classifier accourding to LVQ1 algorithm.
   * @param numProto, number of LVQ1 prototypes. Current
   * implementation considers an equal number of prototypes for each
   * class with equal weights.
   * @param OutPut, the nameof the out-put file, where the weights are
   * stored in.
   */
  void Train(int numProto, const char* OutPut);
  
  /**
   * Train the classifier accourding to LVQ2.1 algorithm.
   * @param numProto, number of LVQ2 prototypes. Current
   * implementation considers an equal number of prototypes for each
   * class with equal weights.
   * @param OutPut, the nameof the out-put file, where the weights are
   * stored in.
   */
  void Train21(int numProto, const char* OutPut);

  /**
   * Sets the learning parameters.
   * @param initConst, Initialization constant, 
   * used to initialize LVQ prototypes.
   * @param etZ, EthaZero, start value for the learning rate.
   * @param etF, Final value for Etha (learning rate)
   * @param Nswp, Number of sweeps through the examples collection set.
   */
  void SetLearnPrameters(const double initConst, const double etZ,
                         const double etF, const int Nswp)
  { m_initConst = initConst; m_ethaZero = etZ; 
    m_ethaFinal = etF; m_NumSweep = Nswp;};
 
  //=========== Protected functions and variables ============
 protected:
  /**
   * Computes the Euclidean distance between two given vectors of
   * event features.
   */
  float ComputeDist(std::vector<float> &EvtData, 
		    std::vector<float> &Example);

  /**
   * Updates the LVQ1 prototypes.
   */
  void UpdateProto( std::vector<float> &EvtData, std::vector<float> &proto, 
		    int delta, double ethaT);

  /**
   * Initialize LVQ prototypes (Code books).
   */
  void InitProtoTypes(int numProto);

  //============= Private functions and variables ===========
 private:
  
  template <typename T>
    const T& minFunct ( const T& a, const T& b ) const {
    return (a < b) ? a : b; // or: return comp(a,b)?a:b; for the comp version
  }
  
  /**
   * Compute the class conditional mean for a given class and store
   * that in the class conditional means container
   */
  void CompClsCondMean(std::string clsName);

  /**
   * Write the prototypes to the out-put file 
   */
  void WriteToFile(const char* outFile);

  //! Class names
  std::vector<std::string> m_ClassNames;
  
  //! Variable Names
  std::vector<std::string> m_VarNames; 
  
  //! Container to keep  the Class Conditional means
  std::vector<std::pair< std::string, std::vector<float>*> > m_ClassCondMeans;
  
  //! Container to keep  the Event data feature vectors
  std::vector< std::pair<std::string, std::vector<float>*> > m_EventsData;
  
  //! Holds the number of available examples per class
  std::map<std::string, int> m_perClsExamples;
  
  //! Holds the start and the end index of events from each class
  std::vector< std::pair<int,int> > m_ClassIndex;

  //! Holds the LVQ proto-types
  std::vector< std::pair<std::string, std::vector<float>*> > m_LVQProtos;
 
  /**
   * Learn parameters: Init constant, start learning rate, End
   * learning rate.
   */
  double m_initConst, m_ethaZero, m_ethaFinal;
  //! Number of sweeps through example set.
  int m_NumSweep;
};
#endif //end of interface definition
