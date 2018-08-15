#include <iostream>
#include <string>

#include "PndDocuSample.h"

PndDocuSample::PndDocuSample() :
 fInitialized(false) {};

PndDocuSample::PndDocuSample(std::string _s, int _i) : 
 fString(_s),
 fNumber(_i),
 fInitialized(true) {
 	PrintValues();
}

/**
 * @brief ... or in the declaration file
 * @details With the header file documentations coming first.
 */
void PndDocuSample::PrintValues() {
	std::cout << "Values are currently set to: " << std::endl;
	std::cout << "  fString: " << fString << std::endl;
	std::cout << "  fNumber: " << fNumber << std::endl;
}

/**
 * @brief Squares the current number
 * @details Squares @p fNumber
 */
void PndDocuSample::SquareNumber() {
	fNumber *= fNumber;
}

/**
 * @brief Adds the current string to itself
 */
void PndDocuSample::AddString() {
	fString += fString;
}

/** @cond CLASSIMP */
ClassImp(PndDocuSample);
/** @endcond */
