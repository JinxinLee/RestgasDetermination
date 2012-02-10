#include "PndSdsCalcPixelDif.h"
#include <cmath>
#include "TRandom.h"


PndSdsCalcPixelDif::PndSdsCalcPixelDif()
{
  fPixelSizeX = 0;
  fPixelSizeY= 0;
  fVerboseLevel = 0;
  fQspread = 0;
}

PndSdsCalcPixelDif::PndSdsCalcPixelDif(Double_t lx, Double_t ly, Double_t qspread,Double_t threshold, Double_t noise)
{
  fPixelSizeX = lx;
  fPixelSizeY= ly;
  fVerboseLevel = 0;
  fQspread = qspread;
  fThreshold=threshold;
  fNoise=noise;
}

Int_t PndSdsCalcPixelDif::GetPixelsAlternative(Double_t inx, Double_t iny,
                                               Double_t outx, Double_t outy,
                                               Double_t energy, 
                                               std::vector<Int_t>& cols, std::vector<Int_t>& rows,
                                               std::vector<Double_t>& charges)
{
  std::vector<PndSdsPixel> pixels = GetPixels(inx,iny,outx,outy,energy);
  Int_t npix=pixels.size();
  for(Int_t i=0;i<npix;i++)
  { 
    if(fVerboseLevel>2) Info("PndSdsCalcPixelDif::GetPixelsAlternative()","pass this pixel: i=%i, c=%i, r=%i, q=%f",i,pixels[i].GetCol(),pixels[i].GetRow(),pixels[i].GetCharge());
    cols.push_back(pixels[i].GetCol());
    rows.push_back(pixels[i].GetRow());
    charges.push_back(pixels[i].GetCharge());
  }
  return npix;
}


std::vector<PndSdsPixel> PndSdsCalcPixelDif::GetPixels(Double_t inx, Double_t iny,
                                                       Double_t outx, Double_t outy,
                                                       Double_t dE)
{
  fPixels.clear();
  if(0>=fPixelSizeX || 0>=fPixelSizeY){
    Error("PndSdsCalcPixelDif::GetPixels()","Invalid Pixel sizes: fPixelSizeX=%g,fPixelSizeY=%g",fPixelSizeX,fPixelSizeY);
    return fPixels;
  }
  // Do charge diffusion integrated analytically over a path length
  // 0.5*(1+erf(x)) is the integral over a gauss from -inf to x
  // factor 0.5 is applied last, the +1 terms cancel in the difference
  // the 2 Dimensions are trated equally
  
  inx/=fPixelSizeX;
  outx/=fPixelSizeX;
  iny/=fPixelSizeY;
  outy/=fPixelSizeY;
  
  Double_t Q = ChargeFromEloss(dE);
  if(outx<inx){ // sort for direction
    Double_t tmp=inx;
    inx=outx;
    outx=tmp;
  }  
  if(outy<iny){ // sort for direction
    Double_t tmp=iny;
    iny=outy;
    outy=tmp;
  }
  
	Double_t DQx = 0., DQy = 0.;
  // transform sigma to col/row numbers
  Double_t sigma_x=fQspread/fPixelSizeX;
  Double_t sigma_y=fQspread/fPixelSizeY;
  // 2sigma shall be collected in extra bins minimum 1 bin
  Int_t xtrax = ceil(2.*sigma_x);
  Int_t xtray = ceil(2.*sigma_y);
  Double_t argu=0;
  for(Int_t i=(Int_t)inx-xtrax;i<(Int_t)outx+1+xtrax;i++)
  {
    DQx=0.;
    if(outx-inx<1e-6){ // too small path, don't integrate over path
      DQx+=TMath::Erf(i+1-0.5*(outx+inx))/(sqrt(2)*sigma_x);
      DQx-=TMath::Erf(i-0.5*(outx+inx))/(sqrt(2)*sigma_x);
    }else{      
      DQx+=CalcFk(i,outx,sigma_x);
      DQx-=CalcFk(i+1,outx,sigma_x);
      DQx-=CalcFk(i,inx,sigma_x);
      DQx+=CalcFk(i+1,inx,sigma_x);
      DQx/=(outx-inx);
    }
    for(Int_t j=(Int_t)iny-xtray;j<(Int_t)outy+1+xtray;j++)
    {
      DQy=0.;
      if(outy-iny<1e-6){ // too small path, don't integrate over path
        DQy+=TMath::Erf(j+1-0.5*(outy+iny))/(sqrt(2)*sigma_y);
        DQy-=TMath::Erf(j-0.5*(outy+iny))/(sqrt(2)*sigma_y);
      }else{  
        DQy+=CalcFk(j,outy,sigma_y);
        DQy-=CalcFk(j,iny,sigma_y);
        DQy-=CalcFk(j+1,outy,sigma_y);
        DQy+=CalcFk(j+1,iny,sigma_y);
        DQy/=(outy-iny);
      }
      InjectPixelCharge(i,j,0.25*Q*DQx*DQy);
    }      
  }
  return fPixels;  
}

//______________________________________________________________________________
Double_t PndSdsCalcPixelDif::CalcFk(Double_t k, Double_t x, Double_t sig)
{
  const Double_t t=(k-x)/(sqrt(2)*sig);
  return ( (k-x)*TMath::Erf(t) + sqrt(2/TMath::Pi())*sig*exp(-t*t) );
}

//______________________________________________________________________________
void PndSdsCalcPixelDif::InjectPixelCharge(Int_t i, Int_t j, Double_t charge)
{
  // cut if out of range
  if(i<0 || j<0) return;
  //if(i>fNrx || j>fNry) return; // TODO put max. pixel number here?
  Double_t smearedCharge = SmearCharge(charge);
  if (smearedCharge<=fThreshold) return;
  if(fVerboseLevel>3) Info("PndSdsCalcPixelDif::InjectPixelCharge","i=%i, j=%i,charge=%f",i,j,charge);
  fActivePixel.SetCol(i); // x axis
  fActivePixel.SetRow(j); // y axis
  fActivePixel.SetCharge(charge);
  fPixels.push_back(fActivePixel); // fActivePixel content will be copied
  return;
}

//______________________________________________________________________________
Double_t PndSdsCalcPixelDif::SmearCharge(Double_t charge)
{
  Double_t smeared = gRandom->Gaus(charge,fNoise);
  if (fVerboseLevel > 3) std::cout<<" charge = "<<charge<<", smeared = "<<smeared<<std::endl;
  return smeared;
}


