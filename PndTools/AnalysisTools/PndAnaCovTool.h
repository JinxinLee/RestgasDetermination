/**********************************************************
 Description: Class PndAnaCovTool
 Taken from Base class for particles used for Vertex fitting
 Authors: Dipak K. Mishra
          R. Kliemt (taking away the cov converters)
***********************************************************/

#ifndef PNDANACOVTOOL_H
#define PNDANACOVTOOL_H

#include <TMatrixT.h>
#include <TMatrixD.h>
#include <TMatrixDSym.h>
#include <TMatrixTSym.h>
#include <TVector3.h>
#include <TLorentzVector.h>

class PndAnaCovTool : public TObject
{
  public:
    //constructor
    PndAnaCovTool();

    //destructor
    virtual ~PndAnaCovTool();

    TMatrixDSym       GetConverted6(TMatrixDSym) const;
    TMatrixD          GetConverted6(TMatrixD) const;
    TMatrixD          GetConverted7(TMatrixD) const;
    TMatrixDSym       GetCovMat(TMatrixD) const;
    TMatrixDSym       GetCovMat1(TMatrixD) const;

    TMatrixDSym       GetFitError(TMatrixDSym) const;
    TMatrixD          GetFitError(TLorentzVector, TMatrixD) const;

  private:

    ClassDef(PndAnaCovTool,1);
};

#endif
