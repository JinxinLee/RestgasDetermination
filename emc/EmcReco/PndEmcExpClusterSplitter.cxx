//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class PndEmcExpClusterSplitter.
//      Implementation of ClusterSplitter which splits
//      on the basis of exponential distance from the bump centroid.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Adapted for the PANDA experiment at GSI
//
// Author List:
//      Phil Strother               
//
// Copyright Information:
//	Copyright (C) 1997               Imperial College
//
// Modified:
// M. Babai
//------------------------------------------------------------------------


//-----------------------
// This Class's Header --
//-----------------------
#include "PndEmcExpClusterSplitter.h"

//---------------
// C++ Headers --
//---------------
//#include <vector>
//#include <set>
//#include <map>
#include <iostream>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

#include "PndEmcTwoCoordIndex.h"

#include "PndEmcBump.h"
#include "PndEmcCluster.h"
#include "PndEmcDigi.h"
#include "PndEmcSharedDigi.h"
#include "PndEmcXtal.h"
using std::endl;

//----------------
// Constructors --
//----------------

PndEmcExpClusterSplitter::PndEmcExpClusterSplitter(PndEmcExpClusterSplitterData  expClusterSplitterData, 
						   Int_t verbose)
{
  fVerbose=verbose;
  
  fMoliereRadius=expClusterSplitterData.MoliereRadius; // Mr in cm
  fExponentialConstant=expClusterSplitterData.ExponentialConstant;
  // Energy fall off with distance from centre of cluster is
  // exp(-a*dist/Mr) Mr is the moliere radius.  dist is distance from
  // centre in cm, a is the above parameter.  The optimized value for
  // logarithimc cluster positioning is 2.5 For linear cluster
  // positioning a should be set to 1.5
  
  fMaxIterations=expClusterSplitterData.MaxIterations;
  // Set the max number of iterations that the splitting algorithm is allowed 
  // to do before it decides that enough is enough.
  
  fCentroidShift=expClusterSplitterData.CentroidShift;
  // Set the tolerance level to which it is said that a bump is 
  // said not to have moved since the last iteration.  
  // The default is a millimetre.
  
  fMaxBumps=expClusterSplitterData.MaxBumps;
  // Set an upper limit on the number of bumps allowed in a cluster if it 
  // is to be split.
  
  fMinDigiEnergy=expClusterSplitterData.MinDigiEnergy;
  // Set minimum SharedDigi energy to 20keV.
}

//--------------
// Destructor --
//--------------

PndEmcExpClusterSplitter::~PndEmcExpClusterSplitter(){}

void PndEmcExpClusterSplitter::splitCluster(const std::set<PndEmcTwoCoordIndex*> &theMaximaDigis, 
					    const PndEmcCluster * const theCluster, Int_t clusterIndex,
					    std::vector<PndEmcBump*> &theBumpList) const
{
  
  if (theBumpList.size()!=0) {
    std::cout << "Error.  In PndEmcExpClusterSplitter:"
	      <<"  this cluster has already been split."  << std::endl;
    return;
  }
  
  if (fVerbose>=3){
    std::cout<<"Cluster position: theta = "
	     <<theCluster->where().Theta()
	     <<", phi = "<<theCluster->where().Phi()<<std::endl;
  }
  
  int numberOfBumps = -1;
  numberOfBumps = theMaximaDigis.size();
  
  if (numberOfBumps<=1 || numberOfBumps >= fMaxBumps){
    // Limit the max number of bumps in the cluster to 8 (default)
    // in this case, we clearly have a cluster, but no bumps to speak of.  
    // Make 1 bump with weights all equal to 1
    
    EmcDigiPtrDict::const_iterator theDigiIterator;//=theCluster->MemberDigiMap()->begin();
    
    PndEmcBump* theNewBump = new PndEmcBump(); 
    theNewBump->MadeFrom(clusterIndex);
    
    //while (theDigiIterator != theCluster->MemberDigiMap()->end()){
    for(theDigiIterator = theCluster->MemberDigiMap()->begin();
	theDigiIterator != theCluster->MemberDigiMap()->end(); ++theDigiIterator){
      //PndEmcDigi *theCurrentDigi = theDigiIterator->second;
      //PndEmcDigi* sharedDigi= new PndEmcSharedDigi( *theCurrentDigi, 1.0 );
      PndEmcDigi* sharedDigi= new PndEmcSharedDigi(*(theDigiIterator->second), 1.0 );            
      theNewBump->addDigi(sharedDigi);
      //++theDigiIterator;
    }
    theBumpList.push_back(theNewBump);
  }
  else {
    std::set<PndEmcTwoCoordIndex*>::iterator theMaximaDigisIterator;// =  theMaximaDigis.begin();
    std::map<PndEmcTwoCoordIndex*, TVector3*> theCentroidPoints;
    std::map<PndEmcTwoCoordIndex*, TVector3*> theMaximaPoints;
    std::map<PndEmcTwoCoordIndex*, PndEmcBump*> theIndexedBumps;
    std::map<PndEmcTwoCoordIndex*, TVector3*> theAllDigiPoints;
    
    EmcDigiPtrDict *theDigiDict = (EmcDigiPtrDict *)(theCluster->MemberDigiMap());
    
    double totalEnergy=0;
    
    //while (theMaximaDigisIterator != theMaximaDigis.end()) {
    for(theMaximaDigisIterator = theMaximaDigis.begin(); 
	theMaximaDigisIterator != theMaximaDigis.end();
	++theMaximaDigisIterator){
      PndEmcTwoCoordIndex *theTCI = *theMaximaDigisIterator;
      PndEmcDigi *theMaxDigi = theDigiDict->find(theTCI)->second;
      totalEnergy += theMaxDigi->GetEnergy();
      
      TVector3 *digiLocation = new TVector3(theMaxDigi->where());
      TVector3 *sameLocation = new TVector3(theMaxDigi->where());
      
      theMaximaPoints.insert(std::map<PndEmcTwoCoordIndex*, TVector3*>::value_type(theTCI, digiLocation));
      theCentroidPoints.insert(std::map<PndEmcTwoCoordIndex*, TVector3*>::value_type(theTCI, sameLocation));
      //++theMaximaDigisIterator;
    }

    EmcDigiPtrDict::iterator theAllDigisIterator;// = (*theDigiDict).begin();
    
    // This loop works out the location of all the digis in the
    // cluster int numberOfDigis = theDigiDict->size();

    //while (theAllDigisIterator != (*theDigiDict).end()){
    for( theAllDigisIterator = (*theDigiDict).begin(); theAllDigisIterator != (*theDigiDict).end();
	 ++theAllDigisIterator){
      PndEmcTwoCoordIndex *theTCI = theAllDigisIterator->first;
      TVector3 *digiLocation = new TVector3(theAllDigisIterator->second->where());
      theAllDigiPoints.insert(std::map<PndEmcTwoCoordIndex*, TVector3*>::value_type( theTCI, digiLocation ));
      //++theAllDigisIterator;
    }

    theMaximaDigisIterator = theMaximaDigis.begin();
    
    // Now we can create the EmcBumps
    
    // The algorithm is as follows: We will index each bump by its
    // maximum digi's PndEmcTwoCoordIndex.  We will set up a list of
    // bump centroids which to start with will be synonymous with the
    // location of the maxima.  We then apportion a weight to each
    // digi, according to its distance from the centroids.  We then
    // construct the bumps according to these weights, which will
    // presumably give a different set of centroids.  This is repeated
    // until the centroids are static within tolerance, or we reach
    // the maximum number of iterations.
    
    Int_t iterations = 0;
    
    Double_t averageCentroidShift;
    
    do {
      if (fVerbose>=3){
	std::cout<<"iteration No "<<iterations<<std::endl;
      }
      //Int_t bumpsSoFar=0;
      averageCentroidShift=0.0;
      
      // First clean up the old bumps
      std::map<PndEmcTwoCoordIndex*, PndEmcBump*>::iterator theBumpKiller = theIndexedBumps.begin();
      while(theBumpKiller != theIndexedBumps.end()){
	PndEmcBump *lambToSlaughter = theBumpKiller->second;
	EmcDigiPtrDict *digiList =  (EmcDigiPtrDict *)(lambToSlaughter->MemberDigiMap());
	EmcDigiPtrDict::iterator digiKiller = (*digiList).begin();
	while (digiKiller != (*digiList).end()) {
	  delete digiKiller->second;
	  ++digiKiller;
	}
	
	delete lambToSlaughter;
	++theBumpKiller;
      }
      theIndexedBumps.clear();
      
      // Then loop over all the maxima and assign weights accordingly
      for (theMaximaDigisIterator = theMaximaDigis.begin();
	   theMaximaDigisIterator != theMaximaDigis.end();
	   ++theMaximaDigisIterator) {
	PndEmcTwoCoordIndex *theCurrentMaximaTCI = *theMaximaDigisIterator;
	
	if (fVerbose>=3){
	  std::cout<<"***************** current maximum: theta = "<<theCurrentMaximaTCI->XCoord()
		   <<", phi = "<<theCurrentMaximaTCI->YCoord()<<"*********"<<std::endl;
	}
	
	// Create the bump which will correspond the this digi maxima
	PndEmcBump* theNewBump = new PndEmcBump;
	theNewBump->MadeFrom(clusterIndex);
	
	theIndexedBumps.insert(std::map<PndEmcTwoCoordIndex*, 
			       PndEmcBump*>::value_type(theCurrentMaximaTCI, theNewBump));
	
	// Now we will look over all the digis and add each of them
	// to this Bump with an appropriate weight
	
	for (theAllDigisIterator = (*theDigiDict).begin();
	     theAllDigisIterator != (*theDigiDict).end();++theAllDigisIterator) {
	  
	  PndEmcDigi *theCurrentDigi = theAllDigisIterator->second;
	  PndEmcTwoCoordIndex *theCurrentTCI = theAllDigisIterator->first;
	  
	  Double_t weight;
	  
	  // We are on the first pass and the digi is not a local max, or we are not on the
	  // first pass.   Assign a weight according to the distance from the centroid position.
	  
	  Double_t myEnergy = 0;
	  Double_t myDistance = 0;
	  
	  // Now share the digi out according to its distance from the maxima, 
	  // and the maxima energies
	  
	  Double_t totalDistanceEnergy=0;
	  
	  std::map<PndEmcTwoCoordIndex*,TVector3*>::iterator theMaxPointsIterator;// = theCentroidPoints.begin();
	  //while(theMaxPointsIterator != theCentroidPoints.end()){
	  for(theMaxPointsIterator = theCentroidPoints.begin(); theMaxPointsIterator != theCentroidPoints.end();
	      ++theMaxPointsIterator){
	    PndEmcTwoCoordIndex *theMaxPointsTCI =theMaxPointsIterator->first; 
	    
	    TVector3 *theMaxPoint = theMaxPointsIterator->second;
	    TVector3 *theCurrentDigiPoint = theAllDigiPoints.find(theCurrentTCI)->second;
	    
	    Double_t theDistance;
	    
	    // This next bit just checks to see if the maxima point in
	    // hand is the same as the crystal from which we are
	    // trying to find distance - just an FP trap really.
	    
	    if ((*theCurrentTCI)==(*theMaxPointsTCI)){
	      theDistance=0.0;
	    } else {
	      TVector3 distance( *theMaxPoint - *theCurrentDigiPoint);
	      
	      theDistance = distance.Mag();
	    }
	    
	    if (*theCurrentMaximaTCI == *(theMaxPointsTCI)){
	      // i.e. the maximum we are trying to find the distance from is 
	      // the one for which we are currently trying to make a bump
	      myDistance = theDistance;
	      myEnergy = theDigiDict->find(theMaxPointsTCI)->second->GetEnergy();
	    }
	    
	    totalDistanceEnergy += theDigiDict->find(theMaxPointsTCI)->second->GetEnergy() *
	      exp(-fExponentialConstant * theDistance/fMoliereRadius);
	    
	    //++theMaxPointsIterator;
	  }
	  
	  if(totalDistanceEnergy > 0.0)
	    weight = myEnergy*exp(-fExponentialConstant* 
				  myDistance/fMoliereRadius) / ( totalDistanceEnergy);
	  else 
	    weight=0;
	  
	  if (fVerbose>=3){
	    std::cout<<"\t digi theta = "<<theCurrentDigi->GetTCI()->XCoord()
		     <<", phi = "<<theCurrentDigi->GetTCI()->YCoord()<<std::endl;
	    std::cout<<"energy = "<<theCurrentDigi->GetEnergy()<<", weight = "<< weight<<std::endl;
	  }
	  
	  PndEmcDigi* sharedDigi= new PndEmcSharedDigi( *theCurrentDigi, weight );
	  
	  if (fVerbose>=3){
	    std::cout<<"shared digi energy = "<<sharedDigi->GetEnergy()<<std::endl;
	  }
	  
	  if( sharedDigi->GetEnergy() > fMinDigiEnergy){
	    theNewBump->addDigi( sharedDigi );
	  } else {
	    delete sharedDigi;
	  }  
	}
	
	// Compute the shift of the centroid we have just calculated
	TVector3 *theOldCentroid = theCentroidPoints.find(theCurrentMaximaTCI)->second;
	
	TVector3 centroidShift(*theOldCentroid - theNewBump->where());
	averageCentroidShift+=centroidShift.Mag();
      }
      
      averageCentroidShift/=(Double_t)numberOfBumps;
      
      // Put the new centroids in the list of centroid points,
      // remembering to delete the old ones.
      std::map<PndEmcTwoCoordIndex*, TVector3*>::iterator 
	theCentroidPointsIterator = theCentroidPoints.begin();
      //while (theCentroidPointsIterator != theCentroidPoints.end()) {
      for(theCentroidPointsIterator = theCentroidPoints.begin();
	  theCentroidPointsIterator != theCentroidPoints.end(); 
	  ++theCentroidPointsIterator) {
	delete theCentroidPointsIterator->second;
	//++theCentroidPointsIterator;
      }
      theCentroidPoints.clear();
      
      theCentroidPointsIterator = theCentroidPoints.begin();
      std::map<PndEmcTwoCoordIndex*, PndEmcBump*>::iterator theIndexedBumpsIterator;// = theIndexedBumps.begin();
      
      //while (theIndexedBumpsIterator != theIndexedBumps.end()){
      for(theIndexedBumpsIterator = theIndexedBumps.begin(); 
	  theIndexedBumpsIterator != theIndexedBumps.end(); ++theIndexedBumpsIterator){
	TVector3 *theNewCentroid = new TVector3(theIndexedBumpsIterator->second->where());
	theCentroidPoints.insert(std::map<PndEmcTwoCoordIndex*, TVector3*>::value_type(theIndexedBumpsIterator->first,
										       theNewCentroid));
	//++theIndexedBumpsIterator;
      }
      iterations++;
      
    } while (iterations < fMaxIterations && averageCentroidShift > fCentroidShift);
    //End of do loop
    
    
    // Finally append the new bumps to the list we are returning.
    std::map<PndEmcTwoCoordIndex*, PndEmcBump*>::iterator theBumpsIterator;// = theIndexedBumps.begin();
    PndEmcBump *theNextBump;
    //while(theBumpsIterator != theIndexedBumps.end()){
    for(theBumpsIterator = theIndexedBumps.begin(); theBumpsIterator != theIndexedBumps.end();
	++theBumpsIterator){
      theNextBump = theBumpsIterator->second;
      //theNextBump->sortDigis();
      theBumpList.push_back(theNextBump);
      //++theBumpsIterator;
    }
    
    std::map<PndEmcTwoCoordIndex*,TVector3*>::iterator theGrimReaper = theMaximaPoints.begin();
    //while(theGrimReaper != theMaximaPoints.end()) {
    for(theGrimReaper = theMaximaPoints.begin();
	theGrimReaper != theMaximaPoints.end();
	++theGrimReaper){
      delete theGrimReaper->second;
      //++theGrimReaper;
    }
    theMaximaPoints.clear();
    
    //theGrimReaper  = theAllDigiPoints.begin();
    //while(theGrimReaper != theAllDigiPoints.end()) {
    for(theGrimReaper  = theAllDigiPoints.begin();
	theGrimReaper != theAllDigiPoints.end();
	++theGrimReaper){
      delete theGrimReaper->second;
      //++theGrimReaper;
    }
    theAllDigiPoints.clear();
    
    //theGrimReaper = theCentroidPoints.begin();
    //while(theGrimReaper != theCentroidPoints.end()){
    for(theGrimReaper = theCentroidPoints.begin(); 
	theGrimReaper != theCentroidPoints.end();
	++theGrimReaper){
      delete theGrimReaper->second;
      //++theGrimReaper;
    }
    theCentroidPoints.clear();
  }
}
