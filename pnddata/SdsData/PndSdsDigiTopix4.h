// --------------------------------------------------------
// ----			PndSdsDigiTopix4 header file	---
// ----			Created 19.6.06 by T.Stockmanns ---
// --------------------------------------------------------

/** PndSdsDigiTopix4.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 ** \brief Data class to store the digi output of a pixel module
 **
 ** \sa PndSdsDigi.h
 **/

#ifndef PndSdsDigiTopix4_H
#define PndSdsDigiTopix4_H

#include "PndSdsDigiPixel.h"
#include "PndDetectorList.h"

#include "TObject.h"
#include "TString.h"
#include <iostream>
#include <vector>


class PndSdsDigiTopix4 : public PndSdsDigiPixel
  {
    friend std::ostream& operator<< (std::ostream& out, PndSdsDigiTopix4& digi){
      out << "PndSdsDigiTopix4 in: " << digi.GetSensorID() << " FE: "
      << digi.GetFE() << " Col/Row " << digi.GetPixelColumn() << "/" << digi.GetPixelRow()
      << " charge " << digi.GetCharge() << " e"
      << " timestamp "<< digi.GetTimeStamp()
      << " leadingEdge " <<

      out << std::endl;
      
      return out;
    }
    
    public : PndSdsDigiTopix4();
    PndSdsDigiTopix4(std::vector<Int_t> index, Int_t detID, Int_t sensorID, Int_t fe, Int_t col, Int_t row, UInt_t leadingEdge, UInt_t trailingEdge, UInt_t frameCount, Double_t timeStamp);

    UInt_t GetLeadingEdge() const  { return fLeadingEdge;}
    UInt_t GetTrailingEdge() const { return fTrailingEdge;}
    UInt_t GetFrameCount() const   { return fFrameCount;}
    
    void SetLeadingEdge(UInt_t le)  { fLeadingEdge = le;}
    void SetTrailingEdge(UInt_t te) { fTrailingEdge = te;}
    void SetFrameCount(UInt_t fc)   { fFrameCount = fc;}

		~PndSdsDigiTopix4(){};
    
    void Print() {
      std::cout << *this;
    }
    

	PndSdsDigiTopix4& operator=(const PndSdsDigiTopix4& pix){
	        if (this != &pix){
	        	this->PndSdsDigiPixel::operator=(pix);
	        }
		return *this;
	}
    
    private :
	UInt_t fLeadingEdge;
	UInt_t fTrailingEdge;
	UInt_t fFrameCount;
    
    ClassDef(PndSdsDigiTopix4,2);
  };

#endif
