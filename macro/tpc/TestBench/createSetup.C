#include <iomanip>
#include <iostream>
#include <map>
#include <TString.h>
#include "TROOT.h"
#include "TFile.h"
#include "TSystem.h"
#include "TGeoManager.h"
#include "TMinuit.h"
#include "TGeoBBox.h"
#include "TEveManager.h"
#include "TEveGeoNode.h"

#include "../../../tpc/PndTpcAlignmentManager.h"
#include "../../../geobase/FairGeoLoader.h"
#include "../../../geobase/FairGeoInterface.h"
#include "../../../geobase/FairGeoMedium.h"
#include "../../../geobase/FairGeoMedia.h"
#include "../../../geobase/FairGeoBuilder.h"

void createSetup()
{

  //*************************
  //
  //Loading libraries and media
  //
  //********************
  using namespace std;
  

  // Gem/SI/Scint disk geometry parameters
  //-----------------------------
  

  

  map<int, Double_t> kdX;
  map<int, Double_t> kdY;
  map<int, vector<TString> > kLayerNames;
  map<int, vector<Double_t> > kLayerThicknes;
  map<int, vector<Double_t> > kDistActive;
  //----------------------
  // Layer initialisation
  // TODO use configuration files
  //----------------------
  {
    const TString  kLayerGEMName [] = {"WindowF_kapton","WindowF_copper",
				       "space",
				       "CathodeF_kapton","CathodeF_copper",
				       "ActiveVolume_Sensor_TPCtestchamber_mix",
				       "Gem1F_copper","Gem1_kapton","Gem1B_copper",
				       "space",
				       "Gem2F_copper","Gem2_kapton","Gem2B_copper",
				       "space",
				       "Gem3F_copper","Gem3_kapton","Gem3B_copper",
				       "space",
				       "PadF_copper","Pad_G10"};
    kLayerNames[101]=vector<TString>(kLayerGEMName,kLayerGEMName+sizeof(kLayerGEMName)/sizeof(kLayerGEMName[0]));
    kLayerNames[102]=vector<TString>(kLayerGEMName,kLayerGEMName+sizeof(kLayerGEMName)/sizeof(kLayerGEMName[0]));
  
    const Double_t kLayerGEMThick[] = {0.0050,  0.0005,          //     2 window
				       1.9235,                   // +1= 3 space
				       0.0050,  0.0005,          // +2= 5 cathode
				       0.3000,                   // +1= 6 space active
				       0.0005,  0.0050,  0.0005, // +3= 9 gemfoil
				       0.2000,                   // +1=10 space
				       0.0005,  0.0050,  0.0005, // +3=13 gemfoil
				       0.2000,                   // +1=14 space
				       0.0005,  0.0050,  0.0005, // +3=17 gemfoil
				       0.2000,                   // +1=18 space
				       0.0012,  0.0500};        // +2=20 padplane
    kLayerThicknes[101]=vector< Double_t>(kLayerGEMThick,kLayerGEMThick+sizeof(kLayerGEMThick)/sizeof(kLayerGEMThick[0]));
    kLayerThicknes[102]=vector< Double_t>(kLayerGEMThick,kLayerGEMThick+sizeof(kLayerGEMThick)/sizeof(kLayerGEMThick[0]));

    //center position of each layer relative to the active volume
    const Double_t kGemLayDistActive[] = {-2.082, -2.07925,        //     2 window
					  -1.11735,                // +1= 3 space
					  -0.153, -0.15025,        // +2= 5 cathode
					  0.0,                     // +1= 6 space active
					  0.15025,0.153, 0.15575,  // +3= 9 gemfoil
					  0.256,                 // +1=10 space
					  0.35625, 0.359,0.36175,   // +3=13 gemfoil
					  0.462,                 // +1=14 space
					  0.56225,0.565,0.56775,    // +3=17 gemfoil
					  0.668,                  // +1=18 space
					  0.7686,0.7942};        // +2=20 padplane
    kDistActive[101]=vector< Double_t>(kGemLayDistActive,kGemLayDistActive+sizeof(kLayerGEMThick)/sizeof(kGemLayDistActive[0]));
    kDistActive[102]=vector< Double_t>(kGemLayDistActive,kGemLayDistActive+sizeof(kLayerGEMThick)/sizeof(kGemLayDistActive[0]));
    assert(kDistActive[101].size()==kLayerNames[101].size());
    assert(kLayerThicknes[101].size()==kLayerNames[101].size());
    kdX[101]=5.12;
    kdX[102]=5.12;
    kdY[101]=5.12;
    kdY[102]=5.12;
    
    const TString  kLayerSIName [] = {"WindowF_mylar","WindowF_aluminium",    //2      window
				      "space",                                //+1 = 3 space
				      "SIWaferF_Sensor_silicon",                       //+1 = 4 wafer
				      "space",                                //+1 = 5 space
				      "SIWaferB_Sensor_silicon",                       //+1 = 6 wafer
				      "space",                                //+1 = 7 space
				      "WindowB_aluminium","WindowB_mylar"};   //+2 = 9 window
    kLayerNames[103]=vector<TString>(kLayerSIName,kLayerSIName+sizeof(kLayerSIName)/sizeof(kLayerSIName[0]));
    kLayerNames[104]=vector< TString>(kLayerSIName,kLayerSIName+sizeof(kLayerSIName)/sizeof(kLayerSIName[0]));
    const Double_t kLayerSIThick[] = {  0.0050,  0.0005,          //     2 window
					4.9545,                   // +1= 3 space
					0.0300,                   // +1= 4 wafer
					0.3700,                   // +1= 5 space
					0.0300,                   // +1= 6 wafer
					3.0045,                   // +1= 7 space
					0.0005,  0.0050};         // +2= 9 window
    kLayerThicknes[103]=vector< Double_t>(kLayerSIThick,kLayerSIThick+sizeof(kLayerSIThick)/sizeof(kLayerSIThick[0]));
    kLayerThicknes[104]=vector< Double_t>(kLayerSIThick,kLayerSIThick+sizeof(kLayerSIThick)/sizeof(kLayerSIThick[0]));
    const Double_t kDistActiveSi[]={-5.1725,-5.16975,
				    -2.69225,
				    -0.2,
				    0,
				    0.2,
				    1.71725,
				    3.21975,
				    3.2225};
    kDistActive[103]=vector< Double_t>(kDistActiveSi,kDistActiveSi+sizeof(kDistActiveSi)/sizeof(kDistActiveSi[0]));
    kDistActive[104]=vector< Double_t>(kDistActiveSi,kDistActiveSi+sizeof(kDistActiveSi)/sizeof(kDistActiveSi[0]));
    
    assert(kDistActive[103].size()==kLayerNames[103].size());
    assert(kLayerThicknes[103].size()==kLayerNames[103].size());
    kdX[103]=0.96;
    kdX[104]=0.96;
    kdY[103]=0.96;
    kdY[104]=0.96;
  }						     
  
  

  //******************
  //LOADING pandaroot
  //*******************
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  TString vmcWorkdir = getenv("VMCWORKDIR");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libPndTpc");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive"); 		  

  PndTpcAlignmentManager* fAlignmentManager=PndTpcAlignmentManager::getInstance("current_alignment.txt");
  TString outfile= "beamTelescopeTestnew.root";
  TFile *fi = new TFile(outfile,"RECREATE");  

  FairGeoLoader *geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  
  //-----------------------------
  //Creating media for setup
  //-----------------------------
  geoFace->setMediaFile(vmcWorkdir+"/geometry/media_pnd.geo");
  geoFace->readMedia();
  geoFace->print();
  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();
  FairGeoMedium *CbmMediumAir       = Media->getMedium("air");
  FairGeoMedium *CbmMediumAluminium = Media->getMedium("aluminium");
  FairGeoMedium *CbmMediumCopper    = Media->getMedium("copper");
  FairGeoMedium *CbmMediumKapton    = Media->getMedium("kapton");
  FairGeoMedium *CbmMediumG10       = Media->getMedium("G10");
  FairGeoMedium *CbmMediumSIlicon   = Media->getMedium("silicon");
  FairGeoMedium *CbmMediumMylar     = Media->getMedium("mylar");
  FairGeoMedium *CbmMediumTPCgas    = Media->getMedium("TPCtestchamber_mix");
  
  Int_t nmed=geobuild->createMedium(CbmMediumAir);
  nmed=geobuild->createMedium(CbmMediumAluminium);
  nmed=geobuild->createMedium(CbmMediumCopper);
  nmed=geobuild->createMedium(CbmMediumKapton);
  nmed=geobuild->createMedium(CbmMediumG10);
  nmed=geobuild->createMedium(CbmMediumSIlicon);
  nmed=geobuild->createMedium(CbmMediumMylar);
  nmed=geobuild->createMedium(CbmMediumTPCgas);


  TGeoManager *gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom"); 
  TGeoVolume *top = new TGeoVolumeAssembly("beamTelescope");
  gGeoMan->SetTopVolume(top);
  //-------------------------------------------------------------------------------------------
  cout << "-------------------------------------------------------------------" << endl;
  TList *mediaList = (TList*)gGeoMan->GetListOfMedia();
  cout << "media known: " << mediaList->GetEntries() << "." << endl;
  for ( Int_t itemp = 0 ; itemp < mediaList->GetEntries() ; itemp++ ) {
    TGeoMedium *medium = (TGeoMedium*)mediaList->At(itemp);
    cout << "medium " << itemp << " name is " << medium->GetName() << endl;
  }
  cout << "-------------------------------------------------------------------" << endl;
  //----------------------------------------------------------------------------------------------


  TGeoRotation *dummyrot = new TGeoRotation();//rotation and translation

  TGeoVolumeAssembly *SubunitVol = new TGeoVolumeAssembly("Beam_Telescope");
  
  std::map<int, TGeoShape*> detShape;
  map<int, TGeoVolume*> detVol;
  map<int, TGeoCombiTrans*> detCombi;

  map<int, vector<TGeoShape*> > detLayerShape;
  map<int, vector<TGeoVolume*> > detLayerVolume;
  map<int, vector<TGeoCombiTrans*> > detLayerCombi;

  //---------------------
  // Detector creation
  //---------------------
  for(int iDet=101;iDet<=104;++iDet){
    cout<<"dx "<<kdX[iDet]<<"dy "<<kdY[iDet]<<endl<<endl;
    detShape[iDet]=new TGeoBBox (Form("det%dshape",iDet),
				 kdX[iDet],kdY[iDet],
				 5);
    detVol[iDet]= new TGeoVolume(Form("Gem_%d_Volume",iDet),
				 detShape[iDet],
				 gGeoMan->GetMedium("TPCtestchamber_mix"));
    detCombi[iDet]=fAlignmentManager->getTransformation(iDet);
    detCombi[iDet]->Print();
    detCombi[iDet]->SetName(Form("Gem_%d_Volume",iDet));
    const unsigned int nLayers =kLayerNames[iDet].size();
    for(Int_t ilay = 0 ; ilay <nLayers  ; ilay++ ) {
      detLayerShape[iDet].push_back( new TGeoBBox  (Form("det%dLayer%d%sshape",
                                                         iDet,ilay, 
                                                         kLayerNames[iDet][ilay].Data()),
						    kdX[iDet],kdY[iDet],
						    kLayerThicknes[iDet][ilay]/2.));
      TString layerMaterial = kLayerNames[iDet][ilay].Data();
      layerMaterial.Remove(0,layerMaterial.Last('_')+1);
      if(layerMaterial=="mix"){
	layerMaterial="TPCtestchamber_mix";
      }
      detLayerVolume[iDet].push_back(new TGeoVolume(Form("det%d_layer%d_%s",
							 iDet,ilay,
							 kLayerNames[iDet][ilay].Data()),
						    detLayerShape[iDet][ilay],
						    gGeoMan->GetMedium(layerMaterial.Data())));
      TGeoTranslation* trans = new TGeoTranslation(0.,0.,kDistActive[iDet][ilay]);
      TGeoCombiTrans* combi=new TGeoCombiTrans(*trans,*dummyrot);
      detVol[iDet]->AddNode(detLayerVolume[iDet].at(ilay),0,
			    combi);
    }
    top->AddNode(detVol[iDet],0,detCombi[iDet]);
  }

  gGeoMan->CloseGeometry();
  top->Write();
  fi->Close();
  //   gGeoManager->Export(outfile);
  //  top->Draw("ogl");

  TEveManager::Create();
  TGeoNode* node = gGeoMan->GetTopNode();
  
  TEveGeoTopNode* en = new TEveGeoTopNode(gGeoMan, node);
  en->SetVisLevel(4);
  en->GetNode()->GetVolume()->SetVisibility(kFALSE);
  
  gEve->AddGlobalElement(en);

  gEve->Redraw3D(kTRUE);
  
}  
