void run(const int NumEvt = 0, const char* inPutFile = "Reco.root",
	 const char* simInFile = "SimFile.root",
	 const char* parInput = "Param.root", 
	 const char* OutPutFile = "PidOut.root",
	 const std::string TreeName ="Een tree"
	 )
{
  gROOT->LoadMacro("PndMakeInputTask.cpp+");
  gROOT->LoadMacro("makeFile.cpp");
  makeFile(NumEvt, inPutFile, simInFile,parInput, 
	   OutPutFile, TreeName);
}
//End of macro
