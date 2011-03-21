/* ********************************************
 * MVA Utility functions and data definitions.*
 * Author: M.Babai@rug.nl                     *
 * LICENSE:                                   *
 * Version:                                   *
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
StepError()
:m_step(0), m_trErr(0.0), m_tsErr(0.0)
  {};
  
  /**
   *@param step Currents step.
   *@param trErr Current training error.
   *@param tsErr Current test error.
   */
StepError(size_t step, float trErr, float tsErr)
: m_step(step), m_trErr(trErr), m_tsErr(tsErr)
  {};
  
  //! Destructor
  virtual ~StepError()
  {};
  
  //! Copy!
StepError(StepError const& ot)
: m_step(ot.m_step), m_trErr(ot.m_trErr), m_tsErr(ot.m_tsErr)
  {};
  
  //! Assignment.
  StepError& operator=(StepError const& ot)
  {
    this->m_step  = ot.m_step;
    this->m_trErr = ot.m_trErr;
    this->m_tsErr = ot.m_tsErr;
    return (*this);
  };
  
  size_t m_step; //Step number
  float  m_trErr;// Train Error
  float  m_tsErr;// Test Error
  
private:
  //! Operator <
  inline bool operator< (StepError const& other) const;
  //! Operator >
  inline bool operator> (StepError const& other) const;
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
PndMvaDistObj()
:m_idx(-1), m_dist(0.0), m_cls("UNKNOWN_LABEL")
  {};
  
PndMvaDistObj(int const id, float const dist, std::string const& cls)
: m_idx(id), m_dist(dist), m_cls(cls)
  {};
  
  // Destructor
  virtual ~PndMvaDistObj()
  {};
  
  //! Operator < 
  inline bool operator< (PndMvaDistObj const& other) const
  {
    return (this->m_dist < other.m_dist);
  };
  
  //! Operator > 
  inline bool operator> (PndMvaDistObj const& other) const
  {
    return (this->m_dist > other.m_dist);
  };
  
  int m_idx;/**< Index of the prototype. */
  float m_dist;/**< Distance to the current example. */
  std::string m_cls;/**< Class name of the prototype. */
};
// End class definition PndMvaDistObj

//! Less than, comparison funtion.
inline  bool CompLess(PndMvaDistObj const* a, PndMvaDistObj const* b)
{
  assert(a && b);
  return ( (*a).m_dist < (*b).m_dist );
};

// ========================================================================

/**
 * Binary Minimum function.
 */

template <typename T>
inline T const& minFunct ( T const& a, T const& b )
{
  // or: return comp(a,b)?a:b; for the comp version
  return (a < b) ? a : b;
};

//! Less than, comparison funtion.
template<typename T>
inline bool compareL(T const* l, T const* r)
{
  assert(l && r);
  return ( (*l) < (*r) );
};

/**
 * Computes the Euclidean distance between two given vectors of
 * event features.
 */
float ComputeDist(std::vector<float> const& EvtData, 
		  std::vector<float> const& Example);

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
