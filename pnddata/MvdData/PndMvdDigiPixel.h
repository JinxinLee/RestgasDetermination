// --------------------------------------------------------
// ----			PndMvdDigiPixel header file	---
// ----			Created 19.6.06 by T.Stockmanns ---
// --------------------------------------------------------

/** PndMvdDigiPixel.h
*@author T.Stockmanns <t.stockmanns@fz-juelich.de>
** \brief Data class to store the digi output of a pixel module
**
** \sa PndMvdDigi.h
**/

#ifndef PNDMVDDIGIPIXEL_H
#define PNDMVDDIGIPIXEL_H

#include "PndMvdDigi.h"
#include "TObject.h"
#include "TString.h"
#include <iostream>
#include <vector>


class PndMvdDigiPixel : public PndMvdDigi
{
    friend std::ostream& operator<< (std::ostream& out, PndMvdDigiPixel& digi){
        out << "PndMvdDigiPixel in: " << digi.GetDetName() << " FE: "
        << digi.GetFE() << " Col/Row " << digi.GetPixelColumn() << "/" << digi.GetPixelRow()
        << " charge " << digi.GetCharge() << " e"
        << ", from Point(s) ";
        std::vector<Int_t>indices = digi.GetIndices();
        for (unsigned int i = 0; i < indices.size(); i++){
        	std::cout << indices[i] << "  ";
        }
      std::cout << std::endl;

        return out;
      }

	public : PndMvdDigiPixel();
		 PndMvdDigiPixel(std::vector<Int_t> index, Int_t detID, TString detName, Int_t fe, Int_t col, Int_t row, Double_t charge);


		~PndMvdDigiPixel(){};

    void Print() {
      std::cout << *this;
    }

		Int_t GetPixelColumn() const { return fCol; }
		Int_t GetPixelRow()    const { return fRow; }

	private :
		Int_t fCol, fRow;

	ClassDef(PndMvdDigiPixel,7);
};

#endif
