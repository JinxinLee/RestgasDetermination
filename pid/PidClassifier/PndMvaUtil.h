/* ********************************************
 * MVA Utility functions and data definitions *
 * Author: M.Babai@rug.nl                     *
 * LICENSE:                                   *
 * Version: 0.1 beta1.                        *
 * License:                                   *
 * *******************************************
 */
#pragma once
#ifndef PND_MVA_UTIL_H
#define PND_MVA_UTIL_H

//#include <typeinfo>
#include <vector>
#include <cmath>
#include <cassert>

#include "PndMvaDataSet.h"

/**
 * Binary Minimum function.
 */

template <typename T>
inline const T& minFunct ( const T& a, const T& b )
{
  // or: return comp(a,b)?a:b; for the comp version
  return (a < b) ? a : b;
}

template<typename T>
inline bool compare(const T* a, const T* b)
{ return ( (*a) < (*b) );}


inline  bool CompLess(const PndMvaDistObj* a, const PndMvaDistObj* b)
{
  return ( (*a).m_dist < (*b).m_dist );
}

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
