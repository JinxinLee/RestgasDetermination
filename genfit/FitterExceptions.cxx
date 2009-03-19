#include"FitterExceptions.h"

FitterException::FitterException(std::string _whatString, int _line, std::string  _file) : whatString(_whatString), line(_line), file(_file) {
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
