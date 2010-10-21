/* ********************************************
 * MVA variable class definition.             *
 * Author: M.Babai@rug.nl                     *
 * LICENSE:                                   *
 * Version: 0.1 beta1.                        *
 * License:                                   *
 * *******************************************
 */
#ifndef PND_MVA_VARIABLE_H
#define PND_MVA_VARIABLE_H

/**
 * Struct to describe a single variable of the feature vector, storing its name
 * and normalization factor.
 */
struct PndMvaVariable
{
  /**
   * Constructor.
   *@param name Variable name.
   *@param normFactor normalization factor for this variable.
   *@param mean Current variable sample mean value.
   */
  PndMvaVariable(const std::string& name = "UNKNOWN_VAR", 
		 float normFactor = 1.0, float mean = 0.0);

  //! Destructor.
  virtual ~PndMvaVariable();

  PndMvaVariable(const PndMvaVariable& oth);

  PndMvaVariable& operator=(const PndMvaVariable& oth);

  std::string Name;   /**< Name of the variable. */
  /**
   * Normalization factor of the variable.
   * Sample Variance IQR.
  */
  float   NormFactor;// Sigma 
  float   Mean;// Mean value
};

//! Constructor implementation.
inline PndMvaVariable::PndMvaVariable(std::string const &name,
				      float normFactor,
				      float mean)
: Name(name), NormFactor(normFactor), Mean(mean)
{};

//! Destructor
inline PndMvaVariable::~PndMvaVariable()
{};

//! Copy constructor
inline PndMvaVariable::PndMvaVariable(PndMvaVariable const &oth)
		      : Name(oth.Name),
		      NormFactor(oth.NormFactor),
		      Mean(oth.Mean)
{};

//! Assignment operator
inline PndMvaVariable &PndMvaVariable::operator=(PndMvaVariable const &oth)
{
  Name = oth.Name;
  NormFactor = oth.NormFactor;
  Mean = oth.Mean;
  return (*this);
}
// End of interface definition.
#endif
