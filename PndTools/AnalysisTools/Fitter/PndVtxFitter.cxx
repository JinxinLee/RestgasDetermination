//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndVtxFitter                             //
//                                                                      //
// Author: D. Mishra, GSI,  2008                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "PndVtxFitter.h"

#include "RhoBase/RhoCandListIterator.h"

#include "PndVtxFitterParticle.h"
#include "PndVtxFitter_Init.h"
#include "PndVtxFitterError.h"
#include "RhoBase/RhoFactory.h"
#include "RhoTools/RhoCalculationTools.h"
using namespace std;

ClassImp ( PndVtxFitter )

TBuffer& operator>> ( TBuffer& buf, PndVtxFitter *&obj )
{
  obj = ( PndVtxFitter* ) buf.ReadObject ( PndVtxFitter::Class() );
  return buf;
}

PndVtxFitter::PndVtxFitter ( RhoCandidate* b ) : RhoFitterBase ( b )
{
  // fHeadOfTree=RhoFactory::Instance()->NewCandidate(b);
  m_mode=0;
  m_vertexChisq=0.;
  m_vertex_b=TVector3 ( 0.,0.,0. );
  m_errVertex_a=3;
  m_errBeam=3;
  m_beam=0;
  m_knownVertex=0;
  m_tube=0;
  m_itrk_tube=-1;
//   m_magField=KF_MAGNETIC_FIELD;
  m_necessaryTrackNum = 2;
}


PndVtxFitter::~PndVtxFitter()
{
  //if(fHeadOfTree) delete fHeadOfTree;
}

void PndVtxFitter::FitNode(RhoCandidate* b)
{
  SetDaugthersFromComposite(b);
  //    if(GetConstraintOpt() == 0)DoVertexFitWOCorr();
  if ( m_beamConstraint == 0 ) { DoVertexFitWOCorr(); }
  else { DoVertexFitBeamConstraint(); }
  SetOutputToRhoCandidate(b);
}

/**
 No Correlations between the tracks.
 ==>Section 4.2 of Belle Note #194: "Vertex Constraint to an Unknown position".
 This is same as "_fit2()" function in "kvertexfitter"
 ***/
unsigned PndVtxFitter::DoVertexFitWOCorr()
{
  m_trackNum = fDaughters.GetLength();
  cout<<"PndVtxFitter::DoVertexFitWOCorr: "<<m_trackNum<<" tracks"<<endl;

  /** Fit function _fit2() Starts **/

  //// if(m_errorFlag != KF_NO_ERROR)return m_errorFlag;
  if ( m_trackNum < m_necessaryTrackNum ) {
    m_errorFlag = KF_TRACK_SIZE;
    return m_errorFlag;
  }
  SetInputMatrix(); // Set the Input matrix
  CalDgf(); //Calculate Dgf
  Double_t chiSq ( 0. );
  Double_t errInverse = 0;
  //  Double_t *errInverse = 0;
  Double_t tmp_chiSq ( KF_INIT_CHI2 );
  Double_t tmp2_chiSq ( KF_INIT_CHI2 );
  //  TMatrixD     m_al_a(m_trackNum*KF_NUM6,1);
  m_al_a.ResizeTo ( m_trackNum*KF_NUM6,1 );
  m_al_a = m_al_0;

  TMatrixD tmp_al_a ( m_al_a );
  TMatrixD tmp_D ( m_D ),tmp_E ( m_E );
  TMatrixD tmp_V_D ( m_V_D ),tmp_V_E ( m_V_E );
  TMatrixD tmp_lam0 ( m_lam0 ),tmp_v_a ( m_v_a );


  TMatrixD tmp2_D ( m_D ),tmp2_E ( m_E );
  TMatrixD tmp2_V_D ( m_V_D ),tmp2_V_E ( m_V_E );
  TMatrixD tmp2_lam0 ( m_lam0 ),tmp2_v_a ( m_v_a );

  Double_t* tmp_eachChisq  = new Double_t[m_trackNum];
  Double_t* tmp2_eachChisq = new Double_t[m_trackNum];

  Int_t KF_MAX_ITERATION_NUMBER1 = 5;
  for ( Int_t j=0; j<KF_MAX_ITERATION_NUMBER1; ++j ) {
    tmp_chiSq = KF_INIT_CHI2;
    for ( Int_t j1=0; j1<KF_MAX_ITERATION_NUMBER1; ++j1 )

    {
      MakeCoreMatrix(); //Make Core Matrix

      //    cout<<"++++++ Fit function _fit2() Continues +++++++ "<<endl;
      TMatrixD tV_Ein ( 3,3 );
      chiSq = 0.;

      for ( int k=0; k<m_trackNum; ++k )

      {
        TMatrixD tD = m_D.GetSub ( 2*k+1-1,2* ( k+1 )-1,KF_NUM6*k+1-1,KF_NUM6* ( k+1 )-1 );//2x6
        TMatrixD tmp_vd = tD;

        TMatrixD tmp_vd1 = ( tD*m_V_al_0.GetSub ( KF_NUM6*k+1-1, ( int ) ( KF_NUM6* ( k+1 )-1 ),KF_NUM6*k+1-1, ( int ) ( KF_NUM6* ( k+1 )-1 ) ) * ( tmp_vd.T() ) ); //Dipak

        if ( tmp_vd1==0 ) { continue; }

        TMatrixD tV_D =  tmp_vd1.Invert ( &errInverse );

        m_V_D.SetSub ( 2*k,2*k,tV_D ); //Dipak
        TMatrixD tE = m_E.GetSub ( 2*k,2* ( k+1 )-1,0,2 );//2x3
        TMatrixD tmp_tE = tE;
        tV_Ein += ( tmp_tE.T() ) *tV_D*tE;//3x3
        TMatrixD tDeltaAl= ( m_al_0-m_al_1 ).GetSub ( KF_NUM6*k,KF_NUM6* ( k+1 )-1,0,0 );//6x1

        TMatrixD td = m_d.GetSub ( 2*k,2* ( k+1 )-1,0,0 );//2x1
        TMatrixD tlam0 = tV_D* ( tD*tDeltaAl+td );//2x2x(2x6x6x1+2x1) = 2x1
        m_lam0.SetSub ( 2*k,0,tlam0 ); //Dipak
        TMatrixD tmp_tlam0 = tlam0;

        m_eachChisq[k] = ( ( tmp_tlam0.T() ) * ( tD*tDeltaAl+tE* ( m_v-m_v_a ) +td ) ) ( 0,0 );//1x2x(2x6x6x1+2x3x3x1+2x1)

        chiSq  += m_eachChisq[k];
      } //for loop 'k'
      m_V_E = tV_Ein.Invert ( &errInverse );
      //m_V_E = tV_Ein.Invert(errInverse);
      /** will put later:Dipak
         if(errInverse != 0)
         {
         delete [] tmp_eachChisq;
         delete [] tmp2_eachChisq;
         m_errorFlag = KF_INVERSE;
         return m_errorFlag;
         }
       **/

      m_v_a = m_v_a - m_V_E* ( m_E_T.Transpose ( m_E ) ) *m_lam0;
      if ( tmp_chiSq > chiSq ) {
        for ( Int_t k=0; k<m_trackNum; ++k ) { tmp_eachChisq[k] = m_eachChisq[k]; }
        tmp_chiSq = chiSq;
        tmp_v_a   = m_v_a;
        tmp_V_E   = m_V_E;
        tmp_V_D   = m_V_D;
        tmp_lam0  = m_lam0;
        tmp_E     = m_E;
        tmp_D     = m_D;

        if ( j1 == KF_MAX_ITERATION_NUMBER-1 ) {
          m_overIterationFlag = KF_OVER_ITERATION;
        } else { continue; }
      } else if ( j1 != 0 ) {
        for ( Int_t k=0; k<m_trackNum; ++k ) { m_eachChisq[k] = tmp_eachChisq[k]; }
        chiSq   = tmp_chiSq;
        m_v_a   = tmp_v_a;
        m_V_E   = tmp_V_E;
        m_V_D   = tmp_V_D;
        m_lam0  = tmp_lam0;
        m_E     = tmp_E;
        m_D     = tmp_D;
        break;
      } else if ( j1 == 0 ) {
        //      cout<<"i'm in this loop 3"<<endl;
        m_errorFlag = KF_INIT_CHISQ;
        break;
      }

    } //for loop 'j1'

    m_al_a = m_al_1;
    m_lam  = m_lam0 - m_V_D*m_E*m_V_E* ( m_E_T.Transpose ( m_E ) ) *m_lam0;
    m_al_1 = m_al_0 - m_V_al_0* ( m_D_T.Transpose ( m_D ) ) *m_lam;

    if ( j == 0 ) {
      for ( Int_t k=0; k<m_trackNum; ++k ) { tmp2_eachChisq[k] = m_eachChisq[k]; }
      tmp2_chiSq = chiSq;
      tmp2_v_a   = m_v_a;
      tmp2_V_E   = m_V_E;
      tmp2_V_D   = m_V_D;
      tmp2_lam0  = m_lam0;
      tmp2_E     = m_E;
      tmp2_D     = m_D;
      tmp_al_a   = m_al_a;
      continue;
    } else {
      if ( tmp2_chiSq > chiSq ) {
        for ( Int_t k=0; k<m_trackNum; ++k ) { tmp2_eachChisq[k] = m_eachChisq[k]; }
        tmp2_chiSq = chiSq;
        tmp2_v_a   = m_v_a;
        tmp2_V_E   = m_V_E;
        tmp2_V_D   = m_V_D;
        tmp2_lam0  = m_lam0;
        tmp2_E     = m_E;
        tmp2_D     = m_D;
        tmp_al_a   = m_al_a;
        if ( j == KF_MAX_ITERATION_NUMBER-1 ) {
          m_overIterationFlag = KF_OVER_ITERATION;
        } else { continue; }
      } else {
        for ( Int_t k=0; k<m_trackNum; ++k ) { m_eachChisq[k] = tmp2_eachChisq[k]; }
        chiSq   = tmp2_chiSq;
        m_v_a   = tmp2_v_a;
        m_V_E   = tmp2_V_E;
        m_V_D   = tmp2_V_D;
        m_lam0  = tmp2_lam0;
        m_E     = tmp2_E;
        m_D     = tmp2_D;
        m_al_a  = tmp_al_a;
        break;
      }
    }

  }// for loop 'j'

  /** will be put later
   if(m_errorFlag != KF_NO_ERROR){
   delete [] tmp_eachChisq;
   delete [] tmp2_eachChisq;
   return m_errorFlag;
   }
   ****/

  m_lam    = m_lam0 - m_V_D*m_E*m_V_E* ( m_E_T.Transpose ( m_E ) ) *m_lam0;
  m_al_1   = m_al_0 - m_V_al_0* ( m_D_T.Transpose ( m_D ) ) *m_lam;

  m_V_Dt   = m_V_D  - m_V_D*m_E*m_V_E* ( m_E_T.Transpose ( m_E ) ) *m_V_D;
  m_V_al_1 = m_V_al_0 - m_V_al_0* ( m_D_T.Transpose ( m_D ) ) *m_V_Dt*m_D*m_V_al_0;


  m_Cov_v_al_1 = m_V_E* ( m_E_T.Transpose ( m_E ) ) *m_V_D*m_D*m_V_al_0; //'-'ve sign missing: Dipak

  cout<<"Final vertex Position is "<<m_v_a[0][0]<<" "<<m_v_a[1][0]<<" "<<m_v_a[2][0]<<endl;
  cout<<"Final Momenta are "<<m_al_1[0][0]<<" "<<m_al_1[1][0]<<" "<<m_al_1[2][0]<<endl;

  cout<<"Final Positions are "<<m_al_1[3][0]<<" "<<m_al_1[4][0]<<" "<<m_al_1[5][0]<<endl;
  //" "<<m_al_1[9][0]<<" "<<m_al_1[10][0]<<" "<<m_al_1[11][0]<<endl;
  cout<<"Chi2 is **********: "<<chiSq<<endl;
  cout<<" ******** "<<m_Cov_v_al_1.GetNrows() <<" m_Cov_v_al_1 ****** "<<m_Cov_v_al_1.GetNcols() <<endl;
  //   m_Cov_v_al_1.Print();
  cout<<"Error on vertex are***========"<<endl;
  cout<<m_V_E[0][0]<<" "<<m_V_E[0][1]<<" "<<m_V_E[0][2]<<endl;
  cout<<m_V_E[1][0]<<" "<<m_V_E[1][1]<<" "<<m_V_E[1][2]<<endl;
  cout<<m_V_E[2][0]<<" "<<m_V_E[2][1]<<" "<<m_V_E[2][2]<<endl;
  //
  /** OutPut Matrix Starts **/
  cout<<"==========OutPut Matrix Starts========="<<endl;
  m_chisq = chiSq;

  //   SetOutputMatrix(); //Set the output matrix
  
  //Output is set from FitNode() now
  //SetOutputToRhoCandidate(); //Set output to RhoCandidate

  delete [] tmp_eachChisq;
  delete [] tmp2_eachChisq;
  return m_errorFlag;
}

/***
 Included beam position constraint (no correlation between the tracks)
 ==>Section C.5 of Belle Note #194: "Fitting with constraints for Unknown parameters".
 Vertex Refitter in the point with Errors.
 This is same as "_fit3()" function in "kvertexfitter".

 **/

unsigned PndVtxFitter::DoVertexFitBeamConstraint()
{
  m_trackNum = fDaughters.GetLength();

  //  if(m_errorFlag != KF_NO_ERROR)return m_errorFlag;
  if ( m_trackNum < m_necessaryTrackNum ) {
    m_errorFlag = KF_TRACK_SIZE;
    return m_errorFlag;
  }

  SetInputMatrix(); //Set input matrix
  CalDgf(); //calculate Dgf

  /** Fit function _fit3() Starts **/
  cout<<"++++++ Fit function _fit3() Starts +++++++ "<<endl;

  Double_t chiSq ( 0. );
  //   Double_t errInverse = 0;
  Double_t* errInverse= 0;
  Double_t tmp_chiSq ( KF_INIT_CHI2 );
  // TMatrixD     m_al_a(m_trackNum*KF_NUM6,1);
  m_al_a.ResizeTo ( m_trackNum*KF_NUM6,1 );
  m_al_a = m_al_0;
  TMatrixD tmp_al_a ( m_al_a );

  TMatrixD tmp_D ( m_D ),tmp_E ( m_E );
  TMatrixD tmp_lam ( m_lam );
  m_errBeam.ResizeTo ( 3,3 );
  //vertex
  m_v[0][0] = m_vertex_b.X();
  m_v[1][0] = m_vertex_b.Y();
  m_v[2][0] = m_vertex_b.Z();


  Double_t* tmp_eachChisq = new Double_t[m_trackNum];
  Double_t tmp_vertexChisq ( KF_INIT_CHI2 );// An init-value is not needed but the C++ complier requires the init-value.
  Int_t itFlag = 0;
  Int_t KF_MAX_ITERATION_NUMBER1 = 1;
  for ( Int_t j=0; j<KF_MAX_ITERATION_NUMBER1; ++j ) {

    MakeCoreMatrix(); //Make Core matrix

    cout<<"============= MakeCoreMatrix() Ends ========="<<endl;

    cout<<"++++++ Fit function _fit3() Continues +++++++ "<<endl;
    chiSq = 0.;

    m_errBeam = GetBeamProfile(); //Using Dummy values: Dipak



    TMatrixD tV_Dtin = m_D*m_V_al_0* ( m_D_T.Transpose ( m_D ) ) +m_E*m_errBeam* ( m_E_T.Transpose ( m_E ) );
    //     if(tV_Dtin.Determinant()==0)continue;


    TMatrixD tV_Dt = tV_Dtin.Invert ( errInverse );
    //      cout<<"rows "<<tV_Dtin.GetNrows()<<" cols "<<tV_Dtin.GetNcols()<<" "<<tV_Dt.GetNrows()<<" "<<tV_Dt.GetNcols()<<endl;

    if ( errInverse != 0 ) {
      delete [] tmp_eachChisq;
      m_errorFlag = KF_INVERSE;
      return m_errorFlag;
    }

    m_lam = tV_Dt* ( m_D* ( m_al_0-m_al_1 ) +m_E* ( m_v-m_v_a ) +m_d );//(2*nTrk)x1

    for ( int k=0; k<m_trackNum; ++k ) {
      TMatrixD tD = m_D.GetSub ( 2*k,2* ( k+1 )-1,
                                 KF_NUM6*k,
                                 KF_NUM6* ( k+1 )-1 );//2x6
      TMatrixD tDeltaAl = ( m_al_0-m_al_1 ).GetSub ( KF_NUM6*k,
                          KF_NUM6* ( k+1 )-1,0,0 );//6x1

      TMatrixD td = m_d.GetSub ( 2*k,2* ( k+1 )-1,0,0 );//2x1
      TMatrixD tE = m_E.GetSub ( 2*k,2* ( k+1 )-1,0,2 );//2x3
      TMatrixD m_lam_tmp = m_lam.GetSub ( 2*k,2* ( k+1 )-1,0,0 );
      chiSq += ( m_lam_tmp.T() * ( tD*tDeltaAl+tE* ( m_v-m_v_a ) +td ) ) ( 0,0 );
      TMatrixD m_lam_tmp1 = m_lam.GetSub ( 2*k,2* ( k+1 )-1,0,0 );
      TMatrixD tD_tmp = tD;
      m_eachChisq[k] = ( m_lam_tmp1.T() *tD*m_V_al_0.
                         GetSub ( KF_NUM6*k,KF_NUM6* ( k+1 )-1,
                                  KF_NUM6*k,KF_NUM6* ( k+1 )-1 ) *
                         ( tD_tmp.T() ) *m_lam.GetSub ( 2*k,2* ( k+1 )-1,0,0 ) ) ( 0,0 );
    }
    TMatrixD mlam1 = m_lam;

    m_vertexChisq = ( mlam1.T() *m_E*m_errBeam* ( m_E_T.Transpose ( m_E ) ) *m_lam ) ( 0,0 );

    m_al_a = m_al_1;

    m_v_a  = m_v - m_errBeam* ( m_E_T.Transpose ( m_E ) ) *m_lam;
    m_al_1 = m_al_0 - m_V_al_0* ( m_D_T.Transpose ( m_D ) ) *m_lam;

    if ( tmp_chiSq > chiSq || ( tmp_chiSq <= chiSq && itFlag == 0 ) ) {
      if ( tmp_chiSq <= chiSq ) { itFlag = 1; }
      for ( int k=0; k<m_trackNum; ++k ) { tmp_eachChisq[k] = m_eachChisq[k]; }
      tmp_vertexChisq = m_vertexChisq;
      tmp_chiSq = chiSq;
      tmp_lam   = m_lam;
      tmp_E     = m_E;
      tmp_D     = m_D;
      tmp_al_a  = m_al_a;
      if ( j == KF_MAX_ITERATION_NUMBER-1 ) {
        m_overIterationFlag = KF_OVER_ITERATION;
      } else { continue; }
    } else if ( j != 0 ) {
      for ( int k=0; k<m_trackNum; ++k ) { m_eachChisq[k] = tmp_eachChisq[k]; }
      m_vertexChisq = tmp_vertexChisq;
      chiSq   = tmp_chiSq;
      m_lam   = tmp_lam;
      m_E     = tmp_E;
      m_D     = tmp_D;
      m_al_a  = tmp_al_a;
      break;
    } else if ( j == 0 ) {
      m_errorFlag = KF_INIT_CHISQ;
      break;
    }

  }

  if ( m_errorFlag != ( int ) KF_NO_ERROR ) {
    delete [] tmp_eachChisq;
    return m_errorFlag;
  }


  m_al_1 = m_al_0 - m_V_al_0* ( m_D_T.Transpose ( m_D ) ) *m_lam;


  m_v_a  = m_v - m_errBeam* ( m_E_T.Transpose ( m_E ) ) *m_lam;

  TMatrixF tV_Dtin = m_D*m_V_al_0* ( m_D_T.Transpose ( m_D ) ) +m_E*m_errBeam* ( m_E_T.Transpose ( m_E ) );
  //  m_V_Dt = tV_Dtin.Invert(&errInverse);

  m_V_Dt = tV_Dtin.Invert ( errInverse );

  if ( errInverse != 0 ) {
    delete [] tmp_eachChisq;
    m_errorFlag = KF_INVERSE;
    return m_errorFlag;
  }

  m_V_al_1 = m_V_al_0 - m_V_al_0* ( m_D_T.Transpose ( m_D ) ) *m_V_Dt*m_D*m_V_al_0;

  m_Cov_v_al_1 = m_errBeam* ( m_E_T.Transpose ( m_E ) ) *m_V_Dt*m_D*m_V_al_0; // '-'ve sign missing: Dipak

  m_V_E = m_errBeam-m_errBeam* ( m_E_T.Transpose ( m_E ) ) *m_V_Dt*m_E*m_errBeam;

  cout<<"Final vertex Position is "<<m_v_a[0][0]<<" "<<m_v_a[1][0]<<" "<<m_v_a[2][0]<<endl;
  cout<<"Final Momenta are "<<m_al_1[0][0]<<" "<<m_al_1[1][0]<<" "<<m_al_1[2][0]<<endl;
  //" "<<m_al_1[6][0]<<" "<<m_al_1[7][0]<<" "<<m_al_1[8][0]<<endl;
  cout<<"Final Positions are "<<m_al_1[3][0]<<" "<<m_al_1[4][0]<<" "<<m_al_1[5][0]<<endl;
  //" "<<m_al_1[9][0]<<" "<<m_al_1[10][0]<<" "<<m_al_1[11][0]<<endl;
  cout<<"Chi2 is **********: "<<chiSq<<endl;

  cout<<"Error on vertex are***========"<<endl;
  cout<<m_V_E[0][0]<<" "<<m_V_E[0][1]<<" "<<m_V_E[0][2]<<endl;
  cout<<m_V_E[1][0]<<" "<<m_V_E[1][1]<<" "<<m_V_E[1][2]<<endl;
  cout<<m_V_E[2][0]<<" "<<m_V_E[2][1]<<" "<<m_V_E[2][2]<<endl;

  /** OutPut Matrix Starts **/
  cout<<"==========OutPut Matrix Starts========="<<endl;

  m_chisq = chiSq;
  //   SetOutputMatrix();

  //Output is set from FitNode() now
  //SetOutputToRhoCandidate();

  cout<<"ChiSquare is: ********* "<<m_chisq<<endl;
  delete [] tmp_eachChisq;
  return m_errorFlag;
}

unsigned PndVtxFitter::CalDgf()
{
  m_trackNum = fDaughters.GetLength();
  m_dgf = 2*m_trackNum - 3;
  if ( m_beam == 1 ) { m_dgf = 2*m_trackNum; }
  if ( m_knownVertex ==  1 ) { m_dgf = 2*m_trackNum; }
  fNDegreesOfFreedom=m_dgf;
  return m_dgf;

}



void PndVtxFitter::RetriveInfoFromTCand()
{

}

//Define the Input matrix
unsigned PndVtxFitter::SetInputMatrix()
{
  //   cout<<" *********m_trackNum is: "<<m_trackNum<<endl;
  if ( m_mode == KF_MODE_WO_CORRELATION ||  m_knownVertex == 1 ) {
    if ( m_trackNum > ( int ) KF_MAX_TRACK_NUMBER2 ) {
      m_errorFlag = KF_INPUT_TRACK_SIZE;
      return m_errorFlag;
    }
  } else {
    if ( m_trackNum > ( int ) KF_MAX_TRACK_NUMBER ) {
      m_errorFlag = KF_INPUT_TRACK_SIZE;
      return m_errorFlag;
    }
  }
  unsigned    index ( 0 );
  TMatrixD    tmp_al_0 ( KF_NUM6*m_trackNum,1 );
  TMatrixDSym tmp_V_al_0 ( KF_NUM6*m_trackNum );
  TMatrixD    tmp_property ( m_trackNum,3 );
  TMatrixDSym tmp_ErrCov ( KF_NUM6 );
  //   m_trackNum = fArr->GetEntriesFast();

  it =new PndVtxFitterParticle;

  for ( Int_t i=0; i<fDaughters.GetLength(); i++ ) {
    RhoCandidate* tc=fDaughters[i];
    Double_t bField = 0.1*RhoCalculationTools::GetBz(fDaughters[i]->Pos()); // T, assume field in z only
    //cout <<"charge:"<<tc->GetCharge()<<endl;
    TMatrixD mat7=tc->Cov7();

    tmp_ErrCov=it->GetCovMat ( mat7 );

    tmp_al_0[index* KF_NUM6+0][0] = tc->P4().Px();
    tmp_al_0[index* KF_NUM6+1][0] = tc->P4().Py();
    tmp_al_0[index* KF_NUM6+2][0] = tc->P4().Pz();
    tmp_al_0[index* KF_NUM6+3][0] = tc->Pos().X();
    tmp_al_0[index* KF_NUM6+4][0] = tc->Pos().Y();
    tmp_al_0[index* KF_NUM6+5][0] = tc->Pos().Z();

    tmp_V_al_0.SetSub ( index*KF_NUM6, tmp_ErrCov ); //Dipak
    tmp_property[index][0] =  tc->GetCharge() /3;//FIXME: Why charge/3 ??
    tmp_property[index][1] = tc->P4().M();//get the mass:Dummy value set by hand :Dipak
    tmp_property[index][2] = -KF_PHOTON_VELOCITY*bField*tc->GetCharge() /3; //FIXME: Why charge/3 ??

    ++index;
  }

  delete it;
  //   cout<<" Total number of Tracks 'PndVtxFitter' SetInputMatrix ***######### "<<m_trackNum<<endl;
  //Define the Size of the Matrices
  m_V_al_1.ResizeTo ( KF_NUM6*m_trackNum,KF_NUM6*m_trackNum );
  m_V_al_0.ResizeTo ( tmp_V_al_0 );
  m_v.ResizeTo ( 3,1 );
  m_v_a.ResizeTo ( 3,1 );
  m_property.ResizeTo ( m_trackNum,3 );
  m_al_0.ResizeTo ( m_trackNum*KF_NUM6,1 );
  m_al_1.ResizeTo ( m_trackNum*KF_NUM6,1 );
  m_D.ResizeTo ( m_trackNum*2,KF_NUM6*m_trackNum );
  m_E.ResizeTo ( m_trackNum*2,3 );
  m_d.ResizeTo ( m_trackNum*2,1 );
  m_V_D.ResizeTo ( m_trackNum*2,m_trackNum*2 );
  m_lam.ResizeTo ( m_trackNum*2,1 );
  m_lam0.ResizeTo ( m_trackNum*2,1 );
  m_V_Dt.ResizeTo ( m_trackNum*2,m_trackNum*2 );
  m_Cov_v_al_1.ResizeTo ( 3,KF_NUM6*m_trackNum );
  m_V_E.ResizeTo ( 3,3 );


  m_E_T.ResizeTo ( 3,m_trackNum*2 );
  m_D_T.ResizeTo ( KF_NUM6*m_trackNum,m_trackNum*2 );


  m_V_al_0 = tmp_V_al_0;



  /******
   //  If there is correlation between track and track then convert
   //    7x7 ==> 6x6 matrix
   //m_setCorrelation function has been defined in kfitterbase.cc
   if(m_knownVertex == 0 && m_beam == 0 && m_mode != KF_MODE_WO_CORRELATION){
   if(m_correlationFlag == 1){
   m_errorFlag = m_setCorrelation();
   if(m_errorFlag != KF_NO_ERROR)return m_errorFlag;
   }
   }
   ******/



  m_v_a[0][0] = m_vertex_b.X();
  m_v_a[1][0] = m_vertex_b.Y();
  m_v_a[2][0] = m_vertex_b.Z();

  m_al_0     = tmp_al_0;
  m_al_1     = m_al_0;
  m_property = tmp_property;

  m_D = m_V_al_1.GetSub ( 0,m_trackNum*2-1,0,KF_NUM6*m_trackNum-1 );
  m_E    = m_V_al_1.GetSub ( 0,m_trackNum*2-1,0,2 );
  m_d    = m_V_al_1.GetSub ( 0,m_trackNum*2-1,0,0 );
  m_V_D  = m_V_al_1.GetSub ( 0,m_trackNum*2-1,0,m_trackNum*2-1 );
  m_lam  = m_V_al_1.GetSub ( 0,m_trackNum*2-1,0,0 );
  m_lam0 = m_V_al_1.GetSub ( 0,m_trackNum*2-1,0,0 );
  m_V_Dt = m_V_al_1.GetSub ( 0,m_trackNum*2-1,0,m_trackNum*2-1 );
  m_Cov_v_al_1 = m_V_al_1.GetSub ( 0,2,0,KF_NUM6*m_trackNum-1 );


  return m_errorFlag;
}


unsigned PndVtxFitter::MakeCoreMatrix()
{
  Double_t px,py,pz,x,y,z,a;
  Double_t pt,invPt,invPt2,dlx,dly,dlz,a1,a2,r2d2,B,Rx,Ry,S,U;
  Double_t sininv,sqrtag;

  for ( int i=0; i<m_trackNum; ++i ) {
    px = m_al_1[i*KF_NUM6+0][0];
    py = m_al_1[i*KF_NUM6+1][0];
    pz = m_al_1[i*KF_NUM6+2][0];
    x  = m_al_1[i*KF_NUM6+3][0];
    y  = m_al_1[i*KF_NUM6+4][0];
    z  = m_al_1[i*KF_NUM6+5][0];
    a  = m_property[i][2];  //charge

    //cout <<"MakeCoreMatrix"<<endl;
    //cout <<px<<" "<<py<<" "<<pz<<" "<<x<<" " <<y<<" "<<z<<" "<<a<<endl;

    pt   =  sqrt ( px*px+py*py );


    if ( pt == 0. ) {
      m_errorFlag = KF_DIV_ZERO;
      return m_errorFlag;
    }

    invPt   = 1./pt;
    invPt2  = invPt*invPt;
    dlx  =  m_v_a[0][0]-x;
    dly  =  m_v_a[1][0]-y;
    dlz  =  m_v_a[2][0]-z;
    a1   = -dlx*py+dly*px;
    a2   =  dlx*px+dly*py;
    r2d2 =  dlx*dlx+dly*dly;
    Rx   =  dlx-2.*px*a2*invPt2;
    Ry   =  dly-2.*py*a2*invPt2;

    if ( a != 0. ) {
      // charged
      B = a*a2*invPt2;

      if ( fabs ( B ) > 1. ) {
        m_errorFlag = KF_ARCSIN;
        return m_errorFlag;
      }
      //sin^(-1)(B)
      sininv = TMath::ASin ( B );
      Double_t tmp0 = 1.0-B*B;
      if ( tmp0 == 0. ) {
        m_errorFlag = KF_DIV_ZERO;
        return m_errorFlag;
      }
      //1/sqrt(1-B^2)
      sqrtag = 1.0/sqrt ( tmp0 );
      S = sqrtag*invPt2;
      U = dlz-pz*sininv/a;
    } else {
      // neutral
      B      = 0.0;
      sininv = 0.0;
      sqrtag = 1.0;
      S = invPt2;
      U = dlz-pz*a2*invPt2;
    }

    //d: Matrix
    m_d[i*2+0][0] = a1-0.5*a*r2d2;
    m_d[i*2+1][0] = U*pt;

    //D: Matrix
    m_D[i*2+0][i* KF_NUM6+0] =  dly;
    m_D[i*2+0][i* KF_NUM6+1] = -dlx;
    m_D[i*2+0][i* KF_NUM6+2] =  0.0;
    m_D[i*2+0][i* KF_NUM6+3] =  py+a*dlx;
    m_D[i*2+0][i* KF_NUM6+4] = -px+a*dly;
    m_D[i*2+0][i* KF_NUM6+5] =  0.0;
    m_D[i*2+1][i* KF_NUM6+0] = -pz*pt*S*Rx+U*px*invPt;
    m_D[i*2+1][i* KF_NUM6+1] = -pz*pt*S*Ry+U*py*invPt;
    if ( a != 0. ) {
      m_D[i*2+1][i* KF_NUM6+2] = -sininv*pt/a;
    } else {
      m_D[i*2+1][i* KF_NUM6+2] = -a2*invPt;
    }
    m_D[i*2+1][i* KF_NUM6+3] =  px*pz*pt*S;
    m_D[i*2+1][i* KF_NUM6+4] =  py*pz*pt*S;
    m_D[i*2+1][i* KF_NUM6+5] = -pt;

    //E: Matrix
    m_E[i*2+0][0] = -py-a*dlx;
    m_E[i*2+0][1] =  px-a*dly;
    m_E[i*2+0][2] =  0.0;
    m_E[i*2+1][0] = -px*pz*pt*S;
    m_E[i*2+1][1] = -py*pz*pt*S;
    m_E[i*2+1][2] =  pt;

  } // for loop 'i'

  return m_errorFlag;
}

//Fill output to RhoCandidate
unsigned PndVtxFitter::SetOutputToRhoCandidate(RhoCandidate* cand)
{
  //cout<<"I'm inside OutputToRhoCandidate *** "<<m_al_1[0][0]<<" "<<m_al_1[1][0]<<" "<<m_al_1[2][0]<<" "<<m_al_1[3][0]<<" "<<m_al_1[4][0]<<" "<<m_al_1[5][0]<<endl;

  TVector3 h3m;
  //  cout<<"# of Tracks are in RhoCandidate *************** "<< fDaughters.GetLength()<< endl;
  TMatrixD trCov ( 7,7 );
  Int_t index1 ( 0 );
  //TClonesArray &cands = *fCandidates;

  TLorentzVector sum ( 0,0,0,0 );
  TVector3 foudVertex( m_v_a[0][0], m_v_a[1][0],m_v_a[2][0] );
  for ( Int_t i=0; i<fDaughters.GetLength(); i++ ) {
    TLorentzVector lv;
    double px=m_al_1[index1*KF_NUM6+0][0];
    double py=m_al_1[index1*KF_NUM6+1][0];
    double pz=m_al_1[index1*KF_NUM6+2][0];
    double m=fDaughters[i]->Mass();

    //      cout <<"SetOutputToRhoCandidate"<<px<<" "<<py<<" "<<pz<<" "<<m<<endl;

    lv.SetXYZM ( px,py,pz,m );
    sum+=lv;
    
    fDaughters[i]->SetP4(lv);
    fDaughters[i]->SetPos ( foudVertex );

    index1++;
  }


  cout <<"sum: "<<sum.Px() <<" "<<sum.Py() <<" "<<sum.Pz() <<" "<<sum.M() <<endl;
  cout <<"vtx: "<<m_v_a[0][0]<<" "<<m_v_a[1][0]<<" "<<m_v_a[2][0]<<endl;
  cand->SetP4 ( sum );
  cand->SetPos ( foudVertex );

  //         cout <<"SetOutputToRhoCandidate head of Tree:"<<(*fHeadOfTree)<<endl;

  cout<<"Vertex Error : "<<endl;
  //  m_V_E.Print();
  cand->SetCovPos ( m_V_E ); //Implemented in TFitParams :Dipak
  cand->PosCov().Print();

  fChiSquare= m_chisq;

  return m_errorFlag;

}

