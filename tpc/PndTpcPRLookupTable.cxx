//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcPRLookupTable
//      see PndTpcPRLookupTable.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Cristoforo Simonetto TUM
//
//
//-----------------------------------------------------------

// This Class' Header ------------------
#include "PndTpcPRLookupTable.h"

// C/C++ Headers ----------------------

// Collaborating Class Headers --------
#include "PndTpcAbsPadShape.h"
#include "PndTpcGem.h"
#include "TError.h"
#include "TMath.h"
// Class Member definitions -----------

PndTpcPRLookupTable::PndTpcPRLookupTable()
  : isbuilt(false),range(0),step(0),nx(0),ny(0),table(0)
{;}


PndTpcPRLookupTable::~PndTpcPRLookupTable()
{
  if(table!=0)
  {
    for(int i=0;i<2*nx;++i)
      delete [] table[i];
    delete [] table;
  }
}

double
PndTpcPRLookupTable::GetValue(const double dx, const double dy) const{
  if(!isbuilt)return 0;
  int ilx=(int)TMath::Floor(dx/step);
  int ily=(int)TMath::Floor(dy/step);
  int iux=ilx+1;
  int iuy=ily+1;

  double lx=ilx*step;
  double ly=ily*step;
  double ux=iux*step;
  double uy=iuy*step;
  
  if(ilx<-nx || ilx>=nx){
    //ErrMsg(warning)<<"ilx out of range"<<endmsg;return 0;
  }
  if(ily<-ny || ily>=ny){
    //ErrMsg(warning)<<"ily out of range"<<endmsg;return 0;
  }
  if(iux<-nx || iux>=nx){
    //ErrMsg(warning)<<"iux out of range"<<endmsg;return 0;
  }
  if(iuy<-ny || iuy>=ny){
    //ErrMsg(warning)<<"iuy out of range"<<endmsg;return 0;
  }
  //bilinear interpolation:
  double t=(dx-lx)/(ux-lx);
  double u=(dy-ly)/(uy-ly);
  double result=(1-t)*(1-u)*table[ilx+nx][ily+ny];
  result+=t*(1-u)*table[iux+nx][ily+ny];
  result+=t*u*table[iux+nx][iuy+ny];
  result+=(1-t)*u*table[ilx+nx][iuy+ny];
  return result;
}

void 
PndTpcPRLookupTable::BuildTable(const PndTpcGem& gem, 
			     const PndTpcAbsPadShape& pad, 
			     const double _range,      
			     const double _step,
			     const double _intStep){
  if(table!=0){
    for(int i=0;i<2*nx;++i){
      delete [] table[i];
    }
    delete [] table;
  }
  range=_range; step=_step;
  double padwidth;
  double padheight;
  pad.EvalBoundingRect(padwidth, padheight, 0);
  nx=(int)ceil( (range+0.5*padwidth) /step);//# of steps in x direction
  ny=(int)ceil( (range+0.5*padheight) /step);//# of steps in y
  //build table
  table=new double*[2*nx];
  for(int i=0;i<2*nx;++i){
    table[i]=new double[2*ny];
  }
  std::ostream& s = std::cerr;
  //loop over table
  s<<"Building TPC LookupTable ";
  for(int ix=-nx;ix<nx;++ix){
    if(ix%10 == 0) s<<".";
    for(int iy=-ny;iy<ny;++iy){
      double thisx=ix*step;
      double thisy=iy*step;
      table[ix+nx][iy+ny]=Integrate(gem,pad,_intStep,thisx,thisy);
    }
  }
  s<<std::endl;
  isbuilt=true;
}


double 
PndTpcPRLookupTable::Integrate(const PndTpcGem& gem, 
			    const PndTpcAbsPadShape& p,
			    const double intStep,
			    const double dx,      
			    const double dy) const 
{
  double padwidth;
  double padheight;
  p.EvalBoundingRect(padwidth, padheight, 0);
  int nintx=(int)floor(padwidth/intStep);
  int ninty=(int)floor(padheight/intStep);
  double sum=0;
  double offsetx=0.5*(padwidth-intStep);
  double offsety=0.5*(padheight-intStep);
  //initialize Avalanche
  for(int ix=0;ix<nintx;++ix)
    for(int iy=0;iy<ninty;++iy)
      {
	if (!p.Contains(ix*intStep-offsetx,iy*intStep-offsety)) 
	  continue;
      double thisx=ix*intStep-offsetx-dx;
      double thisy=iy*intStep-offsety-dy;
      double r=sqrt(thisx*thisx+thisy*thisy);
      sum+=gem.cloudShape()->Eval(r)*1/gem.spread()*1/sqrt(2*3.1416); // TODO: cache these values!!!
    }
  
  //normalize to area
  double s2=intStep*intStep;
  sum*=s2;
  return sum;
}

