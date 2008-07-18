//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TCones.h
//
// Description:
//      Class TCones -    
//      this class calculates and return ( by member fcn  EnergyFlows(int i )   
//      ( i=1,2,...,nmax) )  a set of variables exploited by CLEO in the search
//      for charmless two-bodies decays   in a multivariate analysis environment
//     
// The definition follows:  given a special direction in the event ( sphericity
//  axis, $\pi$ candidate direction)   EnergyFlows(int i )  is the sum of the  
//    energies of the particles whose momenta form, with this special direction
//      an angle of amplitude between (i-1)*(180/nmax) and i*(180/nmax) degrees 
//
//           The "direction" can be passed to the ctors as a TCandidate or a 
//       TVector3.           
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Gianluca Cavoto                   Original author
//      Pete Sanders
//
// Copyright Information:
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#ifndef TCONES_H
#define TCONES_H

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------
#include "RhoBase/TCandList.h"
#include "TVector.h"   
#include "RhoBase/TCandidate.h"
class TEventInfo;

//              ---------------------
//              -- Class Interface --
//              ---------------------
 
class TCones : public TObject {

  //--------------------
  // Instance Members --
  //--------------------

public:
 
  // Constructors

  TCones(TCandList * , 
	   const TCandidate *, 
	   const TEventInfo * , int nbrcones=9 ) ;

  TCones(TCandList * , 
	   const TVector3 , 
	   const TEventInfo * , int nbrcones=9 ) ;

  
  // Destructor
  virtual ~TCones();
  
  // accessors 
  double  EnergyFlows( int ) ;
  double  MomentumFlows( int ) ;
  
  int NumberOfCones();
  
  


protected:




private:

  void MakeCones(TCandList * , 
		 const TEventInfo* ) ;


  int _numberOfCones ; //!Do not stream

  TVector3 _directionUnit; //!Do not stream

  TVector _eflowarray; //!Do not stream
  TVector _mflowarray; //!Do not stream
  
public:
    ClassDef(TCones,1) // Set of variables for charmless two-bodies decays
};

#endif
