#include "PndSdsCalcPixelDif.h"
#include <cmath>

PndSdsCalcPixelDif::PndSdsCalcPixelDif()
{
  fPixelSizeX = 0;
  fPixelSizeY= 0;
  fThreshold = 0;
  fNoise = 0;
  fVerboseLevel = 0;
  fQspread = 0;
}

PndSdsCalcPixelDif::PndSdsCalcPixelDif(Double_t lx, Double_t ly, Double_t threshold, Double_t noise, Double_t qspread)
{
  fPixelSizeX = lx;
  fPixelSizeY= ly;
  fThreshold = threshold;
  fNoise = noise;
  fVerboseLevel = 0;
  fQspread = qspread;
}


std::vector<PndSdsPixel> PndSdsCalcPixelDif::GetPixels(Double_t inx, Double_t iny,
                                                       Double_t outx, Double_t outy,
                                                       Double_t dE)
{
  // Do charge diffusion integrated analytically over a path length
  // 0.5*(1+erf(x)) is the integral over a gauss from -inf to x
  // factor 0.5 is applied last, the +1 terms cancel in the difference
  // the 2 Dimensions are trated equally
  
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
  
  std::vector<PndSdsPixel> array;
  
	Double_t DQx = 0., DQy = 0.;
  // transform sigma to col/row numbers
  Double_t sigma_x=fQspread/fPixelSizeX;
  Double_t sigma_y=fQspread/fPixelSizeY;
  // 2sigma shall be collected in extra bins minimum 1
  Int_t xtrax = ceil(2.*sigma_x);
  Int_t xtray = ceil(2.*sigma_y);
  //if(fabs(pathstart-pathend) < 1e-10) { // too small path, don't integrate over path
  ////std::cout<<"DfRalf - 0"<<std::endl;
  //pathstart=0.5*(pathstart+pathend);
  //for(Int_t i=(Int_t)pathstart-xtra;i<(Int_t)pathstart+1+xtra;i++)
  //{
  //DQ=0;
  //DQ+=TMath::Erf( (i+1-pathstart)/(sqrt(2)*sigma_str) );
  //DQ-=TMath::Erf( (i-pathstart)/(sqrt(2)*sigma_str) );
  //DQ*=0.5*Q;
  //InjectStripCharge(array,i,DQ);
  //}
  //} else {
  // now the general case with a "long" tracklet
  Double_t DQ = 0.25*Q/((outx-inx)*(outy-iny));
  for(Int_t i=(Int_t)inx-xtrax;i<(Int_t)outx+1+xtrax;i++)
  {
    DQx=0;
    DQx+=CalcFk(i,outx,sigma_x);
    DQx-=CalcFk(i,inx,sigma_x);
    DQx-=CalcFk(i+1,outx,sigma_x);
    DQx+=CalcFk(i+1,inx,sigma_x);
    
    for(Int_t j=(Int_t)iny-xtray;j<(Int_t)outy+1+xtray;j++)
    {
      DQy=0;
      DQy+=CalcFk(j,outy,sigma_y);
      DQy-=CalcFk(j,iny,sigma_y);
      DQy-=CalcFk(j+1,outy,sigma_y);
      DQy+=CalcFk(j+1,iny,sigma_y);
      InjectPixelCharge(array,i,j,DQ*DQx*DQy);
    }      
  }
  //  }
  return array;  
}

//______________________________________________________________________________
Double_t PndSdsCalcPixelDif::CalcFk(Double_t k, Double_t x, Double_t sig)
{
  const Double_t t=(k-x)/(sqrt(2)*sig);
  return ( (k-x)*TMath::Erf(t) + sqrt(2/TMath::Pi())*sig*exp(-t*t) );
}

//______________________________________________________________________________
void PndSdsCalcPixelDif::InjectPixelCharge(std::vector<PndSdsPixel>& array, Int_t i, Int_t j, Double_t charge)
{
  // cut if out of range
  if(i<0 || j<0) return;
  //if(i>fNrx || j>fNry) return; // TODO put max. pixel number here?
  //Double_t smearedQ = SmearCharge(charge);
  //if(smearedQ < fThreshold) return;
  if(fVerboseLevel>3) Info("InjectPixelCharge","i=%i, j=%i,charge=%f",i,j,charge);
  fActivePixel.SetCol(i); // x axis
  fActivePixel.SetRow(j); // y axis
  fActivePixel.SetCharge(charge);
  array.push_back(fActivePixel); // fActivePixel content will be copied
  return;
}


