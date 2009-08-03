#pragma once
#ifndef PND_MVA_VARIABLE_H
#define PND_MVA_VARIABLE_H

/**
 * Struct to describe a single variable of the feature vector, storing its name
 * and normalization factor.
 */
struct PndMvaVariable
{
  //! Constructor.
  PndMvaVariable(const std::string& name, float normFactor = 1.0, float mean = 0.0);

  std::string Name;   /**< Name of the variable. */
  /**
   * Normalization factor of the variable.
   * Sample Variance IQR.
  */
  float   NormFactor; 
  float   Mean;
};

inline PndMvaVariable::PndMvaVariable(const std::string& name,
				      float normFactor, float mean)
		      :
		      Name(name),
		      NormFactor(normFactor),
		      Mean(mean)
{}

#endif
