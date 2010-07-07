// --------------------------------------------------------
// ----			PndSdsDigiPixel header file	---
// ----			Created 19.6.06 by T.Stockmanns ---
// --------------------------------------------------------

/** PndSdsDigiPixel.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 ** \brief Data class to store the digi output of a pixel module
 **
 ** \sa PndSdsDigi.h
 **/

#ifndef PNDSDSDIGIPIXEL_H
#define PNDSDSDIGIPIXEL_H

#include "PndSdsDigi.h"
#include "PndDetectorList.h"

#include "TObject.h"
#include "TString.h"
#include <iostream>
#include <vector>


class PndSdsDigiPixel : public PndSdsDigi
  {
    friend std::ostream& operator<< (std::ostream& out, PndSdsDigiPixel& digi){
      out << "PndSdsDigiPixel in: " << digi.GetSensorID() << " FE: "
      << digi.GetFE() << " Col/Row " << digi.GetPixelColumn() << "/" << digi.GetPixelRow()
      << " charge " << digi.GetCharge() << " e"
      << " timestamp "<< digi.GetTimestamp()
      << ", from Point(s) ";
      std::vector<Int_t>indices = digi.GetIndices();
      for (unsigned int i = 0; i < indices.size(); i++){
        std::cout << indices[i] << "  ";
      }
      std::cout << std::endl;
      
      return out;
    }
    
    public : PndSdsDigiPixel();
    PndSdsDigiPixel(std::vector<Int_t> index, Int_t detID, Int_t sensorID, Int_t fe, Int_t col, Int_t row, Double_t charge, Int_t mcPointType);
    
    
		~PndSdsDigiPixel(){};
    
    void Print() {
      std::cout << *this;
    }
    
		Int_t GetPixelColumn() const { return fCol; }
		Int_t GetPixelRow()    const { return fRow; }
    
    private :
		Int_t fCol, fRow;
    
    ClassDef(PndSdsDigiPixel,8);
  };

#endif
