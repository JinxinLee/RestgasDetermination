/*
 * PndLumiClusterBuilder.cxx
 *
 *  Created on: Nov 18, 2008
 *      Author: tsito
 */
#include "PndLumiClusterBuilder.h"

PndLumiClusterBuilder::PndLumiClusterBuilder()
{

}

PndLumiClusterBuilder::PndLumiClusterBuilder(Double_t pitch,
		Double_t orient_front,Double_t orient_back, Double_t W,
		Double_t L,	Double_t thr, Double_t noise, Double_t sigma)
{
	fPitch          = pitch;
	fOrient_front   = orient_front;
	fOrient_back    = orient_back;
	fWidth  		= W;
	fLength 		= L;
	fThreshold      = thr;
	fNoise          = noise;
	fSigma          = sigma;
	//fSide           = side;
}

PndLumiClusterBuilder::~PndLumiClusterBuilder()
{

}

InitStatus PndLumiClusterBuilder::Init()
{
	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman ){
		cout << "-E- PndLumiClusterBuilder::Init: "
		<< "RootManager not instantiated!" << endl;
		return kFATAL;
	}

	// Get input array
	fLumiPoint = (TClonesArray*) ioman->GetObject("LumiPoint");
	if (!fLumiPoint){
		cout << "-W- PndLumiClusterBuilder::Init: "
		<< "No LumiPoint collection!" << endl;
		return kERROR;
	}

	// Create and register output array
	fLumiCluster = new TClonesArray("PndLumiCluster");
	ioman->Register("LumiCluster", "Lumi", fLumiCluster, kTRUE);

	return kSUCCESS;

}

void PndLumiClusterBuilder::Exec(Option_t* opt)
{
	// Reset output array
    if (!fLumiCluster)
    	Fatal("Exec", "No hit collection");

    fLumiCluster->Clear();

    //Variable from MC
    PndLumiPoint *point = NULL;
    TVector3 entryPos, exitPos, MCPosition;
    TString detname;
    Double_t eLoss;

    FairGeoVector posInL, posOutL;

    TVector2 Zero_front, Zero_back;
    Double_t zf, zb;
    Double_t ClusterPositionFront, ClusterPositionBack;

    //Loop over LumiPoint
    Int_t nPoints = fLumiPoint->GetEntriesFast();

    for (Int_t iPoint = 0 ; iPoint< nPoints ; iPoint++){
    	point = (PndLumiPoint*) fLumiPoint->At(iPoint);

    	//Variable definition

    	detname = point->GetDetName();
    	eLoss = point->GetEnergyLoss()* 1E9;

    	entryPos = point->GetEntryPoint(),
    	exitPos = point->GetExitPoint();
    	MCPosition = 0.5*(entryPos + exitPos);

    	//Make some transformations of the coord syst
		PndLumiTransposition trans(fVerboseLevel);
		trans.GetLocalHitPoints(point, posInL, posOutL);

		cout << " ****************** LUMI STRIP CLUSTER ********************* "<< endl;
		//------------------------Front Side
		cout << " * - Front Side :"<<endl;
		Zero_front.Set(fWidth,0.0);
		ClusterPositionFront = GetClusterPosition(fOrient_front, Zero_front, eLoss, posInL, posOutL);
		zf = GetStripZeroId(fOrient_front, Zero_front.X(),Zero_front.Y());
		cout << " * 	Cluster Position  = "<<ClusterPositionFront/fPitch + zf <<endl;
		cout <<" * "<<endl;

		//------------------------Back Side
		cout << " * - Back Side :"<<endl;
		Zero_back.Set(0.0,0.0);
		ClusterPositionBack = GetClusterPosition(fOrient_back, Zero_back, eLoss, posInL, posOutL);
		zb = GetStripZeroId(fOrient_back, Zero_back.X(),Zero_back.Y());
		cout << " * 	Cluster Position = "<<ClusterPositionBack/fPitch + zb  <<endl;
		cout <<" *********************************************************** " <<endl;


		TGeoHMatrix transMatrix = trans.GetTransformation(detname.Data());
		//transMatrix.Print("");
		TVector3 offset = trans.GetSensorDimensions(detname.Data());


		new ((*fLumiCluster)[iPoint]) PndLumiCluster( detname, MCPosition,
				ClusterPositionFront,ClusterPositionBack,transMatrix, offset,
				fPitch, fOrient_front, fOrient_back);

    }
}

std::map<Int_t,PndLumiStrip> PndLumiClusterBuilder::GetClusters(std::vector<PndLumiStrip> strip)
{
	std::map<Int_t,PndLumiStrip> clust;
	std::map<Int_t,PndLumiStrip>::iterator it;
	clust.clear();

	for (Int_t j = 0; j < strip.size(); j++){
		clust[strip[j].GetIndex()] = strip[j];
	}

	for ( it=clust.begin() ; it != clust.end(); it++ )
		cout << " *	" << (*it).second << endl;
	return clust;

}

//Identification of the left and the right strip in a cluster
//return to a map with size two
std::map<Int_t,Double_t> PndLumiClusterBuilder::
GetLeftAndRight(std::map<Int_t,PndLumiStrip> clust)
{
	std::map<Int_t,Double_t> IdEnergy;
	std::map<Int_t,Double_t>::iterator it;
	std::map<Int_t,PndLumiStrip>::iterator its;
	IdEnergy.clear();

	std::vector<Int_t> Index;
	Int_t Id[2];
	Double_t Energy[2];

	for (its = clust.begin(); its!= clust.end(); its++){
			Index.push_back((*its).first);
			//cout << Index.back() << endl;
		}

	Int_t min = *min_element(Index.begin(),Index.end());
	Int_t max = *max_element(Index.begin(),Index.end());

	Int_t Id_emax =  min;
	for (Int_t id = min ; id <= max; id++){
		if ((clust.find(Id_emax)->second).GetCharge() <=(clust.find(id)->second).GetCharge()){
			Id_emax = id ;
		}
	}

	if (clust.size() == 1){
		Energy[0]= (clust.find(min)->second).GetCharge();
		Id[0] = min;
		Energy[1]= 0.0;
		Id[1] = -1;

	} else{
		if (clust.size() == 2){
			Energy[0]=(clust.find(min)->second).GetCharge();
			Id[0] = min;
			Energy[1]=(clust.find(max)->second).GetCharge();
			Id[1]= max;
		}
		if (clust.size() > 2){
			double q_l;
			double q_r;
			if ((clust.find(Id_emax-1)->second).GetCharge() <
					(clust.find(Id_emax+1)->second).GetCharge()){
				Id[1] = Id_emax +1;
				Id[0] = Id_emax;
				for (int i = min; i <= (clust.find(Id_emax)->second).GetIndex(); i++){
					q_l = (clust.find(i)->second).GetCharge();
					Energy[0] += q_l;
				}
				for (int i =(clust.find(Id_emax + 1)->second).GetIndex(); i <= max ; i++){
					q_r = (clust.find(i)->second).GetCharge();
					Energy[1] += q_r;
				}
			}
			if ((clust.find(Id_emax-1)->second).GetCharge() >
			(clust.find(Id_emax+1)->second).GetCharge()){
				Id[1] = Id_emax;
				Id[0] = Id_emax - 1;
				for (int i = min; i <= (clust.find(Id_emax-1)->second).GetIndex(); i++){
					q_l = (clust.find(i)->second).GetCharge();
					Energy[0] += q_l;
				}
				for (int i =(clust.find(Id_emax)->second).GetIndex(); i <= max ; i++){
					q_r = (clust.find(i)->second).GetCharge();
					Energy[1] += q_r;
				}
			}

		}
	}

	for (Int_t j = 0; j < 2; j++){
		IdEnergy[Id[j]] = Energy[j];
	}
	return IdEnergy;
}

Double_t PndLumiClusterBuilder::GetClusterPosition(Double_t dir, TVector2 Zero,
		Double_t eLoss,	FairGeoVector posInL, FairGeoVector posOutL)
{
	Double_t Qr = 0.;
	Double_t Ql = 0.;
	Int_t rId;
	Int_t lId;
	std::vector<PndLumiStrip> digi;
	std::map<Int_t, PndLumiStrip> cluster;
	Int_t c_size;
	Double_t inStripId, outStripId, StripId;
	std::map<Int_t, Double_t> IdEnergy;
	std::map<Int_t, Double_t>::iterator it;
	Double_t dQ;

	PndLumiCalcStripDigi StripDigi(fPitch, dir, fWidth,
			fLength, fThreshold, fNoise, fSigma, Zero);
	digi = StripDigi.GetStripsDigi(posInL, posOutL, eLoss);
	cluster = GetClusters(digi);
	c_size = cluster.size();

	inStripId = StripDigi.CalcStripFromHit(posInL.getX(), posInL.getY());
	outStripId  = StripDigi.CalcStripFromHit(posOutL.getX(), posOutL.getY());
	//StripId = 0.5 * (inStripId + outStripId);

	if (c_size!=0){
		IdEnergy =  GetLeftAndRight(cluster);

		std::vector<Int_t> Id;

		for ( it = IdEnergy.begin(); it != IdEnergy.end(); it++){
			Id.push_back((*it).first);
		}
		Int_t min = *min_element(Id.begin(),Id.end());
		Int_t max = *max_element(Id.begin(),Id.end());

		rId = IdEnergy.find(max)->first;
		lId = IdEnergy.find(min)->first;
		Qr = IdEnergy.find(max)->second;
		Ql = IdEnergy.find(min)->second;
	}

	dQ = Qr/(Ql+Qr);

	Double_t y_reco;
	Double_t y = fSigma*TMath::Sqrt(2.0)*TMath::ErfInverse((2.0*dQ)-1.0);
	Double_t zero = GetStripZeroId(dir, Zero.X(),Zero.Y());

	if (lId<0 ){
		y_reco = ((rId-ceil(zero))*fPitch)+(fPitch/2.);
	}else {
		y_reco = y+((rId-ceil(zero))*fPitch);
	}
	return y_reco;
}
Double_t PndLumiClusterBuilder::GetStripZeroId(Double_t dir, Double_t x0,Double_t y0)
{
	Double_t zero = TMath::Abs(( -x0*sin(dir) + y0*cos(dir))/fPitch);
	return zero;
}

ClassImp(PndLumiClusterBuilder)
