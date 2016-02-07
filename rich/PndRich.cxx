#include "PndRich.h"

#include "PndRichPDPoint.h"
#include "PndRichBarPoint.h"
#include "PndRichGeo.h"
#include "PndRichGeoPar.h"

#include "FairVolume.h"
#include "FairGeoVolume.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndDetectorList.h"
#include "PndStack.h"

#include "TClonesArray.h"
#include "TVirtualMC.h"
#include "PndGeoHandling.h"

#include <iostream>
using std::cout;
using std::endl;

PndRich::PndRich()
  : FairDetector("PndRich", kTRUE, kRICH),
    fTrackID(-1),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1),
    fGeo(new PndRichGeo()),
    fGeoH(NULL),
    fPndRichPDPointCollection(new TClonesArray("PndRichPDPoint")),
    fPndRichBarPointCollection(new TClonesArray("PndRichBarPoint"))
{
  fListOfSensitives.push_back("Sensor");
  if(fVerboseLevel > 0){
    std::cout<<"-I- PndRich: fListOfSensitives contains:";
    for(Int_t k=0; k<fListOfSensitives.size(); k++)
      std::cout<<"\n\t"<<fListOfSensitives[k];
    std::cout<<std::endl;
  } 
   
   if ( fGeoH == NULL )
      fGeoH = PndGeoHandling::Instance();
}

PndRich::PndRich(const char* name, Bool_t active)
  : FairDetector(name, active, kRICH),
    fTrackID(-1),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1),
    fGeo(new PndRichGeo()),
    fGeoH(NULL),
    fPndRichPDPointCollection(new TClonesArray("PndRichPDPoint")),
    fPndRichBarPointCollection(new TClonesArray("PndRichBarPoint"))
{
  fListOfSensitives.push_back("Sensor");
  if(fVerboseLevel > 0){
    std::cout<<"-I- PndRich: fListOfSensitives contains:";
    for(Int_t k=0; k<fListOfSensitives.size(); k++)
      std::cout<<"\n\t"<<fListOfSensitives[k];
    std::cout<<std::endl;
  }
   
   if ( fGeoH == NULL )
      fGeoH = PndGeoHandling::Instance();
}

PndRich::~PndRich()
{
  if (fPndRichPDPointCollection) {
    fPndRichPDPointCollection->Delete();
    delete fPndRichPDPointCollection;
  }
  if (fPndRichBarPointCollection) {
    fPndRichBarPointCollection->Delete();
    delete fPndRichBarPointCollection;
  }
}

void PndRich::Initialize()
{
  FairDetector::Initialize();
  FairRuntimeDb* rtdb= FairRun::Instance()->GetRuntimeDb();
  PndRichGeoPar* par=(PndRichGeoPar*)(rtdb->getContainer("PndRichGeoPar"));
   
  if (0==gGeoManager) 
    cout << "We do not have gGeoManager" << endl;
  else
    cout << "there is gGeoManager" << endl;  
    
  cout << "list of sensitives has " << fListOfSensitives.size() << " entries" << endl;
  fGeoH->CreateUniqueSensorId("", fListOfSensitives);
  if(fVerboseLevel>0) fGeoH->PrintSensorNames();
   trackid.clear();
   fGeo->init(13);
   std::vector<Double_t> nopt = fGeo->nOpt();
   if (nopt.size()>0) {
      fnOpt = 0;
      for(size_t i=0;i<nopt.size();i++)
         fnOpt += nopt.at(i);
      fnOpt /= nopt.size();
      std::cout << "Mid ref. index = " << fnOpt << std::endl;
   }
   // aerogel bar entrance z coordinate
   TVector3 richOffset = fGeo->richOffset();
   TVector3 aerogelOffset = fGeo->aerogelOffset();
   TVector3 aerogelSize = fGeo->aerogelSize();
   fZabar = richOffset.Z() + aerogelOffset.Z();
}

Bool_t  PndRich::ProcessHits(FairVolume* vol)
{
  /** This method is called from the MC stepping */
  TString nam =vol->GetName();
  Int_t num = vol->getMCid();

  //Set parameters at entrance of volume. Reset ELoss.
  Int_t fEventID = gMC->CurrentEvent();
  Int_t fPdgCode = gMC->TrackPid();
  fTrackID = gMC->GetStack()->GetCurrentTrackNumber();	  
  fTime    = gMC->TrackTime() * 1.0e09;
  fLength  = gMC->TrackLength();

  gMC->TrackPosition(fPos);
  gMC->TrackMomentum(fMom);
   
  // Create PndRichPoint at exit of active volume
   if ( fPdgCode == 50000050 && nam.BeginsWith("RichPhDetSi") && gMC->IsTrackEntering()==1 ){
     fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
     fVolumeID = vol->getMCid();
     AddPDPoint(fTrackID, fVolumeID, fPos.Vect(), fMom.Vect(),
                fTime, fLength, fELoss, fEventID);
     
     // Increment number of PndRich det points in TParticle
     PndStack* stack = (PndStack*) gMC->GetStack();
     stack->AddPoint(kRICH);
     gMC->StopTrack();
   }
   
   if( (gMC->TrackCharge() != 0) && (fMom.Beta() > 1.00/fnOpt) &&
       gMC->IsTrackEntering()==1 && (fPos.Z() < fZabar+0.001) &&
       (nam.BeginsWith("RichAerogel")) && (trackid[fTrackID] != 1) ){
      trackid[fTrackID] = 1;
      fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
      fVolumeID = vol->getMCid();
      Double_t fMass = gMC->TrackMass();
      Double_t fP = fMom.Vect().Mag();
      Double_t fEnergy = TMath::Sqrt(fP*fP + fMass*fMass);
      Double_t fThetaC = -1;
      if ( fP != 0 ) fThetaC = fEnergy/fnOpt/fP;
      fThetaC = fThetaC <= 1 ? fThetaC : -1;
      fThetaC = fP/fEnergy; //beta
      AddBarPoint(fTrackID, fVolumeID, fPos.Vect(), fMom.Vect(),
                  fTime, fLength, fPdgCode, fThetaC, fEventID, fMass);
   }

   return kTRUE;
}

void PndRich::EndOfEvent()
{

  fPndRichPDPointCollection->Clear();
  fPndRichBarPointCollection->Clear();
  trackid.clear();
}



void PndRich::Register()
{

  /** This will create a branch in the output tree called
      PndRichPoint, setting the last parameter to kFALSE means:
      this collection will not be written to the file, it will exist
      only during the simulation.
  */

  FairRootManager::Instance()->Register("RichPDPoint", "PndRich",
                                        fPndRichPDPointCollection, kTRUE);
  FairRootManager::Instance()->Register("RichBarPoint", "PndRich",
                                        fPndRichBarPointCollection, kTRUE);

}


TClonesArray* PndRich::GetCollection(Int_t iColl) const
{
  if (iColl == 0) return fPndRichPDPointCollection;
  if (iColl == 1) return fPndRichBarPointCollection;
  return NULL;
}

void PndRich::Reset()
{
  fPndRichPDPointCollection->Clear();
  fPndRichBarPointCollection->Clear();
}

void PndRich::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root"))
     ConstructRootGeometry();
  else {
  /** If you are using the standard ASCII input for the geometry
      just copy this and use it for your detector, otherwise you can
      implement here you own way of constructing the geometry. */

  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  PndRichGeo*  Geo  = new PndRichGeo();
  Geo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(Geo);

  Bool_t rc = geoFace->readSet(Geo);
  if (rc) { Geo->create(geoLoad->getGeoBuilder()); }
  TList* volList = Geo->getListOfVolumes();

  // store geo parameter
  FairRun* fRun = FairRun::Instance();
  FairRuntimeDb* rtdb= FairRun::Instance()->GetRuntimeDb();
  PndRichGeoPar* par=(PndRichGeoPar*)(rtdb->getContainer("PndRichGeoPar"));
  TObjArray* fSensNodes = par->GetGeoSensitiveNodes();
  TObjArray* fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);
  FairGeoNode* node   = NULL;
  FairGeoVolume* aVol=NULL;

  while( (node = (FairGeoNode*)iter.Next()) ) {
    aVol = dynamic_cast<FairGeoVolume*> ( node );
    if ( node->isSensitive()  ) {
      fSensNodes->AddLast( aVol );
    } else {
      fPassNodes->AddLast( aVol );
    }
  }
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);

  ProcessNodes ( volList );
  }
}

void PndRich::ConstructOpGeometry() {
  cout<< " ==================================================== " << endl;
  cout<< " =======  Rich::  ConstructOpticalGeometry()  ======== " << endl; 

   //PndRichGeo *fGeo = new PndRichGeo();
   fGeo->init(13);
   std::vector<Double_t> nOpt        = fGeo->nOpt();
   UInt_t nAerogelLayers = nOpt.size();
   
  // ideal reflectivity:
  Int_t npoints_i = 2;  
  Double_t ephoton_i[npoints_i];
  ephoton_i[0] = 1.907*1.0e-09;  // 1 eV
  ephoton_i[1] = 6.199*1.0e-09; // 10 eV  
  Double_t reflectivity_i[npoints_i];
  reflectivity_i[0] = 0.9;
  reflectivity_i[1] = 0.9;
  Double_t refractiveIndex[npoints_i];
  refractiveIndex[0] = 1.05;
  refractiveIndex[1] = 1.05;
  Double_t abs_i[npoints_i];
  abs_i[0] = 1000.;
  abs_i[1] = 1000.;
  Double_t airRefractiveIndex[npoints_i];
  airRefractiveIndex[0] = 1.0;
  airRefractiveIndex[1] = 1.0;
  Double_t reflectivity0_i[npoints_i];
  reflectivity0_i[0] = 0.;
  reflectivity0_i[1] = 0.;
  // real reflectivity:
  Int_t npoints_r = 10;  
  Double_t ephoton_r[npoints_r];
  Double_t ephoton_r_min = 1.907*1.0e-09;  // 1 eV
  Double_t ephoton_r_max = 6.199*1.0e-09; // 10 eV
  Double_t reflectivity_r[npoints_r];
  Double_t refractiveIndex_r[nAerogelLayers][npoints_r];
  Double_t abs_r[npoints_r];
  Double_t reflectivity0_r[npoints_r];
  Double_t k = 2*3.1415927*197.3269602e-9;
  for(UInt_t i=0; i<npoints_r; i++ ) {
     ephoton_r[i] = ephoton_r_min + i*(ephoton_r_max-ephoton_r_min)/(npoints_r-1);
     reflectivity_r[i] = 1.;
     Double_t wl = k/ephoton_r[i];// wavelength in nm
     for(UInt_t l=0; l<nAerogelLayers; l++ )      
        refractiveIndex_r[l][i] = lhcbaerindex(nOpt[l],wl);
     abs_r[i] = 4.5*std::pow(wl/400,4.0);
     reflectivity0_r[i] = 0.;
  }

// https://www.slac.stanford.edu/grp/eg/minos/dist/dist_aux4/geant4_vmc/examples/E06/src/Ex06DetectorConstruction.cxx
  //gMC->SetCerenkov(gMC->MediumId("FusedSil"),npoints_i, ephoton_i,abs_i , reflectivity0_i, refractiveIndex);
  for(size_t i=0;i<nOpt.size();i++) {
     TString richAerogelMedia = Form("RichAerogel%d",i);
     //refractiveIndex[0] = nOpt[i];
     //refractiveIndex[1] = nOpt[i];
     cout<<i<<" "<<richAerogelMedia<<" "<<gMC->MediumId(richAerogelMedia)<<endl;
     gMC->SetCerenkov(gMC->MediumId(richAerogelMedia),npoints_r, ephoton_r, abs_r, reflectivity0_r, refractiveIndex_r[i]);
  }
  gMC->SetCerenkov(gMC->MediumId("air"),npoints_i, ephoton_i,abs_i , reflectivity0_i, airRefractiveIndex); 
  gMC->DefineOpSurface("RichMirrSurface",  kGlisur, kDielectric_metal, kPolished, 0.0);
  gMC->SetMaterialProperty("RichMirrSurface", "REFLECTIVITY", npoints_i, ephoton_i, reflectivity_i);
  gMC->SetBorderSurface("BarRichMirrorSurface", "RichMirror", 1, "RichAlBoxAir", 1, "RichMirrSurface");
  gMC->SetSkinSurface("RichAirMirrorSurface", "RichMirror", "RichMirrSurface");
  gMC->SetBorderSurface("BarRichMirrorLeftSurface", "RichMirrorLeft", 1, "RichAlBoxAir", 1, "RichMirrSurface");
  gMC->SetSkinSurface("RichAirMirrorLeftSurface", "RichMirrorLeft", "RichMirrSurface");
  gMC->SetBorderSurface("BarRichMirrorRightSurface", "RichMirrorRight", 1, "RichAlBoxAir", 1, "RichMirrSurface");
  gMC->SetSkinSurface("RichAirMirrorRightSurface", "RichMirrorRight", "RichMirrSurface");

  cout<<" =======  RICH::ConstructOpGeometry -> Finished! ====== "<< endl;     
   
}  

/*Refractive index as function of wavelength
 *     for aerogel of the nominal ref. ind. at 400nm.
 *   Scaled from LHCb data:
 *   T. Bellunato et al., "Refractive index dispersion law of silica aerogel",
 *   Eur. Phys. J. C 52 (2007) 759-764
 * */
Double_t PndRich::lhcbaerindex(Double_t n400,Double_t wl)
{
   const double LHCb_a0=0.05639, LHCb_wl0sqr = 83.22*83.22;
   double LHCb_RI2m1ref = LHCb_a0/(1-LHCb_wl0sqr/(400*400)); //(n**2-1) of LHCb aerogel at 400nm
   double ri2m1_lhcb = LHCb_a0/(1-LHCb_wl0sqr/(wl*wl)); //(n**2-1) of LHCb aerogel at wl
   return sqrt( 1 + (n400*n400-1)/LHCb_RI2m1ref*ri2m1_lhcb );
}


// -----   Public method CheckIfSensitive   --------------------------------------
bool PndRich::CheckIfSensitive(std::string name) {
  for (Int_t i = 0; i < fListOfSensitives.size(); i++){
    if (name.find(fListOfSensitives[i]) != std::string::npos)    
      return true;
  }
  return false;
}


PndRichPDPoint* PndRich::AddPDPoint(Int_t trackID, Int_t detID,
                                    TVector3 pos, TVector3 mom,
                                    Double_t time, Double_t length,
                                    Double_t eLoss, UInt_t EventId)
{
  TClonesArray& clref = *fPndRichPDPointCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndRichPDPoint(trackID, detID, pos, mom,
                                         time, length, eLoss, EventId);
}

PndRichBarPoint* PndRich::AddBarPoint(Int_t trackID, Int_t detID,
                                      TVector3 pos, TVector3 mom,
                                      Double_t time, Double_t length,
                                      Int_t pdgCode, Double_t thetaC,
                                      Int_t eventID, Double_t mass )
{
  TClonesArray& clref = *fPndRichBarPointCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndRichBarPoint(trackID, detID, pos, mom,
                                          time, length, pdgCode,
                                          thetaC, eventID, mass );
}

ClassImp(PndRich)
