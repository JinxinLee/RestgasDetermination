/**********************************************************
 Description: Class PndVtxFitterParticle
 Base class for particles used for Vertex fitting
 Authors: Dipak K. Mishra
***********************************************************/

#ifndef PNDVTXFITTERPARTICLE_H
#define PNDVTXFITTERPARTICLE_H

#include <TMatrixT.h>
#include <TMatrixD.h>
#include <TMatrixDSym.h>
#include <TMatrixTSym.h>
#include <TVector3.h>
#include <TLorentzVector.h>


#include "PndVtxFitter_Init.h"

//#include "FairRootManager.h"
//#include "FairTask.h"

class PndVtxFitterParticle : public TObject
{
  public:
    //constructor
    PndVtxFitterParticle();

    PndVtxFitterParticle(TLorentzVector, TVector3, const TMatrixDSym&, Double_t);

    PndVtxFitterParticle(const PndVtxFitterParticle&);

    //destructor
    virtual ~PndVtxFitterParticle();



    //input/setfunctions
    void   SetMomentum(TLorentzVector);
    void   SetPosition(TVector3);
    void   SetCharge(Double_t);
    void   SetError(const TMatrixDSym&);

    void   SetVertex(TVector3);
    void   SetErrVertex(const TMatrixDSym&);
    void   SetErrVertexIn(const TMatrixDSym&);

    void   SetCalDgf(Double_t);


    void   SetErrorCovMatrix(const TMatrixDSym&);

    Double_t SetFitParameter(Int_t);

    //  Int_t SetBeamConstraint(Int_t);



    //accessors
    TLorentzVector    GetMomentum()const;
    TVector3          GetPosition()const;
    Double_t          GetCharge()const;
    TMatrixDSym       GetError()const;

    TVector3          GetVertex() const;
    TMatrixDSym       GetErrVertex() const;
    TMatrixDSym       GetErrVertexOut() const;

    Double_t          GetCalDgf() const;

    TMatrixDSym       GetFitError(TMatrixDSym) const;
    //convert 6x6 cov matrices to 7x7 using momentum
    TMatrixD          GetFitError(TLorentzVector, TMatrixD) const;

    TMatrixDSym       GetErrorCovMatrix() const;

    TMatrixD          GetFitParameter() const;


    TMatrixDSym       GetConverted6(TMatrixDSym) const;
    TMatrixD          GetConverted6(TMatrixD) const;
    TMatrixD          GetConverted7(TMatrixD) const;
    TMatrixDSym       GetCovMat(TMatrixD) const;
    TMatrixDSym       GetCovMat1(TMatrixD) const;

    //  TMatrixD        mompos(const unsigned); // momentum+position
    //  TMatrixD        mompos(const unsigned) const;


    //operator
    //PndVtxFitterParticle & operator = (const PndVtxFitterParticle &);  // not implemented

  private:
    //before fit
    TLorentzVector          m_momentum_b;
    TVector3                m_position_b;
    TMatrixDSym             m_cov;

    TMatrixDSym             m_error_b;

    Double_t                m_charge;
    Double_t                m_dof;


    TVector3                m_vertex;
    TMatrixDSym             m_errvertex;


    Double_t      fCovMatrix[21];
    Double_t      fErrVtx[6];
    Double_t      fErrVtxM[6];

    ClassDef(PndVtxFitterParticle,1);
};



//access functions

inline TLorentzVector
PndVtxFitterParticle::GetMomentum()const
{return m_momentum_b;}

inline TVector3
PndVtxFitterParticle::GetPosition()const
{return m_position_b;}

inline Double_t
PndVtxFitterParticle::GetCharge()const
{return m_charge;}

inline TMatrixDSym
PndVtxFitterParticle::GetError()const
{return m_error_b;}

inline TVector3
PndVtxFitterParticle::GetVertex()const
{return m_vertex;}

inline Double_t
PndVtxFitterParticle::GetCalDgf()const
{return m_dof;}


// set functions
inline void
PndVtxFitterParticle::SetMomentum(TLorentzVector p)
{m_momentum_b = p;}

inline void
PndVtxFitterParticle::SetPosition(TVector3 x)
{m_position_b = x;}

inline void
PndVtxFitterParticle::SetCharge(Double_t q)
{m_charge = q;}

inline void
PndVtxFitterParticle::SetError(const TMatrixDSym& e)
{m_error_b = e;}

inline void PndVtxFitterParticle::SetVertex(TVector3 v)
{m_vertex = v;}

inline void PndVtxFitterParticle::SetCalDgf(Double_t d)
{m_dof = d;}
/*
  inline Int_t PndVtxFitterParticle::SetBeamConstraint(Int_t d)
  {mbc = d}
*/
#endif
