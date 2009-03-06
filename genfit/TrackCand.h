/*  Copyright © 2008, Christian Hoeppner & Sebastian Neubert
    All rights reserved.

    This file is part of Genfit.

    Genfit is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Genfit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser Public License for more details.

    You should have received a copy of the GNU Lesser Public License
    along with Genfit.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef TRACKCAND_HH
#define TRACKCAND_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <vector>

// Collaborating Class Declarations --



class TrackCand : public TObject {
public:

  // Constructors/Destructors ---------
  TrackCand();
  ~TrackCand();

  TrackCand(double curv, double dip, double inv, std::vector<unsigned int> detIDs, std::vector<unsigned int> hitIDs);

  // operators 
  friend bool operator== (const TrackCand& lhs, const TrackCand& rhs);

  // Accessors -----------------------
  void getHit(unsigned int i, 
	      unsigned int& detId,
	      unsigned int& hitId) const {detId=_detId[i];hitId=_hitId[i];}
  unsigned int getNHits() const {return _detId.size();}
  double getCurv() const {return _curv;}
  double getCurvError() const{return _dCurv;}
  double getDip() const {return _dip;}
  double getDipError() {return _dDip;}
  bool inverted() const {return _inv;}
  std::vector<unsigned int> GetHitIDs(int detId=-1);

  // Modifiers -----------------------
  void addHit(unsigned int detId, unsigned int hitId);
  void setCurv(double c){_curv=c;}
  void setCurvError(double c){_dCurv =c;}
  void setDip(double d){_dip=d;}
  void setDipError(double d){_dDip=d;}
  void setInverted(bool f=true) {_inv=f;}
  bool HitInTrack(unsigned int detId, unsigned int hitId);

  // Operations ----------------------
  void reset();

private:

  // Private Data Members ------------
  std::vector<unsigned int> _detId;
  std::vector<unsigned int> _hitId;

  double _curv; // curvature from pattern reco
  double _dCurv; // error in curvature
  double _dip;  // dip angle from pattern reco
  double _dDip; // error in dip angle from pattern reco
  bool _inv;  // true if inverted track
  // Private Methods -----------------

public:
  ClassDef(TrackCand,2)
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
