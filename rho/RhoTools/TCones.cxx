//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TCones.h
//
// Description:
//      Class TCones -  
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Gianluca Cavoto                   Original author
//      Peter Sanders
//
// Copyright Information:
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#include "RhoTools/TCones.h"
#include <math.h>
#include "RhoBase/TRho.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandListIterator.h"
#include "RhoBase/TEventInfo.h"
#include "TLorentzVector.h"

ClassImp(TCones)

TBuffer &operator>>(TBuffer &buf, TCones *&obj)
{
   obj = (TCones *) buf.ReadObject(TCones::Class());
   return buf;
}

#include <iostream>
using namespace std;

TCones::TCones(TCandList * all_list, 
		   const TCandidate * thedaugh , 
		   const TEventInfo * evtinfo, int nbrcones) 
  : _numberOfCones(nbrcones) { 

  TLorentzVector cmsdaugh(thedaugh->P4());
  cmsdaugh.Boost(-(evtinfo->CmFrame().BoostVector()) );
  TVector3  direction(cmsdaugh.X(),cmsdaugh.Y(),cmsdaugh.Z());
  _directionUnit = direction.Unit();
  MakeCones(  all_list,  evtinfo);
}


TCones::TCones(TCandList * all_list, 
		   const TVector3  direction , 
		   const TEventInfo * evtinfo, int nbrcones) 
  : _numberOfCones(nbrcones){  

  
 
  _directionUnit = direction.Unit();
  MakeCones( all_list,  evtinfo);

}

//--------------
// Destructor --
//--------------

TCones::~TCones()
{
}

double
TCones::EnergyFlows(int order) 
{
  return _eflowarray(order-1);
}

double
TCones::MomentumFlows(int order) 
{
  return _mflowarray(order-1);
}

int
TCones::NumberOfCones()
{
  return _numberOfCones;
}


void 
TCones::MakeCones(TCandList * all_list, 
		    const TEventInfo * evtinfo ){

  // zero the vectors of energy and momentum
  TVector initialize(_numberOfCones);
  for (int n=0; n<=_numberOfCones; n++) {initialize(n) = 0.0;}   
  _eflowarray = initialize;
  _mflowarray = initialize;

  // for speed calculate all the cone cosines
  TVector cosinearray(_numberOfCones+1);
  for (int icone=0;icone<_numberOfCones+1;++icone){
    cosinearray(icone) = cos( (double) icone*TMath::Pi()/_numberOfCones );
  }
  // avoid end affects 
  cosinearray(0)=1.1;
  cosinearray(_numberOfCones+1)=-1.1;
  

  // iterate over candidates
  TCandListIterator itercand( *all_list ); 
  TCandidate * onecand;

  while(onecand = itercand.Next()){

    TLorentzVector cms4Vec(onecand->P4());
    cms4Vec.Boost(-(evtinfo->CmFrame().BoostVector()) );
    TVector3 cms3Vec(cms4Vec.X(),cms4Vec.Y(),cms4Vec.Z());
     
    double cosangle = cms3Vec.Unit() * _directionUnit;
     
    for (int icone=0; icone<_numberOfCones; ++icone){
       
      // increment the appropriate angular bin
       
      if (cosangle >  cosinearray(icone+1) &&
	  cosangle <= cosinearray(icone) ) {
	 
	_eflowarray(icone) += cms4Vec.T();   // increment energy
	_mflowarray(icone) += cms3Vec.Mag(); // increment momentum
      }
       
    }      
  } // while
   
}


