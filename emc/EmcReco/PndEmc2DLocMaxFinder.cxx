//--------------------------------------------------------------------------
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

#include "PndEmcClusterProperties.h"
#include "PndEmcTwoCoordIndex.h"
#include "PndEmcDigi.h"
#include "PndEmcCluster.h"
#include "PndEmcRecoPar.h"
#include "PndEmcGeoPar.h"
#include "PndEmcDigiPar.h"
#include "PndEmcStructure.h"
#include "PndEmcMapper.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"


#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;

//----------------
// Constructors --
//----------------

PndEmc2DLocMaxFinder::PndEmc2DLocMaxFinder(Int_t verbose): fClusterArray(new TClonesArray()), fDigiArray(new TClonesArray()), fGeoPar(new PndEmcGeoPar()), fDigiPar(new PndEmcDigiPar()), fRecoPar(new PndEmcRecoPar()), fPersistance(kFALSE), fMaxECut(0), fNeighbourECut(0), fCutSlope(0), fCutOffset(0), fERatioCorr(0), fTheNeighbourLevel(0), fVerbose(verbose)
{
}

//--------------
// Destructor --
//--------------

PndEmc2DLocMaxFinder::~PndEmc2DLocMaxFinder()
{
//   delete fGeoPar;
//   delete fDigiPar;
//   delete fRecoPar;
}

// -----   Public method Init   -------------------------------
InitStatus PndEmc2DLocMaxFinder::Init() {
  
	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman ){
		cout << "-E- PndEmcMakeBump::Init: "
		<< "RootManager not instantiated!" << endl;
		return kFATAL;
	}
		
	// Geometry loading
	fGeoPar->InitEmcMapper();
	PndEmcStructure::Instance();
	
	// Get input array
	fDigiArray = (TClonesArray*) ioman->GetObject("EmcDigi");
	if ( ! fDigiArray ) {
		cout << "-W- PndEmcMakeCluster::Init: "
		<< "No PndEmcDigi array!" << endl;
		return kERROR;
	}
	
	fClusterArray = (TClonesArray*) ioman->GetObject("EmcCluster");
	if ( ! fClusterArray ) {
		cout << "-W- PndEmcMakeBump::Init: "
		<< "No PndEmcCluster array!" << endl;
		return kERROR;
	}
	
	fMaxECut=fRecoPar->GetMaxECut();
	fNeighbourECut=fRecoPar->GetNeighbourECut();
	fCutSlope=fRecoPar->GetCutSlope();
	fCutOffset=fRecoPar->GetCutOffset();
	fERatioCorr=fRecoPar->GetERatioCorr();
	//                                            ^
	//                                            |  ....             /
	//                                         1.0|   ....           /|
	//                                            |                 / | fCutSlope
	//                                            |  splitoffs     /  |       
	//          MaxE of neighbours - fERatioCorr  |    and        /___| 
	// ERatio =  -------------------------------  |  hadrons     /..
	//                    MaxE-fERatioCorr        |             / ..
	//                                            |  ....      /  ..
	//                                            |   ....    /  ...  <-- merged pions
	//                                            |          /  ....      and photons
	//                                            |  ...    /   ....  
	//                                         0.0|        /    ....
	//                                            |------------------->
	//                                             0     /     6 7 8
	//                                            <------->             number of neighbours
	//                                            fCutOffset             with energy > fNeighbourECut
		
	fTheNeighbourLevel=fRecoPar->GetTheNeighbourLevel();
	// This is the range of the search to figure out whether a particular 
	// digi is a local max or not.    Which neighbours are
	// looked at is decided by this parameter:  
	//        0 = nearest neighbours (this is all logical neighbours including corners)
	//        1 = nearest and next nearest neighbours 
	//        etc.  
	
	cout << "-I- PndEmc2DLocMaxFinder: Intialization successfull" << endl;
}

void PndEmc2DLocMaxFinder::Exec(Option_t* opt)
{
	int nClusters = fClusterArray->GetEntriesFast();
	PndEmcCoordIndexSet tmp_CoordSet_set;
	
	PndEmcMapper *fEmcMap=PndEmcMapper::Instance();
	
	//loop over Clusters
	for (Int_t iCluster = 0; iCluster < nClusters; iCluster++){
		PndEmcCluster* theCluster = (PndEmcCluster*) fClusterArray->At(iCluster);

		// map <detId,digiIndex>
		std::map<Int_t, Int_t> theClustersDigis = theCluster->MemberDigiMap();
		std::map<Int_t, Int_t>::iterator theDigiIterator = theClustersDigis.begin(); 
		
		PndEmcCoordIndexSet allTheNeighbours;
		PndEmcCoordIndexSet theNewNeighbours;
		
		if (theClustersDigis.size()==1){
			while( theDigiIterator != theClustersDigis.end() ){
				theCluster->addLocalMax(fDigiArray, theDigiIterator->second);
				++theDigiIterator;
			}
		} 
		else{
			while( theDigiIterator != theClustersDigis.end() ){
				Int_t detId = theDigiIterator->first;
				PndEmcTwoCoordIndex *theTCI = fEmcMap->GetTCI(detId);
				
				allTheNeighbours.clear();
				theNewNeighbours.clear();
				
				// Start the newneighbours off, then get all the neighbours from
				// this.  This is kind of curious, since the seed digi shouldn't
				// end up in the neighbour list, but it should work
				theNewNeighbours.insert(theTCI);
				
				getNeighbourDigis(allTheNeighbours, theNewNeighbours, 
					fTheNeighbourLevel, theClustersDigis);
				PndEmcDigi *theDigi = (PndEmcDigi *) fDigiArray->At(theDigiIterator->second);
				if (isALocalMax(theDigi, theCluster, allTheNeighbours)){
					theCluster->addLocalMax( fDigiArray, theDigiIterator->second);
				}
				++theDigiIterator;
			}
			// If no local maxima was added simply add the absolute maxima 
			if ((theCluster->LocalMaxMap()).size()==0)
			{
				theCluster->addLocalMax(theCluster->Maxima(fDigiArray));
			}
		}
	}
	
}

bool PndEmc2DLocMaxFinder::isALocalMax( const PndEmcDigi *const theDigi, const PndEmcCluster * const theCluster, 
const PndEmcCoordIndexSet &amongstTheseNeighbours ) const
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
		PndEmcCoordIndexSet::const_iterator theNeighbourIterator = amongstTheseNeighbours.begin();
    
		const std::map<Int_t, Int_t> theClustersDigis = theCluster->MemberDigiMap();
    
		Double_t numberOFneighbours(0.0);
		Double_t neighbourMaxE(0.0);
		
		while( (theNeighbourIterator !=  amongstTheseNeighbours.end()) && result ){
			
			std::map<Int_t, Int_t>::const_iterator position = theClustersDigis.find((*theNeighbourIterator)->Index());
			
			if (position != theClustersDigis.end()) {
				PndEmcDigi *digi = (PndEmcDigi *) fDigiArray->At(position->second);
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
		
	if ((result)&&(fVerbose>0)) {
		std::cout << " Digi at (" << theDigi->GetThetaInt() << ", " 
				<< theDigi->GetPhiInt() << ") was a local max. Energy = "<<theDigi->GetEnergy()<< std::endl;
	}
	return result;
}

//		-----------------------------------------
// 		-- Private Function Member Definitions --
//		-----------------------------------------

void PndEmc2DLocMaxFinder::getNeighbourDigis( PndEmcCoordIndexSet &allDigiNeighbours, 
					      PndEmcCoordIndexSet &currentDigiNeighbours, 
					      int neighbourLevel,
					      std::map<Int_t,Int_t> theClusterDigis ) const
{
	PndEmcMapper *fEmcMap=PndEmcMapper::Instance();
	
	PndEmcCoordIndexSet currentDigisCopy(currentDigiNeighbours);
	currentDigiNeighbours.clear();
	PndEmcCoordIndexSet::iterator theCurrentDigiIterator = currentDigisCopy.begin();
	PndEmcCoordIndexSet theNextDigiNeighbours;
	
	while ( theCurrentDigiIterator != currentDigisCopy.end() ) {
		PndEmcTwoCoordIndex *theCurrentTCI = *theCurrentDigiIterator;
 
		std::map<Int_t, Int_t>::const_iterator theDigiIterator = theClusterDigis.begin(); 
		while( theDigiIterator != theClusterDigis.end() ) {
			Int_t detId = theDigiIterator->first;
			PndEmcTwoCoordIndex *theTCI = fEmcMap->GetTCI(detId);
			bool isneighbour=	theCurrentTCI->IsNeighbour(theTCI);
			if(isneighbour)
				allDigiNeighbours.insert(theTCI);
			++theDigiIterator;
		}
	++theCurrentDigiIterator;}
}

void PndEmc2DLocMaxFinder::SetParContainers() {

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");
  // Get Emc digitisation parameter container
  fGeoPar = (PndEmcGeoPar*) db->getContainer("PndEmcGeoPar");
  // Get Emc digitisation parameter container
  fDigiPar = (PndEmcDigiPar*) db->getContainer("PndEmcDigiPar");
  // Get Emc reconstruction parameter container
  fRecoPar = (PndEmcRecoPar*) db->getContainer("PndEmcRecoPar");
}

ClassImp(PndEmc2DLocMaxFinder)
