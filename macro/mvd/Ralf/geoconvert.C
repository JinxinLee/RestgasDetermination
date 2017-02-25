
// #include<>
// #include<>
// #include<>
// #include<>
// #include<>
// #include<>
// #include<>
// #include<>
// #include<>
// #include<>

int geoconvert()
{

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libPndData");
//  gSystem->Load("libStt1"); 
////  gSystem->Load("libStt2");
//  gSystem->Load("libMuo");
//  gSystem->Load("libEmc");
  gSystem->Load("libMvd");
//  gSystem->Load("libDrc");

  gSystem->Load("libGeom.so");

  TFile* file = new TFile("../../../geometry/MVD_20a.root");
//   TFile* outfile = new TFile("../../../geometry/MVD_20a_vol.root","RECREATE");
  TGeoManager *geoMan = (TGeoManager*) file->Get("CADtoROOT");

//   TFile* file = new TFile("../data/mvdevalg4.root");
//   TGeoManager *geoMan = (TGeoManager*) file->Get("FAIRGeom");

//   TCanvas* c1 = new TCanvas("c1", "", 100, 100, 1100, 1100);
//   c1->SetFillColor(10);

  // Number of folded geo levels; 8 for mvd.
  // Try to change the number and to see how the geometry changes.
//   geoMan->SetVisLevel(180);
//   geoMan->DefaultColors();
  TGeoVolume* topvol = geoMan->GetMasterVolume();
  //topvol->Draw("ogl");
//   outfile->cd();
//   topvol->Write();
  epandnode(topvol->GetNode());
  topvol->Export("../../../geometry/MVD_20a_vol.root","TOP","recreate");

std::cout<<"------------------------------"<<std::endl;
std::cout<<"------------------------------"<<std::endl;
std::cout<<"------------------------------"<<std::endl;

//   geoMan->GetListOfVolumes()->Print();
  return 0;
}

int expandnode(TGeoNode* fN)
{
  TGeoVolume *v1=fN->GetVolume();
  TObjArray *NodeList=v1->GetNodes();
  for (Int_t Nod=0; Nod<NodeList->GetEntriesFast();Nod++) {   
    TGeoNode *fNode =(TGeoNode *)NodeList->At(Nod);
    if(fNode->GetNdaughters()>0) expandnode(fNode);
    TGeoVolume *v= fNode->GetVolume();
    TGeoMedium* med1=v->GetMedium();
    if(!med1){
      std::cout<<" -W- WARNING in PndMvdDetector::ExpandNode()!  "
               <<"\tThe volume "<<v->GetName()<<"in\n"
               <</*gGeoManager->GetPath()<<*/"\nHas no medium information. It is forced now to LEAD"
               <<std::endl;
//       v->SetMaterial("lead");
    }
  }
  return 0;
}



