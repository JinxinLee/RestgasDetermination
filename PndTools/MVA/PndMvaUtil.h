/* ********************************************
 * MVA Utility functions and data definitions.*
 * Author: M.Babai@rug.nl                     *
 * LICENSE:                                   *
 * Version: 0.1 beta1.                        *
 * License:                                   *
 * *******************************************
 */
#ifndef PND_MVA_UTIL_H
#define PND_MVA_UTIL_H

#include <typeinfo>
#include <string>
#include <vector>
#include <cmath>
#include <cassert>

// =========================================================
//! Structure to hold the per step error values.
struct StepError
{
  //! Constructor
   StepError():m_step(0), m_trErr(0.0),m_tsErr(0.0)
  {};
  /**
   *@param step Currents step.
   *@param trErr Current training error.
   *@param tsErr Current test error.
   */
   StepError(unsigned int step, float trErr, float tsErr)
   : m_step(step), m_trErr(trErr),m_tsErr(tsErr)
  {};
  
  //! Destructor
  ~StepError()
  {};

  //! Copy!
  StepError(const StepError& ot)
  : m_step(ot.m_step), m_trErr(ot.m_trErr), m_tsErr(ot.m_tsErr)
  {
    /*
      m_step  = ot.m_step;
      m_trErr = ot.m_trErr;
      m_tsErr = ot.m_tsErr;
    */
  };

  //! Assignment.
  StepError& operator=(const StepError& ot)
  {
    /*
      StepError* locTmp = new StepError();
      locTmp->m_step = ot.m_step;
      locTmp->m_trErr = ot.m_trErr;
      locTmp->m_tsErr = ot.m_tsErr;
      return *locTmp;
    */
    m_step  = ot.m_step;
    m_trErr = ot.m_trErr;
    m_tsErr = ot.m_tsErr;
    return (*this);
  };

  unsigned int m_step;
  float m_trErr;
  float m_tsErr;

private:
  //! Operator <
  inline bool operator<(const StepError& other)const;
  //! Operator >
  inline bool operator>(const StepError& other)const;
};
// ========================================================================

/**
 * Class to hold the computed Euclidean distances between the current
 * example and the available LVQ protoTypes (codeBook) in LVQ2.1
 * implementation.
 */
struct PndMvaDistObj
{
  //! Constructor
PndMvaDistObj():m_idx(-1), m_dist(0.0), m_cls("UNKNOWN")
  {};
  
PndMvaDistObj(const int id, const float dist, const std::string& cls)
: m_idx(id), m_dist(dist), m_cls(cls)
  {};
  
  // Destructor
  virtual ~PndMvaDistObj()
  {};
  
  //! Operator < 
  inline bool operator<(const PndMvaDistObj& other)const{
    return (m_dist < other.m_dist);
  };
  
  //! Operator > 
  inline bool operator>(const PndMvaDistObj& other)const{
    return (m_dist > other.m_dist);
  };
  
  int m_idx;/**< Index of the prototype. */
  float m_dist;/**< Distance to the current example. */
  std::string m_cls;/**< Class name of the prototype. */
};
// End class definition PndMvaDistObj

//! Less than, comparison funtion.
inline  bool CompLess(const PndMvaDistObj* a, const PndMvaDistObj* b)
{
  return ( (*a).m_dist < (*b).m_dist );
}

// ========================================================================

/**
 * Binary Minimum function.
 */

template <typename T>
inline const T& minFunct ( const T& a, const T& b )
{// or: return comp(a,b)?a:b; for the comp version
  return (a < b) ? a : b;
}

//! Less than, comparison funtion.
template<typename T>
inline bool compareL(const T* a, const T* b)
{ return ( (*a) < (*b) );}

/**
 * Computes the Euclidean distance between two given vectors of
 * event features.
 */
float ComputeDist(const std::vector<float>& EvtData, 
		  const std::vector<float>& Example);

// C style function declarations
#ifdef __cplusplus
extern "C"
{
#endif
  // Place your C code here.
  
#ifdef __cplusplus
}
#endif

#endif// interface definition
