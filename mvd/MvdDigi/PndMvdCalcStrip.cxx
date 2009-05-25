//
// C++ Implementation: PndMvdCalcStrip
//
// Description: 
//
//
// Author: HG Zaunick <hg.zaunick@physik.tu-dresden.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <cmath>

#include "PndMvdCalcStrip.h"
#include "TRandom3.h"

PndMvdCalcStrip::PndMvdCalcStrip(){
  fPitch = 0.;
  fOrient = 0.;
  fAnchor = TVector2(0.,0.);
  fNrStrips = 0;
  fThreshold = 0.;
  fNoise = 0.;
  fVerboseLevel = 0;
  fRNG = new TRandom3();
}

PndMvdCalcStrip::PndMvdCalcStrip(Double_t pitch, Double_t orient,
             Int_t nrStrips, Int_t nrFeChannels,
             const TVector2& firstStripAnchor,
             Double_t threshold, Double_t noise)
   : fPitch(pitch), fOrient(orient),
     fNrStrips(nrStrips), fNrFeChannels(nrFeChannels),
     fAnchor(firstStripAnchor),
     fThreshold(threshold), fNoise(noise)
{
      fStripDir.Set(cos(fOrient),sin(fOrient));
      fOrthoDir.Set(sin(fOrient),-cos(fOrient));
      fVerboseLevel = 0;
  fRNG = new TRandom3();
  //Print();
}
PndMvdCalcStrip::PndMvdCalcStrip(const PndMvdStripDigiPar* digipar, SensorSide side)
{
  if(side == kTOP)
  {
    fPitch = digipar->GetTopPitch();
    fOrient = digipar->GetOrient();
    fAnchor = digipar->GetTopAnchor();
    fNrStrips = digipar->GetNrTopFE()*digipar->GetNrFECh();
  }
  else if(side == kBOTTOM)
  {
    fPitch = digipar->GetBotPitch();
    fOrient = digipar->GetOrient() + digipar->GetSkew();
    fAnchor = digipar->GetBotAnchor();
    fNrStrips = digipar->GetNrBotFE()*digipar->GetNrFECh();
  }
  fNrFeChannels = digipar->GetNrFECh();
  fThreshold = digipar->GetThreshold();
  fNoise = digipar->GetNoise();

  fStripDir.Set(cos(fOrient),sin(fOrient));
  fOrthoDir.Set(sin(fOrient),-cos(fOrient));
  fVerboseLevel = 0;
  fRNG = new TRandom3();
  if (fVerboseLevel > 0) Print();
}


std::vector<PndMvdStrip>
PndMvdCalcStrip::GetStrips(Double_t inx, Double_t iny, Double_t inz,
                        Double_t outx, Double_t outy, Double_t outz,
                        Double_t eLoss)
{
    if (fVerboseLevel > 2) std::cout<<"-I- PndMvdCalcStrip::GetStrips "<<std::endl;
    
    // 2d-Projection of trajectory
    TVector2 in(inx,iny);
    TVector2 out(outx,outy);
    TVector2 path=out-in;
    
    if (fVerboseLevel > 2){
      std::cout<<" InPoint: ("<<in.X()<<","<<in.Y()<<")"<<std::endl;
      std::cout<<" OutPoint: ("<<out.X()<<","<<out.Y()<<")"<<std::endl;
    }
    
    std::vector<PndMvdStrip> strips;
    Double_t smearedQ;
        
    if (path.Mod()<1E-18) {
      std::cout<<"-W- PndMvdCalcStrip::GetStrips : No Trajectory inside Sensor! (out-in).Mod() = "<<path.Mod()<<std::endl;
      return strips;
    }    
    
    if (fVerboseLevel > 1) std::cout<<" pathlength: "<<path.Mod()<<std::endl;
    
    Double_t nuIn  = CalcStripFromPoint(inx,iny);
    Double_t nuOut = CalcStripFromPoint(outx,outy);
    
    if (fVerboseLevel > 2) std::cout<<" nuIn = "<<nuIn<<" ; nuOut = "<<nuOut<<std::endl;
    
    
    Double_t Q = ChargeFromEloss(eLoss);//*1E9/3.61; // 3.6 eV/Electron in Silicon
    if (fVerboseLevel > 1) std::cout<<" integral charge = "<<Q<<std::endl;
    
    // did we hit the active area ?
//     if ( (nuIn<0.5 && nuOut<0.5) || (((nuIn+0.5) > Double_t(fNrStrips-1)) && ((nuOut+0.5) > Double_t(fNrStrips-1)))){
    if ( (nuIn<0. && nuOut<0.) || ((nuIn > Double_t(fNrStrips)) && (nuOut > Double_t(fNrStrips))) )
    {
      if (fVerboseLevel > 1) std::cout<<"-W- PndMvdCalcStrip::GetStrips: Hit outside active area."<<std::endl;
      return strips;
    }
    
    // is the In-Point inside active area ? 
    // only charge fraction inside active area taken
    if (nuIn<0.){
      Q *= (nuOut)/(nuOut-nuIn);
      nuIn = 0.;
    } else if (nuIn > (Double_t(fNrStrips))){
      Q *= ((Double_t)fNrStrips-nuOut)/(-nuOut+nuIn);
      nuIn = Double_t(fNrStrips);
    }
    
    // is the Out-Point inside active area ?
    if (nuOut<0.){
      Q *= (nuIn)/(-nuOut+nuIn);
      nuOut = 0.;
    } else if (nuOut > (Double_t(fNrStrips))){
      Q *= ((Double_t)fNrStrips-nuIn)/(nuOut-nuIn);
      nuOut = Double_t(fNrStrips);
    }
    
    // only one strip hit ?
    if (Int_t(nuIn) == Int_t(nuOut)){
      // this strip collected the entire charge
      smearedQ = SmearCharge(Q);
      if (smearedQ >= fThreshold)
          strips.push_back(PndMvdStrip(Int_t(nuOut),smearedQ));
      if (fVerboseLevel > 1) std::cout<<" -> 1 strip hit."<<std::endl;
    } else {
      Double_t dQ=Q/std::fabs(nuOut-nuIn);
      Double_t dir = (nuOut>nuIn) ? 1. : -1.;
      Int_t nrHits = 0;
      // calculate portion of track in first strip
      Int_t nextIn = Int_t(nuIn + 0.5+0.5*dir);
      Double_t Q1 = dQ*std::fabs(nextIn-nuIn);
      if (fVerboseLevel > 2){
        std::cout<<" part of first strip : "<<nextIn-nuIn<<std::endl ;
        std::cout<<" charge : "<<Q1<<std::endl ;
        std::cout<<" next strip : "<<nextIn<<std::endl ;
      }              
      smearedQ = SmearCharge(Q1);
      if (smearedQ >= fThreshold) strips.push_back(PndMvdStrip(Int_t(nuIn),smearedQ));
      nrHits++;
      Q -= Q1;
        
      // calculate portion of track in last strip
      Int_t prevOut = Int_t(nuOut + 0.5-0.5*dir);
      Double_t Q2 = dQ*std::fabs(nuOut-prevOut);
      if (fVerboseLevel > 2){
        std::cout<<" part of last strip : "<<(nuOut-prevOut)<<std::endl ;
        std::cout<<" charge : "<<Q2<<std::endl ;
        std::cout<<" end of previous strip : "<<prevOut<<std::endl ;
      }  
      smearedQ = SmearCharge(Q2);
      if (smearedQ >= fThreshold) strips.push_back(PndMvdStrip(Int_t(nuOut),smearedQ));
      nrHits++;
      Q -= Q2;
        

        // Distribute the charge among the intermediate strips
        nextIn = Int_t(nextIn - 0.5 + 0.5*dir);
        prevOut = Int_t(prevOut - 0.5 + 0.5*dir);
        if (fVerboseLevel > 2) {
          std::cout<<" dir="<<Int_t(dir)<<std::endl;
          std::cout<<" begin="<<nextIn<<" end="<<prevOut<<std::endl;
        }

        for (Int_t n = nextIn ; n != prevOut; n += Int_t(dir) )
        {
          if (fVerboseLevel > 2) std::cout<<" n = "<<n<<std::endl;
          smearedQ = SmearCharge(dQ);
          if (smearedQ >= fThreshold) strips.push_back(PndMvdStrip(n,smearedQ));
          nrHits++;
          Q -= dQ;
        }
      if (fVerboseLevel > 2) if (fabs(Q)>1.) std::cout<<" charge Q = "<<Q<<" not detected!"<<std::endl;
      if (fVerboseLevel > 1) std::cout<<" -> "<<nrHits<<" strips hit."<<std::endl;
    }

    return strips;
}

Double_t PndMvdCalcStrip::SmearCharge(Double_t charge)
{
    Double_t smeared = fRNG->Gaus(charge,fNoise);
    if (fVerboseLevel > 3) std::cout<<" charge = "<<charge<<", smeared = "<<smeared<<std::endl;
    return smeared;
}

Double_t PndMvdCalcStrip::CalcStripFromPoint(Double_t x, Double_t y)
{
  // fOrthoDir is already set to magnitude == 1., makes it cheaper here.
  return ((x-fAnchor.X())*fOrthoDir.X() + (y-fAnchor.Y())*fOrthoDir.Y())/fPitch;
}

void PndMvdCalcStrip::CalcStripPointOnLine(Double_t strip,TVector2& point) const
{
  point = fPitch*(strip+0.5)*fOrthoDir + fAnchor;
}

Int_t PndMvdCalcStrip::CalcFEfromStrip(Int_t stripNr) const { return (stripNr/fNrFeChannels); }

Int_t PndMvdCalcStrip::CalcChannelfromStrip(Int_t stripNr) const { return (stripNr%fNrFeChannels); }

void PndMvdCalcStrip::CalcFeChToStrip(Int_t fe, Int_t channel, Int_t& strip, enum SensorSide& side) const
{
    //Caution! The top side s always the reference side!
    Int_t nr = fe * fNrFeChannels + channel;
    if (nr < fNrStrips) {
        side = kTOP;
    } else {
        nr -= fNrStrips;
        side = kBOTTOM;
    }
    strip = nr;
}




void PndMvdCalcStrip::Print() const
{
    std::cout<<"-I- PndMvdCalcStrip Info :"<<std::endl;
    std::cout<<"     pitch                  = "<<fPitch*10000.<<" um"<<std::endl;
    std::cout<<"     orientation angle      = "<<fOrient/TMath::Pi()*180.<<" deg"<<std::endl;
    std::cout<<"     nr of strips           = "<<fNrStrips<<std::endl;
    std::cout<<"     nr of channels per FE  = "<<fNrFeChannels<<std::endl;
    std::cout<<"     nr of frontends        = "<<fNrStrips/fNrFeChannels<<std::endl;
    std::cout<<"     anchor point           = ("<<fAnchor.X()<<","<<fAnchor.Y()<<")"<<std::endl;
    std::cout<<"     strip-direction vector = ("<<fStripDir.X()<<","<<fStripDir.Y()<<")"<<std::endl;
}





