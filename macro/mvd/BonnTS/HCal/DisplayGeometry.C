int DisplayGeometry(TString fileName="HCalMC_Params.root")
{
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libEve");
  
  TFile* file = new TFile(fileName);
  file->Get("FairBaseParSet"); 
  gGeoManager->DefaultColors(); // repair buggy coloring

  // Draw with Eve (browsable paths, modify displaying...):
  TEveManager::Create();
  TGeoNode *N=  gGeoManager->GetTopNode();
  TEveGeoTopNode *TNod=new  TEveGeoTopNode(gGeoManager, N);
  TNod->SetVisLevel(10); // mandatory to see the Mvd
  gEve->AddGlobalElement(TNod);
  gEve->FullRedraw3D(kTRUE);

  // Draw like this: 
  //  gGeoManager->SetVisLevel(10);
  //  gGeoManager->GetMasterVolume()->Draw("ogl");
  
    return 0;

}
