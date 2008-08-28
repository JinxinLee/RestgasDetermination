//
// C++ Interface: PndMvdPixelDigiPar
//
#ifndef PNDMVDPIXELDIGIPAR_H
#define PNDMVDPIXELDIGIPAR_H

#include <TVector2.h>

#include "CbmParGenericSet.h"
#include "CbmParamList.h"

//! Digitization Parameter Class for MVD-Pixel part
class PndMvdPixelDigiPar : public CbmParGenericSet
{
  public :
    PndMvdPixelDigiPar (const char* name="PndMvdParTest",
          const char* title="PndMvd pixel digi parameter",
          const char* context="TestDefaultContext");
    ~PndMvdPixelDigiPar(void){};
    void clear(void){};
    void putParams(CbmParamList* list);
    Bool_t getParams(CbmParamList* list);
    
    void Print();

    Double_t GetXPitch()  const {return dimX;}
    Double_t GetYPitch()  const {return dimY;}
    Double_t GetThreshold() const {return threshold;}
    Double_t GetNoise()     const {return noise;}
    Int_t GetFECols() const {return _feCols;}
    Int_t GetFERows() const {return _feRows;}
    

    void SetXPitch(Double_t x)  {dimX = x;}
    void SetYPitch(Double_t x)  {dimY = x;}
    void SetThreshold(Double_t x) {threshold = x;}
    void SetNoise(Double_t x)     {noise = x;}
    void SetFECols(Int_t x){_feCols = x;}
    void SetFERows(Int_t x){_feRows = x;}
  

  private:
    // Pixel Parameters
    Double_t dimX;
    Double_t dimY;
    Int_t _feCols;
    Int_t _feRows;


    Double_t threshold; // Discriminator threshold
    Double_t noise;     // Complete noise including threshold dispersion
//     Text_t   sensName;  // Sensor name
//     Text_t   feName;    // Frontend name

    
  ClassDef(PndMvdPixelDigiPar,2);
};

#endif /*!MVDSTRIPDIGIPAR_H*/
