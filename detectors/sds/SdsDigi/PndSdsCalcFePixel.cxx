#include "PndSdsCalcFePixel.h"

std::vector<PndSdsPixel> PndSdsCalcFePixel::CalcFEHits()
{
	Int_t col, row, fe, dummy;
	for (UInt_t i = 0; i < fSensorHits.size(); i++){
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
		PndSdsPixel myPixel(fSensorHits[i].GetSensorID(), fe, col, row,
									  fSensorHits[i].GetCharge());
		myPixel.SetAddNoise(fSensorHits[i].GetAddNoise());
		fFeHits.push_back(myPixel);
	}
	return fFeHits;
}

std::vector<PndSdsPixel> PndSdsCalcFePixel::CalcSensorHits(const std::vector<PndSdsPixel> FePixel)
{
	//Int_t col, row, dummy;
	std::vector<PndSdsPixel> result;
	for (UInt_t i = 0; i < FePixel.size(); i++){
		result.push_back(CalcSensorHit(FePixel[i]));
	}
	return result;
}

PndSdsPixel PndSdsCalcFePixel::CalcSensorHit(const PndSdsPixel FePixel) const
{
  Int_t col = FePixel.GetCol();
  Int_t row = FePixel.GetRow();
  CalcSensorColRow(col,row,FePixel.GetFE());
  PndSdsPixel result(FePixel.GetSensorID(), -1, col, row, FePixel.GetCharge());
  return result;
}

void PndSdsCalcFePixel::CalcSensorColRow(Int_t& col, Int_t& row, const Int_t fe) const
{
  Int_t dummy;
  dummy = fe / fMaxFEperCol;
  row = row + dummy * fNrows;
  col = col + (fe - dummy * fMaxFEperCol)*fNcols;
  return;
}
