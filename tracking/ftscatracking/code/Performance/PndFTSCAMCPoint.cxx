// $Id: PndFTSCAMCPoint.cxx,v 1.2 2010/09/01 10:38:27 ikulakov Exp $
//***************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: Sergey Gorbunov <sergey.gorbunov@kip.uni-heidelberg.de> *
//                  Ivan Kisel <kisel@kip.uni-heidelberg.de>                *
//                  for The ALICE HLT Project.                              *
//                                                                          *
// Developed by:   Igor Kulakov <I.Kulakov@gsi.de>                          *
//                 Maksym Zyzak <M.Zyzak@gsi.de>                            *
//                                                                          *
// Permission to use, copy, modify and distribute this software and its     *
// documentation strictly for non-commercial purposes is hereby granted     *
// without fee, provided that the above copyright notice appears in all     *
// copies and that both the copyright notice and this permission notice     *
// appear in the supporting documentation. The authors make no claims       *
// about the suitability of this software for any purpose. It is            *
// provided "as is" without express or implied warranty.                    *
//***************************************************************************

#include "PndFTSCAMCPoint.h"
#include "PndFTSCAMath.h"
#include "PndFtsPoint.h"

PndFTSCAMCPoint::PndFTSCAMCPoint()
    : fX( 0 ), fY( 0 ), fZ( 0 ), fSx( 0 ), fSy( 0 ), fSz( 0 ), fTime( 0 ), fISlice( 0 ), fTrackID( 0 )
{
  //* Default constructor
}

ostream& operator<<(ostream& out, const PndFTSCALocalMCPoint &a)
{
  out << a.fX << " " << a.fY << " " << a.fZ << std::endl;
  out << a.fPx << " " << a.fPy << " " << a.fPz << " " << a.fQP << std::endl;
  out << a.fISlice << " " << a.fIRow << " " << a.fTrackI << " " << a.fTrackID << std::endl;
  return out;
}


istream& operator>>(istream& in, PndFTSCALocalMCPoint &a)
{
  in >> a.fX >> a.fY >> a.fZ;
  in >> a.fPx >> a.fPy >> a.fPz >> a.fQP;
  in >> a.fISlice >> a.fIRow >> a.fTrackI >> a.fTrackID;
  return in;
}

void PndFTSCALocalMCPoint::SetPoint(PndFtsPoint* ppp, Double_t qq)
{
    fX = ppp->GetX();
    fY = ppp->GetY();
    fZ = ppp->GetZ();
    fPx = ppp->GetPx();
    fPy = ppp->GetPy();
    fPz = ppp->GetPz();
    fQP = qq/sqrt( fPx*fPx + fPy*fPy + fPz*fPz );
    fISlice = 0;
    //std::cout<<"ppp->GetLayerID() "<<ppp->GetLayerID()<<std::endl;
    fIRow = ppp->GetLayerID();
    fTrackI = ppp->GetTrackID();
    fTrackID = ppp->GetTrackID();
}

PndFTSCALocalMCPoint& PndFTSCALocalMCPoint::operator =(const PndFTSCALocalMCPoint &rhs)
{
    fX = rhs.X();
    fY = rhs.Y();
    fZ = rhs.Z();
    fPx = rhs.Px();
    fPy = rhs.Py();
    fPz = rhs.Pz();
    fQP = rhs.QP();
    fISlice = 0;
    fIRow = rhs.IRow();
    fTrackI = rhs.TrackI();
    fTrackID = rhs.TrackID();
    
    return *this;
}

void PndFTSCALocalMCPoint::RotateXY( float alpha) {
  const float cA = CAMath::Cos( alpha );
  const float sA = CAMath::Sin( alpha );
  {
    const float x = X(), y = Y();
    SetX(  x*cA +  y*sA );
    SetY( -x*sA +  y*cA );
  }
  {
    const float x = Px(), y = Py();
    SetPx(  x*cA +  y*sA );
    SetPy( -x*sA +  y*cA );
  }
}
