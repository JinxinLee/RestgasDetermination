#ifndef _PNDSDSCHARGEWEIGHTINGALGORITHMS_H
#define _PNDSDSCHARGEWEIGHTINGALGORITHMS_H

#include <vector>
#include "TClonesArray.h"
#include "PndSdsDigiStrip.h"
#include "PndSdsChargeConversion.h"
class PndSdsCluster;
//class RunInfo;
class PndSdsCalcStrip;


class PndSdsChargeWeightingAlgorithms
{
/**\addtogroup algorithms
 @brief contains the three algorithms to find to track

 The three algorithms are Center of gravity, head tail and using the eta function
 @author Lars Ackermann
 @date 07.11.2008
 cluster_reco
	  */
/**@{*/

public:
	PndSdsChargeWeightingAlgorithms(TClonesArray* arr);
	~PndSdsChargeWeightingAlgorithms();

	   /**
	 @fn double center_of_gravity(const StripCluster& Cluster)

       calculate the coordinate of particle crossing the sensor, by using the algorithm center of gravity:
       \n \f$x_g=\sum^n_{k=1}\frac{q_k\cdot ch_k\cdot pitch}{\sum_{i=1}^n q_i}\f$
       @param Cluster investigationg cluster
       @return coordinate
       @example
  	   @code
	  	center_of_gravity(cluster);
  	   @endcode
       */
  std::pair<double,double> center_of_gravity(const PndSdsCluster& Cluster);

         /**
	 @fn std::pair<double,double> head_tail(const StripCluster& Cluster)

       calculate the coordinate of particle crossing the sensor, by using the head tail algorithm:
       \n \f$x_{ht}=\sum^n_{k=1}\left(\frac{x_{head}+x_{tail}}{2}+\frac{q_{head}-q_{tail}}{2\cdot q_\theta}\cdot pitch\right)\f$
       @param Cluster investigationg cluster
       @return coordinate
       @example
  	   @code
	  	head_tail(cluster);
  	   @endcode
       */
	std::pair<double,double> head_tail(const PndSdsCluster& Cluster);

         /**
	 @fn std::pair<double,double> eta(const StripCluster& Cluster)


       @param Cluster investigationg cluster
       @return coordinate
       @example
  	   @code
	  	eta(cluster);
  	   @endcode
       */
	std::pair<double,double> eta(const PndSdsCluster& Cluster);
	         /**
	 @fn std::pair<double,double> binary(const StripCluster& Cluster)

       coordinate of a strip with the highest signal
       @param Cluster investigationg cluster
       @return coordinate
       @example
  	   @code
	  	binary(cluster);
  	   @endcode
       */
	std::pair<double,double> binary(const PndSdsCluster& Cluster);
	/**
	 @fn std::pair<double,double> auto_select(const StripCluster& Cluster)

	 use the algorithm, which seems to bring the best result with this cluster.
       @param Cluster investigationg cluster
       @return coordinate
       @example
  	   @code
	  	auto_select(cluster);
  	   @endcode
       */
	std::pair<double,double> auto_select(const PndSdsCluster& Cluster);

      /**
	 @fn void MakedNdEta(const StripCluster& Cluster, RunInfo info)

       coordinate of a strip with the highest signal
       @param Cluster list of investigation cluster
       @param info all infos about run
       @return coordinate
       @example
  	   @code
	  	MakedNdEta(cluster, info);
  	   @endcode
       */
//	void MakedNdEta(const std::vector<PndSdsCluster>& Cluster, RunInfo& info);
/**@}*/

	void SetCalcStrip(PndSdsCalcStrip* calc){fCalcStrip = calc;};
	void SetChargeConverter(PndSdsChargeConversion* ChargeConverter){fChargeConverter = ChargeConverter;};

private:

	Double_t DigiCharge(Int_t digiIndex){return ((PndSdsDigiStrip*)(fDigiArray->At(digiIndex)))->GetCharge(); };
	Int_t DigiStripno(Int_t digiIndex);
	double _erfmod(double x, double p0, double p1, double p2, double p3);
	TClonesArray* fDigiArray;
	PndSdsCalcStrip* fCalcStrip;
	PndSdsChargeConversion* fChargeConverter;

	ClassDef(PndSdsChargeWeightingAlgorithms,0);
};
#endif
