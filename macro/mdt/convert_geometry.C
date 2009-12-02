convert_geometry()
{
  TString geo = "";
  cout << "Option: ";
  cin >> geo;
  TFile *file;
  if (geo=="Barrel") file = TFile::Open("../../geometry/muon_TS_barrel_v3.root","READ");
  if (geo=="Endcap") file = TFile::Open("../../geometry/muon_TS_endcap.root","READ");
  if (geo=="Forward") file = TFile::Open("../../geometry/muon_FS.root","READ");
  if (geo=="MuonFilter") file = TFile::Open("../../geometry/muon_MF.root","READ");
 
  TFile *out = TFile::Open("muon_"+geo+"_noGeo.root","RECREATE");
  
  TGeoManager *geoMan = (TGeoManager*) file->Get("Assembly");
  TGeoVolume* topvol = geoMan->GetMasterVolume();
  topvol->SetName("Mdt"+geo);
  
  topvol->Write();
  out->Save();
  out->Close();
  cout << endl << "File " << out->GetName() << " created!!" << endl;
}
