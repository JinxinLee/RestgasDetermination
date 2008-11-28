//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class PndEmcBump
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//
// Author List:
//	Stephen J. Gowdy	Originator
// Copyright Information:
//	Copyright (C) 1997	University of Edinburgh
//
// Dima Melnychuk, adaption for PANDA
//------------------------------------------------------------------------


//-----------------------
// This Class's Header --
//-----------------------

#include "PndEmcBump.h"

//---------------
// C++ Headers --
//---------------

#include <iostream>
#include <iomanip>
#include <vector>
using std::vector;

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

#include "PndEmcSharedDigi.h"
using std::endl;
using std::ios;
using std::ostream;
using std::setw;

//----------------
// Constructors --
//----------------

PndEmcBump::PndEmcBump()
  : fClusterIndex( 0 )
{
  SetNBumps(1);
}

//Copy
PndEmcBump::PndEmcBump(const PndEmcBump &copy)
{
	fClusterIndex=copy.fClusterIndex;
	fNbumps=copy.fNbumps;
  
	fLocalMaxList=copy.fLocalMaxList;
	fDigiList=copy.fDigiList;
	fMemberDigiMap=copy.fMemberDigiMap;
	fEnergyValid=copy.fEnergyValid;
	fEnergy=copy.fEnergy;
	fWhereValid=copy.fWhereValid;
	fWhere=copy.fWhere;
}

//--------------
// Destructor --
//--------------

PndEmcBump::~PndEmcBump()
{
}

//-------------
// Modifiers --
//-------------

void
PndEmcBump::MadeFrom( Int_t clusterIndex )
{
  fClusterIndex = clusterIndex;
}

Double_t
PndEmcBump::energy() const
{
	Double_t weight=0;
	if ( ! fEnergyValid )
	{
		Double_t sum=0;
      std::vector<PndEmcDigi*>::const_iterator digi_iter;

      for (digi_iter=fDigiList.begin();digi_iter!=fDigiList.end();++digi_iter)
		{
			weight=(*digi_iter)->dynamic_cast_PndEmcSharedDigi()->weight();
			sum+=(*digi_iter)->GetEnergy()*weight;
		}
		
      fEnergy = sum;
      fEnergyValid = true;
    }
  
  return fEnergy;
}

Double_t
PndEmcBump::GetEnergyCorrected() const
{
	Double_t e=this->energy();
	TVector3 clusterPosition= this->where();
	Double_t theta=clusterPosition.Theta();
		
	Double_t e1=e;
	Double_t theta1=theta;
	
	if ( (clusterPosition.Z() < 180.0)&&(theta<140.*TMath::Pi()/180.))
	{
		if (e<0.03) e1 = 0.03;
		if (e>8.0)  e1 = 8.0 ;
	}
	
	if ( (clusterPosition.Z() < 180.0)&&(theta>140.*TMath::Pi()/180.))
	{
		if (e<0.03) e1 = 0.03;
		if (e>2.0)  e1 = 2.0 ;
	}
	
	if (clusterPosition.Z() > 180.0)
	{
		if (e<0.01) e1 = 0.01;
		if (e>16.0)  e1 = 16.0 ;
	}

  double b0 = 1.45312;
  double b1 = 2.79086e-02;
  double b2 = 3.91932e-04;
  double b3 =-1.23117e-03;
  double b4 = 2.72270e-01;
  double b5 =-1.31540;
  double b6 = 1.44447;
  double b7 =-4.05724e-01;
  double b8 =-2.07396;
  double b9 = 4.80507e-02;

  double p0=0;
  double p1=0;
  double p2=0;
  double p3=0;
  double p4=0;
  double p5=0;
  double p6=0;
  double p7=0;
  double p8=0;
  double p9=0;

  if(e1<1.0)
  {
     p0 =  4.13189e-02;   
     p1 = -2.03834e-02;  
     p2 = -2.58086e-03;   
     p3 = -1.77821e-03;   
     p4 = -1.73738e-02;   
     p5 =  7.40362e-02;   
     p6 = -6.41892e-02;   
     p7 = -9.85564e-02;   
     p8 =  1.50123e-01;   
     p9 = -7.87742e-04;   
  }
  else
  {
     p0 =  5.05003e-02;  
     p1 = -3.47672e-02;   
     p2 =  3.72767e-02;   
     p3 = -1.26492e-02;   
     p4 = -2.16876e-02;   
     p5 =  1.02682e-01;   
     p6 = -9.85242e-02;   
     p7 = -1.39872e-01;   
     p8 =  2.02309e-01;   
     p9 =  1.11696e-03;   
  }


  double t0 = 1.81631;
  double t1 =-1.71202e-02;
  double t2 = 3.59161e-03;
  double t3 =-3.46712e-04;
  double t4 =-3.73691e-01;
  double t5 =-1.56688;
  double t6 =-1.62618;
  double t7 =-4.10972e-01;
  double t8 = 2.2222;
  double t9 = 4.60908e-03;


  double factor1= p0
		+p1*log(e1)
		+p2*log(e1)*log(e1)
		+p3*log(e1)*log(e1)*log(e1)
		+p4*cos(theta1)
		+p5*cos(theta1)*cos(theta1)
		+p6*cos(theta1)*cos(theta1)*cos(theta1)
		+p7*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)
		+p8*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)
		+p9*log(e1)*cos(theta1);

  double factor2= t0
                +t1*log(e1)
                +t2*log(e1)*log(e1)
                +t3*log(e1)*log(e1)*log(e1)
                +t4*cos(theta1)
                +t5*cos(theta1)*cos(theta1)
                +t6*cos(theta1)*cos(theta1)*cos(theta1)
                +t7*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)
                +t8*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)
                +t9*log(e1)*cos(theta1);


  double factor3= b0
                +b1*log(e1)
                +b2*log(e1)*log(e1)
                +b3*log(e1)*log(e1)*log(e1)
                +b4*cos(theta1)
                +b5*cos(theta1)*cos(theta1)
                +b6*cos(theta1)*cos(theta1)*cos(theta1)
                +b7*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)
                +b8*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)
                +b9*log(e1)*cos(theta1);


  double eout1=e* exp(factor1); 
  double eout2=e* exp(factor2); 
  double eout3=e* exp(factor3); 

  double eout4=(3.31694-0.0183379/sqrt(e1)+0.0327113/e1+0.00040156/(e1*e1)-0.00641305/(e1*sqrt(e1)))*e;

	if ( clusterPosition.Z() > 500.0)
		return eout4;
	else if ( (clusterPosition.Z() < 180.0)&&(theta>140.*TMath::Pi()/180.))
		return eout3;
	else  if ( (clusterPosition.Z() < 180.0)&&(theta<140.*TMath::Pi()/180.))
		return eout1;
	else 
		return eout2;

}

Double_t
PndEmcBump::RnumberOfDigis() const
{
	Double_t sum = 0;
	PndEmcDigi* current;
	
	std::vector<PndEmcDigi*>::const_iterator digi_iter;
	for (digi_iter=fDigiList.begin();digi_iter!=fDigiList.end();++digi_iter)
	{
		sum+=(*digi_iter)->dynamic_cast_PndEmcSharedDigi()->weight();
	}
		
	return sum;
}

Int_t
PndEmcBump::NumberOfDigis() const
{
	Int_t numberOfDigis = Int_t( RnumberOfDigis() + 0.5 );
	if( numberOfDigis < 1 ) numberOfDigis = 1;
	
	return( numberOfDigis );
}
