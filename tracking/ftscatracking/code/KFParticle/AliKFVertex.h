//---------------------------------------------------------------------------------
// The AliKFVertex class
// .
// @author  S.Gorbunov, I.Kisel
// @version 1.0
// @since   13.05.07
// 
// Class to reconstruct and store primary and secondary vertices.
// The method is described in CBM-SOFT note 2007-003, 
// ``Reconstruction of decayed particles based on the Kalman filter'', 
// http://www.gsi.de/documents/DOC-2007-May-14-1.pdf
//
// This class is ALICE interface to general mathematics in AliKFParticleBase
// 
//  -= Copyright &copy ALICE HLT Group =-
//_________________________________________________________________________________

#ifndef ALIKFVERTEX_H
#define ALIKFVERTEX_H

#include "AliKFParticle.h"
#include "KFPVertex.h"

class AliKFVertex : public AliKFParticle
{
  
 public:

  //*
  //*  INITIALIZATION
  //*

  //* Constructor (empty)

  AliKFVertex():AliKFParticle(),fIsConstrained(0){ } 

  //* Destructor (empty)

  ~AliKFVertex(){}

  //* Initialisation from VVertex 

  AliKFVertex( const KFPVertex &vertex );


  //*
  //*  ACCESSORS
  //*

  //* Number of tracks composing the vertex

  Int_t GetNContributors() const { return fIsConstrained ?fNDF/2:(fNDF+3)/2; }

  //* 
  //* CONSTRUCTION OF THE VERTEX BY ITS DAUGHTERS 
  //* USING THE KALMAN FILTER METHOD
  //*


  //* Simple way to construct vertices ex. D0 = Pion + Kaon;   

  void operator +=( const AliKFParticle &Daughter );  

  //* Subtract particle from vertex

  AliKFVertex operator -( const AliKFParticle &Daughter ) const;

  void operator -=( const AliKFParticle &Daughter );  

  //* Set beam constraint to the primary vertex

  void SetBeamConstraint( float X, float Y, float Z, 
			  float ErrX, float ErrY, float ErrZ );

  //* Set beam constraint off

  void SetBeamConstraintOff();

  //* Construct vertex with selection of tracks (primary vertex)

  void ConstructPrimaryVertex( const AliKFParticle *vDaughters[], int NDaughters,
			       Bool_t vtxFlag[], float ChiCut=3.5  );

 protected:

  Bool_t fIsConstrained; // Is the beam constraint set

};


//---------------------------------------------------------------------
//
//     Inline implementation of the AliKFVertex methods
//
//---------------------------------------------------------------------


inline void AliKFVertex::operator+=( const AliKFParticle &Daughter )
{
  AliKFParticle::operator+=( Daughter );
}
  

inline void AliKFVertex::operator-=( const AliKFParticle &Daughter )
{
  Daughter.SubtractFromVertex( *this );
}
  
inline AliKFVertex AliKFVertex::operator-( const AliKFParticle &Daughter ) const 
{
  AliKFVertex tmp = *this;
  Daughter.SubtractFromVertex( tmp );
  return tmp;
}


#endif 
