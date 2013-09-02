// -----------------------------------------
// PndDrcLutInfo.h
//
// Created on: 13.07.2013
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------
// Container for look-up table

#ifndef PNDDRCLUTINFO_H
#define PNDDRCLUTINFO_H

#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include <vector>

class PndDrcLutInfo : public TObject {

public:    
  
  // Default constructor
  PndDrcLutInfo ();

  // Standard constructors


  // Copy constructor 
  PndDrcLutInfo (PndDrcLutInfo& node) { *this = node; }  

  // Modifiers
  void AddAngle(Double_t val);
  void AddPixelEnd(Double_t val);
  void AddTime(Double_t val);
  void AddPath(Double_t val);
  void AddChDiff(Double_t val);
  void AddNOfBounces(Double_t val);
  void AddNOfEVReflections(Int_t val);

  void SetLambda(Double_t val);
  void SetChPartDir(TVector3 val);
  void SetChPartDirInBar(TVector3 val);
  void SetChPartDirInBar2(TVector3 val);
  void SetChPartPdg(Int_t val);  
  void SetTime(Double_t val);
  void SetPath(Double_t val);
  void SetCherenkovMC(Double_t val);
  void SetCherenkovReal(Double_t val);

  // Accessors
  Int_t AngleEntries() 		    {return fAnglesSize; }
  Int_t PixelEntries() 		    {return fPixelSize; }
  Int_t TimeEntries() 		    {return fTimesSize; }
  Int_t PathEntries() 		    {return fPathsSize; }
  Int_t ChDiffEntries() 	    {return fChDiffsSize; }
  Int_t NOfBouncesEntries() 	    {return fNOfBouncesSize; }
  Int_t NOfEVReflectionEntries()    {return fNOfEVReflectionsSize; }
  
  Double_t GetLambda()		    {return fLambda;}
  TVector3 GetChPartDir()	    {return fChPartDir;}
  TVector3 GetChPartDirInBar()	    {return fChPartDirInBar;}
  TVector3 GetChPartDirInBar2()	    {return fChPartDirInBar2;}
  Int_t    GetChPartPdg()	    {return fChPartPdg;} 
  Double_t GetTime()		    {return fTime;}
  Double_t GetPath()		    {return fPath;}
  Double_t GetCherenkovMC()	    {return fCherenkovMC;}
  Double_t GetCherenkovReal()	    {return fCherenkovReal;}
  
  Double_t GetAngle(Int_t entry);
  Int_t    GetPixelEnd(Int_t entry);
  Double_t GetTime(Int_t entry);
  Double_t GetPath(Int_t entry);
  Double_t GetChDiff(Int_t entry);
  Int_t    GetNOfBounces(Int_t entry);
  Int_t    GetNOfEVReflections(Int_t entry);
    
protected:

  Int_t fAnglesSize;
  Int_t fPixelSize;
  Int_t fTimesSize;
  Int_t fPathsSize;
  Int_t fChDiffsSize;
  Int_t fNOfBouncesSize;
  Int_t fNOfEVReflectionsSize;

  // arrays of values based on the LUT ambiguities:
  std::vector<Double_t> fAnglesArray;

  std::vector<Int_t>    fPixelEndArray;
  std::vector<Double_t> fTimesArray;
  std::vector<Double_t> fPathsArray;
  std::vector<Double_t> fChDiffsArray;
  std::vector<Int_t>    fNOfBouncesArray;
  std::vector<TVector3> fPhotonStartPosition; 
  std::vector<Int_t>   fNOfEVReflectionsArray;

  // MC information
  Double_t fLambda;
  TVector3 fChPartDir;
  TVector3 fChPartDirInBar;
  TVector3 fChPartDirInBar2;
  Int_t    fChPartPdg;  
  Double_t fTime;
  Double_t fPath;
  Double_t fCherenkovMC;
  Double_t fCherenkovReal;
  
  ClassDef(PndDrcLutInfo,2)
};

#endif
