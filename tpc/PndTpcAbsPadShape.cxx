//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcAbsPadShape
//      see PndTpcAbsPadShape.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Cristoforo Simonetto    TUM            (original author)
//
//
//-----------------------------------------------------------

// This Class' Header ------------------
#include "PndTpcAbsPadShape.h"

// C/C++ Headers ----------------------
#include "TPolyLine.h"
#include "TMath.h"

// Collaborating Class Headers --------
#include "PndTpcPRLookupTable.h"
// Class Member definitions -----------

PndTpcAbsPadShape::PndTpcAbsPadShape(const unsigned int fID)
  :lookupTable(0), ID(fID)
{;}
PndTpcAbsPadShape::~PndTpcAbsPadShape()
{
  if (lookupTable != 0)
    delete lookupTable;
}

double
PndTpcAbsPadShape::GetValue(const double x, const double y) const
{
  return(lookupTable->GetValue(x,y));
}

bool operator== (const PndTpcAbsPadShape& lhs, const PndTpcAbsPadShape& rhs)
{
  return(lhs.ID==rhs.ID);
}

std::ostream& operator<< (std::ostream& s, const PndTpcAbsPadShape& me)
{
  return s << "PndTpcAbsPadShape\n"
           << "ID="<<me.ID<<"\n"
           << "lookupTable="<<me.lookupTable<<"\n";
}


void
PndTpcAbsPadShape::Draw(double x, double y, double alpha, int color) const {
  unsigned int n=GetNBoundaryPoints();
  TPolyLine* line=new TPolyLine(n);
  double ca=TMath::Cos(alpha);
  double sa=TMath::Sin(alpha);
  for(unsigned int i=0; i<n; ++i){
    double fx; double fy;
    GetBoundaryPoint(i,fx,fy);
    double u=ca*fx-sa*fy+x;
    double v=sa*fx+ca*fy+y;
    line->SetPoint(i,u,v);
  }
  line->SetLineColor(color);
  line->Draw();
}
