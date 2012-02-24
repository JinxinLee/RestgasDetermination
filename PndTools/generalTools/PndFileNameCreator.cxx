#include "PndFileNameCreator.h"
#include "PndStringSeparator.h"
#include <sstream>
#include <iostream>


PndFileNameCreator::PndFileNameCreator():
	fFileName(), fExtPar("par"), fExtSim("sim"), fExtDigi("digi"), fExtReco("reco"),
	fExtTrackF("trackF"), fExtIdealTrackF("idealTrackF"), fExtKalman("kalman"),
	fExtRiemann("riemann"),fExtCombinedRiemann("combRiemann"),fExtVertex("vertex"),
	fVerbose(0)
{
}

PndFileNameCreator::PndFileNameCreator(std::string fileName):
	fFileName(fileName), fExtPar("par"), fExtSim("sim"), fExtDigi("digi"), fExtReco("reco"),
	fExtTrackF("trackF"), fExtIdealTrackF("idealTrackF"), fExtKalman("kalman"),
	fExtRiemann("riemann"),fExtCombinedRiemann("combRiemann"),fExtVertex("vertex"),
	fVerbose(0)
{
}




std::string PndFileNameCreator::GetParFileName(bool cut)
{
  return GetCustomFileName(fExtPar, cut);
}

std::string PndFileNameCreator::GetSimFileName(bool cut)
{
  return GetCustomFileName(fExtSim, cut);
}

std::string PndFileNameCreator::GetDigiFileName(bool cut)
{
	return GetCustomFileName(fExtDigi, cut);
}

std::string PndFileNameCreator::GetRecoFileName(bool cut)
{
	return GetCustomFileName(fExtReco, cut);
}

std::string PndFileNameCreator::GetTrackFindingFileName(bool cut)
{
	return GetCustomFileName(fExtTrackF, cut);
}

std::string PndFileNameCreator::GetIdealTrackFindingFileName(bool cut)
{
	return GetCustomFileName(fExtIdealTrackF, cut);
}

std::string PndFileNameCreator::GetRiemannFileName(bool cut)
{
	return GetCustomFileName(fExtRiemann, cut);
}

std::string PndFileNameCreator::GetCombinedRiemannFileName(bool cut)
{
	return GetCustomFileName(fExtCombinedRiemann, cut);
}


std::string PndFileNameCreator::GetKalmanFileName(bool cut)
{
	return GetCustomFileName(fExtKalman, cut);
}

std::string PndFileNameCreator::GetVertexFileName(bool cut)
{
	return GetCustomFileName(fExtVertex, cut);
}
 
// -----------------

std::string PndFileNameCreator::GetParFileName(std::string inputFileName, bool cut)
{
	fFileName = inputFileName;
	return GetParFileName(cut);
}

std::string PndFileNameCreator::GetSimFileName(std::string inputFileName, bool cut)
{
	fFileName = inputFileName;
	return GetSimFileName(cut);
}

std::string PndFileNameCreator::GetDigiFileName(std::string inputFileName, bool cut)
{
	fFileName = inputFileName;
	return GetDigiFileName(cut);
}

std::string PndFileNameCreator::GetRecoFileName(std::string inputFileName, bool cut)
{
	fFileName = inputFileName;
	return GetRecoFileName(cut);
}

std::string PndFileNameCreator::GetTrackFindingFileName(std::string inputFileName, bool cut)
{
	fFileName = inputFileName;
	return GetTrackFindingFileName(cut);
}

std::string PndFileNameCreator::GetIdealTrackFindingFileName(std::string inputFileName, bool cut)
{
	fFileName = inputFileName;
	return GetIdealTrackFindingFileName(cut);
}

std::string PndFileNameCreator::GetKalmanFileName(std::string inputFileName, bool cut)
{
	fFileName = inputFileName;
	return GetKalmanFileName(cut);
}

std::string PndFileNameCreator::GetVertexFileName(std::string inputFileName, bool cut)
{
	fFileName = inputFileName;
	return GetVertexFileName(cut);
}

std::string PndFileNameCreator::GetRiemannFileName(std::string inputFileName, bool cut)
{
	fFileName = inputFileName;
	return GetRiemannFileName(cut);
}

std::string PndFileNameCreator::GetCombinedRiemannFileName(std::string inputFileName, bool cut)
{
	fFileName = inputFileName;
	return GetCombinedRiemannFileName(cut);
}

std::string PndFileNameCreator::GetCustomFileName(std::string ext, bool cut)
{
	std::string result = TruncateFileName(cut);
	if (ext.size() > 0){
		result += "_";
		result += ext;
	}
	result += ".root";
  if(fVerbose>0) std::cout<<" -I- PndFileNameCreator file: "<<result.c_str()<<std::endl;
	return result;
}

std::string PndFileNameCreator::GetCustomFileName(std::string inputFileName, std::string ext, bool cut)
{
	fFileName = inputFileName;
	return GetCustomFileName(ext, cut);
}

std::string PndFileNameCreator::GetCustomFileNameInitial(std::string ext)
{
	std::string result = ext;
	result += TruncateInitial();

	if (fVerbose>0) std::cout << " -I- PndFileNameCreator::GetCustomFileNameInitial: " << result.c_str() << std::endl;
	return result;
}

std::string PndFileNameCreator::GetPath()
{
	std::stringstream result;
	std::vector<std::string> resString;

	PndStringSeparator pathAna(fFileName,"/");
	resString = pathAna.GetStringVector();
	if(fVerbose>1) pathAna.Print();
	if (fFileName.find("/") == 0)
		result << "./";
	for (UInt_t i = 0; i < resString.size()-1; i++){
		result << resString[i] << "/";
	}
	return result.str();
}

std::string PndFileNameCreator::GetFileName()
{
	std::stringstream result;
	std::vector<std::string> resString;

	PndStringSeparator pathAna(fFileName,"/");
	resString = pathAna.GetStringVector();

	return resString[resString.size()-1];
}

std::string PndFileNameCreator::TruncateFileName(bool cut)
{
	std::vector<std::string> resString;
	std::stringstream result;
	std::string path, name;
	Int_t cutLast = 0;
  

	path = GetPath();
	name = GetFileName();
	//if(fVerbose>1) std::cout << "Path: " << path << " FileName: " << name << std::endl;
	result.str("");
  
	PndStringSeparator stringAna(name, "._");
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
	for (UInt_t i = 0; i < resString.size()-1 - cutLast; i++){
		result << resString[i] << "_";
	}
	result << resString[resString.size()-1 - cutLast];
	return result.str();
}

std::string PndFileNameCreator::TruncateInitial()
{
	std::vector<std::string> resString;
	std::stringstream result;
	std::string path, name;

	path = GetPath();
	name = GetFileName();

	PndStringSeparator stringAna(name, "._");
	resString = stringAna.GetStringVector();

	for (UInt_t i = 1; i < resString.size() - 1; i++){
		result << "_" << resString[i];
	}
	result << ".root";
	return result.str();
}
ClassImp(PndFileNameCreator);

