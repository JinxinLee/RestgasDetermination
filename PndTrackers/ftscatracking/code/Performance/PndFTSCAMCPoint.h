//-*- Mode: C++ -*-
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef PNDFTSCAMCPOINT_H
#define PNDFTSCAMCPOINT_H

#include "PndFTSCADef.h"
#include <vector>

#include <iostream>
using std::ostream;
using std::istream;

class PndFtsPoint;

/**
 * @class PndFTSCAMCPoint
 * store MC point information for PndFTSCAPerformance
 */
class PndFTSCAMCPoint
{
  public:

    PndFTSCAMCPoint();

    float  X()           const { return fX; }
    float  Y()           const { return fY; }
    float  Z()           const { return fZ; }
    float  Sx()          const { return fSx; }
    float  Sy()          const { return fSy; }
    float  Sz()          const { return fSz; }
    float  Time()        const { return fTime; }
    int    ISlice()      const { return fISlice; }
    int    TrackID()     const { return fTrackID; }

    void SetX( float v )           { fX = v; }
    void SetY( float v )           { fY = v; }
    void SetZ( float v )           { fZ = v; }
    void SetSx( float v )          { fSx = v; }
    void SetSy( float v )          { fSy = v; }
    void SetSz( float v )          { fSz = v; }
    void SetTime( float v )        { fTime = v; }
    void SetISlice( int v )      { fISlice = v; }
    void SetTrackID( int v )     { fTrackID = v; }

    static bool Compare( const PndFTSCAMCPoint &p1, const PndFTSCAMCPoint &p2 ) {
      return ( p1.fTrackID < p2.fTrackID );
    }

  protected:

    float fX;         //* global X position
    float fY;         //* global Y position
    float fZ;         //* global Z position
    float fSx;        //* slice X position
    float fSy;        //* slice Y position
    float fSz;        //* slice Z position
    float fTime;      //* time
    int   fISlice;    //* slice number
    int   fTrackID;   //* mc track number
};


/**
 * @class PndFTSCALocalMCPoint
 * store MC point information for PndFTSCAPerformance
 */
class PndFTSCALocalMCPoint
{
  public:

    PndFTSCALocalMCPoint(){};
    ~PndFTSCALocalMCPoint(){};

    void SetPoint(PndFtsPoint* ppp, Double_t qq);
    PndFTSCALocalMCPoint& operator =(const PndFTSCALocalMCPoint &rhs);
    
    float  X()           const { return fX; } // global coordinates
    float  Y()           const { return fY; }
    float  Z()           const { return fZ; }
    float  Px()          const { return fPx; }
    float  Py()          const { return fPy; }
    float  Pz()          const { return fPz; }
    float  QP()          const { return fQP; }
    int    ISlice()      const { return fISlice; }
    int    IRow()      const { return fIRow; }
    int    TrackI()     const { return fTrackI; }
    int    TrackID()     const { return fTrackID; }

  float Angle() const { return fAngle; }
  float Err2Z() const { return fErr2Z; }
  float Err2Y() const { return fErr2Y; }
  
    void SetX( float v )           { fX = v; }
    void SetY( float v )           { fY = v; }
    void SetZ( float v )           { fZ = v; }
    void SetPx( float v )          { fPx = v; }
    void SetPy( float v )          { fPy = v; }
    void SetPz( float v )          { fPz = v; }
    void SetQP( float v )        { fQP = v; }
    void SetISlice( int v )      { fISlice = v; }
    void SetIRow( int v )        { fIRow = v; }
    void SetTrackI( int v )     { fTrackI = v; }
    void SetTrackID( int v )     { fTrackID = v; }

  void SetAngle( float v ) { fAngle = v; }
  void SetErr2Z( float v ) { fErr2Z = v; }
  void SetErr2Y( float v ) { fErr2Y = v; }
  
    void RotateXY( float alpha );
  
    static bool Compare( const PndFTSCALocalMCPoint &p1, const PndFTSCALocalMCPoint &p2 ) {
      return ( p1.fTrackID < p2.fTrackID );
    }

    friend ostream& operator<<(ostream& out, const PndFTSCALocalMCPoint &a);
    friend istream& operator>>(istream& in, PndFTSCALocalMCPoint &a);


  protected:

    float fX;         //* local X position
    float fY;         //* local Y position
    float fZ;         //* local Z position
    float fPx;        //* local momentum projection - Px
    float fPy;
    float fPz;
    float fQP;        //* Q/P

    float fAngle; // angle of the module
    float fErr2Z, fErr2Y; // errors for correspondent measurement
    int   fISlice;   //* slice number
    int   fIRow;
    int   fTrackI;   //* mc track index
    int   fTrackID;  //* mc track simulation ID
  public:
    std::vector<int> fHits;
};

#endif
