#ifndef PNDSDSCALCFEPIXEL_H
#define PNDSDSCALCFEPIXEL_H

#include <vector>
#include "PndSdsPixel.h"
#include "TVector3.h"

//! Class to calculate the position of digis on a front-end from the digis on a sensor
/** @author T.Stockmanns <t.stockmanns@fz-juelich.de>
 *
 * This class calculates the column, row and front-end number of a digi
 * from the column, row number on a sensor and vice versa
 */

class PndSdsCalcFePixel
  {
    public :
    ///Default constructor
    PndSdsCalcFePixel() : 
    fNcols(1),
    fNrows(1),
    fMaxFEperCol(1),
    fSensorHits(),
    fFeHits()
    { };
    
    ///Main constructor
    /// @param ncols number of columns on a front-end chip
    /// @param nrows number of rows on a front-end chip
    /// @param maxFe maximum number of front-end chip per column on a sensor
    PndSdsCalcFePixel(Int_t ncols, Int_t nrows, Int_t maxFe) :
    fNcols(ncols),
    fNrows(nrows),
    fMaxFEperCol(maxFe),
    fSensorHits(),
    fFeHits()
    { };
    
    ~PndSdsCalcFePixel(){};
    
    void SetNCols(Int_t ncols){fNcols = ncols;};
    void SetNRows(Int_t nrows){fNrows = nrows;};
    void SetMaxFE(Int_t maxFE){fMaxFEperCol = maxFE;};
    Int_t GetNCols() const { return fNcols;};
    Int_t GetNRows() const { return fNrows;};
    Int_t GetMaxFE() const { return fMaxFEperCol;};
    
    void SetSensorHits(std::vector<PndSdsPixel> hits){
      fSensorHits = hits;
    }
    
    ///Main function to calculate front-end hits
    std::vector<PndSdsPixel> CalcFEHits();
    
    ///Main function to calculate front-end hits
    std::vector<PndSdsPixel> CalcFEHits(std::vector<PndSdsPixel> SensorHits){
      SetSensorHits(SensorHits);
      return CalcFEHits();
    };
    
    ///Main function to calculate sensor hits
    std::vector<PndSdsPixel> CalcSensorHits(const std::vector<PndSdsPixel> FePixel);
    PndSdsPixel CalcSensorHit(const PndSdsPixel FePixel) const;
    void CalcSensorColRow(Int_t& col, Int_t& row, const Int_t fe) const;
  protected : 
    Int_t fNcols;
    Int_t fNrows;
    Int_t fMaxFEperCol;
    std::vector<PndSdsPixel> fSensorHits;
    std::vector<PndSdsPixel> fFeHits;
  };


#endif /*PNDSDSCALCFEPIXEL_H*/
