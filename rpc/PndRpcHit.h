//RpcTofHit
//Author E. Cordier
//Modified by A.Galoyan 4/12/07
//From PndRpcPoint to smeared hits

#ifndef PNDRPCHIT_H
#define PNDRPCHIT_H

#include "CbmHit.h"
#include "TVector3.h"
 
class PndRpcHit : public CbmHit
{

 public:

  /** Default constructor **/
  PndRpcHit();

  /** Constructor with hit parameters (2) [not the flag]**/
  PndRpcHit(Int_t detectorID, Int_t Gap, Int_t Cell, Int_t Module, 
TVector3 pos, TVector3 dpos, Double_t tof, Int_t index );

  /** Destructor **/
  virtual ~PndRpcHit();

  void Print(const Option_t* opt = 0) const;

  /** Accessors **/
  Double_t GetTime()       const { return fTime; };

   Int_t GetCell()         const   {return fCell; };
   Int_t GetModule()       const   {return fModule; };
   Int_t GetGap()          const   {return fGap; };
   Int_t GetIndex()        const  {return  fIndex; }; 
   
  TVector3 Getpos()        const {return fpos; };
  TVector3 Getdpos()       const {return fdpos; };  

  /** Modifiers **/
  void SetTime(Double_t time)        { fTime = time; };
//  void SetFlag(Int_t flag)           { fFlag = flag; };

  

 protected:

  Double_t fTime;              // Time since event start [ns]
  Int_t fdetectorID;      
  Int_t fGap;
  Int_t fCell;
  Int_t fModule;
  Int_t fIndex;
  TVector3 fpos;
  TVector3 fdpos;
  
//  Int_t  fFlag;          // Flag for general purposes [TDC, event tagging...]

  ClassDef(PndRpcHit,1)

};


#endif
