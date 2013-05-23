// --------------------------------------------------------
// ----			PndDrcDigi header file	---
// ----			Created 19.6.06 by T.Stockmanns ---
// --------------------------------------------------------

/** PndDrcDigi.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 ** \brief Data class to store the digi output of a pixel module
 **
 ** \sa PndDrcDigi.h
 **/

#ifndef PNDDRCDIGI_H
#define PNDDRCDIGI_H

#include "PndDrcDigi.h"
#include "PndDetectorList.h"
#include "FairTimeStamp.h"
#include "TObject.h"
#include "TString.h"
#include <iostream>
#include <vector>


class PndDrcDigi : public FairTimeStamp
  {
    friend std::ostream& operator<< (std::ostream& out, PndDrcDigi& digi){
      out << "PndDrcDigi in: " << digi.GetSensorID()
      << " charge " << digi.GetCharge() << " e"
      << " timestamp "<< digi.GetTimeStamp()
      << ", from Point(s) ";
      std::vector<Int_t>indices = digi.GetIndices();
      for (unsigned int i = 0; i < indices.size(); i++){
        out << indices[i] << "  ";
      }
      out << std::endl;
      
      return out;
    }
    
    public : PndDrcDigi();
    PndDrcDigi(std::vector<Int_t> index, Int_t sensorID, Double_t charge, Double_t timeStamp);
    PndDrcDigi(Int_t index, Int_t sensorID, Double_t charge, Double_t timeStamp);
    
		~PndDrcDigi(){};
    
    void Print() {
      std::cout << *this;
    }
    		
	Int_t GetSensorID() const { return fSensorID; }
	Double_t GetCharge()	 const { return fCharge; }	
	std::vector<Int_t> GetIndices() const { return fIndex;}
	Int_t GetIndex(int i = 0) const{ return fIndex[i];}
	Int_t GetNIndices() const { return fIndex.size();}
	
	virtual void AddIndex(int index)
		{
			fIndex.push_back(index);
			AddLink(FairLink(fSensorID, index));
		}
    
	virtual void AddIndex(std::vector<Int_t> index)
		{
			fIndex = index;
			AddLinks(FairMultiLinkedData(fSensorID, index));
		}	
		  
    
    protected:
    		std::vector<Int_t> fIndex;   // indice of mc points contributing to this digi
		Int_t fSensorID;             // Geometry ID for sensor volume		
		Double_t fCharge;            // collected charge
    
    ClassDef(PndDrcDigi,1);
  };

#endif
