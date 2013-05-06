#ifndef PndVtxFitter_H
#define PndVtxFitter_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndVtxFitter                 //
//                                                                      //
// Author: D. Mishra, K. Goetzen, GSI, 2008       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/RhoFitterBase.h"
#include "TVector3.h"
#include "TMatrixDSym.h"
#include "TMatrixD.h"
#include "RhoBase/RhoCandList.h"

#include "PndVtxFitterParticle.h"

class PndVtxFitter : public RhoFitterBase
{
  public:
    PndVtxFitter( RhoCandidate& b ) ;
    virtual ~PndVtxFitter();// {if(fHeadOfTree) delete fHeadOfTree;}
    void FitNode(RhoCandidate* b);

  private:
    //  unsigned   _fit2(PndVtxFitterParticleCollection& ); // small Matrix

    RhoCandList   fDaughters;

    unsigned   _fit2(); // small Matrix

    unsigned     m_mode;
    Double_t     m_eachChisq[KF_MAX_TRACK_NUMBER2];
    Double_t     m_vertexChisq;
    TVector3     m_vertex_b;
    TVector3     m_vertex_a;
    TMatrixDSym  m_errVertex_a;
    TMatrixDSym   m_errBeam;
    unsigned      m_beam;
    unsigned      m_knownVertex;
    Int_t         m_beamConstraint;
    Bool_t        m_tube;
    Int_t         m_itrk_tube;
    Int_t         m_dgf;


    unsigned    DoVertexFitWOCorr(); //Fit without correlation
    unsigned    DoVertexFitBeamConstraint(); //include beam position constraint
    unsigned    SetInputMatrix(); //Set the input matrix
    unsigned    SetInputSubMatrix();
    unsigned    CalDgf(); //Calculate the Degree of Freedom
    unsigned    MakeCoreMatrix();
    unsigned    SetOutputMatrix();//Set the output matrix

    unsigned    SetOutputToRhoCandidate(RhoCandidate* cand); //fill output to RhoCandidate
    void RetriveInfo();
    void RetriveInfoFromTCand();

    PndVtxFitterParticle* it;
    RhoCandidate*       tcand;
    //virtual unsigned m_setInputSubMatrix(void) = 0;

    //these were from Base2
    //matrix definitions
    TMatrixD    m_E;
    TMatrixD    m_V_E;
    TMatrixD    m_lam0;
    TMatrixD    m_v;
    TMatrixD    m_v_a;
    TMatrixD    m_V_Dt;
    TMatrixD    m_Cov_v_al_1;

    //Following are from Base


    //statistic parameter
    Double_t   m_chisq;
    Double_t   m_cl;

    Int_t       m_trackNum;
    Int_t       m_necessaryTrackNum;
    Int_t       m_errorFlag;
    Int_t       m_overIterationFlag;
    Double_t    m_magField;

    //  TMatrixDSym  m_V_al_0;
    TMatrixD     m_V_al_0;
    TMatrixD     m_al_0;
    TMatrixD     m_al_1;
    TMatrixD     m_al_a;
    TMatrixD     m_property;
    TMatrixD     m_V_al_1;

    TMatrixD     m_D;
    TMatrixD     m_d;
    TMatrixD     m_V_D;
    TMatrixD     m_lam;

    TMatrixD     m_E_T;
    TMatrixD     m_D_T;


  public:
    //void         SetBeamProfile(const TMatrixDSym&);  // not implemented
    //void         SetVertexProfile(const TMatrixDSym&); // = beamProfile  // not implemented
    TMatrixDSym  GetBeamProfile() const;
    //TMatrixDSym  GetVertexProfile()const;  // not implemented

    Double_t      fErrVtxPro[6];
    Double_t      fErrBeamPro[6];

    //set constraint Options
    void SetBeamConstraint(int);

    ClassDef(PndVtxFitter,1) //A super duper fitter
};

inline TMatrixDSym PndVtxFitter::GetBeamProfile() const
{
  TMatrixDSym m_errbeam(3);
  for(int i=0; i<3; i++) {
    for(int j=i; j<3; j++) {
      if(i<j) { m_errbeam[i][j] = m_errbeam[j][i] = 0.001; } //Dummy value
      else { m_errbeam[i][j] = m_errbeam[j][i] = 0.002; } //Dummy value
      // m_errbeam[i][j] = m_errbeam[j][i] = 0.005; //Dummy value
    }
  }
  return m_errbeam;
}
inline void PndVtxFitter::SetBeamConstraint(int var) {m_beamConstraint = var;}
#endif
