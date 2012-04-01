#ifndef _PNDSDSCHARGEWEIGHTINGALGORITHMS_H
#define _PNDSDSCHARGEWEIGHTINGALGORITHMS_H

#include <vector>
#include "TClonesArray.h"
#include "TH2F.h"
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
	virtual ~PndSdsChargeWeightingAlgorithms();

	   /**
	 @fn Double_t center_of_gravity(const StripCluster& Cluster)

       calculate the coordinate of particle crossing the sensor, by using the algorithm center of gravity:
       \n \f$x_g=\sum^n_{k=1}\frac{q_k\cdot ch_k\cdot pitch}{\sum_{i=1}^n q_i}\f$
       @param Cluster investigationg cluster
       @return coordinate
       @example
  	   @code
	  	center_of_gravity(cluster);
  	   @endcode
       */
  std::pair<Double_t,Double_t> CenterOfGravity(const PndSdsCluster* Cluster);

         /**
	 @fn std::pair<Double_t,Double_t> head_tail(const StripCluster& Cluster)

       calculate the coordinate of particle crossing the sensor, by using the head tail algorithm:
       \n \f$x_{ht}=\sum^n_{k=1}\left(\frac{x_{head}+x_{tail}}{2}+\frac{q_{head}-q_{tail}}{2\cdot q_\theta}\cdot pitch\right)\f$
       @param Cluster investigationg cluster
       @return coordinate
       @example
  	   @code
	  	head_tail(cluster);
  	   @endcode
       */
	std::pair<Double_t,Double_t> HeadTail(const PndSdsCluster* Cluster);

         /**
	 @fn std::pair<Double_t,Double_t> eta(const StripCluster& Cluster)


       @param Cluster investigationg cluster
       @return coordinate
       @example
  	   @code
	  	eta(cluster);
  	   @endcode
       */
	std::pair<Double_t,Double_t> Eta(const PndSdsCluster* Cluster, const TH1F* PosVsEta);

	std::pair<Double_t,Double_t> EtaValue(const PndSdsCluster* Cluster, Double_t &stripno, Int_t &NmbOfStrips); // calculates the eta value for the given cluster.
	         /**
	 @fn std::pair<Double_t,Double_t> binary(const StripCluster& Cluster)

       coordinate of a strip with the highest signal
       @param Cluster investigationg cluster
       @return coordinate
       @example
  	   @code
	  	binary(cluster);
  	   @endcode
       */
	std::pair<Double_t,Double_t> Binary(const PndSdsCluster* Cluster);
	/**
	 @fn std::pair<Double_t,Double_t> auto_select(const StripCluster& Cluster)

	 use the algorithm, which seems to bring the best result with this cluster.
       @param Cluster investigationg cluster
       @return coordinate
       @example
  	   @code
	  	auto_select(cluster);
  	   @endcode
       */
	std::pair<Double_t,Double_t> AutoSelect(const PndSdsCluster* Cluster);

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

	void SetDigiArray(TClonesArray* darray){fDigiArray = darray;};
	void SetCalcStrip(PndSdsCalcStrip* calc){fCalcStrip = calc;};
	void SetChargeConverter(PndSdsChargeConversion* ChargeConverter){fChargeConverter = ChargeConverter;};
  void SetNoise(Double_t n=0){fNoise=n;};
  void SetVerbose(Int_t level=0){fVerbose=level;};

private:

	Double_t DigiCharge(Int_t digiIndex);
	Double_t DigiChargeError(Int_t digiIndex);
	Int_t DigiStripno(Int_t digiIndex);
	Double_t Erfmod(Double_t x, Double_t p0, Double_t p1, Double_t p2, Double_t p3);
	TClonesArray* fDigiArray;
	PndSdsCalcStrip* fCalcStrip;
	PndSdsChargeConversion* fChargeConverter;
  Double_t fNoise;
  Int_t fVerbose;

	ClassDef(PndSdsChargeWeightingAlgorithms,1);
};
#endif
