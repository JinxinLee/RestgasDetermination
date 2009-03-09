/*
 * PndLumiCluster.cxx
 *
 *  Created on: Nov 18, 2008
 *      Author: tsito
 */

#include "PndLumiCluster.h"

PndLumiCluster::PndLumiCluster()
{
	fSide = 0;
	fSize = -1;
	fQ_l = 0.0;
	fQ_r = 0.0;
}

PndLumiCluster::~PndLumiCluster()
{

}

PndLumiCluster::PndLumiCluster(TString detname, Double_t side,
		TVector3 pos_strip, TVector3 mcpos, std::map<Int_t, PndLumiStrip> cluster,
		TGeoHMatrix trans, TVector3 sensdim, Int_t nhits, TVector2 stripzero,
		Double_t pitch, Double_t orient)
{
	fDetName = detname;
	fSide = side;
	fPositionStrip = pos_strip;
	fMCPosition = mcpos;
	fSize = cluster.size();
	fTransMatrix = trans;
	fSensorDim = sensdim;
	fNHits = nhits;
	fStripZeroId = stripzero;
	fPitch = pitch;
	fOrient = orient;

	std::map<Int_t, PndLumiStrip>::iterator it_clust;
	Double_t Q_l , Q_r;
	Int_t lId, rId;

	if (fSize <= 0){
		cout<< "-W- No Cluster recorded " << endl;

	}else{
		Int_t id_dim = 1000;
		Int_t index[id_dim];

		Int_t k = 0;
		for (it_clust = cluster.begin(); it_clust != cluster.end(); ++it_clust){
			index[k] = (*it_clust).first;
			k++;
		}
		id_dim = k;

		if (id_dim == 1){
			lId = (cluster.find(index[0])->second).GetIndex();
			Q_l = (cluster.find(index[0])->second).GetCharge();
			rId = -1;
			Q_r = 0.0;
		}

		if (id_dim == 2){
			lId = (cluster.find(index[0])->second).GetIndex();
			Q_l = (cluster.find(index[0])->second).GetCharge();
			rId = (cluster.find(index[1])->second).GetIndex();
			Q_r = (cluster.find(index[1])->second).GetCharge();
		}

		if (id_dim > 2){
			Int_t stripId_max = index[0];
			Double_t q_l = 0.0;
			Double_t q_r = 0.0;
			for (Int_t i = 0 ; i < id_dim ; i++){
				if ((cluster.find(stripId_max)->second).GetCharge() <=
							(cluster.find(index[i])->second).GetCharge()){
					stripId_max = index[i] ;

				}
			}

			/** Identify left and right strip in the cluster
			 * and determine respective charge collected */

			if((cluster.find(stripId_max-1)->second).GetCharge() >
					(cluster.find(stripId_max+1)->second).GetCharge()){
				lId = (cluster.find(stripId_max-1)->second).GetIndex();
				rId = (cluster.find(stripId_max)->second).GetIndex();
				for (Int_t j = index[0] ; j <= stripId_max-1; j++){
					q_l = (cluster.find(index[j])->second).GetCharge();
					Q_l += q_l;
				}
				for (Int_t j = stripId_max ; j <= index[id_dim]; j++){
					q_r = (cluster.find(j)->second).GetCharge();
					Q_r += q_r;
				}

			}

			if((cluster.find(stripId_max-1)->second).GetCharge() <
				(cluster.find(stripId_max+1)->second).GetCharge()){
				lId = (cluster.find(stripId_max)->second).GetIndex();
				rId = (cluster.find(stripId_max+1)->second).GetIndex();
				for (Int_t j = index[0] ; j <= stripId_max; j++){
					q_l = (cluster.find(j)->second).GetCharge();
					Q_l += q_l;
				}
				for (Int_t j = stripId_max+1 ; j <= index[id_dim]; j++){
					q_r = (cluster.find(j)->second).GetCharge();
					Q_r += q_r;
				}

			}
		}

	}

	fLeftId = lId ;
	fRightId = rId ;
	fQ_l = Q_l ;
	fQ_r = Q_r;

}
Double_t PndLumiCluster::GetStripZeroId()
{
	Double_t zeroId;
	zeroId =TMath::Abs(( (-fStripZeroId.X()*sin(fOrient)) +
			(fStripZeroId.Y()*cos(fOrient)))/fPitch);
	return zeroId;
}

ClassImp(PndLumiCluster)
