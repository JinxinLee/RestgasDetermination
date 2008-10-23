#ifndef PNDMVDCALCFEPIXEL_H
#define PNDMVDCALCFEPIXEL_H

#include <vector>
#include "PndMvdPixel.h"
#include "TVector3.h"

//! Class to calculate the position of digis on a front-end from the digis on a sensor
/** @author T.Stockmanns <t.stockmanns@fz-juelich.de>
 *
 * This class calculates the column, row and front-end number of a digi
 * from the column, row number on a sensor and vice versa
*/

class PndMvdCalcFePixel
{
  public :
      ///Default constructor
      PndMvdCalcFePixel(){ fNcols = 0; fNrows = 0; fMaxFEperCol = 0;};

      ///Main constructor
      /// @param ncols number of columns on a front-end chip
      /// @param nrows number of rows on a front-end chip
      /// @param maxFe maximum number of front-end chip per column on a sensor
      PndMvdCalcFePixel(Int_t ncols, Int_t nrows, Int_t maxFe){
          fNcols = ncols;
          fNrows = nrows;
          fMaxFEperCol = maxFe;
        };

        void SetNCols(Int_t ncols){fNcols = ncols;};
        void SetNRows(Int_t nrows){fNrows = nrows;};
        void SetMaxFE(Int_t maxFE){fMaxFEperCol = maxFE;};
        Int_t GetNCols() const { return fNcols;};
        Int_t GetNRows() const { return fNrows;};
        Int_t GetMaxFE() const { return fMaxFEperCol;};

        void SetSensorHits(std::vector<PndMvdPixel> hits){
          fSensorHits = hits;
        }

        ///Main function to calculate front-end hits
        std::vector<PndMvdPixel> CalcFEHits();

        ///Main function to calculate front-end hits
        std::vector<PndMvdPixel> CalcFEHits(std::vector<PndMvdPixel> SensorHits){
          SetSensorHits(SensorHits);
          return CalcFEHits();
        };

        ///Main function to calculate sensor hits
        std::vector<PndMvdPixel> CalcSensorHits(const std::vector<PndMvdPixel> FePixel);
        PndMvdPixel CalcSensorHit(const PndMvdPixel FePixel) const;
        void CalcSensorColRow(Int_t& col, Int_t& row, const Int_t fe) const;
  private : Int_t fNcols;
        Int_t fNrows;
        Int_t fMaxFEperCol;
        std::vector<PndMvdPixel> fSensorHits;
        std::vector<PndMvdPixel> fFeHits;
};


#endif /*MVDCALCFEPIXEL_H*/
