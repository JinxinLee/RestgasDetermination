// Extracts the TOP volume of a geomanager
// Ralf Kliemt 05.03.2010
createGeoSim(TString infile  = "TrackingStation.root")
{
//  TString infile  = "TrackingStation.root";
//  TString outfile = "TrackingStationSim.root";
  TString outfile = infile;
  outfile.ReplaceAll(".root","Sim.root");
  TString topVolumeName = "TS";
  TString geomanagerName = "FAIRGeom";

  // load libraries  
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  TFile* fileIn = new TFile(infile,"READ"); 
	
  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject(geomanagerName.Data());	
  TGeoVolume *top = gGeoMan->GetTopVolume();
  if(topVolumeName!="") top->SetName(topVolumeName);

  TFile* fileOut = new TFile(outfile,"RECREATE");
  top->Write();
  fileOut->Close();

  fileIn->Close();
  //top->Draw("ogl");
} 

