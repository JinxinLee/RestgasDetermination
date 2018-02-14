//#pragma once
#ifndef PNDEMCCLUSTERADDUPDUPLCRYS_H_
#define PNDEMCCLUSTERADDUPDUPLCRYS_H_

#include "FairTask.h"
#include <vector>		

class TClonesArray;
class TObjectArray;
class PndEmcDigi;
class PndEmcCluster;
class PndEmcGeoPar;
class PndEmcDigiPar;
class PndEmcRecoPar;

/**
 * @brief Task to remove duplicate crystals
 * 
 * In timebased cluster reconstruction, same detectorId might appear multiple times in the same cluster, 
 * which could cause undefined behavior in classical (eventbased) routines.
 * The class provides a workaround, adding up the energies of all digis with the same origin (=crystal, detectorId). 
 * (One could also think off dumping the second hit, etc...)
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 */
class PndEmcClusterRemoveDuplCrys : public FairTask
{


	public:

		// Constructor
		PndEmcClusterRemoveDuplCrys(Int_t verbose=0);

		// Destructor
		virtual ~PndEmcClusterRemoveDuplCrys() {};

		/** Virtual method Init **/
		virtual InitStatus Init();

		/** Virtual method Exec **/
		virtual void Exec(Option_t* opt);

private:
  // don't allow copying (-Weffc++)
  PndEmcClusterRemoveDuplCrys(const PndEmcClusterRemoveDuplCrys&);	// no implementation
  PndEmcClusterRemoveDuplCrys& operator= (const PndEmcClusterRemoveDuplCrys&);	// no implementation


	private:

		//PndEmcGeoPar*     fGeoPar;       /** Geometry parameter container **/
		//PndEmcDigiPar*    fDigiPar;      /** Digitisation parameter container **/
		//PndEmcRecoPar*    fRecoPar;      /** Reconstruction parameter container **/

		/** Get parameter containers **/
		//virtual void SetParContainers();

		/** Verbosity level **/

		TClonesArray* fClusterArray;
		TClonesArray* fDigiArray;

		ClassDef(PndEmcClusterRemoveDuplCrys,1)
};



#endif
