void DisplayGeometry(TString fileName)
{
TFile* f = new TFile(fileName);
TGeoManager *geoMan = (TGeoManager*)f->Get("CBMGeom");
TEveManager::Create();
TGeoNode *N=  gGeoManager->GetTopNode();
 TEveGeoTopNode *TNod=new  TEveGeoTopNode(gGeoManager, N);
 gEve->AddGlobalElement(TNod);
 gEve->FullRedraw3D(kTRUE);
}
