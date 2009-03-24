/** @addtogroup genfit
 * @{ */

/**
 *  @author Christian H&ouml;ppner (Technische Universit&auml;t M&uuml;nchen, original author)
 *  @author Sebastian Neubert  (Technische Universit&auml;t M&uuml;nchen, original author)
 * 
 */

#ifndef FITTEREXCEPTIONS_H
#define FITTEREXCEPTIONS_H

#include <exception>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "TMatrixT.h"

/** @brief Exception class - provides storage for diagnostic informations
 *
 * This is a utility class that allows to store numbers and matrices together
 * with an error string. The exception class can then be thrown when an error
 * is detected and the C++ exception handling facilities can be used to
 * catch and process the exception.
 */
class FitterException : public std::exception
{
 private:

  std::string excString;
  int line;
  std::string file;

  std::string numbersLabel;
  std::string matricesLabel;
  std::vector<double> numbers;
  std::vector< TMatrixT<double> > matrices;


 public:
  /** @brief Initializing constructor 
   *
   * @param what error message
   * @param line line at which the exception is created. Can be set through
   * __LINE__ macro
   * @param file sorcefile in which the exception is created. 
   * Can be set through __FILE__ macro
   */
  FitterException(std::string, int, std::string);
  virtual ~FitterException() throw();
  
  /** @brief set list of numbers with description */
  void setNumbers (std::string, const std::vector<double>&);
  /** @brief set list of matrices with description */
  void setMatrices(std::string, const std::vector< TMatrixT<double> >&);

  /** @brief print information in the exception object */
  void info();

  virtual const char* what() const throw();

  std::string getExcString(){return excString;}
};

#endif

/** @} */
