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

      
class FitterException : public std::exception
{
 private:

  std::string whatString;
  int line;
  std::string file;

  std::string numbersLabel;
  std::string matricesLabel;
  std::vector<double> numbers;
  std::vector< TMatrixT<double> > matrices;


 public:
  FitterException(std::string, int, char*);
  virtual ~FitterException() throw();
  
  void setNumbers (std::string, const std::vector<double>&);
  void setMatrices(std::string, const std::vector< TMatrixT<double> >&);

  void info();

  virtual const char* what() const throw();
};

#endif
