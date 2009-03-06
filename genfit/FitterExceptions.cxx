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

#include"FitterExceptions.h"

FitterException::FitterException(std::string _whatString, int _line, char* _file) : whatString(_whatString), line(_line), file(_file) {
}

FitterException::~FitterException() throw() {
}

void FitterException::setNumbers(std::string _numbersLabel,
								 const std::vector<double>& _numbers) {
  numbersLabel = _numbersLabel;
  numbers = _numbers;
}

void FitterException::setMatrices(std::string _matricesLabel,
								  const std::vector< TMatrixT<double> >& _matrices) {
  matricesLabel = _matricesLabel;
  matrices = _matrices;
}

const char* FitterException::what() const throw(){
  std::ostringstream returnStream;
  returnStream << "FitterException thrown with whatString:" 
			   << std::endl << whatString << std::endl 
			   << "in line: " << line << " in file: " << file;
  return returnStream.str().c_str();
}

void FitterException::info() {
  std::cout << "FitterException Info Output" << std::endl;
  std::cout << "===========================" << std::endl;
  if(numbersLabel != "") {
	std::cout << "Numbers Label String:" << std::endl;
	std::cout << numbersLabel << std::endl;
  }
  if(numbers.size() > 0) {
	std::cout << "---------------------------" << std::endl;
	std::cout << "Numbers:" << std::endl;
	for(unsigned int i=0;i<numbers.size(); i++ ) std::cout << numbers.at(i) << std::endl;
  }
  if(matricesLabel != "") {
	std::cout << "---------------------------" << std::endl;
	std::cout << "Matrices Label String:" << std::endl;
	std::cout << matricesLabel << std::endl;
  }
  if(matrices.size() > 0) {
	std::cout << "---------------------------" << std::endl;
	std::cout << "Matrices:" << std::endl;
	for(unsigned int i=0;i<matrices.size(); i++ ) matrices.at(i).Print();
  }
  std::cout << "===========================" << std::endl;  
}
