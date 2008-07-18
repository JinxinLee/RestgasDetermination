/**********************************************************
 Description: Class VtxFitterParticle
 Base class for particles used for Vertex fitting
 Authors: Dipak K. Mishra
***********************************************************/

#ifndef VTXFITTERPARTICLE_H
#define VTXFITTERPARTICLE_H

#include <TMatrixT.h>
#include <TMatrixD.h>
#include <TMatrixDSym.h>
#include <TMatrixTSym.h>
#include <TVector3.h>
#include <TLorentzVector.h>


#include "VtxFitter_Init.h"

#include "CbmRootManager.h"
#include "CbmTask.h"

class VtxFitterParticle : public TObject
{
 public:
  //constructor
  VtxFitterParticle();
 
  VtxFitterParticle(TLorentzVector, TVector3, const TMatrixDSym&, Double_t);

  VtxFitterParticle(const VtxFitterParticle&);

  //destructor
  virtual ~VtxFitterParticle();


  
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
  VtxFitterParticle & operator = (const VtxFitterParticle &);

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
  
  ClassDef(VtxFitterParticle,1);
};



//access functions

inline TLorentzVector 
VtxFitterParticle::GetMomentum()const
{return m_momentum_b;}

inline TVector3 
VtxFitterParticle::GetPosition()const
{return m_position_b;}

inline Double_t 
VtxFitterParticle::GetCharge()const
{return m_charge;}

inline TMatrixDSym 
VtxFitterParticle::GetError()const
{return m_error_b;}

inline TVector3 
VtxFitterParticle::GetVertex()const
{return m_vertex;}

inline Double_t
VtxFitterParticle::GetCalDgf()const
{return m_dof;}


// set functions
inline void 
VtxFitterParticle::SetMomentum(TLorentzVector p)
{m_momentum_b = p;}

inline void 
VtxFitterParticle::SetPosition(TVector3 x)
{m_position_b = x;}

inline void 
VtxFitterParticle::SetCharge(Double_t q)
{m_charge = q;}

inline void 
VtxFitterParticle::SetError(const TMatrixDSym& e)
{m_error_b = e;}

inline void VtxFitterParticle::SetVertex(TVector3 v) 
{m_vertex = v;}

inline void VtxFitterParticle::SetCalDgf(Double_t d)
{m_dof = d;}
/*
  inline Int_t VtxFitterParticle::SetBeamConstraint(Int_t d)
  {mbc = d}
*/
#endif
