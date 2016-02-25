//----------------------------------------------------------------------
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//
// Author List:
//	Xiaorong Shi            Lawrence Livermore National Lab
//	Steve Playfer           University of Edinburgh
//	Stephen Gowdy           University of Edinburgh
//
//-----------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCCLUSTER_H
#define PNDEMCCLUSTER_H

#include "TObject.h"
#include "FairTimeStamp.h"
#include "TMatrixD.h"
#include "TVector3.h"
#include <vector>
#include <map>
		
class PndEmcDigi;
class PndEmcTwoCoordIndex;

struct LinkScoreBoard{
	Short_t score;

	void SetValShift(Bool_t val, Int_t shift){
		if (shift < 4)
			score |= val << shift;
	}

	Bool_t GetValShift(Int_t shift){ return score >> shift & 0x1;}
};


/**
 * @brief a cluster (group of neighboring crystals) of hit emc crystals
 * @ingroup PndEmc
 */
class PndEmcCluster : public FairTimeStamp
{

public:
	// Constructors
	PndEmcCluster();
	virtual void Print(const Option_t* opt ="") const;
	// Destructor
	virtual ~PndEmcCluster();
	
	// List of Digi indexes
	const std::vector<Int_t> &DigiList() const {return fDigiList;};
	std::vector<Int_t> &DigiList() {return fDigiList;};
	const std::map<Int_t,Int_t> &LocalMaxMap() const {return fLocalMaxMap;};
	const std::map<Int_t,Int_t> &MemberDigiMap() const {return fMemberDigiMap;};

	// Digi with largest energy in cluster
	virtual const PndEmcDigi* Maxima(const TClonesArray *digiArray) const;
	virtual PndEmcDigi* Maxima(const TClonesArray *digiArray);
	
	// Total Energy of Cluster
	virtual Double_t energy() const;
	// Theta centroid of cluster
	Double_t theta() const;
	// Phi   centroid of cluster
	Double_t phi() const;
	// Theta centroid of cluster in index units
	TVector3 position() const;
	// Where is cluster?
	TVector3 where() const;
	// x co-ordinate of clus
	Double_t x() const;
	// y co-ordinate of clus
	Double_t y() const;
	// z co-ordinate of clus
	Double_t z() const;
	// Cluster moments
	// Zernike moment (2,0)
	Double_t Z20() const {return fZ20;}
	// Zernike moment (5,3)
	Double_t Z53() const {return fZ53;}
	// Lateral energy deposition within the cluster
	Double_t LatMom() const {return fLatMom;}
	
	// Get Module in which cluster is located
	Short_t GetModule() const;


	bool IsEnergyValid() const {return fEnergyValid;};
	bool IsPositionValid() const {return fWhereValid;};
	
	Int_t NumberOfDigis() const;
	Int_t NBumps() const;
	
	bool isInCluster(PndEmcDigi* theDigi, const TClonesArray *digiArray);

	  // Distance from centre of cluster to a point
	virtual Double_t DistanceToCentre( const TVector3& aPoint ) const;
	virtual Double_t DistanceToCentre( const PndEmcDigi* aDigi ) const;
	// Returns the difference between the two phi angles, defined as the 
   // first minus the second, but deals with the overlap at 2 pi.
	static Double_t FindPhiDiff(Double_t, Double_t);

	// Modifiers
	virtual void addDigi(const TClonesArray *digiArray, Int_t iDigi);
	virtual void removeDigi(const TClonesArray *digiArray, Int_t iDigi);
	virtual std::vector<Int_t>::iterator removeDigi(const TClonesArray *digiArray, std::vector<Int_t>::iterator it);
	void addCluster( PndEmcCluster* cluster, const TClonesArray *digiArray);
	virtual void addLocalMax( const TClonesArray *digiArray, Int_t iDigi);
	virtual void addLocalMax( const PndEmcDigi *digi);
	virtual void SetNBumps( unsigned nbumps);
	void SetEnergy(Double_t en){fEnergy=en; fEnergyValid=true;}
	void SetPosition(TVector3 pos){fWhere=pos; fWhereValid=true;}
	void SetZ20(Double_t z20){fZ20=z20;}
	void SetZ53(Double_t z53){fZ53=z53;}
	void SetLatMom(Double_t latMom){fLatMom=latMom;}
	void SetTrackEntering(const FairMultiLinkedData& tracks){fTrackEntering = tracks;}
	void SetTrackExiting(const FairMultiLinkedData& tracks){fTrackExiting = tracks;}


	void AddTracksEnteringExiting(const FairMultiLinkedData& tracksEntering, const FairMultiLinkedData& tracksExiting);

		
	Double_t GetEnergyCorrected() const;

	const std::vector<Int_t> &GetMcList() const;
	Int_t GetMcSize()                     { return fMcList.size(); }
	Int_t GetMcIndex(Int_t i = 0)         { return fMcList[i]; }
	FairMultiLinkedData GetTrackEntering() const { return fTrackEntering;}
	FairMultiLinkedData GetTrackExiting() const { return fTrackExiting;}


private:
	friend class PndEmcMakeCluster;
	void invalidateCache(bool );

protected:

	void FillScoreBoard(FairMultiLinkedData tracks, std::map<FairLink, LinkScoreBoard>& scoreBoard, Int_t shift);

	std::vector<Int_t> fDigiList;
	mutable std::vector<Int_t> fMcList;
	mutable std::map<Int_t, Int_t> fMcMap;//<track, count>
	std::map<Int_t,Int_t> fMemberDigiMap; // Map <detId,digiIndex>
	std::map<Int_t,Int_t> fLocalMaxMap;   // Map<detId, digiIndex> for the maxima

	bool fEnergyValid;
	Double_t fEnergy;
	bool fWhereValid;
	TVector3 fWhere;
	unsigned fNbumps;
	Double_t fZ20; // Zernike moment (2,0)
	Double_t fZ53; // Zernike moment (5,3)
	Double_t fLatMom; // Lateral energy deposition within the cluster

	FairMultiLinkedData fTrackEntering;
	FairMultiLinkedData fTrackExiting;

	ClassDef(PndEmcCluster,3)
};

#endif // EMCCLUSTER_HH
