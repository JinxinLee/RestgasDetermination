/******************************************************

Storage of Data collected during HypGe Gamma Analysis Task created by M.Steinen steinen@kph.uni-mainz.de
Storage of Analysis of Gamma Simulation with hypGe detectors
*******************************************************/ 


#include "PndHypGeGammaAnaStorage.h"


PndHypGeGammaAnaStorage::PndHypGeGammaAnaStorage()
{
	
}


PndHypGeGammaAnaStorage::~PndHypGeGammaAnaStorage()
{
	
}

void PndHypGeGammaAnaStorage::SetAllEntries(Int_t Entries) 
{
	AllEntries = Entries;
}

Int_t PndHypGeGammaAnaStorage::GetAllEntries() 
{
	return AllEntries;
}

void PndHypGeGammaAnaStorage::SetSumPeak(Int_t SumPeak_Ext) 
{
	SumPeak = SumPeak_Ext;
}

Int_t PndHypGeGammaAnaStorage::GetSumPeak() 
{
	return SumPeak;
}

void PndHypGeGammaAnaStorage::SetInFile(TString InFile_Ext)
{
	InFile = InFile_Ext;
}
		
TString PndHypGeGammaAnaStorage::GetInFile()
{
	return InFile;
}

ClassImp(PndHypGeGammaAnaStorage)
