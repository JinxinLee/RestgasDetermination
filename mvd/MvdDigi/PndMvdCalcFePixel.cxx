#include "PndMvdCalcFePixel.h"

std::vector<PndMvdPixel> PndMvdCalcFePixel::CalcFEHits()
{
	Int_t col, row, fe, dummy;
	for (Int_t i = 0; i < fSensorHits.size(); i++){
		col = fSensorHits[i].GetCol();
		row = fSensorHits[i].GetRow();
// 		std::cout << "col " << col << " row " << row << " ";
		fe = col / fNcols;
// 		std::cout << "fe " << fe << " ";
		col -= fe * fNcols;
		dummy = row / fNrows;
// 		std::cout << "dummy " << dummy << " ";
		fe += fMaxFEperCol * (dummy);
		row -= dummy * fNrows;
		PndMvdPixel myPixel(fSensorHits[i].GetDetName(), fe, col, row,
									  fSensorHits[i].GetCharge());
		fFeHits.push_back(myPixel);
	}
	return fFeHits;
}

std::vector<PndMvdPixel> PndMvdCalcFePixel::CalcSensorHits(const std::vector<PndMvdPixel> FePixel)				
{
	Int_t col, row, dummy;
	std::vector<PndMvdPixel> result;
	for (Int_t i = 0; i < FePixel.size(); i++){
		result.push_back(CalcSensorHit(FePixel[i]));
	}
	return result;
}

PndMvdPixel PndMvdCalcFePixel::CalcSensorHit(const PndMvdPixel FePixel)
{
  Int_t col = FePixel.GetCol();
  Int_t row = FePixel.GetRow();
  CalcSensorColRow(col,row,FePixel.GetFE());
  PndMvdPixel result(FePixel.GetDetName(), -1, col, row, FePixel.GetCharge());
  return result;
}

void PndMvdCalcFePixel::CalcSensorColRow(Int_t& col, Int_t& row, const Int_t fe) const
{
  Int_t dummy;
  dummy = fe / fMaxFEperCol;
  row = row + dummy * fNrows;
  col = col + (fe - dummy * fMaxFEperCol)*fNcols;
  return;
}
