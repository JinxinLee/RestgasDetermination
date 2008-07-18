// -------------------------------------------------------------------------
// -----                PndRpcSimpleReco header file             -----
//----                  Created  16/06/08 by A.Galoyan                ----
// -------------------------------------------------------------------------


/** PndRpcSimpleReco.h
** Ideal Reconstruction of Mass and Velocity  
** using RPC Barrel TOF simulation and hit production results:
** PndRpcPoint Array  and PndRpcHit Array, and also track length,
** time of start signal from MC tracks"  
**/


#ifndef PNDRPCSIMPLERECO_H
#define PNDRPCSIMPLERECO_H 1

#include "CbmTask.h"

class TClonesArray;
class TH1F;
class TH2F;
class PndRpcHit;
class PndRpcPoint;
class TVector3;
class CbmMCTrack;

class PndRpcSimpleReco : public CbmTask
{

 public:

  /** Default constructor **/  
  PndRpcSimpleReco();


  /** Destructor **/
  ~PndRpcSimpleReco();


  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Finish **/
  virtual void Finish();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);


 private:

  /** Input array of PndRpcPoints **/
  TClonesArray* fPointArray;

  /** Input  array of PndRpcHits **/
  TClonesArray* fHitArray;  

 /** Input array of MCTracks **/
  TClonesArray* MCTrackArray ;

  TH1F* RecoMass2;   // Reconstructed squared Mass Distribution
  TH2F* Velocity_P;  // Reconstructed 1/Velocity % Momentum
  TH2F* Mass2_P;     // Reconstructed Mass2 % Momentum
 

 TString fOutFile;   //< name of file to save histograms

  ClassDef(PndRpcSimpleReco,1);

};

#endif
