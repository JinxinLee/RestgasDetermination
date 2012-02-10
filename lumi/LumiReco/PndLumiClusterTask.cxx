#include "PndLumiClusterTask.h"
#include <math.h>

#include <algorithm>
#include <cmath>
#include <Math/EulerAngles.h>
#include <Math/Rotation3D.h>

using namespace ROOT::Math;
using namespace std;

// -----   Default constructor   -------------------------------------------

PndLumiClusterTask::PndLumiClusterTask(Int_t ClusterMod, Int_t RadChannel,
		Int_t RadTime, Int_t verb) :
	FairTask("LUMI Clustertization Task") {
	fChargeCut = 1.e8; // this ist really large and shall have no effect
	fClusterMod = ClusterMod;
	if (fClusterMod > 1 || fClusterMod < 0)
		fClusterMod = 0;
	fRadChannel = RadChannel;
	fRadTime = RadTime;
	fVerbose = verb;
}

// -------------------------------------------------------------------------


PndLumiClusterTask::PndLumiClusterTask(Double_t chargecut, Int_t clustermod,
		Int_t RadChannel, Int_t RadTime, Int_t verb) :
	FairTask("LUMI Clustertization Task") {
	fChargeCut = chargecut;
	fClusterMod = clustermod;
	if (fClusterMod > 1 || fClusterMod < 0)
		fClusterMod = 0;
	fRadChannel = RadChannel;
	fRadTime = RadTime;
	fVerbose = verb;
}

// -----   Destructor   ----------------------------------------------------
PndLumiClusterTask::~PndLumiClusterTask() {
	//if(0!=fGeoH)  delete fGeoH;
	//if(0!=fChargeAlgos) delete fChargeAlgos;
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndLumiClusterTask::SetParContainers() {
	// Get Base Container

	FairRun* ana = FairRun::Instance();
	FairRuntimeDb* rtdb = ana->GetRuntimeDb();
	fDigiPar = (PndLumiDigiPara*) (rtdb->getContainer("LumiStripDigiPara"));
}

InitStatus PndLumiClusterTask::ReInit() {
	SetParContainers();
	return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndLumiClusterTask::Init() {
	// Get RootManager
	FairRun* ana = FairRun::Instance();
	FairRootManager* ioman = FairRootManager::Instance();
	//fGeoH = new PndLumiTransposition(fVerbose, gGeoManager);

	if (!ioman) {
		cout << "-E- PndLumiClusterBuilder::Init: "
				<< "RootManager not instantiated!" << endl;
		return kFATAL;
	}

	// Get input array
	fDigiArray = (TClonesArray*) ioman->GetObject("LumiDigi");
	if (!fDigiArray) {
		cout << "-W- PndLumiClusterBuilder::Init: "
				<< "No LumiDigi collection!" << endl;
		return kERROR;
	}

	fDigiPar->Print();

	//fVerboseLevel = verbose ;
	fPitch = fDigiPar->GetPitch();
	fOrient_front = fDigiPar->GetFrontOrient();
	fOrient_back = fDigiPar->GetBackOrient();
	fSigma = fDigiPar->GetGausSigma();
	fStripZeroFrontPos = fDigiPar->GetFrontAnchor();
	fStripZeroBackPos = fDigiPar->GetBackAnchor();

	// Create and register output array
	fHitArray = new TClonesArray("PndLumiHit");
	ioman->Register("LumiHit", "Lumi", fHitArray, kTRUE);

	fClusterArray = new TClonesArray("PndLumiCluster");
	ioman->Register("LumiCluster", "Lumi", fClusterArray, kTRUE);

	std::cout << "-I- PndLumiClusterTask: Successful Initialization"
			<< std::endl;

	return kSUCCESS;
}

// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndLumiClusterTask::Exec(Option_t* opt) {
	if (fVerbose > 2)
		std::cout << " ** Starting PndLumiClusterTask::Exec() **" << std::endl;

	std::vector<PndLumiDigi> digiStripArray;

	// Reset output array

	if (!fClusterArray)
		Fatal("Exec", "No ClusterArray");
	fClusterArray->Delete();

	if (!fHitArray)
		Fatal("Exec", "No HitArray");
	fHitArray->Delete();

	// a std::map is a SORTED container, it is sorted by the identifier
	TString detName;
	PndLumiStrip strip;
	SensorSide side = kTOP;
	PndLumiDigi* myDigi = 0;

	// load the Clusterfinder
	if (0 == fClusterMod) {
		fClusterfinder = new PndLumiSimpleStripClusterFinder(fRadChannel); //search radius in channel no.

	} else if (1 == fClusterMod) {
		fClusterfinder = new PndLumiStripClusterFinder(fRadChannel, fRadTime);

	}

	// Sort Digi indice into the clusterfinder
	for (Int_t iDigi = 0; iDigi < fDigiArray->GetEntriesFast(); iDigi++) { // sort digis by sensor name and stripnumber

		myDigi = (PndLumiDigi*) fDigiArray->At(iDigi);
		detName = myDigi->GetDetName();
		side = myDigi->GetSide();
		strip = myDigi->GetStrip();

		//we use the top side as "first" side
		//fStripCalcTop->CalcFeChToStrip(myDigi->GetFE(), myDigi->GetChannel(), strip, side);
		fClusterfinder->AddDigi(detName.Data(), side, 0, strip.GetIndex(),
				iDigi);
	}

	std::vector<PndLumiCluster> clusters;
	std::vector<Int_t> topclusters;// contains index to fClusterArray
	std::vector<Int_t> botclusters;// contains index to fClusterArray
	std::vector<Int_t> oneclustertop;
	std::vector<Int_t> oneclusterbot;
	std::vector<Int_t> leftDigis;
	clusters.clear();
	topclusters.clear();
	botclusters.clear();
	oneclustertop.clear();
	oneclusterbot.clear();
	leftDigis.clear();

	Int_t detID, mcindex, clindex, botIndex;
	Double_t mycharge;
	TVector2 meantopPoint, meanbotPoint, onsensorPoint;
	TVector3 hitPos, hitErr;

	// -------   SEARCH  ------
	clusters = fClusterfinder->SearchClusters();

	// fetch ids in 'clusters' to the top and bot side
	topclusters = fClusterfinder->GetTopClusterIDs();
	botclusters = fClusterfinder->GetBotClusterIDs();

	if (fVerbose > 3) {
		leftDigis = fClusterfinder->GetLeftDigiIDs();

		if (0 < leftDigis.size()) {
			std::cout << "There are " << leftDigis.size()
					<< " Digis not assigned to" << " clusters:\n";

			for (unsigned int s = 0; s < leftDigis.size(); s++) {
				std::cout << leftDigis[s] << "|";
			}
			std::cout << std::endl;

		} else
			std::cout << "All Digis assigned to clusters" << std::endl;
	}

	// Fill the ClonesArray for output TODO: do this better, don't copy objects around

	for (std::vector<PndLumiCluster>::iterator clit = clusters.begin(); clit
			!= clusters.end(); ++clit) {
		clindex = fClusterArray->GetEntriesFast();
		new ((*fClusterArray)[clindex]) PndLumiCluster(*clit);
	}

	//printout for checking
	if (fVerbose > 4) {
		std::cout << "Top Clusters: ";
		for (std::vector<Int_t>::iterator itTop = topclusters.begin(); itTop
				!= topclusters.end(); ++itTop) {
			std::cout << *itTop << " | ";
		}
		std::cout << std::endl;
		std::cout << "Bot Clusters: ";
		for (std::vector<Int_t>::iterator itBot = botclusters.begin(); itBot
				!= botclusters.end(); ++itBot) {
			std::cout << *itBot << " | ";
		}
		std::cout << std::endl;
	}

	// ---------------------------------------------
	// Loop structure :
	// - top clusters
	// |-calculate mean positon
	// |-bot clusters
	// |-|-calc chargew. mean
	// |-|-fill hit array
	//
	// -----  match top/bot clusters for the case of double-sided sensors -----
	// -----  set pos and charge to -1 and -1, respectively, for inactive sides in case of
	// -----  single-sided sensors

	//	mcindex = -1;
	SensorSide topside;
	SensorSide botside;
	Int_t iHit = 0;

	for (std::vector<Int_t>::iterator itTop = topclusters.begin();
			itTop	!= topclusters.end(); ++itTop) {

		Double_t topcharge=0., meantopstrip=0., meantoperr=0.;
		oneclustertop = (clusters[*itTop]).GetClusterList();

		PndLumiDigi* atopDigi =
				((PndLumiDigi*) fDigiArray->At(oneclustertop[0]));
		TString detnametop = atopDigi->GetDetName();
		topside = atopDigi->GetSide();

		detID = atopDigi->GetDetId();

		CalcClusterPositionAndCharge(oneclustertop, topside, meantopstrip,
				meantoperr, topcharge);

		// loop on bottom side
		for (std::vector<Int_t>::iterator itBot = botclusters.begin();
				itBot	!= botclusters.end(); ++itBot) {

			Double_t botcharge = 0., meanbotstrip = 0., meanboterr = 0.;
			oneclusterbot = (clusters[*itBot]).GetClusterList();

			PndLumiDigi* abotDigi = ((PndLumiDigi*) fDigiArray->At(oneclusterbot[0]));
			TString detnamebot = abotDigi->GetDetName();
			botside = abotDigi->GetSide();

			CalcClusterPositionAndCharge(oneclusterbot, botside,
					meanbotstrip, meanboterr, botcharge);

			if (detnametop == detnamebot){
				if (fabs(botcharge - topcharge) < fChargeCut) {// look if the charges are not too different
					Bool_t test = BackmapPoint(meantopstrip,
							meantoperr, meanbotstrip, meanboterr,
							hitPos, hitErr, detnametop);

					if (kFALSE == test)	continue;
					iHit = fHitArray->GetEntriesFast();

					mcindex = atopDigi->GetRefIndex();

					new ((*fHitArray)[iHit]) PndLumiHit(detID,
							hitPos, hitErr, mcindex, detnametop,
							0.5*(topcharge+botcharge), *itTop,
							oneclusterbot.size()	+oneclustertop.size());
				}
			}
		}
	}




	if (fVerbose > 1) {
		std::cout << "-I- PndLumiClusterTask: " << iHit << " out of "
				<< fDigiArray->GetEntriesFast() << " Digis" << std::endl;
		cout << "" << endl;
	}

	if (0 != fClusterfinder)
		delete fClusterfinder;
	return;
}

void PndLumiClusterTask::Finish() {
}

std::map<Int_t, Double_t> PndLumiClusterTask::GetLeftAndRight(std::map<Int_t,
		PndLumiStrip> clust) {
	std::map<Int_t, Double_t> IdEnergy;
	std::map<Int_t, Double_t>::iterator it;
	std::map<Int_t, PndLumiStrip>::iterator its;
	IdEnergy.clear();

	std::vector<Int_t> Index;
	Int_t Id[2];
	Double_t Energy[2];

	for (its = clust.begin(); its != clust.end(); its++) {
		Index.push_back((*its).first);
	}

	Int_t min = *min_element(Index.begin(), Index.end());
	Int_t max = *max_element(Index.begin(), Index.end());

	Int_t Id_emax = min;
	for (Int_t id = min; id <= max; id++) {
		if ((clust.find(Id_emax)->second).GetCharge()
				<= (clust.find(id)->second).GetCharge()) {
			Id_emax = id;
		}
	}

	if (clust.size() == 1) {
		Energy[0] = (clust.find(min)->second).GetCharge();
		Id[0] = min;
		Energy[1] = 0.0;
		Id[1] = -1;

	} else {
		if (clust.size() == 2) {
			Energy[0] = (clust.find(min)->second).GetCharge();
			Id[0] = min;
			Energy[1] = (clust.find(max)->second).GetCharge();
			Id[1] = max;
		}
		if (clust.size() > 2) {
			Double_t q_l;
			Double_t q_r;
			if ((clust.find(Id_emax - 1)->second).GetCharge() < (clust.find(
					Id_emax + 1)->second).GetCharge()) {
				Id[1] = Id_emax + 1;
				Id[0] = Id_emax;
				for (Int_t i = min; i
						<= (clust.find(Id_emax)->second).GetIndex(); i++) {
					q_l = (clust.find(i)->second).GetCharge();
					Energy[0] += q_l;
				}
				for (Int_t i = (clust.find(Id_emax + 1)->second).GetIndex(); i
						<= max; i++) {
					q_r = (clust.find(i)->second).GetCharge();
					Energy[1] += q_r;
				}
			}
			if ((clust.find(Id_emax - 1)->second).GetCharge() > (clust.find(
					Id_emax + 1)->second).GetCharge()) {
				Id[1] = Id_emax;
				Id[0] = Id_emax - 1;
				for (Int_t i = min; i
						<= (clust.find(Id_emax - 1)->second).GetIndex(); i++) {
					q_l = (clust.find(i)->second).GetCharge();
					Energy[0] += q_l;
				}
				for (Int_t i = (clust.find(Id_emax)->second).GetIndex(); i
						<= max; i++) {
					q_r = (clust.find(i)->second).GetCharge();
					Energy[1] += q_r;
				}
			}

		}
	}

	for (Int_t j = 0; j < 2; j++) {
		IdEnergy[Id[j]] = Energy[j];
	}
	return IdEnergy;
}

void PndLumiClusterTask::CalcClusterPositionAndCharge(
		std::vector<Int_t> &onecluster, SensorSide side, Double_t &meanstrip,
		Double_t &meanerr, Double_t &charge) {

	//get the charge and stripnumber
	std::vector<PndLumiStrip> digi;
	digi.clear();

	PndLumiDigi* myDigi = 0;
	Double_t q;
	Double_t dq = 0.0;
	for (std::vector<Int_t>::iterator itDigi = onecluster.begin(); itDigi
			!= onecluster.end(); ++itDigi) {

		myDigi = (PndLumiDigi*) fDigiArray->At(*itDigi);
		digi.push_back(myDigi->GetStrip());
		q = myDigi->GetStrip().GetCharge();
		meanerr += q * q;
		dq += q;

	}
	charge = dq;
	Double_t Qr = 0.;
	Double_t Ql = 0.;
	Int_t rId;
	Int_t lId;

	std::map<Int_t, PndLumiStrip> cluster;
	cluster.clear();
	Int_t cl_size;
	std::map<Int_t, Double_t> IdEnergy;
	IdEnergy.clear();
	std::map<Int_t, Double_t>::iterator it;
	Double_t dQ;

	// Arrange digi in the cluster according the index
	for (Int_t j = 0; j < digi.size(); j++) {
		cluster[digi[j].GetIndex()] = digi[j];
	}

	cl_size = cluster.size();

	if (cl_size != 0) {
		IdEnergy = GetLeftAndRight(cluster);

		std::vector<Int_t> Id;

		for (it = IdEnergy.begin(); it != IdEnergy.end(); it++) {
			Id.push_back((*it).first);
		}

		Int_t min = *min_element(Id.begin(), Id.end());
		Int_t max = *max_element(Id.begin(), Id.end());

		rId = IdEnergy.find(max)->first;
		lId = IdEnergy.find(min)->first;
		Qr = IdEnergy.find(max)->second;
		Ql = IdEnergy.find(min)->second;
	}

	dQ = Qr / (Ql + Qr);

	Double_t y_reco;
	Double_t y = fSigma * TMath::Sqrt(2.0)
			* TMath::ErfInverse((2.0 * dQ) - 1.0);

	Double_t zero = GetStripZeroId(side);

	if (lId < 0) {
		y_reco = ((rId - ceil(zero)) * fPitch) - (fPitch / 2.);
	} else {
		y_reco = y + (((rId - 1) - ceil(zero)) * fPitch);
	}

	meanstrip = y_reco;///fPitch;

	meanerr = TMath::Sqrt(meanerr / (charge * charge));
	if (side == kTOP) {
		meanerr = meanerr * (fPitch / TMath::Sqrt(12.));
	} else {
		meanerr = meanerr * (fPitch / TMath::Sqrt(12.));
	}


	if (fVerbose > 2) {
		cout << "Strip mean position : " << meanstrip / fPitch << " +/- "
				<< meanerr << endl;
		cout << " Charge = " << charge << " electrons" << endl;
	}

}

Double_t PndLumiClusterTask::GetStripZeroId(SensorSide side) {
	Double_t dir;
	TVector2 stripzeroId;
	if (side == kTOP) {
		dir = fOrient_front;
		stripzeroId = fStripZeroFrontPos;

	} else if (side == kBOTTOM) {
		dir = fOrient_back;
		stripzeroId = fStripZeroBackPos;

	}
	Double_t zero = TMath::Abs((-stripzeroId.X() * sin(dir) + stripzeroId.Y()
			* cos(dir)) / fPitch);

	return zero;
}

TVector2 PndLumiClusterTask::CalcLineCross(Double_t yf, Double_t yb,
		Double_t of, Double_t ob) const {
	Double_t D, dx, dy, x, y;

	D = -cos(of) * sin(ob) + cos(ob) * sin(of);
	dy = -yf * sin(ob) + yb * sin(of);
	dx = yb * cos(of) - yf * cos(ob);

	x = dx / D;
	y = dy / D;

	TVector2 result(x, y);

	return result;
}

Bool_t PndLumiClusterTask::BackmapPoint(Double_t &meantopPoint,
		Double_t &meantoperr, Double_t &meanbotPoint, Double_t &meanboterr,
		TVector3 &hitPos, TVector3 &hitErr, TString &detname) {

	PndLumiTransposition *trans = new PndLumiTransposition(fVerbose,
			gGeoManager);
	TVector3 localpos;
	Double_t t, b;
	TGeoHMatrix transMatrix;
	TVector3 offset;

	transMatrix = trans->GetTransformation(detname.Data());

	offset = trans->GetSensorDimensions(detname.Data());


	TVector2 onsensorPoint = CalcLineCross(meantopPoint, meanbotPoint,
			fOrient_front, fOrient_back);

	localpos.SetXYZ(onsensorPoint.X(), onsensorPoint.Y(), 0.0);

	Double_t local[3], locDpos[3], global[3], gloDpos[3];

	local[0] = localpos.X() - offset.X();
	local[1] = localpos.Y() - offset.Y();
	local[2] = localpos.Z();

	transMatrix.LocalToMaster(local, global);

	hitPos.SetXYZ(global[0], global[1], global[2]);

	hitErr.SetXYZ(0., 0., 0.);

	if (fVerbose > 1) {
		cout << "( " << hitPos.X() << " , " << hitPos.Y() << " , "
				<< hitPos.Z() << " ) " << endl;
	}

	return kTRUE;
}
ClassImp(PndLumiClusterTask)

