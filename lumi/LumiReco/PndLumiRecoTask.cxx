/*
 * PndLumiRecoTask.cxx
 *
 *  Created on: Nov 10, 2008
 *      Author: tsito
 */

#include "PndLumiRecoTask.h"
PndLumiRecoTask::PndLumiRecoTask(Double_t pitch, Double_t sigma,
		Double_t orient)
{
	fPitch = pitch;
	fSigma = sigma;
	fOrient = orient;

}

InitStatus PndLumiRecoTask::Init()
{
	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman ){
		cout << "-E- PndLumiRecoTask::Init: "
		     << "RootManager not instantiated!" << endl;
		return kFATAL;
	}

	// Get input array

	fLumiClusterArrayFront = (TClonesArray*) ioman->GetObject("LumiClusterFront");
	fLumiClusterArrayBack = (TClonesArray*) ioman->GetObject("LumiClusterBack");
	if (!fLumiClusterArrayFront){
		cout << "-W- PndLumiRecoTask::Init: "
		     << "No LumiCluster collection!" << endl;
		return kERROR;
	}
	if (!fLumiClusterArrayBack){
			cout << "-W- PndLumiRecoTask::Init: "
			     << "No LumiCluster collection!" << endl;
			return kERROR;
		}

	//Get Output array
	fLumiRecoArray = new TClonesArray("PndLumiRecoHit");
	ioman->Register("LumiRecoHit", "Lumi", fLumiRecoArray, kTRUE);

	return kSUCCESS;
}

void PndLumiRecoTask::Exec(Option_t* opt)
{
	if (!fLumiRecoArray)
		Fatal("Exec", "No Reconstructed Array");
	fLumiRecoArray->Clear();

	//Variables declaration
	Int_t clustsize_front, clustsize_back;
	Double_t Q_r_front , Q_l_front, RecoCharge_front;
	Double_t Q_r_back , Q_l_back, RecoCharge_back;
	TString detname;
	TGeoHMatrix transMatrix ;
	TVector3 offset ;
	Int_t nhits;

	Double_t recoY_strip_front, recoPos_st_front;
	Double_t recoY_strip_back, recoPos_st_back;
	TVector3 MCPos_strip_front, RecoPosition_front;
	TVector3 MCPos_strip_back, RecoPosition_back;
	TVector3 MCPosition, RecoPosition;
	FairGeoVector RecoPos_strip_front, RecoPos_strip_back;
	TVector3 loc_front, Loc_front;
	TVector3 loc_back, Loc_back;
	Double_t zeroId_front, zeroId_back;
	// Loop over PndLumiCluster
	Int_t nClusters = fLumiClusterArrayFront->GetEntriesFast();

	for (Int_t iCluster = 0; iCluster < nClusters; iCluster++){
		cout << "iCluster : "<< iCluster<<endl;

		PndLumiCluster *ClusterFront = (PndLumiCluster*) fLumiClusterArrayFront->At(iCluster);
		PndLumiCluster *ClusterBack = (PndLumiCluster*) fLumiClusterArrayBack->At(iCluster);

		//** Get information from cluster

		// For both sides
		transMatrix = ClusterFront->GetTransformationMatrix();
		//transMatrix.Print("");
		offset = ClusterFront->GetSensorDimension();
		detname = ClusterFront->GetDetName();
		cout << detname <<endl;
		MCPosition = ClusterFront->GetMCPosition();
		nhits = ClusterFront->GetHitNumber();

		// For front side only
		clustsize_front = ClusterFront->GetClusterSize();
		zeroId_front = ClusterFront->GetStripZeroId();
		//cout << " Size ="<< clustsize_front <<endl;
		Q_r_front = ClusterFront->GetRightCharge();
		Q_l_front = ClusterFront->GetLeftCharge();
		MCPos_strip_front = ClusterFront->GetStripPosition();
		//cout << "A :" << MCPos_strip_front.X()<< " , "<<MCPos_strip_front.Y()<<endl;

		// For back side only
		clustsize_back = ClusterBack->GetClusterSize();
		zeroId_back = ClusterBack->GetStripZeroId();
		Q_r_back = ClusterBack->GetRightCharge();
		Q_l_back = ClusterBack->GetLeftCharge();
		MCPos_strip_back = ClusterBack->GetStripPosition();


		/*****************************************************
		 * 				DO RECONSTRUCTION
		 * ****************************************************/

		// ****  For FRONT side only
		// y-position reconstructed in strip coord syst
		RecoCharge_front = Q_r_front + Q_l_front;
		if (clustsize_front == 1){
			//recoPos_st_front = MCPos_strip_front.Y();
			recoPos_st_front = 0.5*fPitch;
			recoY_strip_front = ((ClusterFront->GetLeftId()-zeroId_front) * fPitch )+ recoPos_st_front;

		}else{
			recoPos_st_front = SmearPosition(Q_r_front/ RecoCharge_front);
			recoY_strip_front = (ClusterFront->GetRightId()-zeroId_front) * fPitch
								+ recoPos_st_front;
		}

		// Get local hit position reconstructed in strip coord syst
		Double_t local_front[3], global_front[3];

		RecoPos_strip_front.setXYZ(MCPos_strip_front.X(), recoY_strip_front, MCPos_strip_front.Z());

		cout << "RecoPos_strip_front :"<<endl;
		cout << RecoPos_strip_front.X() << " , "<<RecoPos_strip_front.Y()<< " , "<<RecoPos_strip_front.Z()<<endl;

		loc_front =
			StripDirtoLoc((2*(TMath::Pi())-fOrient), RecoPos_strip_front);//inverse

		cout << loc_front.X() <<" , "<<loc_front.Y()<< " , "<<loc_front.Z()<<endl;

		Loc_front.SetX( loc_front.X() - offset.X() );
		Loc_front.SetY( loc_front.Y() - offset.Y() );
		Loc_front.SetZ( loc_front.Z() - offset.Z() );

		cout << Loc_front.X() <<" , "<<Loc_front.Y()<< " , "<<Loc_front.Z()<<endl;

		local_front[0] = Loc_front.X();
		local_front[1] = Loc_front.Y();
		local_front[2] = Loc_front.Z();

		// Get global hit position reconstructed
		transMatrix.LocalToMaster(local_front , global_front);
		RecoPosition_front.SetXYZ(global_front[0], global_front[1], global_front[2]);

		// ****  For BACK side only

		// y-position reconstructed in strip coord syst
		RecoCharge_back = Q_r_back + Q_l_back;
		if (clustsize_back == 1){
			//recoPos_st_back = MCPos_strip_back.Y();
			recoPos_st_back = fPitch/2.;
			recoY_strip_back = (ClusterBack->GetLeftId()) * fPitch
			+ recoPos_st_back;

		}else{
			recoPos_st_back = SmearPosition(Q_r_back/ RecoCharge_back);
			recoY_strip_back = (ClusterBack->GetRightId()) * fPitch
			+ recoPos_st_back;
		}

		// Get local hit position reconstructed in strip coord syst
		Double_t local_back[3], global_back[3];

		RecoPos_strip_back.setXYZ(MCPos_strip_back.X(), recoY_strip_back, MCPos_strip_back.Z());

		loc_back =
			StripDirtoLoc(((TMath::Pi()/2)-fOrient), RecoPos_strip_back);//inverse

		Loc_back.SetX( loc_back.X() - offset.X() );
		Loc_back.SetY( loc_back.Y() - offset.Y() );
		Loc_back.SetZ( loc_back.Z() - offset.Z() );

		local_back[0] = Loc_back.X();
		local_back[1] = Loc_back.Y();
		local_back[2] = Loc_back.Z();

		// Get global hit position reconstructed
		transMatrix.LocalToMaster(local_back , global_back);
		RecoPosition_back.SetXYZ(global_back[0], global_back[1], global_back[2]);

		RecoPosition = 0.5*(RecoPosition_front + RecoPosition_back);

		//cout << "MC : ( "<< MCPosition.X() <<" , " <<MCPosition.Y()<<" , "<< MCPosition.Z()<< " )"<< endl;

		//cout << "Reco : ( "<< RecoPosition.X() <<" , " <<RecoPosition.Y()<<" , "<< RecoPosition.Z()<< " )"<< endl;

		//cout << "RecoFront : ( "<< RecoPosition_front.X() <<" , " <<RecoPosition_front.Y()<<" , "<< RecoPosition_front.Z()<< " )"<< endl;
		//cout << "RecoBack : ( "<< RecoPosition_back.X() <<" , " <<RecoPosition_back.Y()<<" , "<< RecoPosition_back.Z()<< " )"<< endl;
		//cout << RecoCharge_front << " VS "<< RecoCharge_back<<endl;

		/***********************************************************
		 * 				END RECONSTRUCTION
		 **********************************************************/

		new((*fLumiRecoArray)[iCluster]) PndLumiRecoHit(MCPosition, RecoPosition,
				RecoCharge_front, nhits);

	}

}

Double_t PndLumiRecoTask::SmearPosition(Double_t p)
{
	Double_t xSmrd;
	xSmrd = fSigma*(TMath::Sqrt(2.0))*TMath::ErfInverse((2.0 * p)-1.0);
	cout <<" Dyreco = "<<xSmrd <<endl;
	return xSmrd;
}

TVector3 PndLumiRecoTask :: StripDirtoLoc(Double_t angle,
		FairGeoVector  loc)
{
	TVector3 strip_orient;
	//Ligne vector
	Double_t x = loc.getX();
	Double_t y = loc.getY();
	Double_t z = loc.getZ();

	strip_orient.SetX(x*cos(angle) + y*sin(angle));
	strip_orient.SetY(-x*sin(angle) + y*cos(angle));
	strip_orient.SetZ(z);

	return strip_orient;
}
ClassImp(PndLumiRecoTask)
