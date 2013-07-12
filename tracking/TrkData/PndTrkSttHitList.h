/** PndTrkSttHitList

** @author Lia Lavezzi 
**/

#ifndef PNDTRKSTTHITLIST_H
#define PNDTRKSTTHITLIST_H 1

#include "PndTrkHitList.h"

#include "TVector3.h"
#include "PndTrkHit.h"
#include "PndSttHit.h"

#define MAXNOFSTTHITS 1000  // CHECK consistency

class PndTrkSttHitList : public PndTrkHitList
{

 public:    

  PndTrkSttHitList();
  PndTrkSttHitList(TClonesArray *tubearray);
  ~PndTrkSttHitList();    

  static PndTrkSttHitList * Instance();
  PndTrkSttHitList* Instanciate();

  void AddHit(Int_t hitid, Int_t detid, FairHit *hit);



  int GetNofHitsInLayer(int ilay);
  std::vector< PndTrkHit* > GetHitListFromLayer(int ilay);
  PndTrkHit *GetHitFromLayer(int ihit, int ilay);
  void PrintLayer(int ilay);
  void DrawLayer(int ilay, Color_t color);
  void PrintLayers();


  int GetNofHitsInSector(int isec);
  std::vector< PndTrkHit* > GetHitListFromSector(int isec);
  PndTrkHit *GetHitFromSector(int ihit, int isec);
  void DrawSector(int isec, Color_t color = kBlack);
  void PrintSector(int isec);
  void PrintSectors();

  int GetNofHitsInSectorAndLayer(int isec, int ilay);
  std::vector< PndTrkHit* > GetHitListFromSectorAndLayer(int isec, int ilay);
  PndTrkHit *GetHitFromSectorAndLayer(int ihit, int isec, int ilay);
  void DrawSectorAndLayer(int isec, int ilay, Color_t color = kBlack);
  void PrintSectorAndLayer(int isec, int ilay);
  void PrintSectorsAndLayers();

  Int_t FromSecLayToGlo(int isec, int ilay);
  void FromGloToSecLay(int iglo, int &isec, int &ilay);

  std::vector< PndTrkHit* > GetHitList();

 protected:
  static PndTrkSttHitList * fInstance;
  TClonesArray *fTubeArray;

  std::multimap< int , int > hitmap;
  std::multimap< int, int > hitmap2;
  std::multimap< int, int > hitmap3;
  ClassDef(PndTrkSttHitList,1);
};


#endif
