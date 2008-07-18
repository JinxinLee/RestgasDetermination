#ifndef FITTER_MATRIXPRINTER_H
#define FITTER_MATRIXPRINTER_H

#include<iostream>
#include <iomanip>
#include<string>

#include "TMatrixT.h"

inline void printMatrix(const TMatrixT<double>& theMatrix, std::string name) {
            int nCols = theMatrix.GetNcols();
            int nRows = theMatrix.GetNrows();
            std::cout << "----------------------------" << std::endl;
            std::cout << "Printing contents of matrix \""
                    << name << "\" of dimension: " << nRows
                    << " x " << nCols << std::endl;
            std::cout.precision(4);
            std::cout << std::showpoint;
            std::cout.width(5);
            for(int i=0;i<nRows;i++) {
                for(int j=0;j<nCols;j++) {
                    std::cout << theMatrix[i][j] << "\t"; 
                }
                std::cout << std::endl;
            }
            std::cout << "----------------------------" << std::endl;
        }

#endif // FITTER_MATRIXPRINTER_H
