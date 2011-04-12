#include "PndSdsCalcPixel.h"
#include <cmath>

PndSdsCalcPixel::PndSdsCalcPixel()
{
  fCperL = -1;
  fQuad = kQuadUNDEF;
  fNextPixel = kPixelUNDEF;
  fVerboseLevel = 0;
}

PndSdsCalcPixel::PndSdsCalcPixel(Double_t w, Double_t l, Double_t threshold, Double_t noise)
{
  fPixelWidth = w;
  fPixelLength= l;
  fThreshold = threshold;
  fNoise = noise;
  fVerboseLevel = 0;
}


std::vector<PndSdsPixel> PndSdsCalcPixel::GetPixels(Double_t inx, Double_t iny, Double_t inz,
                Double_t outx, Double_t outy, Double_t outz,
                Double_t energy)
{
  fIn.setXYZ(inx, iny, 0);
  fOut.setXYZ(outx, outy, 0);

  fDir = fOut - fIn;
  fPos = fIn;
  if (fDir.length() < 0.001){   //1 m
    CalcStartPixel();
    fActivePixel.SetCharge(energy/(3.61e-9));
    fPixels.push_back(fActivePixel);
    return fPixels;
  }
//  CalcConMatrix(); //fCon berechnen
//  ApplyConMatrix(); //fCon anwenden
  if (fVerboseLevel > 1){
    std::cout << "Converted Vectors: " << std::endl;
    std::cout << fIn << fOut << fDir << fPos << std::endl;
  }
  CalcQuadrant();
  CalcCperL(energy);
  
  CalcStartPixel();

  fStop = false;
  while (fStop != true){
    CalcPixel();
    Int_t col = fActivePixel.GetCol();
    Int_t row = fActivePixel.GetRow();
    switch (fNextPixel) {
      case kU : fActivePixel.SetRow(++row); break;
      case kD : fActivePixel.SetRow(--row); break;
      case kL : fActivePixel.SetCol(--col); break;
      case kR : fActivePixel.SetCol(++col); break;
      case kPixelUNDEF : std::cout << "PndSdsCalcPixel::GetPixels no next pixel."<<std::endl; break;
    }
    fActivePixel.SetCharge(-1.0);
  }

//  ConvertPixels(); //fCon rueckgaengig
  return fPixels;
}
void PndSdsCalcPixel::CalcConMatrix()
{
  if (fIn.getX() < 0)
    fCon.setX(-1);
  else fCon.setX(1);
  if (fIn.getY() < 0)
    fCon.setY(-1);
  else fCon.setY(1);
}

void PndSdsCalcPixel::ApplyConMatrix()
{
  fIn.setX(fCon.getX() * fIn.getX());
  fIn.setY(fCon.getY() * fIn.getY());
  
  fOut.setX(fCon.getX() * fOut.getX());
  fOut.setY(fCon.getY() * fOut.getY());
  
  fPos.setX(fCon.getX() * fPos.getX());
  fPos.setY(fCon.getY() * fPos.getY());
  
  fDir.setX(fCon.getX() * fDir.getX());
  fDir.setY(fCon.getY() * fDir.getY());
    
}

void PndSdsCalcPixel::CalcQuadrant()
{
  if (fDir.getX() > 0)
    if (fDir.getY() > 0)
      fQuad = kUR;
    else
      fQuad = kDR;
  else if (fDir.getY() > 0)
    fQuad = kUL;
  else
    fQuad = kDL;

  if (fVerboseLevel > 1){
    std::cout << "CalcQuadrant: " << fQuad << std::endl;
  }
}

void PndSdsCalcPixel::CalcCperL(Double_t Energy)
{
  Double_t Charge = Energy / (3.61e-9);
  fCperL = Charge / fDir.length();  
}

void PndSdsCalcPixel::CalcStartPixel ()
{
  Int_t col = Int_t(fIn.getX() / fPixelLength);
  Int_t row = Int_t(fIn.getY() / fPixelWidth);
  fActivePixel.SetCol(col);
  fActivePixel.SetRow(row);
}

void PndSdsCalcPixel::CalcPixel()
{
  Double_t borderX = 0;
  Double_t borderY = 0;
  FairGeoVector OutPoint;
  bool xBeforeY = false;

/*  if ( fabs(fDir.getX()) < 1e-8 ) {
  xBeforeY = false;
    borderY = fPixelWidth * (fActivePixel.GetRow()+1);
  }
  else  if ( fabs(fDir.getY()) < 1e-8 ) {
  xBeforeY = true;
    borderX = fPixelWidth
  }
  else  switch (fQuad){
*/
switch (fQuad){
    case kUR : borderX = fPixelLength * (fActivePixel.GetCol()+1);
        borderY = fPixelWidth  * (fActivePixel.GetRow()+1);
        if ( fabs(fDir.getX()) < 1e-8 ) xBeforeY = false;
        else if ( ( (borderX - fPos.getX()) * fDir.getY() / fDir.getX() )+fPos.getY() < borderY)
          xBeforeY = true;
        else xBeforeY = false;
        break;
    case kUL : borderX = fPixelLength * (fActivePixel.GetCol());
        borderY = fPixelWidth  * (fActivePixel.GetRow()+1);
        if ( fabs(fDir.getX()) < 1e-8 ) xBeforeY = false;
        else if ( ( (borderX - fPos.getX()) * fDir.getY() / fDir.getX() )+fPos.getY() < borderY)
          xBeforeY = true;
        else xBeforeY = false;
        break;
    case kDL : borderX = fPixelLength * (fActivePixel.GetCol());
        borderY = fPixelWidth  * (fActivePixel.GetRow());
        if ( fabs(fDir.getX()) < 1e-8 ) xBeforeY = false;
        else if ( ( (borderX - fPos.getX()) * fDir.getY() / fDir.getX() )+fPos.getY() > borderY)
          xBeforeY = true;
        else xBeforeY = false;
        break;
    case kDR : borderX = fPixelLength * (fActivePixel.GetCol()+1);
        borderY = fPixelWidth  * (fActivePixel.GetRow());
        if ( fabs(fDir.getX()) < 1e-8 ) xBeforeY = false;
        else if ( ( (borderX - fPos.getX()) * fDir.getY() / fDir.getX() )+fPos.getY() > borderY)
          xBeforeY = true;
        else xBeforeY = false;
        break;
    case kQuadUNDEF : std::cout<<"PndSdsCalcPixel::CalcPixel : Quadrant not defined!"<<std::endl;
    //default : ErrMsg(fatal) << " Quadrant not defined! " << endmsg;
  }
  
  if ( xBeforeY ) { //Is borderX reached berfore borderY
    OutPoint.setXYZ(borderX, ((borderX-fPos.getX())*fDir.getY()/fDir.getX())+fPos.getY(),0);
    
    if (fQuad == kUR || fQuad == kDR)
      fNextPixel = kR;
    else fNextPixel = kL;
  }
  else {
    OutPoint.setXYZ(((borderY-fPos.getY())*fDir.getX()/fDir.getY())+fPos.getX(), borderY,0);
    
    if (fQuad == kUR || fQuad == kUL)
      fNextPixel = kU;
    else fNextPixel = kD;
  } 
//  ErrMsg(warning) << " OutPoint: " << OutPoint << endmsg;
  
      
  if ((OutPoint - fPos).length() > (fOut - fPos).length()){
    fStop = true;
    OutPoint = fOut;
  }
    

  Double_t depCharge = (OutPoint - fPos).length() * fCperL;
  //if (fVerboseLevel > 1){
    //std::cout << "depCharge w/o noise: " << depCharge << std::endl;
  //}
  //TRandom* rgen = gRandom;//new TRandom3();
  //Double_t addNoise = rgen->Gaus(0,fNoise);
  //depCharge += addNoise;
  
//  //if (fVerboseLevel > 1){
    //std::cout << "depCharge w noise " << fNoise << ": " << depCharge << " " << addNoise << std::endl;
  //}

  fPos = OutPoint;
  
  //fActivePixel.SetAddNoise(addNoise);

  //if (depCharge > fThreshold){
    fActivePixel.SetCharge(depCharge);
  //}
  //else
  //  fActivePixel.SetCharge(0);

  fPixels.push_back(fActivePixel);
  
  if (fVerboseLevel > 1){
    std::cout << fActivePixel << std::endl;
   // std::cout << fActivePixel.GetAddNoise() << std::endl;
  }
      
}

void PndSdsCalcPixel::ConvertPixels()
{
  Int_t col, row;
  for (UInt_t i = 0; i < fPixels.size(); i++){
    col = fPixels[i].GetCol();
    row = fPixels[i].GetRow();
    fPixels[i].SetCol(Int_t(col * fCon.getX()));
    fPixels[i].SetRow(Int_t(row * fCon.getY()));
  }
    
}

std::ostream& PndSdsCalcPixel::operator<<(std::ostream& out)
{
  out << "PixelWidth: " << fPixelWidth << " PixelLength: " <<
    fPixelLength << " Threshold: " << fThreshold << std::endl;
  
  return out;
}
