//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class Emc2DLocMaxMaxFinder.
//      Searches for local maxima in a cluster.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//      Phil Strother         
//      Helmut Schmuecker      
//
// Copyright Information:
//	Copyright (C) 1997	            Imperial College
// Modified:
// M. Babai
//------------------------------------------------------------------------

#include "PndEmc2DLocMaxFinder.h"

#include "PndEmcTwoCoordIndex.h"
#include "PndEmcDigi.h"
#include "PndEmcCluster.h"
#include "PndEmcRecoPar.h"
		
#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;

//----------------
// Constructors --
//----------------

PndEmc2DLocMaxFinder::PndEmc2DLocMaxFinder(PndEmc2DLocMaxFinderData locMaxData, Int_t verbose)
{
  fVerbose=verbose;
  fMaxECut=locMaxData.MaxECut;
  fNeighbourECut=locMaxData.NeighbourECut;
  fCutSlope=locMaxData.CutSlope;
  fCutOffset=locMaxData.CutOffset;
  fERatioCorr=locMaxData.ERatioCorr;
  //                                            ^
  //                                            |  ....             /
  //                                         1.0|   ....           /|
  //                                            |                 / | fCutSlope
  //                                            |  splitoffs     /  |       
  //           MaxE of neighbours - fERatioCorr  |    and        /___| 
  // ERatio =  -------------------------------  |  hadrons     /..
  //                     MaxE-fERatioCorr        |             / ..
  //                                            |  ....      /  ..
  //                                            |   ....    /  ...  <-- merged pions
  //                                            |          /  ....      and photons
  //                                            |  ...    /   ....  
  //                                         0.0|        /    ....
  //                                            |------------------->
  //                                             0     /     6 7 8
  //                                            <------->             number of neighbours
  //                                            fCutOffset             with energy > fNeighbourECut
   
  fTheNeighbourLevel=locMaxData.TheNeighbourLevel;
  // This is the range of the search to figure out whether a particular 
  // digi is a local max or not.    Which neighbours are
  // looked at is decided by this parameter:  
  //        0 = nearest neighbours (this is all logical neighbours including corners)
  //        1 = nearest and next nearest neighbours 
  //        etc.  
  
  cout<<"fMaxECut = "<<fMaxECut<<endl;
  cout<<"fNeighbourECut = "<<fNeighbourECut<<endl;
  cout<<"fCutSlope = "<<fCutSlope<<endl;
  cout<<"fCutOffset = "<<fCutOffset<<endl;
  cout<<"fERatioCorr = "<<fERatioCorr<<endl;
  cout<<"fTheNeighbourLevel = "<<fTheNeighbourLevel<<endl;
}

//--------------
// Destructor --
//--------------

PndEmc2DLocMaxFinder::~PndEmc2DLocMaxFinder()
{delete fRecoPar;}

//-------------
// Methods   --
//-------------
void PndEmc2DLocMaxFinder::findMaxima( const PndEmcCluster * const theCluster, std::set<PndEmcTwoCoordIndex*>& res) const
{
  // We own the EmcLocMaxInfo objects.  Delete from last time.
  //Clean-up res, We need an empty set to store the results.
  res.clear();

  EmcDigiPtrDict *theClustersDigis = (EmcDigiPtrDict *)theCluster->MemberDigiMap();
  
  EmcDigiPtrDict::iterator theDigiIterator = (*theClustersDigis).begin(); 
  
  EmcCoordIndexSet allTheNeighbours;
  EmcCoordIndexSet theNewNeighbours;
  
  if (theClustersDigis->size()==1){
    while( theDigiIterator != (*theClustersDigis).end() ){
      PndEmcTwoCoordIndex *theTCI = theDigiIterator->first;
      res.insert(theTCI);
      ++theDigiIterator;
    }
  } 
  else{
    while( theDigiIterator != (*theClustersDigis).end() ){
      PndEmcTwoCoordIndex *theTCI = theDigiIterator->first;
      
      allTheNeighbours.clear();
      theNewNeighbours.clear();
      
      // Start the newneighbours off, then get all the neighbours from
      // this.  This is kind of curious, since the seed digi shouldn't
      // end up in the neighbour list, but it should work
      theNewNeighbours.insert(theTCI);
      
      getNeighbourDigis(allTheNeighbours, theNewNeighbours, 
			fTheNeighbourLevel,(const EmcDigiPtrDict * const) theClustersDigis);

      if (isALocalMax(theDigiIterator->second, theCluster, allTheNeighbours)){
	res.insert(theTCI);
      }
      ++theDigiIterator;
    }
  } 
}

bool PndEmc2DLocMaxFinder::isALocalMax( const PndEmcDigi *const theDigi, const PndEmcCluster * const theCluster, 
					const EmcCoordIndexSet &amongstTheseNeighbours ) const
{
  // Loop over all our neighbours and check to see if the one in hand is a local max
  
  Bool_t result=true;
  Double_t theDigiEnergy = theDigi->GetEnergy();
  
  if (theDigiEnergy<fMaxECut) {
    //std::cout << "Digi is not a local max because its energy " 
    //    << theDigiEnergy << " is too low. " << std::endl;
    result=false;
  }
  else {
    EmcCoordIndexSet::const_iterator theNeighbourIterator = amongstTheseNeighbours.begin();
    
    const EmcDigiPtrDict *theClustersDigis = theCluster->MemberDigiMap();
    
    Double_t numberOFneighbours(0.0);
    Double_t neighbourMaxE(0.0);
    
    while( (theNeighbourIterator !=  amongstTheseNeighbours.end()) && result ){
      EmcDigiPtrDict::const_iterator position = theClustersDigis->find(*theNeighbourIterator);
      if (position != theClustersDigis->end()) {
	PndEmcDigi *digi = position->second;
	double digiE(digi->GetEnergy());
	if(digiE>theDigiEnergy)
	  result=false;
	if(digiE>=neighbourMaxE)
	  neighbourMaxE=digiE;
	if(digiE>fNeighbourECut)
	  numberOFneighbours+=1.0;
      }
      ++theNeighbourIterator;
    }
    if(numberOFneighbours==0.0)
      return false;
    else {
      if(fERatioCorr>=fNeighbourECut){
	std::cout<< "Hi this is warning from your PndEmc2DLocMaxFinder,\n"
		 <<" please choose a smaller value for fERatioCorr (EmcMakeBump)"
		 << std::endl;
	return false;
      }
      else {
	if(fCutSlope*(numberOFneighbours-fCutOffset) <
	   (neighbourMaxE-fERatioCorr)/(theDigiEnergy-fERatioCorr))
	  result=false;
      }
    }
  }
  if (result) {
    std::cout << " Digi at (" << theDigi->GetThetaInt() << ", " 
	      << theDigi->GetPhiInt() << ") was a local max. Energy = "<<theDigi->GetEnergy()<< std::endl;
  }
  return result;
}

//		-----------------------------------------
// 		-- Private Function Member Definitions --
//		-----------------------------------------

void PndEmc2DLocMaxFinder::getNeighbourDigis( EmcCoordIndexSet &allDigiNeighbours, 
					      EmcCoordIndexSet &currentDigiNeighbours, 
					      int neighbourLevel,
					      const EmcDigiPtrDict * const theClusterDigis ) const
{
  EmcCoordIndexSet currentDigisCopy(currentDigiNeighbours);
  currentDigiNeighbours.clear();
  EmcCoordIndexSet::iterator theCurrentDigiIterator = currentDigisCopy.begin();
  EmcCoordIndexSet theNextDigiNeighbours;
  
  while ( theCurrentDigiIterator != currentDigisCopy.end() ) {
    PndEmcTwoCoordIndex *theCurrentTCI = *theCurrentDigiIterator;
    EmcDigiPtrDict::const_iterator theDigiIterator = (*theClusterDigis).begin(); 
    while( theDigiIterator != (*theClusterDigis).end() ) {
      PndEmcTwoCoordIndex *theTCI = theDigiIterator->first;
      //std::cout<<"theTCI->itsIndex() = "<<theTCI->itsIndex()<<std::endl;
      //std::cout<<"theCurrentTCI->itsIndex() = "<<theCurrentTCI->itsIndex()<<std::endl;
      bool isneighbour=	theCurrentTCI->IsNeighbour(theTCI);
      if(isneighbour)
	allDigiNeighbours.insert(theTCI);
      ++theDigiIterator;
    }
    ++theCurrentDigiIterator;}
}
