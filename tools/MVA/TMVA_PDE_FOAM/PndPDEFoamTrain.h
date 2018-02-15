/* ***************************************
 * PDEFoam  Trainerer                    *
 * Author: M.Babai@rug.nl                *
 * LICENSE:                              *
 * Version: 0.1 beta1.                   *
 * License:                              *
 * ***************************************
 */
#pragma once
#ifndef PND_PDE_FOAM_TRAIN_H
#define PND_PDE_FOAM_TRAIN_H

#include "PndMvaTrainer.h"

//#ifdef _OPENMP
//#undef _OPENMP
//#include<omp.h>
//#endif

class PndPDEFoamTrain: public PndMvaTrainer
{
 public:
  PndPDEFoamTrain(const std::string& InPutFile,
		  const std::vector<std::string>& ClassNames, 
		  const std::vector<std::string>& VarNames,
		  bool trim = true);
  
  virtual ~PndPDEFoamTrain();
  
  void Train();
  void TrainPar();

 protected:
  void WriteFoamsToFile();
  void InitFoam(TMVA::PDEFoam *pdefoam, TMVA::EFoamType ft);
  void SetXminXmax( TMVA::PDEFoam *pdefoam );

  // calculate Xmin and Xmax for Foam
  void CalcXminXmax();
  
  // fill variable names into foam
  void FillVariableNamesToFoam() const;

 private:
  PndPDEFoamTrain(const PndPDEFoamTrain& other);
  PndPDEFoamTrain& operator=(const PndPDEFoamTrain& other);

  void Init();

  std::vector<TMVA::PDEFoam*> m_foams;

  // range for histograms and foams
  std::vector<float> Xmin, Xmax;

  int m_VolFrac;
  int m_nCells;
  int m_nSampl;
  int m_nBin;
  int m_OptRej;
  int m_OptDrive;
  int m_EvPerBin;
  int m_Chat;
  bool m_SigBgSeparated;
  double m_Frac;
  int m_nActiveCells;
  
  TMVA::EKernel m_Kernel; // default: use no kernel

  // cuts
  bool   m_CutNmin;// cut on minimal number of events per cell
  int    m_Nmin;
  bool   m_CutRMSmin; // cut on minimal RMS in cell
  double m_RMSmin;

  TRandom3 *PseRan;
};
#endif
