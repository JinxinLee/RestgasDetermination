//----------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
// Description:
//	Class Template for PndEmcCluster
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//
// Author List:
//	Xiaorong Shi            Lawrence Livermore National Lab
//	Steve Playfer           University of Edinburgh
//	Stephen Gowdy           University of Edinburgh
//
//-----------------------------------------------------------------------
#pragma once
#ifndef PNDEMCCLUSTER_H
#define PNDEMCCLUSTER_H

#include "TObject.h"
#include "PndEmcDataTypes.h"
#include "PndEmcClusterLiloPos.h"
#include "TMatrixD.h"
#include <vector>
		
class PndEmcClusterLiloPos;
class PndEmcClusterLinearPos;
class PndEmcClusterEnergySums;
class PndEmcClusterMoments;
class PndEmcXClMoments;
class PndEmcClusterDistances;
class PndEmcDigi;
class PndEmcTwoCoordIndex;
class TVector3;


class PndEmcCluster : public TObject
{

public:

	// Typedefs, consts, and enums
	
	enum CentroidMethod { lilo, linear };
	
	// Constructors
	PndEmcCluster();
	
	PndEmcCluster(Int_t digiList, Int_t localMaxList);
	
	virtual void Print(const Option_t* opt ="") const;
	
	// Destructor
	virtual ~PndEmcCluster();
	
	// List of Digis
	std::vector<PndEmcDigi*> DigiList() const {return fDigiList;};
	const PndEmcDigiPtrDict *MemberDigiMap() const {return fMemberDigiMap;};
	// Since fMemberDigiMap is transient element the following methods resore it after reading from the root file
	
	void ValidateDigiMap();
		
	// Total Energy of Cluster
	virtual Double_t energy() const;
	
	// Digi with largest energy in cluster
	virtual const PndEmcDigi* Maxima() const;
	virtual PndEmcDigi* Maxima();
		
	// Theta centroid of cluster
	Double_t theta() const;
	// Phi   centroid of cluster
	Double_t phi() const;
	
	// Theta centroid of cluster in index units
	Double_t thetaIndex() const;
	//Int_t    thetaIndexInt() const;  // not implemented
	// Phi   centroid of cluster in index units 
	Double_t phiIndex() const;
	//Int_t    phiIndexInt() const;  // not implemented
	
	// Direction to ctr clus
	TVector3 position() const;

	// Where is cluster?
	TVector3 where() const;
	
	// x co-ordinate of clus
	Double_t x() const;
	// y co-ordinate of clus
	Double_t y() const;
	// z co-ordinate of clus
	Double_t z() const;
	
	Int_t NumberOfDigis() const;
	
	Int_t NBumps() const;
	
	bool isInCluster(PndEmcDigi* theDigi);

	  // Distance from centre of cluster to a point
	virtual Double_t DistanceToCentre( const TVector3& aPoint ) const;
	virtual Double_t DistanceToCentre( const PndEmcDigi* aDigi ) const;

	// Modifiers
	
	virtual void addDigi( PndEmcDigi* d );
	void addCluster( PndEmcCluster* c);
	virtual void SetNBumps( unsigned nbumps);
	
	//  virtual const EmcClusterLogPos& logPositions() const;
	virtual const PndEmcClusterLiloPos& liloPositions() const;
	virtual const PndEmcClusterLinearPos& linearPositions() const;
	
	virtual const PndEmcClusterEnergySums& Esums() const;
	virtual const PndEmcClusterMoments& Moments() const;
	virtual const PndEmcClusterDistances& Distances() const;
	virtual const PndEmcXClMoments& Xmoments() const;
	
	// Angle of major axis wrt phi
	virtual Double_t Major_axis() const;
	
	// Centre of gravity centroid method/
	static TVector3 GravWhere( const PndEmcCluster* me );
	
	// Returns the difference between the two phi angles, defined as the 
   // first minus the second, but deals with the overlap at 2 pi.
	static Double_t FindPhiDiff(Double_t, Double_t);
	
	// Error Matrix for Cluster
	TMatrixD GetErrorMatrix() const; // 4x4 matrix in E, theta, phi, R
	TMatrixD Get4MomentumErrorMatrix() const; // 4x4 matrix in (px, py, pz, E)
	
	virtual Double_t Mass() const; // Claster mass
	
	Double_t GetEnergyCorrected() const;

	const std::vector<Int_t> &GetMcList() {return fMcList;}
	Int_t GetMcSize()                     { return fMcList.size(); }
	Int_t GetMcIndex(Int_t i = 0)         { return fMcList[i]; }
private:
	
	friend class PndEmcMakeCluster;
	
	void invalidateCache();

protected:
	unsigned fNbumps;
  
	std::vector<PndEmcDigi*> fLocalMaxList;
	std::vector<PndEmcDigi*> fDigiList;
	PndEmcDigiPtrDict* fMemberDigiMap;//!

	std::vector <Int_t> fMcList;

	// Cachable data
	mutable bool fEnergyValid;
	mutable Double_t fEnergy;
	mutable bool fWhereValid;
	mutable TVector3* fWhere;
	
	mutable PndEmcClusterLiloPos* fTheClusLiloPos; //! transient element
	mutable PndEmcClusterLinearPos* fTheClusLinearPos; //! transient element
	mutable PndEmcClusterEnergySums* fTheClusEnergySums; //! transient element
	mutable PndEmcClusterMoments* fTheClusMoments; //! transient element
	mutable PndEmcXClMoments* fTheClusXClMoments; //! transient element
	mutable PndEmcClusterDistances* fTheClusDistances; //! transient element


  // Modifiers
private:

	// Methods
	
	// Select which method to use for cluster centroid
	static void selectCentroidMethod( CentroidMethod, PndEmcClusterLiloPosData ClusterPositionParameters );
	
	// Used by normal where() method to determine centroid taking Int_to account which method is selected 
	static TVector3 algWhere( const PndEmcCluster* me );
	
	// Retains information about which method to use for centroid
	// Returns a pointer to a function which takes a const EmcCluster*
	//  as an argument
	static TVector3 (*&algPointer())( const PndEmcCluster* );
	
	ClassDef(PndEmcCluster,1)
};


// helper function to convert error matrix
// copied from BbrGeom/BbrError.cc
TMatrixD similarityWith(const TMatrixD& mat, const TMatrixD& m1);
#endif // EMCCLUSTER_HH
