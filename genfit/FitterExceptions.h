/*  Copyright © 2008, Christian Hoeppner & Sebastian Neubert
    All rights reserved.

    This file is part of Genfit.

    Genfit is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Genfit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser Public License for more details.

    You should have received a copy of the GNU Lesser Public License
    along with Genfit.  If not, see <http://www.gnu.org/licenses/>. */

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

#endif //FITTEREXCEPTIONS_H
