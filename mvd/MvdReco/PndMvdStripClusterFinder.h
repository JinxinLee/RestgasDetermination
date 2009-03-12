// -------------------------------------------------------------------------
// -----             PndMvdStripClusterBuilder header file             -----
// -----          Converted 27.11.2007 from R.Jaekel by R.Kliemt       -----
// -------------------------------------------------------------------------


/** PndMvdStripCluster.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 *@author L.Ackermann
 **
 ** Calculator class to find clusters in channel dimension and time dimension
 ** for the MVD strip sensors
 **/

#ifndef PNDMVDSTRIPCLUSTERFINDER_H
#define PNDMVDSTRIPCLUSTERFINDER_H


#include <vector>
#include <map>
#include <string>

#include "PndMvdDigiStrip.h"
#include "PndMvdCluster.h"
#include "PndMvdStripClusterBuilder.h"
// #include "PndMvdStripCluster.h"

/**   
 @class PndMvdStripClusterFinder : public PndMvdStripClusterBuilder 
 @brief Find Clusters on a strip sensor in two dimensions
 
 Search for clusters in channel and trigger timestamp dimension. It is possible to set a whole e.g. defect strip.
 @author Lars Ackermann
 @author Hans-Georg Zaunick
 @author Ralf Kliemt
 @date 11.03.2009
*/
class PndMvdStripClusterFinder : public PndMvdStripClusterBuilder 
{
 public:
	/** default constructor **/
	PndMvdStripClusterFinder();

	/**
	main constructor Set number of maximum missing channels or time to get the cluster
	@param NrofmissedChannels maximum of missing channels
	@param NrofmissedTimestamps maximum of missing time
	*/
	PndMvdStripClusterFinder(Int_t NrofmissedChannels, Int_t NrofmissedTimestamps);

	/** Destructor **/
	~PndMvdStripClusterFinder();

	/**
	@fn std::vector< PndMvdCluster > SearchClusters()

	calculate clusters in the two dimensional way
	@return vector of clusters
	*/
	std::vector< PndMvdCluster > SearchClusters();

private:

	/**
	@fn void ClusterIterator(Int_t newCh, Int_t newFrameID, std::map<Int_t, std::map<Int_t, std::vector<Int_t> > >& hitmap, std::vector< Int_t >& onecluster)

	iterates through the hitlist and find clusters. It will stop when reaching the end of hitlist or the maximum of channels or time is reached so that this cluster has no more entries
	@param newCh the current channel to search for
	@param newFrameID the current time to search for
	@param hitmap the hitlist where the first element in the vector is negative for already identified hits the second member of the vector knows the hitID
	@param onecluster list of hitIDs belonging to this cluster
	@return void
	*/
	void ClusterIterator(Int_t newCh, Int_t newFrameID, std::map<Int_t, std::map<Int_t, std::vector<Int_t> > >& hitmap, std::vector< Int_t >& onecluster);

	/// maximum of missing channels
	Int_t fNmCh;

	/// maximum of missing time
	Int_t fNmTS;

ClassDef(PndMvdStripClusterFinder,1);
};



#endif // PndMvdSimpleStripClusterFinder
