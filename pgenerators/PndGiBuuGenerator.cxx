// -------------------------------------------------------------------------
// -----                PndGiBuuGenerator source file                  -----
// -------------------------------------------------------------------------


#include <iostream>
#include "PndGiBuuGenerator.h"
#include "FairPrimaryGenerator.h"

// -----   Standard constructor   -----------------------------------------
PndGiBuuGenerator::PndGiBuuGenerator(const Char_t* fileName, const Char_t* pidDataFileName) :
		fEvent(0), fFileName(fileName), fPidDataFileName(pidDataFileName), fInputAsciiFile(0), fPx(0), fPy(0), fPz(0), fPdg(0)
{
	std::cout << "Constructor Called!" << std::endl;
}
// ------------------------------------------------------------------------


// -----   Destructor   ---------------------------------------------------
PndGiBuuGenerator::~PndGiBuuGenerator() {
  CloseInput();
  for (std::map<Int_t, PndGiBuuDecayerAbs*>::const_iterator iter = fDecayerMap.begin(); iter != fDecayerMap.end(); iter++){
	  delete(iter->second);
  }
}
// ------------------------------------------------------------------------

Bool_t PndGiBuuGenerator::Init()
{

	fInputAsciiFile = new std::ifstream(fFileName.Data(), std::ifstream::in);
	if (0 == fInputAsciiFile) {
		Fatal("PndGiBuuGenerator", "Cannot open ASCII input file.");
	}

	FillPidMap();

	return kTRUE;
}

// ------------------------------------------------------------------------



// -----   Public method ReadEvent   --------------------------------------
Bool_t PndGiBuuGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{

	Int_t GiBuuPid = 0, GiBuuCharge = 0;
	Int_t runId = 0;
	Int_t motherId1 = 0;
	Int_t motherId2 = 0;
	Int_t dummyInt = 0;
	Double_t mass = 0.;
	Double_t energy = 0.;
	Int_t oldEventNr = fEvent;

	Double_t dummyDouble = 0.;

	std::cout << "ReadEvent: " << fEvent << std::endl;

	if (fEvent > 0){
		primGen->AddTrack(fPdg, fPx, fPy, fPz, 0., 0., 0.);
	} else {
		fEvent = 1;
		oldEventNr = 1;
	}

	while (fEvent == oldEventNr && !(fInputAsciiFile->eof())){
		*fInputAsciiFile >> GiBuuPid >> GiBuuCharge >> motherId1 >> motherId2 >> dummyInt >> mass >> dummyInt >> energy >> fPx >> fPy >> fPz >> dummyDouble >> fEvent >> runId >> dummyDouble;
		fPdg = GetPdgParticleId(GiBuuPid, GiBuuCharge);
//		std::cout << fPdg << " " << GiBuuPid << " " << GiBuuCharge << " " << motherId1 << " " << motherId2 << " " << dummyInt << " " << mass << " " << dummyInt << " " << energy << " " << fPx << " " << fPy << " " << fPz << " " << dummyDouble << " " << fEvent << " " << runId << " " << dummyDouble;
//		std::cout << std::endl;

		if (fDecayerMap.count(fPdg) > 0){
			std::vector<PndGiBuuTrack> tracks = fDecayerMap[fPdg]->DecayTrack(PndGiBuuTrack(fPdg, fPx, fPy, fPz, 0., 0.,0.));
			for (int i = 0; i < tracks.size(); i++){
				primGen->AddTrack(tracks[i].GetPdgId(), tracks[i].GetMomentum().X(), tracks[i].GetMomentum().Y(),tracks[i].GetMomentum().Z(),
						 	 	  tracks[i].GetVertex().X(), tracks[i].GetVertex().Y(),tracks[i].GetVertex().Z());
			}
		}

		if (fEvent == oldEventNr){
			primGen->AddTrack(fPdg, fPx, fPy, fPz, 0.,0.,0.);
		}
	}

	return kTRUE;
}

void PndGiBuuGenerator::FillPidMap()
{
	std::ifstream* PidFile = new std::ifstream(fPidDataFileName.Data(), std::ifstream::in);
	if (0 == PidFile){
		Fatal("PndGiBuuGenerator::FillPidMap", "No pid file!");
	}
	Int_t giPid = 0;
	Int_t giCharge = 0;
	Int_t pdgPid = 0;

//	std::cout << "Reading In PidFile: " << fPidDataFileName.Data() <<  " " << PidFile << " " << PidFile->eof() << std::endl;

	while (!(PidFile->eof())){
		*PidFile >> giPid >> giCharge >> pdgPid;
		std::pair<Int_t, Int_t> GiBuuPid(giPid, giCharge);
		fMapGiBuuToPdg[GiBuuPid] = pdgPid;
//		std::cout << giPid << " " << giCharge << " = " << pdgPid << std::endl;
	}
	PidFile->close();
	PidFile = 0;

}

Int_t PndGiBuuGenerator::GetPdgParticleId(Int_t GiBuuId, Int_t charge)
{
	std::pair<Int_t, Int_t> GiBuuPid(GiBuuId, charge);
	if (fMapGiBuuToPdg.count(GiBuuPid) > 0){
		return fMapGiBuuToPdg[GiBuuPid];
	} else {
		return 0;
	}
}

// -----   Private method CloseInput   ------------------------------------
void PndGiBuuGenerator::CloseInput() {

	fInputAsciiFile->close();
	//delete fInputAsciiFile;
	fInputAsciiFile = NULL;

}
// ------------------------------------------------------------------------



ClassImp(PndGiBuuGenerator)
