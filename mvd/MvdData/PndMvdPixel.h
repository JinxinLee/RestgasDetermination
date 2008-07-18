#ifndef PndMvdPixel_HH
#define PndMvdPixel_HH

#include <iostream>
#include <string>
#include <vector>
#include "Rtypes.h"

class PndMvdPixel
{
	public :  PndMvdPixel(){
			fCol = -1;
			fRow = -1;
			fCharge = -1.0;
			fFe = -1;
		  };
		  
		  PndMvdPixel(std::string detName, Int_t fe, Int_t col, Int_t row, Double_t charge, int index=-1){
		  	fDetName = detName;
			fFe = fe;
			fCol = col;
			fRow = row;
			fCharge = charge;
			if (index >= 0)
				fMCIndex.push_back(index);
		  };
		  
		  void SetCol(Int_t col) {fCol = col;};
		  void SetRow(Int_t row) {fRow = row;};
		  void SetCharge(Double_t charge) {fCharge = charge;};
		  void SetDetName(std::string detName) {fDetName = detName;};
		  void SetFE (Int_t fe) {fFe = fe;};
		  
		  Int_t GetCol() const {return fCol;};
		  Int_t GetRow() const {return fRow;};
		  Double_t GetCharge() const {return fCharge;};
		  std::string GetDetName() const {return fDetName;};
		  Int_t GetFE() const {return fFe;};
		  std::vector<int> GetMCIndex(){return fMCIndex;};
		  int GetFirstMCIndex(){return fMCIndex[0];};
		  
		  void AddCharge(Double_t charge){fCharge+= charge;};
		  void AddMCIndex(int i){fMCIndex.push_back(i);};
		  
		  friend std::ostream& operator<< (std::ostream& out, PndMvdPixel pixel)
		  {
			out << "Detector: " << pixel.GetDetName() << " FE: " << pixel.GetFE() << " Pixel (C/R): " << pixel.GetCol() << " " << pixel.GetRow()
			    << " Charge: " << pixel.GetCharge() << " from MCHit: " << pixel.GetFirstMCIndex();
			return out;
		  };	

	
	private : 
		  Int_t fFe;
		  Int_t fCol;
		  Int_t fRow;
		  Double_t fCharge;
		  std::string fDetName;
		  std::vector<int> fMCIndex;		///< List of indices of the corresponding MC hits
};


#endif
