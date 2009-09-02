#include "PndMvdFileNameCreator.h"
#include "PndStringVector.h"
#include <sstream>
#include <iostream>


PndMvdFileNameCreator::PndMvdFileNameCreator(){
  fExtSim = "";
  fExtDigi = "digi";
  fExtReco = "reco";
  fExtTrackF = "trackF";
  fExtIdealTrackF = "idealTrackF";
  fExtKalman = "kalman";
  fExtRiemann = "riemann";
  fExtCombinedRiemann = "combRiemann";
  fVertex = "vertex";

  fVerbose = 0;
}
PndMvdFileNameCreator::PndMvdFileNameCreator(std::string fileName){
  fFileName = fileName;
  fExtSim = "";
  fExtDigi = "digi";
  fExtReco = "reco";
  fExtTrackF = "trackF";
  fExtIdealTrackF = "idealTrackF";
  fExtKalman = "kalman";
  fExtRiemann = "riemann";
  fExtCombinedRiemann = "combRiemann";
  fVertex = "vertex";

  fVerbose = 0;
}




 std::string PndMvdFileNameCreator::GetSimFileName(bool cut)
{
// 	return GetCustomFileName(fExtSim, cut);
  return fFileName;
}

std::string PndMvdFileNameCreator::GetDigiFileName(bool cut)
{
	return GetCustomFileName(fExtDigi, cut);
}

std::string PndMvdFileNameCreator::GetRecoFileName(bool cut)
{
	return GetCustomFileName(fExtReco, cut);
}

std::string PndMvdFileNameCreator::GetTrackFindingFileName(bool cut)
{
	return GetCustomFileName(fExtTrackF, cut);
}

std::string PndMvdFileNameCreator::GetIdealTrackFindingFileName(bool cut)
{
	return GetCustomFileName(fExtIdealTrackF, cut);
}

std::string PndMvdFileNameCreator::GetRiemannFileName(bool cut)
{
	return GetCustomFileName(fExtRiemann, cut);
}

std::string PndMvdFileNameCreator::GetCombinedRiemannFileName(bool cut)
{
	return GetCustomFileName(fExtCombinedRiemann, cut);
}


std::string PndMvdFileNameCreator::GetKalmanFileName(bool cut)
{
	return GetCustomFileName(fExtKalman, cut);
}

std::string PndMvdFileNameCreator::GetVertexFileName(bool cut)
{
	return GetCustomFileName(fVertex, cut);
}


std::string PndMvdFileNameCreator::GetSimFileName(std::string inputFileName, bool cut)
{
	fFileName = inputFileName;
	return GetSimFileName(cut);
}

std::string PndMvdFileNameCreator::GetDigiFileName(std::string inputFileName, bool cut)
{
	fFileName = inputFileName;
	return GetDigiFileName(cut);
}

std::string PndMvdFileNameCreator::GetRecoFileName(std::string inputFileName, bool cut)
{
	fFileName = inputFileName;
	return GetRecoFileName(cut);
}

std::string PndMvdFileNameCreator::GetTrackFindingFileName(std::string inputFileName, bool cut)
{
	fFileName = inputFileName;
	return GetTrackFindingFileName(cut);
}

std::string PndMvdFileNameCreator::GetIdealTrackFindingFileName(std::string inputFileName, bool cut)
{
	fFileName = inputFileName;
	return GetIdealTrackFindingFileName(cut);
}

std::string PndMvdFileNameCreator::GetKalmanFileName(std::string inputFileName, bool cut)
{
	fFileName = inputFileName;
	return GetKalmanFileName(cut);
}

std::string PndMvdFileNameCreator::GetVertexFileName(std::string inputFileName, bool cut)
{
	fFileName = inputFileName;
	return GetVertexFileName(cut);
}

std::string PndMvdFileNameCreator::GetRiemannFileName(std::string inputFileName, bool cut)
{
	fFileName = inputFileName;
	return GetRiemannFileName(cut);
}

std::string PndMvdFileNameCreator::GetCombinedRiemannFileName(std::string inputFileName, bool cut)
{
	fFileName = inputFileName;
	return GetCombinedRiemannFileName(cut);
}

std::string PndMvdFileNameCreator::GetCustomFileName(std::string ext, bool cut)
{
	std::string result = TruncateFileName(cut);
	if (ext.size() > 0){
		result += "_";
		result += ext;
	}
	result += ".root";
  if(fVerbose>0) std::cout<<" -I- PndMvdFileNameCreator file: "<<result.c_str()<<std::endl;
	return result;
}
std::string PndMvdFileNameCreator::GetCustomFileName(std::string inputFileName, std::string ext, bool cut)
{
	fFileName = inputFileName;
	return GetCustomFileName(ext, cut);
}

std::string PndMvdFileNameCreator::TruncateFileName(bool cut)
{
	std::vector<std::string> resString;
	std::stringstream result;
	std::string path, name;
	Int_t cutLast = 0;

	PndStringVector pathAna(fFileName,"/");
	resString = pathAna.GetStringVector();
	if(fVerbose>1) pathAna.Print();
	if (fFileName.find("/") == 0)
		result << "/";
	for (Int_t i = 0; i < resString.size()-1; i++){
		result << resString[i] << "/";
	}
	path = result.str();
	name = resString[resString.size()-1];
	//if(fVerbose>1) std::cout << "Path: " << path << " FileName: " << name << std::endl;
	result.str("");

	PndStringVector stringAna(name, "._");
	resString = stringAna.GetStringVector();
	//if(fVerbose>1) stringAna.Print();

	if (cut == true)
		cutLast = 2;
	else cutLast = 1;

	if (resString[resString.size()-1] != "root")
		cutLast--;

	if (resString.size() - cutLast <= 0)
		return "";

	result << path;
	for (Int_t i = 0; i < resString.size()-1 - cutLast; i++){
		result << resString[i] << "_";
	}
	result << resString[resString.size()-1 - cutLast];
	return result.str();
}

ClassImp(PndMvdFileNameCreator);

