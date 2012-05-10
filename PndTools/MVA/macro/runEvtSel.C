void runEvtSel( int pdg = 11,
		char partName    [] = "elect",
		char paramFile   [] = "params_sttcombi.root",
		char simFile     [] = "points_sttcombi.root",
		char digiFile    [] = "digi_sttcombi.root",
		char recoFile    [] = "reco_sttcombi.root",
		char outFileName [] = "CleanEvts.root"
		)
{
  gROOT->LoadMacro("selectEvents.cpp+");
  selectEvents( pdg, partName, paramFile, simFile, 
		digiFile, recoFile, outFileName);
  exit(0);
}
